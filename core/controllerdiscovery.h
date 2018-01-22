#ifndef CONTROLLERDISCOVERY_H
#define CONTROLLERDISCOVERY_H

#include <QObject>
#include "mdnsdiscothread.h"
#include <memory>

class ControllerDiscovery : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int timeoutSec READ timeout WRITE setTimeout)
    Q_PROPERTY(bool isRunning READ running WRITE setRunning NOTIFY runningChanged)
public:
    ControllerDiscovery(QObject* parent = 0);
    ~ControllerDiscovery();

    Q_INVOKABLE void discovery();

    int timeout() const;
    void setTimeout(int timeoutSec);

    bool running() const;
    void setRunning(bool isRunning);
signals:
    void found(const QString& mac, const QString& ip);
    void started();
    void finished();
    void runningChanged();
private:
    int mTimeoutSec;
    bool mIsRunning;

    std::unique_ptr<MDNSDiscoThread> mThread;

    void stop();
    void onFinished();
};

#endif // CONTROLLERDISCOVERY_H
