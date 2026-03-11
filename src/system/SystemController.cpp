#include "SystemController.h"

#include <QProcess>
#include <QNetworkInterface>
#include <QAbstractSocket>
#include <QThread>

namespace {

bool isWirelessInterfaceName(const QString& name)
{
    return name.startsWith("wl") || name.startsWith("wlan");
}

constexpr unsigned long CAN_SWITCH_DELAY_MS = 120;

}

// =====================================================
// Konstruktor
// =====================================================

SystemController::SystemController(QObject* parent)
    : QObject(parent)
{
}

// =====================================================
// Helper
// =====================================================

bool SystemController::runCommand(const QString& program,
                                  const QStringList& args,
                                  QString* output)
{
    QProcess process;
    process.start(program, args);

    if (!process.waitForFinished(5000))
        return false;

    if (output)
        *output = QString::fromUtf8(process.readAllStandardOutput());

    return process.exitStatus() == QProcess::NormalExit &&
           process.exitCode() == 0;
}

// =====================================================
// CAN
// =====================================================

bool SystemController::configureCAN(const QString& iface, int bitrate)
{
    if (iface.isEmpty() || bitrate <= 0)
        return false;

    bool ok = true;

    ok &= runCommand("sudo",
                     {"ip", "link", "set", iface, "down"});

    QThread::msleep(CAN_SWITCH_DELAY_MS);

    ok &= runCommand("sudo",
                     {"ip", "link", "set", iface,
                      "type", "can",
                      "bitrate", QString::number(bitrate)});

    QThread::msleep(CAN_SWITCH_DELAY_MS);

    ok &= runCommand("sudo",
                     {"ip", "link", "set", iface, "up"});

    return ok;
}

bool SystemController::resetCAN(const QString& iface)
{
    if (iface.isEmpty())
        return false;

    return runCommand("sudo",
                      {"ip", "link", "set", iface, "down"});
}

// =====================================================
// POWER
// =====================================================

bool SystemController::shutdown()
{
    return runCommand("sudo", {"shutdown", "-h", "now"});
}

bool SystemController::reboot()
{
    return runCommand("sudo", {"reboot"});
}

// =====================================================
// WIFI BASIC (sterowanie)
// =====================================================

bool SystemController::setWifiEnabled(bool enabled)
{
    return runCommand("sudo",
                      {"nmcli", "radio", "wifi",
                       enabled ? "on" : "off"});
}

// =====================================================
// WIFI STATUS (stabilne, bez nmcli)
// =====================================================

QString SystemController::wifiStatus()
{
    const QList<QNetworkInterface> interfaces =
        QNetworkInterface::allInterfaces();

    bool wifiDetected = false;

    for (const QNetworkInterface &iface : interfaces)
    {
        if (isWirelessInterfaceName(iface.name()))
        {
            wifiDetected = true;

            bool isUp =
                iface.flags().testFlag(QNetworkInterface::IsUp) &&
                iface.flags().testFlag(QNetworkInterface::IsRunning);

            if (!isUp)
                return "disconnected";

            const auto entries = iface.addressEntries();

            for (const auto &entry : entries)
            {
                if (entry.ip().protocol() ==
                    QAbstractSocket::IPv4Protocol)
                {
                    return "connected";
                }
            }

            // interfejs działa, ale brak IP
            return "connecting";
        }
    }

    return wifiDetected ? "disconnected" : "unavailable";
}

bool SystemController::isWifiConnected()
{
    return wifiStatus() == "connected";
}

// =====================================================
// WIFI ADVANCED
// =====================================================

QStringList SystemController::wifiScan()
{
    QString output;

    runCommand("nmcli",
               {"-t", "-f", "SSID", "dev", "wifi", "list"},
               &output);

    QStringList list =
        output.split("\n", Qt::SkipEmptyParts);

    list.removeDuplicates();
    return list;
}

bool SystemController::wifiConnect(const QString& ssid,
                                   const QString& password)
{
    if (ssid.isEmpty())
        return false;

    if (password.isEmpty())
    {
        return runCommand("sudo",
                          {"nmcli",
                           "dev", "wifi", "connect",
                           ssid});
    }
    else
    {
        return runCommand("sudo",
                          {"nmcli",
                           "dev", "wifi", "connect",
                           ssid,
                           "password",
                           password});
    }
}

bool SystemController::wifiDisconnect()
{
    return runCommand("sudo",
                      {"nmcli", "radio", "wifi", "off"});
}
