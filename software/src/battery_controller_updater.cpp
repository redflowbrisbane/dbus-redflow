#include <cmath>
#include <QsLog.h>
#include <QTimer>
#include "batteryController.h"
#include "battery_controller_settings.h"
#include "battery_controller_updater.h"
#include "battery_controller_bridge.h"
#include "modbus_rtu.h"

#define MODBUSREG_CLEAR_STATUS_REGISTER_FLAGS 					0x9031
#define MODBUSREG_ENABLE_SELF_MAINTENANCE_END_OF_DISCHARGE 		0x9032
#define MODBUSREG_SELF_DISCHARGE_AND_MAINTENANCE_CYCLE			0x9034



static const int MaxAcquisitionIndex = 1;
static const int MaxRegCount = 6;
static const int MaxTimeoutCount = 5;

static const int ConnectionLostWaitInterval = 60 * 1000;  // 60 seconds in ms
static const int UpdateSettingsInterval = 10 * 60 * 1000; // 10 minutes in ms

enum ParameterType {
	None,
	BattVolts,
	BussVolts,
	BattAmps,
	BussAmps,
	BattTemp,
	AirTemp,
	SOC,
	SOC_AmpHrs,
	StsRegWarning,
	StsRegSummary,
	StsRegHardwareFailure,
	StsRegOperationalFailure,
	NotUsed,
	StsRegOperationalMode,
	HealthIndication,
	ZBMState,
	DeviceAddress,
	ClearStatusRegisterFlags,
	EnableSelfMaintenanceAtTheEndOfDischarge,
	EnterRunCommand,
	SelfDischargeAndMaintenanceCycle
};

struct RegisterCommand {
	int regOffset;
	ParameterType action;
};

struct CompositeCommand {
	int reg;
	int inverval;
	RegisterCommand actions[MaxRegCount];
};

static const CompositeCommand ZBMCommands[] = {
	{ 0x9011, 0, { { 0, SOC }, { 1, SOC_AmpHrs }, { 2, BattVolts }, { 3, BattAmps }, { 4, BattTemp }, { 5, AirTemp } } },
	{ 0x9001, 0, { { 0, StsRegSummary }, { 1, StsRegHardwareFailure }, { 2, StsRegOperationalFailure }, { 3, StsRegWarning }, { 4, NotUsed }, { 5, NotUsed } } },
	{ 0x9008, 0, { { 0, StsRegOperationalMode }, { 1, NotUsed }, { 2, NotUsed }, { 3, NotUsed }, { 4, NotUsed }, { 5, NotUsed } } },
	{ 0x9017, 0, { { 0, HealthIndication }, { 1, BussVolts }, { 2, ZBMState }, { 3, NotUsed }, { 4, NotUsed }, { 5, NotUsed } } } ,		
	{ 0x9030, 0, { { 0, DeviceAddress }, { 1, ClearStatusRegisterFlags }, { 2, EnableSelfMaintenanceAtTheEndOfDischarge }, { 3, EnterRunCommand }, { 4, SelfDischargeAndMaintenanceCycle }, { 5, NotUsed } } }		
};

static const int ZBMCommandCount = sizeof(ZBMCommands) / sizeof(ZBMCommands[0]);


BatteryControllerUpdater::BatteryControllerUpdater(BatteryController *mBatteryController, ModbusRtu *modbus, QObject *parent):
	QObject(parent),
	mSettings(0),
	mModbus(0),
	mAcquisitionTimer(new QTimer(this)),
	mSettingsUpdateTimer(new QTimer(this)),
	mTimeoutCount(0),
	mSetupRequested(false),
	mApplication(0),
	mState(DeviceId),
	mCommands(0),
	mCommandCount(0),
	mCommandIndex(0),
	mAcquisitionIndex(0),
	mBatteryController(mBatteryController)
{
	Q_ASSERT(mBatteryController != 0);
	mModbus = modbus;
	connect(mModbus, SIGNAL(readCompleted(int, quint8, const QList<quint16> &)),
			this, SLOT(onReadCompleted(int, quint8, QList<quint16>)));
	connect(mModbus, SIGNAL(writeCompleted(int, quint8, quint16, quint16)),
			this, SLOT(onWriteCompleted(int, quint8, quint16, quint16)));
	connect(mModbus, SIGNAL(errorReceived(int, quint8, int)),
			this, SLOT(onErrorReceived(int, quint8, int)));
	connect(mAcquisitionTimer, SIGNAL(timeout()),
			this, SLOT(onWaitFinished()));
	connect(mSettingsUpdateTimer, SIGNAL(timeout()),
			this, SLOT(onUpdateSettings()));

	/*  signals from color control */

	connect(mBatteryController, SIGNAL(clearStatusRegisterFlagsChanged()),
		this, SLOT(onClearStatusRegisterFlagsChanged()));
	connect(mBatteryController, SIGNAL(requestDelayedSelfMaintenanceChanged()),
		this, SLOT(onRequestDelayedSelfMaintenanceChanged()));
	connect(mBatteryController, SIGNAL(requestImmediateSelfMaintenaceChanged()),
		this, SLOT(onRequestImmediateSelfMaintenanceChanged()));


	mSettingsUpdateTimer->setInterval(UpdateSettingsInterval);
	mSettingsUpdateTimer->start();
	mAcquisitionTimer->setSingleShot(true);
	mStopwatch.start();
	startNextAction();
}

BatteryControllerSettings *BatteryControllerUpdater::settings()
{
	return mSettings;
}

void BatteryControllerUpdater::onErrorReceived(int errorType, quint8 addr, int exception)
{
	if (addr != mBatteryController->DeviceAddress())
		return;

	if (errorType == ModbusRtu::Timeout) {
		if (mTimeoutCount == MaxTimeoutCount) {
			if (!mBatteryController->serial().isEmpty()) {
				QLOG_ERROR() << "Lost connection to battery controller";
			}
			mState = WaitOnConnectionLost;
			delete mSettings;
			mSettings = 0;
			mTimeoutCount = MaxTimeoutCount - 1;
			mBatteryController->setSerial(QString());
			mBatteryController->setConnectionState(Disconnected);
		} else {
			++mTimeoutCount;
		}
	}
	startNextAction();
}

void BatteryControllerUpdater::onReadCompleted(int function, quint8 addr,
										 const QList<quint16> &registers)
{
	if (addr != mBatteryController->DeviceAddress())
		return;
	Q_UNUSED(function)
	switch (mState) {
	case DeviceId:
		QLOG_INFO() << "EquipmentId:" << registers[0];
		//mBatteryController->setDeviceType(registers[0]);
			mState = Serial;

		break;
	case VersionCode:
		//mBatteryController->setDeviceSubType(registers[0]);
		mState = Serial;
		break;
	case Serial:
	{

		QString serial;
			
		serial = QString::number(((registers[0]<<16)&0xffff) + registers[1]);

		mBatteryController->setSerial(serial);
		QLOG_INFO() << "Serial number:" << registers[0] << registers[1];
		mState = FirmwareVersion;
		break;
	}
	case FirmwareVersion:
		mBatteryController->setFirmwareVersion(registers[0]);
		QLOG_INFO() << "FirmwareVersion: " << registers[0] << registers[1];
		mState = WaitForStart;
		break;
	case CheckSetup:
		Q_ASSERT(registers.size() == 2);
		mApplication = registers[0];
		mState =  Acquisition;
		break;
	case CheckMeasurementMode:
		Q_ASSERT(registers.size() == 1);
		mState = registers[0] == 0x01 ? Acquisition : SetMeasurementMode;
		break;
	case Acquisition:
		processAcquisitionData(registers);
		++mCommandIndex;
		break;
	case Wait:
		mState = Acquisition;
		break;
	default:
		QLOG_ERROR() << "Unknown updater state" << mState;
		mState = mSettings == 0 ? DeviceId : Acquisition;
		break;
	}
	mTimeoutCount = 0;
	startNextAction();
}

void BatteryControllerUpdater::onWriteCompleted(int function, quint8 addr,
										  quint16 address, quint16 value)
{
	if (addr != mBatteryController->DeviceAddress())
		return;
	Q_UNUSED(function)
	Q_UNUSED(address)
	Q_UNUSED(value)
	switch (mState) {
	case SetApplication:
		Q_ASSERT(function == ModbusRtu::WriteSingleRegister);
		Q_ASSERT(address == RegApplication);
		mState = Acquisition;
		break;
	case SetMeasuringSystem:
		Q_ASSERT(function == ModbusRtu::WriteSingleRegister);
		Q_ASSERT(address == RegMeasurementSystem);
		mState = Acquisition;
		break;
	case SetMeasurementMode:
		mState = Acquisition;
		break;
	case SetAddress:
		QLOG_WARN() << "Slave Address Changed";
		mState = Serial;
		break;
	default:
		mState = 
			Acquisition;
		break;
	}
	mTimeoutCount = 0;
	startNextAction();
}

void BatteryControllerUpdater::onWaitFinished()
{
	switch (mState) {
	case Wait:
		mStopwatch.restart();
		mState = Acquisition;
		break;
	case WaitOnConnectionLost:
		mState = DeviceId;
		break;
	default:
		mState = 
			Acquisition;
		break;
	}
	startNextAction();
}

void BatteryControllerUpdater::onUpdateSettings()
{
}

void BatteryControllerUpdater::startNextAction()
{

	if (mSetupRequested) {
		mSetupRequested = false;
			mState = CheckSetup;
	}
	switch (mState) {
	case DeviceId:
		mBatteryController->setConnectionState(Searched); 
		readRegisters(RegDevice, 1);
		break;
	case VersionCode:
		readRegisters(RegEm24VersionCode, 1);
		break;
	case Serial:
		readRegisters(RegSerial, 2);
		break;
	case FirmwareVersion:
		readRegisters(RegFirmwareVersion, 2);
		break;
	case CheckSetup:
		readRegisters(RegApplication, 2);
		break;
	case SetApplication:
		//writeRegister(RegApplication, ApplicationH);
		break;
	case WaitForStart:
		QLOG_INFO() << "serial == " << mBatteryController->serial();
		mSettings = new BatteryControllerSettings(mBatteryController->deviceType(),
											mBatteryController->serial(),
											mBatteryController);
		mBatteryController->setConnectionState(Detected);
		mSetupRequested = true;

		startNextAction();
		
		break;
	case Acquisition:
		mCommands = ZBMCommands;
		mCommandCount = ZBMCommandCount;
		startNextAcquisition();
		break;
	case Wait:
	{
		int sleep = mStopwatch.elapsed();
		sleep = 250 - sleep;
		if (sleep > 50) {
			mAcquisitionTimer->setInterval(/*sleep*/5000);
			mAcquisitionTimer->start();
		} else {
			onWaitFinished();
		}
		break;
	}
	case WaitOnConnectionLost:
		mAcquisitionTimer->setInterval(ConnectionLostWaitInterval);
		mAcquisitionTimer->start();
		break;
	case SetAddress:
		//writeRegister(0x2000, 2);
		break;
	default:
		QLOG_ERROR() << "Invalid state while starting new action" << mState;
		mState = Acquisition;
		startNextAction();
		break;
	}
}

void BatteryControllerUpdater::startNextAcquisition()
{
	const CompositeCommand *cmd = 0;

	for (;;) {
		if (mCommandIndex < mCommandCount)
			cmd = &mCommands[mCommandIndex];
		if (cmd !=0 && (cmd->inverval == 0 || mAcquisitionIndex == cmd->inverval)) {
			break;
		} else {
			++mCommandIndex;
			if (mCommandIndex >= mCommandCount) {
				mState = Wait;
				mCommandIndex = 0;
				++mAcquisitionIndex;
				if (mAcquisitionIndex == MaxAcquisitionIndex) {
					mAcquisitionIndex = 0;
					mBatteryController->setConnectionState(Connected);
				}
				startNextAction();
				return;
			}
		}
	}
	int maxOffset = 0;
	for (int i=0; i<MaxRegCount; ++i) {
		const RegisterCommand &ra = cmd->actions[i];
		if (ra.action == None)
			break;
		maxOffset = qMax(maxOffset, ra.regOffset);
	}
	readRegisters(cmd->reg, maxOffset + 2);
}

void BatteryControllerUpdater::readRegisters(quint16 startReg, quint16 count)
{
	mModbus->readRegisters(ModbusRtu::ReadHoldingRegisters,
						   mBatteryController->DeviceAddress(), startReg, count);
}

void BatteryControllerUpdater::writeRegister(quint16 reg, quint16 value)
{
	mModbus->writeRegister(ModbusRtu::WriteSingleRegister,
						   mBatteryController->DeviceAddress(), reg, value);
}

void BatteryControllerUpdater::processAcquisitionData(const QList<quint16> &registers)
{
	QString stemp;

	const CompositeCommand &cmd = mCommands[mCommandIndex];
	for (int i=0; i<MaxRegCount; ++i) {
		const RegisterCommand &ra = cmd.actions[i];
		if (ra.action == None)
			break;

			{
			switch (ra.action) {
			case BattVolts:
				QLOG_INFO() << "BattVolts: " << registers[ra.regOffset];
				mBatteryController->setBattVolts(registers[ra.regOffset]);
				break;
			case BussVolts:
				QLOG_INFO() << "BussVolts: " << registers[ra.regOffset];
				mBatteryController->setBussVolts(registers[ra.regOffset]);
				break;
			case BattAmps:
				stemp =  QString::number((short) registers[ra.regOffset],10);
				QLOG_INFO() << "BattAmps: " << stemp;
				mBatteryController->setBattAmps(stemp.toInt());
				break;
			case BussAmps:
				stemp = QString::number((short) registers[ra.regOffset],10);
				QLOG_INFO() << "BussAmps: " << registers[ra.regOffset];
				mBatteryController->setBussAmps(stemp.toInt());
				break;
			case BattTemp:
				QLOG_INFO() << "BattTemp: " << registers[ra.regOffset];
				mBatteryController->setBattTemp(static_cast<qint16>(registers[ra.regOffset]));
				break;
			case AirTemp:
				QLOG_INFO() << "AirTemp: " << registers[ra.regOffset];
				mBatteryController->setAirTemp(static_cast<qint16>(registers[ra.regOffset]));
				break;
			case SOC:
				QLOG_INFO() << "SOC: " << registers[ra.regOffset];
				mBatteryController->setSOC(registers[ra.regOffset]);
				break;	
			case StsRegSummary:
				//QLOG_INFO() << "StsRegSummary: " << registers[ra.regOffset];
				mBatteryController->setStsRegSummary(registers[ra.regOffset]);
				break;	
			case StsRegHardwareFailure:
				//QLOG_INFO() << "StsRegHardwareFailure: " << registers[ra.regOffset];
				mBatteryController->setStsRegHardwareFailure(registers[ra.regOffset]);
				break;
			case StsRegOperationalFailure:
				//QLOG_INFO() << "StsRegOperationalFailure: " << registers[ra.regOffset];
				mBatteryController->setStsRegOperationalFailure(registers[ra.regOffset]);
				break;	
			case StsRegWarning:
				//QLOG_INFO() << "StsRegWarning: " << registers[ra.regOffset];
				mBatteryController->setStsRegWarning(registers[ra.regOffset]);
				break;	
			case StsRegOperationalMode:
				QLOG_INFO() << "StsRegOperationalMode: " << registers[ra.regOffset];
				mBatteryController->setStsRegOperationalMode(registers[ra.regOffset]);
				break;
			case SOC_AmpHrs:
				QLOG_INFO() << "SOC_AmpHrs: " << registers[ra.regOffset];
				mBatteryController->setSOCAmpHrs(static_cast<qint16>(registers[ra.regOffset]));
				break;			
			case HealthIndication:
				//QLOG_INFO() << "HealthIndication: " << registers[ra.regOffset];
				mBatteryController->setHealthIndication(registers[ra.regOffset]);
				break;	
			case ZBMState:
				QLOG_INFO() << "ZBMState: " << registers[ra.regOffset];
				mBatteryController->setState(registers[ra.regOffset]);
				break;
			case DeviceAddress:
				QLOG_INFO() << "DeviceAddress: " << registers[ra.regOffset];
				mBatteryController->setDeviceAddress(registers[ra.regOffset]);
				break;
			case ClearStatusRegisterFlags:
				QLOG_INFO() << "ClearStatusRegisterFlags: " << registers[ra.regOffset];
				mBatteryController->setClearStatusRegisterFlags(registers[ra.regOffset]);
				break;
			case EnableSelfMaintenanceAtTheEndOfDischarge:
				QLOG_INFO() << "EnableSelfMaintenanceAtTheEndOfDischarge: " << registers[ra.regOffset];
				mBatteryController->setEnableSelfMaintenanceAtTheEndOfDischarge(registers[ra.regOffset]);
				break;
			case EnterRunCommand:
				QLOG_INFO() << "EnterRunCommand: " << registers[ra.regOffset];
				mBatteryController->setEnterRunCommand(registers[ra.regOffset]);
				break;		
			case SelfDischargeAndMaintenanceCycle:
				QLOG_INFO() << "SelfDischargeAndMaintenanceCycle: " << registers[ra.regOffset];
				mBatteryController->setSelfDischargeAndMaintenanceCycle(registers[ra.regOffset]);
				break;	
			default:
				break;
			}
		}
	}
}


void BatteryControllerUpdater::onRequestDelayedSelfMaintenanceChanged()
/* This function writes back the changes from the Victron color control to the ZBM registers */
{
	QLOG_INFO() <<"ONREQUESTDELAYEDSELFMAINTENANCECHANGED";
	this->writeRegister(MODBUSREG_ENABLE_SELF_MAINTENANCE_END_OF_DISCHARGE,(quint16) mBatteryController->RequestDelayedSelfMaintenance());
}


void BatteryControllerUpdater::onRequestImmediateSelfMaintenanceChanged()
/* This function writes back the changes from the Victron color control to the ZBM registers */
{
	QLOG_INFO() << "ONREQUESTIMMEDIATESELFMAINTENANCECHANGED";
	this->writeRegister(MODBUSREG_SELF_DISCHARGE_AND_MAINTENANCE_CYCLE,(quint16) mBatteryController->RequestImmediateSelfMaintenance());
}

void BatteryControllerUpdater::onClearStatusRegisterFlagsChanged()
/* This function writes back the changes from the Victron color control to the ZBM registers */
{
	QLOG_INFO() << "ONCLEARSTATUSREGISTERFLAGSCHANGED";
	this->writeRegister(MODBUSREG_CLEAR_STATUS_REGISTER_FLAGS,(quint16) mBatteryController->ClearStatusRegisterFlags());
}
