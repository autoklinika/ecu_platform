#pragma once
#include <QObject>
#include <QTimer>
#include <QStringList>
#include "virtual_cockpit/VirtualCockpit.h"

class CockpitController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString vin READ vin NOTIFY vinChanged)
    Q_PROPERTY(QString sw READ sw NOTIFY swChanged)
    Q_PROPERTY(QString hw READ hw NOTIFY hwChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool ecuReady READ ecuReady NOTIFY ecuReadyChanged)

    Q_PROPERTY(double pressure1Bar READ pressure1Bar NOTIFY pressure1BarChanged)
    Q_PROPERTY(double pressure2Bar READ pressure2Bar NOTIFY pressure2BarChanged)
    Q_PROPERTY(double voltagePermanent READ voltagePermanent NOTIFY voltagePermanentChanged)
    Q_PROPERTY(double voltageIgnition READ voltageIgnition NOTIFY voltageIgnitionChanged)

    Q_PROPERTY(bool pressure1Valid READ pressure1Valid NOTIFY pressure1ValidChanged)
    Q_PROPERTY(bool pressure2Valid READ pressure2Valid NOTIFY pressure2ValidChanged)
    Q_PROPERTY(bool voltagePermanentValid READ voltagePermanentValid NOTIFY voltagePermanentValidChanged)
    Q_PROPERTY(bool voltageIgnitionValid READ voltageIgnitionValid NOTIFY voltageIgnitionValidChanged)

    Q_PROPERTY(QStringList dtcList READ dtcList NOTIFY dtcListChanged)
    Q_PROPERTY(bool dtcBusy READ dtcBusy NOTIFY dtcBusyChanged)
    Q_PROPERTY(bool dtcReady READ dtcReady NOTIFY dtcReadyChanged)
    Q_PROPERTY(QString dtcError READ dtcError NOTIFY dtcErrorChanged)

public:
    explicit CockpitController(QObject* parent=nullptr);

    Q_INVOKABLE bool start(QString iface, int bitrate);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE QString readVIN() const;
    Q_INVOKABLE void setRuntimePollingEnabled(bool enabled);
    Q_INVOKABLE void startDTCRead();

    QString vin() const { return m_vin; }
    QString sw() const { return m_sw; }
    QString hw() const { return m_hw; }
    QString error() const { return m_error; }
    bool connected() const { return m_connected; }
    bool ecuReady() const { return m_ecuReady; }

    double pressure1Bar() const { return m_pressure1Bar; }
    double pressure2Bar() const { return m_pressure2Bar; }
    double voltagePermanent() const { return m_voltagePermanent; }
    double voltageIgnition() const { return m_voltageIgnition; }

    bool pressure1Valid() const { return m_pressure1Valid; }
    bool pressure2Valid() const { return m_pressure2Valid; }
    bool voltagePermanentValid() const { return m_voltagePermanentValid; }
    bool voltageIgnitionValid() const { return m_voltageIgnitionValid; }

    QStringList dtcList() const { return m_dtcList; }
    bool dtcBusy() const { return m_dtcBusy; }
    bool dtcReady() const { return m_dtcReady; }
    QString dtcError() const { return m_dtcError; }

signals:
    void vinChanged();
    void swChanged();
    void hwChanged();
    void errorChanged();
    void connectedChanged();
    void ecuReadyChanged();

    void pressure1BarChanged();
    void pressure2BarChanged();
    void voltagePermanentChanged();
    void voltageIgnitionChanged();

    void pressure1ValidChanged();
    void pressure2ValidChanged();
    void voltagePermanentValidChanged();
    void voltageIgnitionValidChanged();

    void dtcListChanged();
    void dtcBusyChanged();
    void dtcReadyChanged();
    void dtcErrorChanged();

private:
    void poll();

    VirtualCockpit engine;
    QTimer pollTimer;

    QString m_vin;
    QString m_sw;
    QString m_hw;
    QString m_error;
    bool m_connected = false;
    bool m_ecuReady = false;

    double m_pressure1Bar = 0.0;
    double m_pressure2Bar = 0.0;
    double m_voltagePermanent = 0.0;
    double m_voltageIgnition = 0.0;

    bool m_pressure1Valid = false;
    bool m_pressure2Valid = false;
    bool m_voltagePermanentValid = false;
    bool m_voltageIgnitionValid = false;

    QStringList m_dtcList;
    bool m_dtcBusy = false;
    bool m_dtcReady = false;
    QString m_dtcError;
};
