#include "CockpitController.h"

namespace
{
QString formatDTCCode(uint32_t code)
{
    return QString("%1")
        .arg(static_cast<unsigned int>(code), 6, 16, QChar('0'))
        .toUpper();
}

QString decodeDTCStatus(uint8_t status)
{
    const bool active = status & 0x01;
    const bool pending = status & 0x04;
    const bool confirmed = status & 0x08;
    const bool mil = status & 0x80;

    QString s;
    if (active)
        s = "ACTIVE";
    else if (pending)
        s = "PENDING";
    else if (confirmed)
        s = "STORED";
    else
        s = "INACTIVE";

    if (mil)
        s += " MIL";

    return s;
}

QString formatDTCLine(const VirtualCockpit::DTCRecord& rec)
{
    return QString("0x%1    %2")
        .arg(formatDTCCode(rec.code))
        .arg(decodeDTCStatus(rec.status));
}
}

CockpitController::CockpitController(QObject* parent)
    : QObject(parent)
{
    engine.start();

    connect(&pollTimer, &QTimer::timeout,
            this, &CockpitController::poll);

    pollTimer.start(50);
}

bool CockpitController::start(QString iface, int bitrate)
{
    m_connected = false;
    emit connectedChanged();

    m_ecuReady = false;
    emit ecuReadyChanged();

    m_error.clear();
    emit errorChanged();

    m_vin.clear();
    m_sw.clear();
    m_hw.clear();
    emit vinChanged();
    emit swChanged();
    emit hwChanged();

    clearDTCData();

    engine.stop();
    engine.start();

    if(!engine.configureCAN(iface.toStdString(), bitrate))
        return false;

    engine.selectECU("SAC");
    engine.connect();

    return true;
}

void CockpitController::disconnect()
{
    engine.disconnect();

    if (m_connected != false)
    {
        m_connected = false;
        emit connectedChanged();
    }

    m_ecuReady = false;
    emit ecuReadyChanged();

    m_error.clear();
    emit errorChanged();

    m_vin.clear();
    m_sw.clear();
    m_hw.clear();
    emit vinChanged();
    emit swChanged();
    emit hwChanged();

    clearDTCData();
}

QString CockpitController::readVIN() const
{
    return m_vin;
}

void CockpitController::startDTCRead()
{
    clearDTCData();
    engine.readDTC();
}

void CockpitController::clearDTCData()
{
    if (!m_dtcList.isEmpty())
    {
        m_dtcList.clear();
        emit dtcListChanged();
    }

    if (m_dtcBusy != false)
    {
        m_dtcBusy = false;
        emit dtcBusyChanged();
    }

    if (m_dtcReady != false)
    {
        m_dtcReady = false;
        emit dtcReadyChanged();
    }

    if (!m_dtcError.isEmpty())
    {
        m_dtcError.clear();
        emit dtcErrorChanged();
    }
}

void CockpitController::poll()
{
    auto data = engine.getRuntime();
    auto st = engine.getState();

    bool conn = (st == VirtualCockpit::State::Connected) && data.ecuReady;

    if(conn != m_connected)
    {
        m_connected = conn;
        emit connectedChanged();
    }

    auto vin = QString::fromStdString(data.vin);
    if(vin != m_vin)
    {
        m_vin = vin;
        emit vinChanged();
    }

    auto sw = QString::fromStdString(data.sw);
    if(sw != m_sw)
    {
        m_sw = sw;
        emit swChanged();
    }

    auto hw = QString::fromStdString(data.hw);
    if(hw != m_hw)
    {
        m_hw = hw;
        emit hwChanged();
    }

    auto err = QString::fromStdString(data.lastError);
    if(err != m_error)
    {
        m_error = err;
        emit errorChanged();
    }

    if(data.ecuReady != m_ecuReady)
    {
        m_ecuReady = data.ecuReady;
        emit ecuReadyChanged();
    }

    QStringList newDtcList;
    newDtcList.reserve(static_cast<int>(data.dtcs.size()));

    for (const auto& rec : data.dtcs)
        newDtcList.push_back(formatDTCLine(rec));

    if (newDtcList != m_dtcList)
    {
        m_dtcList = newDtcList;
        emit dtcListChanged();
    }

    if (data.dtcBusy != m_dtcBusy)
    {
        m_dtcBusy = data.dtcBusy;
        emit dtcBusyChanged();
    }

    if (data.dtcReady != m_dtcReady)
    {
        m_dtcReady = data.dtcReady;
        emit dtcReadyChanged();
    }

    QString newDtcError = QString::fromStdString(data.dtcError);
    if (newDtcError != m_dtcError)
    {
        m_dtcError = newDtcError;
        emit dtcErrorChanged();
    }
}
