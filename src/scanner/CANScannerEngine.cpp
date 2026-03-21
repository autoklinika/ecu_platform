#include "CANScannerEngine.h"

#include <QDateTime>
#include <QElapsedTimer>
#include <QProcess>
#include <QThread>
#include <QStringList>
#include <QSet>

#include <algorithm>
#include <cstring>

#include <unistd.h>
#include <poll.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

namespace {
constexpr int kRecoverDelayMs = 150;
constexpr int kCanSwitchDelayMs = 120;
}

ScannerWorker::ScannerWorker(QObject* parent)
    : QObject(parent)
{
}

void ScannerWorker::requestStop()
{
    m_stop = true;
}

bool ScannerWorker::configureCAN(const QString& iface, int bitrate)
{
    auto run = [](const QString& program, const QStringList& args) -> bool {
        QProcess p;
        p.start(program, args);
        if (!p.waitForFinished(5000))
            return false;
        return p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0;
    };

    bool ok = true;
    ok &= run("sudo", {"ip", "link", "set", iface, "down"});
    QThread::msleep(kCanSwitchDelayMs);
    ok &= run("sudo", {"ip", "link", "set", iface, "type", "can", "bitrate", QString::number(bitrate)});
    QThread::msleep(kCanSwitchDelayMs);
    ok &= run("sudo", {"ip", "link", "set", iface, "up"});
    return ok;
}

bool ScannerWorker::resetCAN(const QString& iface)
{
    QProcess p;
    p.start("sudo", {"ip", "link", "set", iface, "down"});
    if (!p.waitForFinished(5000))
        return false;
    return p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0;
}

bool ScannerWorker::openSocket(const QString& iface, int& fd)
{
    fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0)
        return false;

    struct ifreq ifr {};
    std::strncpy(ifr.ifr_name, iface.toStdString().c_str(), IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
        ::close(fd);
        fd = -1;
        return false;
    }

    int enable = 1;
    setsockopt(fd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &enable, sizeof(enable));

    struct sockaddr_can addr {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(fd);
        fd = -1;
        return false;
    }

    return true;
}

void ScannerWorker::closeSocket(int& fd)
{
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}

bool ScannerWorker::sendFrame(int fd, uint32_t id, const uint8_t* data, uint8_t len)
{
    struct can_frame frame {};
    frame.can_id = id | CAN_EFF_FLAG;
    frame.can_dlc = len;
    std::memcpy(frame.data, data, len);
    return ::write(fd, &frame, sizeof(frame)) == sizeof(frame);
}

bool ScannerWorker::receiveFrameTimeout(int fd, uint32_t& canId, uint8_t* data, uint8_t& len, int timeoutMs)
{
    struct pollfd pfd {};
    pfd.fd = fd;
    pfd.events = POLLIN;

    const int pr = ::poll(&pfd, 1, timeoutMs);
    if (pr <= 0)
        return false;

    struct can_frame frame {};
    const int n = ::read(fd, &frame, sizeof(frame));
    if (n <= 0)
        return false;

    canId = frame.can_id & CAN_EFF_MASK;
    len = frame.can_dlc;
    std::memcpy(data, frame.data, len);
    return true;
}

int ScannerWorker::drainSocket(int fd, int drainMs, QStringList* firstIds)
{
    const qint64 start = QDateTime::currentMSecsSinceEpoch();
    int count = 0;

    while (QDateTime::currentMSecsSinceEpoch() - start < drainMs) {
        uint32_t id = 0;
        uint8_t data[8] = {};
        uint8_t len = 0;

        if (!receiveFrameTimeout(fd, id, data, len, 5))
            continue;

        ++count;
        if (firstIds && firstIds->size() < 5)
            firstIds->append(QString("0x%1").arg(id, 8, 16, QChar('0')).toUpper());
    }

    return count;
}

uint32_t ScannerWorker::buildDiagRequestId(int ecuAddr, int testerAddr) const
{
    return 0x18DA0000u | ((ecuAddr & 0xFF) << 8) | (testerAddr & 0xFF);
}

bool ScannerWorker::isExpectedDiagResponse(uint32_t canId, int ecuAddr, int testerAddr) const
{
    const uint32_t expected = 0x18DA0000u | ((testerAddr & 0xFF) << 8) | (ecuAddr & 0xFF);
    return canId == expected;
}

QString ScannerWorker::formatData(const uint8_t* data, int len) const
{
    QStringList parts;
    for (int i = 0; i < len; ++i)
        parts << QString("%1").arg(data[i], 2, 16, QChar('0')).toUpper();
    return parts.join(' ');
}

QString ScannerWorker::detectResponseType(const QByteArray& data) const
{
    if (data.size() >= 4 && static_cast<uint8_t>(data[1]) == 0x7F)
        return QString("NRC 0x%1").arg(static_cast<uint8_t>(data[3]), 2, 16, QChar('0')).toUpper();
    if (data.size() >= 2)
        return QString("SID 0x%1").arg(static_cast<uint8_t>(data[1]), 2, 16, QChar('0')).toUpper();
    return "Odpowiedź";
}

QString ScannerWorker::servicePayloadNameToBytes(const QString& serviceName, QByteArray& out) const
{
    if (serviceName.contains("TesterPresent")) {
        out = QByteArray::fromHex("023E000000000000");
        return "TesterPresent (3E 00)";
    }
    if (serviceName.contains("DiagnosticSessionControl")) {
        out = QByteArray::fromHex("0210010000000000");
        return "DiagnosticSessionControl (10 01)";
    }

    out = QByteArray::fromHex("0322F19000000000");
    return "ReadDataByIdentifier VIN (22 F1 90)";
}

QString ScannerWorker::detectBitrateByListen(const QString& iface, int listenMs, bool extendedOnly)
{
    const QList<QPair<QString, int>> bitrates = {
        {"125k", 125000},
        {"250k", 250000},
        {"500k", 500000},
        {"1M", 1000000}
    };

    QString bestBitrate;
    int bestScore = -1;

    emit progress(0, bitrates.size());
    emit log("TRYB: listen_only");

    for (int i = 0; i < bitrates.size() && !m_stop; ++i) {
        emit progress(i + 1, bitrates.size());

        const auto& pair = bitrates[i];
        const QString label = pair.first;
        const int bitrate = pair.second;

        emit log(QString("LISTEN [%1/%2] bitrate=%3").arg(i + 1).arg(bitrates.size()).arg(label));

        if (!configureCAN(iface, bitrate)) {
            emit log("CONFIG ERROR");
            continue;
        }

        int fd = -1;
        if (!openSocket(iface, fd)) {
            emit log("SOCKET ERROR");
            resetCAN(iface);
            continue;
        }

        int score = 0;
        int frames = 0;
        int extCount = 0;

        const qint64 start = QDateTime::currentMSecsSinceEpoch();
        while (QDateTime::currentMSecsSinceEpoch() - start < listenMs && !m_stop) {
            uint32_t id = 0;
            uint8_t data[8] = {};
            uint8_t len = 0;

            if (!receiveFrameTimeout(fd, id, data, len, 20))
                continue;

            const bool isExt = id > 0x7FF;
            if (extendedOnly && !isExt)
                continue;

            ++frames;
            if (isExt) {
                ++extCount;
                score += 8;
            } else {
                score += 1;
            }
        }

        closeSocket(fd);
        resetCAN(iface);

        emit log(QString("LISTEN RESULT bitrate=%1: frames=%2 ext=%3 score=%4")
                 .arg(label).arg(frames).arg(extCount).arg(score));

        if (score > bestScore) {
            bestScore = score;
            bestBitrate = label;
        }

        QThread::msleep(kRecoverDelayMs);
    }

    if (bestScore <= 0)
        return QString();

    return bestBitrate;
}

QString ScannerWorker::detectBitrateByProbe(const QString& iface, int testerSa)
{
    const QList<QPair<QString, int>> bitrates = {
        {"125k", 125000},
        {"250k", 250000},
        {"500k", 500000},
        {"1M", 1000000}
    };

    emit progress(0, bitrates.size());
    emit log("TRYB: probe_only");

    for (int i = 0; i < bitrates.size() && !m_stop; ++i) {
        emit progress(i + 1, bitrates.size());

        const auto& pair = bitrates[i];
        const QString label = pair.first;
        const int bitrate = pair.second;

        emit log(QString("PROBE [%1/%2] bitrate=%3").arg(i + 1).arg(bitrates.size()).arg(label));

        if (!configureCAN(iface, bitrate))
            continue;

        int fd = -1;
        if (!openSocket(iface, fd)) {
            resetCAN(iface);
            continue;
        }

        const uint8_t payload[8] = {0x02, 0x3E, 0x00, 0, 0, 0, 0, 0};
        const QList<int> addrs = {0x00, 0x17, 0x33};

        bool ok = false;

        for (int addr : addrs) {
            if (m_stop)
                break;

            const uint32_t reqId = buildDiagRequestId(addr, testerSa);
            const uint32_t expId = 0x18DA0000u | ((testerSa & 0xFF) << 8) | (addr & 0xFF);

            emit log(QString("TX PROBE: dest=0x%1 req_id=0x%2 expected=0x%3")
                     .arg(addr, 2, 16, QChar('0')).toUpper()
                     .arg(reqId, 8, 16, QChar('0')).toUpper()
                     .arg(expId, 8, 16, QChar('0')).toUpper());

            sendFrame(fd, reqId, payload, 8);

            const qint64 start = QDateTime::currentMSecsSinceEpoch();
            while (QDateTime::currentMSecsSinceEpoch() - start < 200 && !m_stop) {
                uint32_t rid = 0;
                uint8_t data[8] = {};
                uint8_t len = 0;

                if (!receiveFrameTimeout(fd, rid, data, len, 20))
                    continue;

                emit log(QString("RX PROBE: ID=0x%1 DATA=%2")
                         .arg(rid, 8, 16, QChar('0')).toUpper()
                         .arg(formatData(data, len)));

                if (rid == expId) {
                    emit log(QString("RX PROBE MATCH: ID=0x%1 DATA=%2")
                             .arg(rid, 8, 16, QChar('0')).toUpper()
                             .arg(formatData(data, len)));
                    ok = true;
                    break;
                }
            }

            if (ok)
                break;
        }

        closeSocket(fd);
        resetCAN(iface);
        QThread::msleep(kRecoverDelayMs);

        if (ok)
            return label;
    }

    return QString();
}

void ScannerWorker::runBitrateScan(const QString& iface,
                                   const QString& detectMode,
                                   int testerSa,
                                   int listenMs,
                                   bool extendedOnly)
{
    m_stop = false;
    emit status("Skanowanie bitrate...");

    QElapsedTimer timer;
    timer.start();

    QString detected;

    if (detectMode == "listen_only") {
        detected = detectBitrateByListen(iface, listenMs, extendedOnly);
    } else if (detectMode == "probe_only") {
        detected = detectBitrateByProbe(iface, testerSa);
    } else {
        detected = detectBitrateByListen(iface, listenMs, extendedOnly);
        if (detected.isEmpty() && !m_stop) {
            emit log("AUTO: brak ramek na nasłuchu, przejście do aktywnego sondowania");
            detected = detectBitrateByProbe(iface, testerSa);
        }
    }

    const bool ok = !detected.isEmpty();

    if (ok) {
        emit detectedBitrate(detected);
        emit status("Wykryto bitrate");
    } else {
        emit status("Nie wykryto bitrate");
    }

    QString summary;
    summary += "========================================================================\n";
    summary += "RAPORT KOŃCOWY SKANU BITRATE\n";
    summary += "========================================================================\n";
    summary += QString("Wynik końcowy          : %1\n").arg(ok ? "POZYTYWNY" : "NEGATYWNY");
    if (!detected.isEmpty())
        summary += QString("Wykryty bitrate        : %1\n").arg(detected);
    summary += QString("Czas operacji          : %1 ms\n").arg(timer.elapsed());
    summary += "========================================================================\n";

    emit log(summary);
    emit finished(ok, summary);
}

void ScannerWorker::runECUScan(const QString& iface,
                               int bitrate,
                               const QString& testerMode,
                               int testerSa,
                               int testerSaFrom,
                               int testerSaTo,
                               int ecuFrom,
                               int ecuTo,
                               int timeoutMs,
                               const QString& serviceName,
                               bool stopOnFirst,
                               bool debugRx)
{
    m_stop = false;
    emit status("Skanowanie ECU...");

    QElapsedTimer timer;
    timer.start();

    QByteArray payload;
    const QString serviceLabel = servicePayloadNameToBytes(serviceName, payload);

    if (!configureCAN(iface, bitrate)) {
        emit log("Błąd konfiguracji CAN");
        emit status("Błąd");
        emit finished(false, "Błąd konfiguracji CAN");
        return;
    }

    int fd = -1;
    if (!openSocket(iface, fd)) {
        emit log("Błąd otwarcia gniazda CAN");
        resetCAN(iface);
        emit status("Błąd");
        emit finished(false, "Błąd otwarcia gniazda CAN");
        return;
    }

    QList<int> testerCandidates;
    if (testerMode == "auto") {
        if (testerSaFrom > testerSaTo)
            std::swap(testerSaFrom, testerSaTo);
        for (int v = testerSaFrom; v <= testerSaTo; ++v)
            testerCandidates << v;
    } else {
        testerCandidates << testerSa;
    }

    if (ecuFrom > ecuTo)
        std::swap(ecuFrom, ecuTo);

    const int total = (ecuTo - ecuFrom + 1) * testerCandidates.size();
    emit progress(0, total);

    emit log(QString("Skan usługi: %1").arg(serviceLabel));
    emit log(QString("Tester SA mode: %1").arg(testerMode));
    emit log(QString("Bitrate: %1").arg(bitrate));
    emit detectedBitrate(QString::number(bitrate) + " bit/s");

    bool anyFound = false;
    bool firstFoundCaptured = false;
    QString firstFoundEcu;
    QString firstFoundTester;
    int step = 0;

    for (int ecu = ecuFrom; ecu <= ecuTo && !m_stop; ++ecu) {
        bool matchedForEcu = false;

        for (int tester : testerCandidates) {
            if (m_stop)
                break;

            ++step;
            emit progress(step, total);

            emit log(QString("SCAN TRY: ECU=0x%1 TESTER_SA=0x%2")
                     .arg(ecu, 2, 16, QChar('0')).toUpper()
                     .arg(tester, 2, 16, QChar('0')).toUpper());

            QStringList ids;
            const int drained = drainSocket(fd, 80, &ids);

            emit log(QString("RX flush before TX ECU=0x%1: drained=%2, first_ids=%3")
                     .arg(ecu, 2, 16, QChar('0')).toUpper()
                     .arg(drained)
                     .arg(ids.isEmpty() ? "-" : ids.join(", ")));

            const uint32_t reqId = buildDiagRequestId(ecu, tester);

            if (!sendFrame(fd, reqId,
                           reinterpret_cast<const uint8_t*>(payload.constData()),
                           static_cast<uint8_t>(payload.size()))) {
                QVariantMap r;
                r["ecu"] = QString("0x%1").arg(ecu, 2, 16, QChar('0')).toUpper();
                r["tester"] = QString("0x%1").arg(tester, 2, 16, QChar('0')).toUpper();
                r["reqId"] = QString("0x%1").arg(reqId, 8, 16, QChar('0')).toUpper();
                r["respId"] = "-";
                r["data"] = "-";
                r["status"] = "TX fail";
                emit addResult(r);
                continue;
            }

            emit log(QString("TX  ID=0x%1 DATA=%2")
                     .arg(reqId, 8, 16, QChar('0')).toUpper()
                     .arg(formatData(reinterpret_cast<const uint8_t*>(payload.constData()), payload.size())));

            bool matched = false;
            const qint64 start = QDateTime::currentMSecsSinceEpoch();

            while (QDateTime::currentMSecsSinceEpoch() - start < timeoutMs && !m_stop) {
                uint32_t rid = 0;
                uint8_t data[8] = {};
                uint8_t len = 0;

                if (!receiveFrameTimeout(fd, rid, data, len, 20))
                    continue;

                if (debugRx) {
                    emit log(QString("RX DEBUG ECU=0x%1: ID=0x%2 DATA=%3")
                             .arg(ecu, 2, 16, QChar('0')).toUpper()
                             .arg(rid, 8, 16, QChar('0')).toUpper()
                             .arg(formatData(data, len)));
                }

                if (isExpectedDiagResponse(rid, ecu, tester)) {
                    const QString ecuHex = QString("0x%1").arg(ecu, 2, 16, QChar('0')).toUpper();
                    const QString testerHex = QString("0x%1").arg(tester, 2, 16, QChar('0')).toUpper();

                    emit log(QString("RX MATCH ECU=%1: ID=0x%2 DATA=%3")
                             .arg(ecuHex)
                             .arg(rid, 8, 16, QChar('0')).toUpper()
                             .arg(formatData(data, len)));

                    if (!firstFoundCaptured) {
                        firstFoundCaptured = true;
                        firstFoundEcu = ecuHex;
                        firstFoundTester = testerHex;
                        emit detectedEcu(firstFoundEcu);
                    }

                    QVariantMap r;
                    r["ecu"] = ecuHex;
                    r["tester"] = testerHex;
                    r["reqId"] = QString("0x%1").arg(reqId, 8, 16, QChar('0')).toUpper();
                    r["respId"] = QString("0x%1").arg(rid, 8, 16, QChar('0')).toUpper();
                    r["data"] = formatData(data, len);
                    r["status"] = detectResponseType(QByteArray(reinterpret_cast<const char*>(data), len))
                                  + QString(" | tester=%1").arg(testerHex);
                    emit addResult(r);

                    matched = true;
                    matchedForEcu = true;
                    anyFound = true;
                    break;
                }
            }

            if (!matched) {
                emit log(QString("RX DEBUG SUMMARY ECU=0x%1: no match, timeout_ms=%2")
                         .arg(ecu, 2, 16, QChar('0')).toUpper()
                         .arg(timeoutMs));
            }

            if (matchedForEcu)
                break;
        }

        if (matchedForEcu && stopOnFirst) {
            emit log("Zatrzymano po pierwszej odpowiedzi ECU.");
            break;
        }
    }

    closeSocket(fd);
    resetCAN(iface);

    QString summary;
    summary += "========================================================================\n";
    summary += "RAPORT KOŃCOWY SKANU ECU\n";
    summary += "========================================================================\n";
    summary += QString("Wynik końcowy          : %1\n").arg(anyFound ? "POZYTYWNY" : "NEGATYWNY");
    summary += QString("Bitrate                : %1 bit/s\n").arg(bitrate);
    if (firstFoundCaptured) {
        summary += QString("Wykryty adres ECU      : %1\n").arg(firstFoundEcu);
        summary += QString("Tester SA              : %1\n").arg(firstFoundTester);
    }
    summary += QString("Czas operacji          : %1 ms\n").arg(timer.elapsed());
    summary += "========================================================================\n";

    emit log(summary);
    emit status(anyFound ? "Skanowanie zakończone" : "Brak odpowiedzi ECU");
    emit finished(anyFound, summary);
}

CANScannerEngine::CANScannerEngine(QObject* parent)
    : QObject(parent)
{
    m_worker = new ScannerWorker;
    m_worker->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    connect(this, &CANScannerEngine::startBitrateScanRequested,
            m_worker, &ScannerWorker::runBitrateScan);
    connect(this, &CANScannerEngine::startECUScanRequested,
            m_worker, &ScannerWorker::runECUScan);
    connect(this, &CANScannerEngine::stopRequested,
            m_worker, &ScannerWorker::requestStop);

    connect(m_worker, &ScannerWorker::log, this, &CANScannerEngine::onWorkerLog);
    connect(m_worker, &ScannerWorker::status, this, &CANScannerEngine::onWorkerStatus);
    connect(m_worker, &ScannerWorker::progress, this, &CANScannerEngine::onWorkerProgress);
    connect(m_worker, &ScannerWorker::detectedBitrate, this, &CANScannerEngine::onWorkerDetectedBitrate);
    connect(m_worker, &ScannerWorker::detectedEcu, this, &CANScannerEngine::onWorkerDetectedEcu);
    connect(m_worker, &ScannerWorker::addResult, this, &CANScannerEngine::onWorkerAddResult);
    connect(m_worker, &ScannerWorker::finished, this, &CANScannerEngine::onWorkerFinished);

    m_workerThread.start();
}

CANScannerEngine::~CANScannerEngine()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

int CANScannerEngine::progressPercent() const
{
    if (m_progressTotal <= 0)
        return 0;
    int pct = static_cast<int>((100.0 * m_progressCurrent) / m_progressTotal);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    return pct;
}

void CANScannerEngine::setBusy(bool v)
{
    if (m_busy == v)
        return;
    m_busy = v;
    emit busyChanged();
}

void CANScannerEngine::clearLog()
{
    m_log.clear();
    emit logTextChanged();
}

void CANScannerEngine::clearResults()
{
    m_results.clear();
    emit resultsChanged();
}

void CANScannerEngine::scanBitrate(const QString& iface,
                                   const QString& detectMode,
                                   int testerSa,
                                   int listenMs,
                                   bool extendedOnly)
{
    if (m_busy)
        return;

    clearLog();
    clearResults();

    m_detectedBitrate.clear();
    emit detectedBitrateChanged();

    m_progressCurrent = 0;
    m_progressTotal = 0;
    emit progressChanged();

    setBusy(true);
    m_status = "Skanowanie bitrate...";
    emit statusChanged();

    emit startBitrateScanRequested(iface, detectMode, testerSa, listenMs, extendedOnly);
}

void CANScannerEngine::scanECU(const QString& iface,
                               int bitrate,
                               const QString& testerMode,
                               int testerSa,
                               int testerSaFrom,
                               int testerSaTo,
                               int ecuFrom,
                               int ecuTo,
                               int timeoutMs,
                               const QString& serviceName,
                               bool stopOnFirst,
                               bool debugRx)
{
    if (m_busy)
        return;

    clearLog();
    clearResults();

    m_detectedEcu.clear();
    m_ecuFound = false;
    emit detectedEcuChanged();
    emit ecuFoundChanged();

    m_progressCurrent = 0;
    m_progressTotal = 0;
    emit progressChanged();

    setBusy(true);
    m_status = "Skanowanie ECU...";
    emit statusChanged();

    emit startECUScanRequested(iface, bitrate, testerMode, testerSa,
                               testerSaFrom, testerSaTo,
                               ecuFrom, ecuTo,
                               timeoutMs, serviceName,
                               stopOnFirst, debugRx);
}

void CANScannerEngine::stopScan()
{
    if (!m_busy)
        return;

    emit stopRequested();
    m_status = "Zatrzymywanie...";
    emit statusChanged();
}

void CANScannerEngine::onWorkerLog(const QString& line)
{
    if (!m_log.isEmpty())
        m_log += "\n";
    m_log += line;
    emit logTextChanged();
}

void CANScannerEngine::onWorkerStatus(const QString& status)
{
    m_status = status;
    emit statusChanged();
}

void CANScannerEngine::onWorkerProgress(int current, int total)
{
    m_progressCurrent = current;
    m_progressTotal = total;
    emit progressChanged();
}

void CANScannerEngine::onWorkerDetectedBitrate(const QString& bitrate)
{
    m_detectedBitrate = bitrate;
    emit detectedBitrateChanged();
}

void CANScannerEngine::onWorkerDetectedEcu(const QString& ecuAddress)
{
    if (m_detectedEcu == ecuAddress && m_ecuFound)
        return;

    m_detectedEcu = ecuAddress;
    m_ecuFound = true;
    emit detectedEcuChanged();
    emit ecuFoundChanged();
}

void CANScannerEngine::onWorkerAddResult(const QVariantMap& result)
{
    m_results.append(result);
    emit resultsChanged();
}

void CANScannerEngine::onWorkerFinished(bool, const QString&)
{
    setBusy(false);
}

void CANScannerEngine::setEcuInterface(const QString& v)
{
    if (m_ecuInterface == v)
        return;
    m_ecuInterface = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuBitrate(int v)
{
    if (m_ecuBitrate == v)
        return;
    m_ecuBitrate = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuTesterMode(const QString& v)
{
    if (m_ecuTesterMode == v)
        return;
    m_ecuTesterMode = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuTesterSa(int v)
{
    if (m_ecuTesterSa == v)
        return;
    m_ecuTesterSa = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuTesterFrom(int v)
{
    if (m_ecuTesterFrom == v)
        return;
    m_ecuTesterFrom = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuTesterTo(int v)
{
    if (m_ecuTesterTo == v)
        return;
    m_ecuTesterTo = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuAddrFrom(int v)
{
    if (m_ecuAddrFrom == v)
        return;
    m_ecuAddrFrom = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuAddrTo(int v)
{
    if (m_ecuAddrTo == v)
        return;
    m_ecuAddrTo = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuTimeoutMs(int v)
{
    if (m_ecuTimeoutMs == v)
        return;
    m_ecuTimeoutMs = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuServiceName(const QString& v)
{
    if (m_ecuServiceName == v)
        return;
    m_ecuServiceName = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuStopOnFirst(bool v)
{
    if (m_ecuStopOnFirst == v)
        return;
    m_ecuStopOnFirst = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setEcuDebugRx(bool v)
{
    if (m_ecuDebugRx == v)
        return;
    m_ecuDebugRx = v;
    emit ecuSettingsChanged();
}

void CANScannerEngine::setBitrateInterface(const QString& v)
{
    if (m_bitrateInterface == v)
        return;
    m_bitrateInterface = v;
    emit bitrateSettingsChanged();
}

void CANScannerEngine::setBitrateDetectMode(const QString& v)
{
    if (m_bitrateDetectMode == v)
        return;
    m_bitrateDetectMode = v;
    emit bitrateSettingsChanged();
}

void CANScannerEngine::setBitrateTesterSa(int v)
{
    if (m_bitrateTesterSa == v)
        return;
    m_bitrateTesterSa = v;
    emit bitrateSettingsChanged();
}

void CANScannerEngine::setBitrateListenMs(int v)
{
    if (m_bitrateListenMs == v)
        return;
    m_bitrateListenMs = v;
    emit bitrateSettingsChanged();
}

void CANScannerEngine::setBitrateExtendedOnly(bool v)
{
    if (m_bitrateExtendedOnly == v)
        return;
    m_bitrateExtendedOnly = v;
    emit bitrateSettingsChanged();
}