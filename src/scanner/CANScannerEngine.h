#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QThread>

class ScannerWorker : public QObject
{
    Q_OBJECT

public:
    explicit ScannerWorker(QObject* parent = nullptr);

signals:
    void log(const QString& line);
    void status(const QString& text);
    void progress(int current, int total);
    void detectedBitrate(const QString& bitrate);
    void addResult(const QVariantMap& result);
    void finished(bool success, const QString& summary);

public slots:
    void requestStop();

    void runBitrateScan(const QString& iface,
                        const QString& detectMode,
                        int testerSa,
                        int listenMs,
                        bool extendedOnly);

    void runECUScan(const QString& iface,
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
                    bool debugRx);

private:
    bool configureCAN(const QString& iface, int bitrate);
    bool resetCAN(const QString& iface);
    bool openSocket(const QString& iface, int& fd);
    void closeSocket(int& fd);

    bool sendFrame(int fd, uint32_t id, const uint8_t* data, uint8_t len);
    bool receiveFrameTimeout(int fd, uint32_t& canId, uint8_t* data, uint8_t& len, int timeoutMs);
    int drainSocket(int fd, int drainMs, QStringList* firstIds = nullptr);

    uint32_t buildDiagRequestId(int ecuAddr, int testerAddr) const;
    bool isExpectedDiagResponse(uint32_t canId, int ecuAddr, int testerAddr) const;
    QString formatData(const uint8_t* data, int len) const;
    QString detectResponseType(const QByteArray& data) const;
    QString servicePayloadNameToBytes(const QString& serviceName, QByteArray& out) const;

    QString detectBitrateByListen(const QString& iface, int listenMs, bool extendedOnly);
    QString detectBitrateByProbe(const QString& iface, int testerSa);

private:
    bool m_stop = false;
};

class CANScannerEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString logText READ logText NOTIFY logTextChanged)
    Q_PROPERTY(int progressCurrent READ progressCurrent NOTIFY progressChanged)
    Q_PROPERTY(int progressTotal READ progressTotal NOTIFY progressChanged)
    Q_PROPERTY(int progressPercent READ progressPercent NOTIFY progressChanged)
    Q_PROPERTY(QVariantList results READ results NOTIFY resultsChanged)
    Q_PROPERTY(QString detectedBitrate READ detectedBitrate NOTIFY detectedBitrateChanged)
    Q_PROPERTY(QString ecuInterface READ ecuInterface WRITE setEcuInterface NOTIFY ecuSettingsChanged)
    Q_PROPERTY(int ecuBitrate READ ecuBitrate WRITE setEcuBitrate NOTIFY ecuSettingsChanged)
    Q_PROPERTY(QString ecuTesterMode READ ecuTesterMode WRITE setEcuTesterMode NOTIFY ecuSettingsChanged)
    Q_PROPERTY(int ecuTesterSa READ ecuTesterSa WRITE setEcuTesterSa NOTIFY ecuSettingsChanged)
    Q_PROPERTY(int ecuTesterFrom READ ecuTesterFrom WRITE setEcuTesterFrom NOTIFY ecuSettingsChanged)
    Q_PROPERTY(int ecuTesterTo READ ecuTesterTo WRITE setEcuTesterTo NOTIFY ecuSettingsChanged)
    Q_PROPERTY(int ecuAddrFrom READ ecuAddrFrom WRITE setEcuAddrFrom NOTIFY ecuSettingsChanged)
    Q_PROPERTY(int ecuAddrTo READ ecuAddrTo WRITE setEcuAddrTo NOTIFY ecuSettingsChanged)
    Q_PROPERTY(int ecuTimeoutMs READ ecuTimeoutMs WRITE setEcuTimeoutMs NOTIFY ecuSettingsChanged)
    Q_PROPERTY(QString ecuServiceName READ ecuServiceName WRITE setEcuServiceName NOTIFY ecuSettingsChanged)
    Q_PROPERTY(bool ecuStopOnFirst READ ecuStopOnFirst WRITE setEcuStopOnFirst NOTIFY ecuSettingsChanged)
    Q_PROPERTY(bool ecuDebugRx READ ecuDebugRx WRITE setEcuDebugRx NOTIFY ecuSettingsChanged)
    Q_PROPERTY(QString bitrateInterface READ bitrateInterface WRITE setBitrateInterface NOTIFY bitrateSettingsChanged)
    Q_PROPERTY(QString bitrateDetectMode READ bitrateDetectMode WRITE setBitrateDetectMode NOTIFY bitrateSettingsChanged)
    Q_PROPERTY(int bitrateTesterSa READ bitrateTesterSa WRITE setBitrateTesterSa NOTIFY bitrateSettingsChanged)
    Q_PROPERTY(int bitrateListenMs READ bitrateListenMs WRITE setBitrateListenMs NOTIFY bitrateSettingsChanged)
    Q_PROPERTY(bool bitrateExtendedOnly READ bitrateExtendedOnly WRITE setBitrateExtendedOnly NOTIFY bitrateSettingsChanged)

public:
    explicit CANScannerEngine(QObject* parent = nullptr);
    ~CANScannerEngine();

    bool busy() const { return m_busy; }
    QString status() const { return m_status; }
    QString logText() const { return m_log; }
    int progressCurrent() const { return m_progressCurrent; }
    int progressTotal() const { return m_progressTotal; }
    int progressPercent() const;
    QVariantList results() const { return m_results; }
    QString detectedBitrate() const { return m_detectedBitrate; }

    Q_INVOKABLE void clearLog();
    Q_INVOKABLE void clearResults();

    Q_INVOKABLE void scanBitrate(const QString& iface,
                                 const QString& detectMode,
                                 int testerSa,
                                 int listenMs,
                                 bool extendedOnly);

    Q_INVOKABLE void scanECU(const QString& iface,
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
                             bool debugRx);

    Q_INVOKABLE void stopScan();

    QString ecuInterface() const { return m_ecuInterface; }
int ecuBitrate() const { return m_ecuBitrate; }
QString ecuTesterMode() const { return m_ecuTesterMode; }
int ecuTesterSa() const { return m_ecuTesterSa; }
int ecuTesterFrom() const { return m_ecuTesterFrom; }
int ecuTesterTo() const { return m_ecuTesterTo; }
int ecuAddrFrom() const { return m_ecuAddrFrom; }
int ecuAddrTo() const { return m_ecuAddrTo; }
int ecuTimeoutMs() const { return m_ecuTimeoutMs; }
QString ecuServiceName() const { return m_ecuServiceName; }
bool ecuStopOnFirst() const { return m_ecuStopOnFirst; }
bool ecuDebugRx() const { return m_ecuDebugRx; }

void setEcuInterface(const QString& v);
void setEcuBitrate(int v);
void setEcuTesterMode(const QString& v);
void setEcuTesterSa(int v);
void setEcuTesterFrom(int v);
void setEcuTesterTo(int v);
void setEcuAddrFrom(int v);
void setEcuAddrTo(int v);
void setEcuTimeoutMs(int v);
void setEcuServiceName(const QString& v);
void setEcuStopOnFirst(bool v);
void setEcuDebugRx(bool v);
QString bitrateInterface() const { return m_bitrateInterface; }
QString bitrateDetectMode() const { return m_bitrateDetectMode; }
int bitrateTesterSa() const { return m_bitrateTesterSa; }
int bitrateListenMs() const { return m_bitrateListenMs; }
bool bitrateExtendedOnly() const { return m_bitrateExtendedOnly; }

void setBitrateInterface(const QString& v);
void setBitrateDetectMode(const QString& v);
void setBitrateTesterSa(int v);
void setBitrateListenMs(int v);
void setBitrateExtendedOnly(bool v);

signals:
    void busyChanged();
    void statusChanged();
    void logTextChanged();
    void progressChanged();
    void resultsChanged();
    void detectedBitrateChanged();
    void ecuSettingsChanged();
    void bitrateSettingsChanged();

    void startBitrateScanRequested(const QString& iface,
                                   const QString& detectMode,
                                   int testerSa,
                                   int listenMs,
                                   bool extendedOnly);

    void startECUScanRequested(const QString& iface,
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
                               bool debugRx);

    void stopRequested();
    

public slots:
    void onWorkerLog(const QString& line);
    void onWorkerStatus(const QString& status);
    void onWorkerProgress(int current, int total);
    void onWorkerDetectedBitrate(const QString& bitrate);
    void onWorkerAddResult(const QVariantMap& result);
    void onWorkerFinished(bool success, const QString& summary);

private:
    void setBusy(bool v);

private:
    bool m_busy = false;
    QString m_status = "Gotowy";
    QString m_log;
    int m_progressCurrent = 0;
    int m_progressTotal = 0;
    QVariantList m_results;
    QString m_detectedBitrate;

    QThread m_workerThread;
    ScannerWorker* m_worker = nullptr;
    QString m_ecuInterface = "can0";
int m_ecuBitrate = 250000;
QString m_ecuTesterMode = "manual";
int m_ecuTesterSa = 241;
int m_ecuTesterFrom = 240;
int m_ecuTesterTo = 255;
int m_ecuAddrFrom = 0;
int m_ecuAddrTo = 254;
int m_ecuTimeoutMs = 120;
QString m_ecuServiceName = "TesterPresent (3E 00)";
bool m_ecuStopOnFirst = true;
bool m_ecuDebugRx = true;
QString m_bitrateInterface = "can0";
QString m_bitrateDetectMode = "auto";
int m_bitrateTesterSa = 241;
int m_bitrateListenMs = 800;
bool m_bitrateExtendedOnly = true;
};