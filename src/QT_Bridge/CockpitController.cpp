#include "CockpitController.h"

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
}

QString CockpitController::readVIN() const
{
    return m_vin;
}

void CockpitController::poll()
{
    auto data = engine.getRuntime();
    auto st = engine.getState();

    bool conn = (st == VirtualCockpit::State::Connected);

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
}
