#include "virtual_cockpit/VirtualCockpit.h"
#include <cstring>
#include <thread>

using namespace std::chrono;

VirtualCockpit::VirtualCockpit() {}

VirtualCockpit::~VirtualCockpit()
{
    stop();
}

void VirtualCockpit::start()
{
    if (running) return;
    running = true;
    engineThread = std::thread(&VirtualCockpit::engineLoop, this);
}

void VirtualCockpit::stop()
{
    if (!running) return;
    pushCommand(CommandType::Stop);
    if (engineThread.joinable())
        engineThread.join();
}

VirtualCockpit::State VirtualCockpit::getState() const
{
    return state.load();
}

VirtualCockpit::RuntimeData VirtualCockpit::getRuntime() const
{
    std::lock_guard<std::mutex> lock(runtimeMutex);
    return runtime;
}

bool VirtualCockpit::configureCAN(const std::string& iface, int bitrate)
{
    if (state != State::Idle && state != State::Configured)
        return false;

    canInterface = iface;
    canBitrate = bitrate;
    state = State::Configured;
    return true;
}

bool VirtualCockpit::selectECU(const std::string& ecu)
{
    selectedECU = ecu;
    return true;
}

void VirtualCockpit::connect()
{
    pushCommand(CommandType::Connect);
}

void VirtualCockpit::disconnect()
{
    pushCommand(CommandType::Disconnect);
}

void VirtualCockpit::readDTC()
{
    pushCommand(CommandType::ReadDTC);
}

void VirtualCockpit::clearDTC()
{
    pushCommand(CommandType::ClearDTC);
}

void VirtualCockpit::setRuntimePollingEnabled(bool enabled)
{
    runtimePollingEnabled = enabled;
}

void VirtualCockpit::pushCommand(CommandType t)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    commandQueue.push({t});
}

void VirtualCockpit::setError(const std::string& msg)
{
    logger.log("ERROR: " + msg);

    {
        std::lock_guard<std::mutex> lock(runtimeMutex);
        runtime.lastError = msg;
        runtime.dtcBusy = false;
        runtime.dtcReady = false;
        runtime.dtcError = msg;
        runtime.dtcs.clear();
    }

    state = State::Error;
}

void VirtualCockpit::engineLoop()
{
    while (running)
    {
        auto startTs = steady_clock::now();
        processCommands();
        engineTick();
        std::this_thread::sleep_until(startTs + CYCLE_TIME);
    }
    closeStack();
}

void VirtualCockpit::processCommands()
{
    auto resetRuntime = [this]() {
        std::lock_guard<std::mutex> lock(runtimeMutex);
        runtime.vin.clear();
        runtime.sw.clear();
        runtime.hw.clear();
        runtime.lastError.clear();
        runtime.ecuReady = false;

        runtime.pressure1Bar = 0.0f;
        runtime.pressure2Bar = 0.0f;
        runtime.voltagePermanent = 0.0f;
        runtime.voltageIgnition = 0.0f;

        runtime.pressure1Valid = false;
        runtime.pressure2Valid = false;
        runtime.voltagePermanentValid = false;
        runtime.voltageIgnitionValid = false;

        runtime.dtcBusy = false;
        runtime.dtcReady = false;
        runtime.dtcError.clear();
        runtime.dtcs.clear();
    };

    std::queue<Command> local;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        std::swap(local, commandQueue);
    }

    while (!local.empty())
    {
        auto cmd = local.front();
        local.pop();

        if (cmd.type == CommandType::Stop)
        {
            running = false;
        }
        else if (cmd.type == CommandType::Connect &&
                 state == State::Configured)
        {
            resetRuntime();
            state = State::Connecting;

            if (openStack())
                state = State::Connected;
            else
                setError("Stack open failed");
        }
        else if (cmd.type == CommandType::Disconnect)
        {
            state = State::Configured;
            closeStack();
            resetRuntime();
        }
        else if (cmd.type == CommandType::ReadDTC || cmd.type == CommandType::ClearDTC)
        {
            bool ecuReady = false;
            {
                std::lock_guard<std::mutex> lock(runtimeMutex);
                ecuReady = runtime.ecuReady;
            }

            if (state != State::Connected || !udsCore || !ecuReady)
                continue;

            if (sacDtc && sacDtc->isBusy())
                continue;

            {
                std::lock_guard<std::mutex> lock(runtimeMutex);
                runtime.dtcBusy = true;
                runtime.dtcReady = false;
                runtime.dtcError.clear();
                runtime.dtcs.clear();
            }

            sacDtc = std::make_unique<SAC_DTC_Module>(*udsCore);

            if (cmd.type == CommandType::ReadDTC)
                sacDtc->startRead();
            else
                sacDtc->startClear();
        }
    }
}

void VirtualCockpit::engineTick()
{
    if (state == State::Connected)
    {
        if (duration_cast<milliseconds>(steady_clock::now() - lastFrameTime) > CAN_TIMEOUT)
        {
            logger.log("CAN timeout -> reconnect");
            state = State::Reconnecting;
        }
    }

    if (state == State::Reconnecting)
    {
        closeStack();
        std::this_thread::sleep_for(milliseconds(200));

        if (openStack())
        {
            reconnectAttempts = 0;
            state = State::Connected;
        }
        else
        {
            reconnectAttempts++;
            if (reconnectAttempts > 5)
                setError("Reconnect failed");
        }
        return;
    }

    if (state != State::Connected)
        return;

    Frame f;
    while (frameQueue && frameQueue->tryPop(f))
    {
        lastFrameTime = steady_clock::now();
        dispatcher->dispatch(f.id, f.data, f.len);
    }

    if (isotp)
        for (int i = 0; i < 3; i++)
            isotp->update();

    if (udsCore)
        udsCore->update();

    if (sac)
    {
        sac->update();

        if (sac->isReady())
        {
            auto vin = sac->getVIN();
            if (vin.size() == 17)
            {
                std::lock_guard<std::mutex> lock(runtimeMutex);
                runtime.vin = vin;
                runtime.sw = sac->getSW();
                runtime.hw = sac->getHW();
                runtime.ecuReady = true;
            }
            sac.reset();
        }
        else if (sac->hasError())
        {
            setError("SAC identification failed");
            sac.reset();
        }
    }

    if (sacDtc)
    {
        sacDtc->update();

        if (sacDtc->isDone())
        {
            auto dtcs = sacDtc->getDTCs();

            std::lock_guard<std::mutex> lock(runtimeMutex);
            runtime.dtcs.clear();
            runtime.dtcs.reserve(dtcs.size());

            for (const auto& d : dtcs)
            {
                DTCRecord rec;
                rec.code = d.code;
                rec.status = d.status;
                runtime.dtcs.push_back(rec);
            }

            runtime.dtcBusy = false;
            runtime.dtcReady = true;
            runtime.dtcError.clear();

            sacDtc.reset();
        }
        else if (sacDtc->hasError())
        {
            std::lock_guard<std::mutex> lock(runtimeMutex);
            runtime.dtcBusy = false;
            runtime.dtcReady = false;
            runtime.dtcError = sacDtc->getError();
            runtime.dtcs.clear();

            sacDtc.reset();
        }
    }

    if (sacRuntime)
    {
        bool ecuReady = false;
        {
            std::lock_guard<std::mutex> lock(runtimeMutex);
            ecuReady = runtime.ecuReady;
        }

        const bool dtcBusy = (sacDtc && sacDtc->isBusy());

        if (ecuReady && runtimePollingEnabled.load() && !dtcBusy)
        {
            sacRuntime->update();

            std::lock_guard<std::mutex> lock(runtimeMutex);

            if (auto v = sacRuntime->pressure1Bar())
            {
                runtime.pressure1Bar = *v;
                runtime.pressure1Valid = true;
            }

            if (auto v = sacRuntime->pressure2Bar())
            {
                runtime.pressure2Bar = *v;
                runtime.pressure2Valid = true;
            }

            if (auto v = sacRuntime->voltagePermanent())
            {
                runtime.voltagePermanent = *v;
                runtime.voltagePermanentValid = true;
            }

            if (auto v = sacRuntime->voltageIgnition())
            {
                runtime.voltageIgnition = *v;
                runtime.voltageIgnitionValid = true;
            }
        }
    }
}

bool VirtualCockpit::openStack()
{
    transport = std::make_unique<Transport_CAN_Linux>();

    if (!transport->open(canInterface, canBitrate))
        return false;

    frameQueue = std::make_unique<FrameQueue>();
    dispatcher = std::make_unique<CAN_Dispatcher>();

    uint32_t testerToEcu = 0x18DA30F9;
    uint32_t ecuToTester = 0x18DAF930;

    isotp = std::make_unique<ISOTP>(*transport, testerToEcu, ecuToTester);
    udsCore = std::make_unique<UDS_Core>(*isotp);

    dispatcher->registerHandler(isotp.get());

    if (selectedECU == "SAC")
    {
        sac = std::make_unique<SAC_Module>(*udsCore);
        sac->startIdentification();

        sacRuntime = std::make_unique<SAC_Runtime_Module>(*udsCore);
        sacRuntime->begin();
        dispatcher->registerHandler(sacRuntime.get());
    }

    lastFrameTime = steady_clock::now();
    rxThread = std::thread(&VirtualCockpit::rxLoop, this);

    return true;
}

void VirtualCockpit::closeStack()
{
    if (transport)
    {
        transport->close();
        transport.reset();
    }

    if (rxThread.joinable())
        rxThread.join();

    sacDtc.reset();
    sacRuntime.reset();
    sac.reset();
    udsCore.reset();
    isotp.reset();
    dispatcher.reset();
    frameQueue.reset();
}

void VirtualCockpit::rxLoop()
{
    while (running && state == State::Connected)
    {
        uint32_t id;
        uint8_t data[8];
        uint8_t len;

        if (transport && transport->receiveFrame(id, data, len))
        {
            Frame f{};
            f.id = id;
            f.len = len;
            std::memcpy(f.data, data, len);

            if (frameQueue)
                frameQueue->push(f);
        }
    }
}
