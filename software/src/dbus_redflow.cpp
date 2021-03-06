#include <QsLog.h>
#include "battery_controller_bridge.h"
#include "battery_controller_settings.h"
#include "battery_controller_settings_bridge.h"
#include "battery_controller_updater.h"
#include "dbus_redflow.h"
#include "dbus_service_monitor.h"
#include "settings.h"
#include "settings_bridge.h"
#include "batteryController.h"

DBusRedflow::DBusRedflow(const QString &portName, QObject *parent):
	QObject(parent),
	/*mServiceMonitor(new DbusServiceMonitor("com.victronenergy.vebus", this)),*/
	mModbus(new ModbusRtu(portName, 19200, this))
{
	qRegisterMetaType<ConnectionState>();
	qRegisterMetaType<QList<quint16> >();

		BatteryController *m = new BatteryController(portName, 1 /*slave address*/, this);
		new BatteryControllerUpdater(m, mModbus, m);
				mBatteryController.append(m);
		connect(m, SIGNAL(connectionStateChanged()),
				this, SLOT(onConnectionStateChanged()));
	
	mSettings = new Settings(this);
	new SettingsBridge(mSettings, this);

	connect(mModbus, SIGNAL(serialEvent(const char *)),
			this, SLOT(onSerialEvent(const char *)));
}

void DBusRedflow::onConnectionStateChanged()
{
	BatteryController *m = static_cast<BatteryController *>(sender());
	switch (m->connectionState()) {
	case Disconnected:
		onConnectionLost();
		break;
	case Searched:
		break;
	case Detected:
		onDeviceFound();
		break;
	case Connected:
		onDeviceInitialized();
		break;
	}
}

void DBusRedflow::onDeviceFound()
{
	BatteryController *m = static_cast<BatteryController *>(sender());
	BatteryControllerUpdater *mu = m->findChild<BatteryControllerUpdater *>();
	QLOG_INFO() << "Device found:" << m->serial()
				<< '@' << m->portName();
	BatteryControllerSettings *settings = mu->settings();
	settings->setParent(m);
	connect(settings, SIGNAL(serviceTypeChanged()),
			this, SLOT(onServiceTypeChanged()));
	BatteryControllerSettingsBridge *b =
			new BatteryControllerSettingsBridge(settings, settings);
	connect(b, SIGNAL(initialized()),
			this, SLOT(onDeviceSettingsInitialized()));
	mSettings->registerDevice(m->serial());
}

void DBusRedflow::onDeviceSettingsInitialized()
{
	BatteryControllerSettingsBridge *b = static_cast<BatteryControllerSettingsBridge *>(sender());
	BatteryControllerSettings *s = static_cast<BatteryControllerSettings *>(b->parent());
	BatteryController *m = static_cast<BatteryController *>(s->parent());
	BatteryControllerUpdater *mu = m->findChild<BatteryControllerUpdater *>();
}

void DBusRedflow::onDeviceInitialized()
{
	BatteryController *m = static_cast<BatteryController *>(sender());
	BatteryControllerUpdater *mu = m->findChild<BatteryControllerUpdater *>();
	new BatteryControllerBridge(m, mu->settings(), mSettings, m, mu);
}

void DBusRedflow::onServiceTypeChanged()
{
	BatteryControllerSettings *s = static_cast<BatteryControllerSettings *>(sender());
	BatteryController *m = static_cast<BatteryController *>(s->parent());
	BatteryControllerBridge *bridge = m->findChild<BatteryControllerBridge *>();
	BatteryControllerUpdater *mu = m->findChild<BatteryControllerUpdater *>();

	if (bridge == 0) {
		// Settings have not been fully initialized yet. We need to have all
		// settings before creating the D-Bus service.
		return;
	}
	// Deleting and recreating the bridge will force recreation of the D-Bus
	// service with another name.
	delete bridge;
	new BatteryControllerBridge(m, s, mSettings, m, mu);
}

void DBusRedflow::onControlLoopEnabledChanged()
{

}

void DBusRedflow::onConnectionLost()
{

}

void DBusRedflow::onSerialEvent(const char *description)
{
	QLOG_ERROR() << "Serial event:" << description
				 << "Application will shut down.";
	exit(1);
}

void DBusRedflow::onServicesChanged()
{

}

