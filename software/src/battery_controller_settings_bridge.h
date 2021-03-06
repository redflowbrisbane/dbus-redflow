#ifndef BATTERY_CONTROLLER_SETTINGS_BRIDGE_H
#define BATTERY_CONTROLLER_SETTINGS_BRIDGE_H

#include "dbus_bridge.h"

class BatteryControllerSettings;

/*!
 * Bridge between the settings of a single Battery Controller and the D-Bus.
 *
 * D-Bus path: com.victronenergy.settings/Settings/Redflow/Devices/D<serial>
 * The serial is prefixed with a 'D', because the serial may start with a
 * digit, which is not allowed by the localsettings as start of a settings
 * names (causes problems when storing settings data as xml file).
 */
class BatteryControllerSettingsBridge : public DBusBridge
{
	Q_OBJECT
public:
	BatteryControllerSettingsBridge(BatteryControllerSettings *settings,
							  QObject *parent = 0);

protected:
	virtual bool toDBus(const QString &path, QVariant &v);

	virtual bool fromDBus(const QString &path, QVariant &v);
};

#endif // BATTERY_CONTROLLER_SETTINGS_BRIDGE_H
