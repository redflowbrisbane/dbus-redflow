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
	Q_PROPERTY(double BussAmps READ BussAmps WRITE setBussAmps NOTIFY bussAmpsChanged)
	Q_PROPERTY(double BattVolts READ BattVolts WRITE setBattVolts NOTIFY battVoltsChanged)
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
	Q_PROPERTY(int StsRegOperationalMode READ StsRegOperationalMode WRITE setStsRegOperationalMode NOTIFY stsRegOperationalModeChanged)
	Q_PROPERTY(int StsRegSummary READ StsRegSummary WRITE setStsRegSummary NOTIFY stsRegSummaryChanged)
	Q_PROPERTY(int StsRegOperationalFailure READ StsRegOperationalFailure WRITE setStsRegOperationalFailure NOTIFY stsRegOperationalFailureChanged)
	Q_PROPERTY(int StsRegHardwareFailure READ StsRegHardwareFailure WRITE setStsRegHardwareFailure NOTIFY stsRegHardwareFailureChanged)
	Q_PROPERTY(int StsRegWarning READ StsRegWarning WRITE setStsRegWarning NOTIFY stsRegWarningChanged)
	Q_PROPERTY(int SOCAmpHrs READ SOCAmpHrs WRITE setSOCAmpHrs NOTIFY socAmpHrsChanged)
	Q_PROPERTY(int AirTemp READ AirTemp WRITE setAirTemp NOTIFY airTempChanged)
	Q_PROPERTY(int HealthIndication READ HealthIndication WRITE setHealthIndication NOTIFY healthIndicationChanged)
	Q_PROPERTY(int BussVolts READ BussVolts WRITE setBussVolts NOTIFY bussVoltsChanged)
	Q_PROPERTY(int State READ State WRITE setState NOTIFY stateChanged)
	Q_PROPERTY(int DeviceAddress READ DeviceAddress WRITE setDeviceAddress NOTIFY deviceAddressChanged)
	Q_PROPERTY(int ClearStatusRegisterFlags READ ClearStatusRegisterFlags WRITE setClearStatusRegisterFlags NOTIFY clearStatusRegisterFlagsChanged)
	Q_PROPERTY(int RequestDelayedSelfMaintenance READ RequestDelayedSelfMaintenance WRITE setRequestDelayedSelfMaintenance NOTIFY requestDelayedSelfMaintenanceChanged)
	Q_PROPERTY(int SetOperationalMode READ SetOperationalMode WRITE setSetOperationalMode NOTIFY setOperationalModeChanged)
	Q_PROPERTY(int RequestImmediateSelfMaintenance READ RequestImmediateSelfMaintenance WRITE setRequestImmediateSelfMaintenance NOTIFY requestImmediateSelfMaintenanceChanged)
	Q_PROPERTY(int EnableSelfMaintenanceAtTheEndOfDischarge READ EnableSelfMaintenanceAtTheEndOfDischarge WRITE setEnableSelfMaintenanceAtTheEndOfDischarge NOTIFY enableSelfMaintenanceAtTheEndOfDischargeChanged)
	Q_PROPERTY(int EnterRunCommand READ EnterRunCommand WRITE setEnterRunCommand NOTIFY enterRunCommandChanged)
	Q_PROPERTY(int SelfDischargeAndMaintenanceCycle READ SelfDischargeAndMaintenanceCycle WRITE setSelfDischargeAndMaintenanceCycle NOTIFY selfDischargeAndMaintenanceCycleChanged)
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
	void enableSelfMaintenanceAtTheEndOfDischargeChanged();
	void enterRunCommandChanged();
	void selfDischargeAndMaintenanceCycleChanged();
public:
	BatteryController(const QString &portName, int deviceAddress, QObject *parent = 0);

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

	double BattVolts() const;

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
	int EnableSelfMaintenanceAtTheEndOfDischarge() const;
	void setEnableSelfMaintenanceAtTheEndOfDischarge(int t);
	int EnterRunCommand() const;
	void setEnterRunCommand(int t);
	int SelfDischargeAndMaintenanceCycle() const;
	void setSelfDischargeAndMaintenanceCycle(int t);
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
	int mEnableSelfMaintenanceAtTheEndOfDischarge;
	int mEnterRunCommand;
	int mSelfDischargeAndMaintenanceCycle;	
};

#endif // BATTERY_CONTROLLER_H
