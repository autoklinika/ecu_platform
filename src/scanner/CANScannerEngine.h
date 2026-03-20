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

signals:
    void busyChanged();
    void statusChanged();
    void logTextChanged();
    void progressChanged();
    void resultsChanged();
    void detectedBitrateChanged();

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
};