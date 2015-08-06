#include <QsLog.h>
#include "batteryController.h"

BatteryController::BatteryController(const QString &portName, int slaveAddress, QObject *parent) :
	QObject(parent),
	mConnectionState(Disconnected),
	mDeviceType(0),
	mDeviceSubType(0),
	mErrorCode(0),
	mFirmwareVersion(0),
	mPortName(portName),
	mSlaveAddress(slaveAddress)
{
	
}

ConnectionState BatteryController::connectionState() const
{
	return mConnectionState;
}

void BatteryController::setConnectionState(ConnectionState state)
{
	if (mConnectionState == state)
		return;
	mConnectionState = state;
	emit connectionStateChanged();
}

int BatteryController::deviceType() const
{
	return mDeviceType;
}

void BatteryController::setDeviceType(int t)
{
	if (mDeviceType == t)
		return;
	mDeviceType = t;
	emit deviceTypeChanged();
}

int BatteryController::deviceSubType() const
{
	return mDeviceSubType;
}

void BatteryController::setDeviceSubType(int t)
{
	if (mDeviceSubType == t)
		return;
	mDeviceSubType = t;
	emit deviceSubTypeChanged();
}

QString BatteryController::productName() const
{
	return "ZBM";
}

int BatteryController::errorCode() const
{
	return mErrorCode;
}

void BatteryController::setErrorCode(int code)
{
	if (mErrorCode == code)
		return;
	mErrorCode = code;
	emit errorCodeChanged();
}

QString BatteryController::portName() const
{
	return mPortName;
}

int BatteryController::slaveAddress() const
{
	return mSlaveAddress;
}

QString BatteryController::serial() const
{
	return mSerial;
}

void BatteryController::setSerial(const QString &s)
{
	if (mSerial == s)
		return;
	mSerial = s;
	emit serialChanged();
}

int BatteryController::firmwareVersion() const
{
	return mFirmwareVersion;
}

void BatteryController::setFirmwareVersion(int v)
{
	if (mFirmwareVersion == v)
		return;
	mFirmwareVersion = v;
	emit firmwareVersionChanged();
}

int BatteryController::BattVolts() const
{
	return mBattVolts /10;
}

void BatteryController::setBattVolts(int t)
{
	if (mBattVolts == t)
		return;
	mBattVolts = t;
	emit battVoltsChanged();
	emit battPowerChanged();
}

int BatteryController::BussVolts() const
{
	return mBussVolts /10;
}

void BatteryController::setBussVolts(int t)
{
	if (mBussVolts == t)
		return;
	mBussVolts = t;
	emit battVoltsChanged();	// since every emit invokes the same publish routine
}

double BatteryController::BattAmps() const
{
	return (double) (((float)mBattAmps)/10)*(-1) ;
}

void BatteryController::setBattAmps(int t)
{
	if (mBattAmps == t)
		return;
	mBattAmps = t;
	emit battAmpsChanged();
	emit battPowerChanged();
}

double BatteryController::BussAmps() const
{
	return (double)  (((float)mBussAmps)/10)*(-1);
}

void BatteryController::setBussAmps(int t)
{
	if (mBussAmps == t)
		return;
	mBussAmps = t;
	//this function is never called since we are not reading BussAmps from the battery controller
}

double BatteryController::BattTemp() const
{
	return (double) (((float) mBattTemp) / 10);
}

void BatteryController::setBattTemp(int t)
{
	if (mBattTemp == t)
		return;
	mBattTemp = t;
	emit battTempChanged();
}

int BatteryController::AirTemp() const
{
	return mAirTemp;
}

void BatteryController::setAirTemp(int t)
{
	if (mAirTemp == t)
		return;
	mAirTemp = t;
	emit battTempChanged();	// since every emit calls the same publish function
}

int BatteryController::SOC() const
{
	return mSOC / 100;
}

void BatteryController::setSOC(int t)
{
	if (mSOC == t)
		return;
	mSOC = t;
	emit socChanged();
}

int BatteryController::BattPower() const
{
	return (mBattAmps /10) * (mBattVolts / 10) * (-1);
}

void BatteryController::setBattPower(int t)
{
	if (mBattPower == t)
		return;
	mBattPower = t;
	// this function is never called since we do not read power but calculate it 
}

int BatteryController::StsRegSummary() const
{
	return mStsRegSummary;
}

void BatteryController::setStsRegSummary(int t)
{
	if (mStsRegSummary == t)
		return;
	mStsRegSummary = t;
	emit socChanged();	 
}


int BatteryController::StsRegHardwareFailure() const
{
	return mStsRegHardwareFailure;
}

void BatteryController::setStsRegHardwareFailure(int t)
{
	if (mStsRegHardwareFailure == t)
		return;
	mStsRegHardwareFailure = t;
	emit socChanged();	 
}


int BatteryController::StsRegOperationalFailure() const
{
	return mStsRegOperationalFailure;
}

void BatteryController::setStsRegOperationalFailure(int t)
{
	if (mStsRegOperationalFailure == t)
		return;
	mStsRegOperationalFailure = t;
	emit socChanged();	 
}


int BatteryController::StsRegWarning() const
{
	return mStsRegWarning;
}

void BatteryController::setStsRegWarning(int t)
{
	if (mStsRegWarning == t)
		return;
	mStsRegWarning = t;
	emit socChanged();	 
}


int BatteryController::StsRegOperationalMode() const
{
	return mStsRegOperationalMode;
}

void BatteryController::setStsRegOperationalMode(int t)
{
	if (mStsRegOperationalMode == t)
		return;
	mStsRegOperationalMode = t;
	emit socChanged();	 
}


int BatteryController::SOCAmpHrs() const
{
	return mSOCAmpHrs;
}

void BatteryController::setSOCAmpHrs(int t)
{
	if (mSOCAmpHrs == t)
		return;
	mSOCAmpHrs = t;
	emit socChanged();	 
}


int BatteryController::HealthIndication() const
{
	return mHealthIndication;
}

void BatteryController::setHealthIndication(int t)
{
	if (mHealthIndication == t)
		return;
	mHealthIndication = t;
	emit socChanged();	 
}


int BatteryController::State() const
{
	return mState;
}

void BatteryController::setState(int t)
{
	if (mState == t)
		return;
	mState = t;
	emit socChanged();	 
}
