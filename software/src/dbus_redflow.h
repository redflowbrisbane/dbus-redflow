#ifndef DBUS_REDFLOW_H
#define DBUS_REDFLOW_H

#include <QObject>
#include <QList>

class BatteryController;
class BatteryControllerUpdater;
class ControlLoop;
class DbusServiceMonitor;
class ModbusRtu;
class Settings;

/*!
 * Main object which ties everything together.
 * This class will coordinate the search for new AC sensors, and start
 * acquisition when found. The class `AcSensorUpdater` is responsible for
 * communication with the sensors. Sensor data is stored in `AcSensor`.
 *
 * The class will also make sure that the Hub-4 control loop is started when
 * apropriate. The control loop itself is implemented in `ControlLoop`.
 *
 * Finally, this class will detect the presence of a multi within the setup,
 * which we need for the Hub-4 control loop.
 */
class DBusRedflow : public QObject
{
	Q_OBJECT
public:
	DBusRedflow(const QString &portName, QObject *parent = 0);

signals:
	void connectionLost();

private slots:
	void onDeviceFound();

	void onDeviceSettingsInitialized();

	void onDeviceInitialized();

	void onConnectionLost();

	void onConnectionStateChanged();

	void onSerialEvent(const char *description);

	void onServicesChanged();

	void onServiceTypeChanged();

	void onControlLoopEnabledChanged();

private:
	void updateControlLoop();

	DbusServiceMonitor *mServiceMonitor;
	ModbusRtu *mModbus;
	QList<BatteryController *> mBatteryController;
	Settings *mSettings;
	QList<ControlLoop *> mControlLoops;
};

#endif // DBUS_REDFLOW_H
