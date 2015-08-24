#include <cmath>
#include <QCoreApplication>
#include <QsLog.h>
#include <QStringList>
#include <velib/vecan/products.h>
#include "batteryController.h"
#include "battery_controller_bridge.h"
#include "battery_controller_settings.h"
#include "battery_controller_updater.h"
#include "settings.h"
#include "version.h"


BatteryControllerBridge::BatteryControllerBridge(BatteryController *BatteryController,
							   BatteryControllerSettings *emSettings,
							   Settings *settings,
							   QObject *parent,
							   BatteryControllerUpdater *BatteryControllerUpdater) :
	DBusBridge(parent),
	mBatteryController(BatteryController)
{

	connect(BatteryController, SIGNAL(destroyed()), this, SLOT(deleteLater()));
	connect(emSettings, SIGNAL(destroyed()), this, SLOT(deleteLater()));

	setUpdateInterval(1000);

	setServiceName(QString("com.victronenergy.battery.redflow_%1").
			arg(BatteryController->serial()));

	produce(BatteryController, "connectionState", "/Connected");
	produce(BatteryController, "errorCode", "/ErrorCode");

	//produce(emSettings, "position", "/Position");
	produce(emSettings, "customName", "/CustomName");

	QString processName = QCoreApplication::arguments()[0];
	// The values of the items below will not change after creation, so we don't
	// need an update mechanism.
	produce("/Mgmt/ProcessName", processName);
	produce("/Mgmt/ProcessVersion", VERSION);
	produce("/FirmwareVersion", BatteryController->firmwareVersion());
	produce("/ProductName", BatteryController->productName());
	produce("/ProductId", VE_PROD_ID_REDFLOW_ZBM2);
	produce("/DeviceType", BatteryController->deviceType());
	QString portName = BatteryController->portName();
	int deviceInstance = getDeviceInstance(portName, "/dev/ttyUSB", 288);
	if (deviceInstance == -1)
		deviceInstance = getDeviceInstance(portName, "/dev/ttyO", 256);
	produce("/Mgmt/Connection", portName);
	produce("/DeviceInstance", deviceInstance);
	produce("/Capabilities", "Redflow,IntegratedSoc");
	QString serial = BatteryController->serial();
	produce("/Serial", serial);

	produceBatteryInfo(BatteryController, "");

	registerService();
}

bool BatteryControllerBridge::toDBus(const QString &path, QVariant &value)
{
	if (path == "/Connected") {
		value = QVariant(value.value<ConnectionState>() == Connected ? 1 : 0);
	} else if (path == "/CustomName") {
		QString name = value.toString();
		if (name.isEmpty())
			value = mBatteryController->productName();
	}
	if (value.type() == QVariant::Double && !std::isfinite(value.toDouble()))
		value = QVariant();
	return true;
}


void BatteryControllerBridge::produceBatteryInfo(BatteryController *bc, const QString &path)
{
	produce(bc, "BattAmps", path + "/Dc/0/Current", "A", 1);
	produce(bc, "BattVolts", path + "/Dc/0/Voltage", "V", 0);
	produce(bc, "BattPower", path + "/Dc/0/Power", "W", 0);
	produce(bc, "BattTemp", path + "/Dc/0/Temperature", "C", 1);
	produce(bc, "SOC", path + "/Soc", "%", 1);

	produce(bc, "StsRegSummary", path + "/StsRegSummary", "", 0);
	produce(bc, "StsRegHardwareFailure", path + "/StsRegHardwareFailure", "", 0);
	produce(bc, "StsRegOperationalFailure", path + "/StsRegOperationalFailure", "", 0);
	produce(bc, "StsRegWarning", path + "/StsRegWarning", "", 0);
	produce(bc, "StsRegOperationalMode", path + "/StsRegOperationalMode", "", 0);
	produce(bc, "SOCAmpHrs", path + "/SOCAmpHrs", "", 0 );
	produce(bc, "AirTemp", path + "/AirTemp", "", 0);
	produce(bc, "HealthIndication", path + "/HealthIndication", "", 0);
	produce(bc, "BussVolts", path + "/BussVolts", "", 0);
	produce(bc, "State", path + "/State", "", 0);
	produce(bc, "DeviceAddress", path + "/DeviceAddress", "", 0);
	produce(bc, "ClearStatusRegisterFlags", path + "/ClearStatusRegisterFlags", "", 0);
	produce(bc, "RequestDelayedSelfMaintenance", path + "/RequestDelayedSelfMaintenance", "", 0);
	produce(bc, "SetOperationalMode", path + "/SetOperationalMode", "", 0);
	produce(bc, "RequestImmediateSelfMaintenance", path + "/RequestImmediateSelfMaintenance", "", 0);
	
}


bool BatteryControllerBridge::fromDBus(const QString &path, QVariant &value)
{
	if (path == "/CustomName") {
		QString name = value.toString();
		if (name == mBatteryController->productName())
			value = "";
		return true;
	}
	// Return value false means that changes from the D-Bus will not be passed
	// to the QT properties.
	return true;
}


int BatteryControllerBridge::getDeviceInstance(const QString &path,
										 const QString &prefix,
										 int instanceBase)
{
	if (path.startsWith(prefix)) {
		return instanceBase + path.mid(prefix.size()).toInt();
	}
	return -1;
}
