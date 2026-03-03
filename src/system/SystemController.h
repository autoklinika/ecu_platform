#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class SystemController : public QObject
{
    Q_OBJECT

public:
    explicit SystemController(QObject* parent = nullptr);

    // CAN
    Q_INVOKABLE bool configureCAN(const QString& iface, int bitrate);

    // Power
    Q_INVOKABLE bool shutdown();
    Q_INVOKABLE bool reboot();

    // WiFi basic
    Q_INVOKABLE bool setWifiEnabled(bool enabled);
    Q_INVOKABLE QString wifiStatus();

    // WiFi advanced
    Q_INVOKABLE QStringList wifiScan();
    Q_INVOKABLE bool wifiConnect(const QString& ssid,
                                 const QString& password);
    Q_INVOKABLE bool wifiDisconnect();

private:
    bool runCommand(const QString& program,
                    const QStringList& args,
                    QString* output = nullptr);
};