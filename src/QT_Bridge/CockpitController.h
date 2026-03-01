#pragma once
#include <QObject>
#include <QTimer>
#include "virtual_cockpit/VirtualCockpit.h"

class CockpitController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString vin READ vin NOTIFY vinChanged)
    Q_PROPERTY(QString sw READ sw NOTIFY swChanged)
    Q_PROPERTY(QString hw READ hw NOTIFY hwChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

public:
    explicit CockpitController(QObject* parent=nullptr);

    Q_INVOKABLE void start(QString iface, int bitrate);
    Q_INVOKABLE void disconnect();

    QString vin() const { return m_vin; }
    QString sw() const { return m_sw; }
    QString hw() const { return m_hw; }
    QString error() const { return m_error; }
    bool connected() const { return m_connected; }

signals:
    void vinChanged();
    void swChanged();
    void hwChanged();
    void errorChanged();
    void connectedChanged();

private:
    void poll();

    VirtualCockpit engine;
    QTimer pollTimer;

    QString m_vin;
    QString m_sw;
    QString m_hw;
    QString m_error;
    bool m_connected=false;
};