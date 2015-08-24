#ifndef BATTERY_CONTROLLER_H
#define BATTERY_CONTROLLER_H

#include <QMetaType>
#include <QObject>
#include "defines.h"

enum ConnectionState {
	Disconnected,
	Searched,
	Detected,
	Connected
};

Q_DECLARE_METATYPE(ConnectionState)


class BatteryController : public QObject
{
	Q_OBJECT
	Q_PROPERTY(double BattAmps READ BattAmps WRITE setBattAmps NOTIFY battAmpsChanged)
	Q_PROPERTY(int BattVolts READ BattVolts WRITE setBattVolts NOTIFY battVoltsChanged)
	Q_PROPERTY(double BattTemp READ BattTemp WRITE setBattTemp NOTIFY battTempChanged)
	Q_PROPERTY(int BattPower READ BattPower WRITE setBattPower NOTIFY battPowerChanged)
	Q_PROPERTY(int SOC READ SOC WRITE setSOC NOTIFY socChanged);
	Q_PROPERTY(ConnectionState connectionState READ connectionState WRITE setConnectionState NOTIFY connectionStateChanged)
	Q_PROPERTY(int deviceType READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
	Q_PROPERTY(int deviceSubType READ deviceSubType WRITE setDeviceSubType NOTIFY deviceSubTypeChanged)
	Q_PROPERTY(QString productName READ productName)
	Q_PROPERTY(QString serial READ serial WRITE setSerial NOTIFY serialChanged)
	Q_PROPERTY(int firmwareVersion READ firmwareVersion WRITE setFirmwareVersion NOTIFY firmwareVersionChanged)
	Q_PROPERTY(int errorCode READ errorCode WRITE setErrorCode NOTIFY errorCodeChanged)
	Q_PROPERTY(QString portName READ portName)
	Q_PROPERTY(double StsRegOperationalMode READ StsRegOperationalMode WRITE setStsRegOperationalMode NOTIFY stsRegOperationalModeChanged)
	Q_PROPERTY(double StsRegSummary READ StsRegSummary WRITE setStsRegSummary NOTIFY stsRegSummaryChanged)
	Q_PROPERTY(double StsRegOperationalFailure READ StsRegOperationalFailure WRITE setStsRegOperationalFailure NOTIFY stsRegOperationalFailureChanged)
	Q_PROPERTY(double StsRegHardwareFailure READ StsRegHardwareFailure WRITE setStsRegHardwareFailure NOTIFY stsRegHardwareFailureChanged)
	Q_PROPERTY(double StsRegWarning READ StsRegWarning WRITE setStsRegWarning NOTIFY stsRegWarningChanged)
	Q_PROPERTY(double SOCAmpHrs READ SOCAmpHrs WRITE setSOCAmpHrs NOTIFY socAmpHrsChanged)
	Q_PROPERTY(double AirTemp READ AirTemp WRITE setAirTemp NOTIFY airTempChanged)
	Q_PROPERTY(double HealthIndication READ HealthIndication WRITE setHealthIndication NOTIFY healthIndicationChanged)
	Q_PROPERTY(double BussVolts READ BussVolts WRITE setBussVolts NOTIFY bussVoltsChanged)
	Q_PROPERTY(double State READ State WRITE setState NOTIFY stateChanged)
	Q_PROPERTY(double DeviceAddress READ DeviceAddress WRITE setDeviceAddress NOTIFY deviceAddressChanged)
	Q_PROPERTY(double ClearStatusRegisterFlags READ ClearStatusRegisterFlags WRITE setClearStatusRegisterFlags NOTIFY clearStatusRegisterFlagsChanged)
	Q_PROPERTY(double RequestDelayedSelfMaintenance READ RequestDelayedSelfMaintenance WRITE setRequestDelayedSelfMaintenance NOTIFY requestDelayedSelfMaintenanceChanged)
	Q_PROPERTY(double SetOperationalMode READ SetOperationalMode WRITE setSetOperationalMode NOTIFY setOperationalModeChanged)
	Q_PROPERTY(double RequestImmediateSelfMaintenance READ RequestImmediateSelfMaintenance WRITE setRequestImmediateSelfMaintenance NOTIFY requestImmediateSelfMaintenanceChanged)

signals:
	void battAmpsChanged();
	void battVoltsChanged();
	void battTempChanged();
	void bussAmpsChanged();
	void bussVoltsChanged();
	void airTempChanged();
	void socChanged();
	void battPowerChanged();

	void stsRegSummaryChanged();
	void stsRegHardwareFailureChanged();
	void stsRegOperationalFailureChanged();
	void stsRegWarningChanged();
	void stsRegOperationalModeChanged();
	void socAmpHrsChanged();
	void healthIndicationChanged();
	void stateChanged();
	void deviceAddressChanged();
	void clearStatusRegisterFlagsChanged();
	void requestDelayedSelfMaintenanceChanged();
	void setOperationalModeChanged();
	void requestImmediateSelfMaintenanceChanged();
public:
	BatteryController(const QString &portName, int slaveAddress, QObject *parent = 0);

	ConnectionState connectionState() const;

	void setConnectionState(ConnectionState state);

	/*!
	 * Returns the device type as reported by the energy meter.
	 */
	int deviceType() const;

	void setDeviceType(int t);

	/*!
	 * Returns the device sub type as reported by the energy meter. This value
	 * is non-zero for EM24 energy meters only.
	 */
	int deviceSubType() const;

	void setDeviceSubType(int t);

	QString productName() const;

	QString serial() const;

	void setSerial(const QString &s);

	int firmwareVersion() const;

	void setFirmwareVersion(int v);

	int BattVolts() const;

	void setBattVolts(int t);

	double BattAmps() const;

	void setBattAmps(int t);

	int BussVolts() const;

	void setBussVolts(int t);

	double BussAmps() const;

	void setBussAmps(int t);

	double BattTemp() const;

	void setBattTemp(int t);

	int AirTemp() const;

	void setAirTemp(int t);

	int SOC() const;

	void setSOC(int t);

	int BattPower() const;

	void setBattPower(int t);

	int StsRegSummary() const;
	void setStsRegSummary(int t);
	int StsRegHardwareFailure() const;
	void setStsRegHardwareFailure(int t);
	int StsRegOperationalFailure() const;
	void setStsRegOperationalFailure(int t);
	int StsRegWarning() const;
	void setStsRegWarning(int t);
	int StsRegOperationalMode() const;
	void setStsRegOperationalMode(int t);
	int SOCAmpHrs() const;
	void setSOCAmpHrs(int t);
	int HealthIndication() const;
	void setHealthIndication(int t);
	int State() const;
	void setState(int t);
	int DeviceAddress() const;
	void setDeviceAddress(int t);
	int ClearStatusRegisterFlags() const;
	void setClearStatusRegisterFlags(int t);
	int RequestDelayedSelfMaintenance() const;
	void setRequestDelayedSelfMaintenance(int t);
	int SetOperationalMode() const;
	void setSetOperationalMode(int t);
	int RequestImmediateSelfMaintenance() const;
	void setRequestImmediateSelfMaintenance(int t);

	/*!
	 * Returns the error code.
	 * Possible values:
	 * - 0: no error
	 * - 1: front selector is locked (EM24 only). When the selector is locked,
	 * in is not possible to change settings in the energy meter. Relevant
	 * settings are: power returned to grid should be negative (application H)
	 * and (optionnally) change between single phase and multi phase
	 * measurement.
	 */
	int errorCode() const;

	void setErrorCode(int code);

	/*!
	 * Returns the logical name of the communication port. (eg. /dev/ttyUSB1).
	 */
	QString portName() const;

	int slaveAddress() const;

signals:
	void connectionStateChanged();

	void deviceTypeChanged();

	void deviceSubTypeChanged();

	void serialChanged();

	void firmwareVersionChanged();

	void errorCodeChanged();

private:
	ConnectionState mConnectionState;
	int mDeviceType;
	int mDeviceSubType;
	int mErrorCode;
	int mFirmwareVersion;
	QString mPortName;
	int mSlaveAddress;
	QString mSerial;
	int mBattVolts;
	int mBussVolts;
	int mBattAmps;
	int mBussAmps;
	int mBattTemp;
	int mAirTemp;
	int mBattPower;
	int mSOC;
	int mStsRegSummary;
	int mStsRegHardwareFailure;
	int mStsRegOperationalFailure;
	int mStsRegWarning;
	int mStsRegOperationalMode;
	int mSOCAmpHrs;
	int mHealthIndication;
	int mState;
	int mDeviceAddress;
	int mClearStatusRegisterFlags;
	int mRequestDelayedSelfMaintenance;
	int mSetOperationalMode;
	int mRequestImmediateSelfMaintenance;
};

#endif // BATTERY_CONTROLLER_H
