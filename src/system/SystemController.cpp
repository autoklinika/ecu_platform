#include "SystemController.h"
#include <QProcess>

SystemController::SystemController(QObject* parent)
    : QObject(parent)
{
}

bool SystemController::runCommand(const QString& program,
                                  const QStringList& args,
                                  QString* output)
{
    QProcess process;
    process.start(program, args);
    process.waitForFinished();

    if (output)
        *output = QString::fromUtf8(process.readAllStandardOutput());

    return process.exitStatus() == QProcess::NormalExit &&
           process.exitCode() == 0;
}

// =========================
// CAN
// =========================

bool SystemController::configureCAN(const QString& iface, int bitrate)
{
    if (iface.isEmpty() || bitrate <= 0)
        return false;

    bool ok = true;

    ok &= runCommand("sudo",
                     {"ip", "link", "set", iface, "down"});

    ok &= runCommand("sudo",
                     {"ip", "link", "set", iface,
                      "type", "can",
                      "bitrate", QString::number(bitrate)});

    ok &= runCommand("sudo",
                     {"ip", "link", "set", iface, "up"});

    return ok;
}

// =========================
// POWER
// =========================

bool SystemController::shutdown()
{
    return runCommand("sudo", {"shutdown", "-h", "now"});
}

bool SystemController::reboot()
{
    return runCommand("sudo", {"reboot"});
}

// =========================
// WIFI BASIC
// =========================

bool SystemController::setWifiEnabled(bool enabled)
{
    if (enabled)
        return runCommand("sudo", {"nmcli", "radio", "wifi", "on"});
    else
        return runCommand("sudo", {"nmcli", "radio", "wifi", "off"});
}

QString SystemController::wifiStatus()
{
    QString output;
    runCommand("nmcli",
               {"-t", "-f", "WIFI", "general"},
               &output);

    return output.trimmed().toLower(); // enabled / disabled
}

// =========================
// WIFI ADVANCED
// =========================

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
                      {"nmcli", "networking", "off"});
}