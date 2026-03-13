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

    Q_PROPERTY(QStringList dtcList READ dtcList NOTIFY dtcListChanged)
    Q_PROPERTY(bool dtcBusy READ dtcBusy NOTIFY dtcBusyChanged)
    Q_PROPERTY(bool dtcReady READ dtcReady NOTIFY dtcReadyChanged)
    Q_PROPERTY(QString dtcError READ dtcError NOTIFY dtcErrorChanged)

public:
    explicit CockpitController(QObject* parent=nullptr);

    Q_INVOKABLE bool start(QString iface, int bitrate);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE QString readVIN() const;
    Q_INVOKABLE void startDTCRead();
    Q_INVOKABLE void clearDTCData();

    QString vin() const { return m_vin; }
    QString sw() const { return m_sw; }
    QString hw() const { return m_hw; }
    QString error() const { return m_error; }
    bool connected() const { return m_connected; }
    bool ecuReady() const { return m_ecuReady; }

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

    QStringList m_dtcList;
    bool m_dtcBusy = false;
    bool m_dtcReady = false;
    QString m_dtcError;
};
