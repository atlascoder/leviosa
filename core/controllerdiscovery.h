#ifndef CONTROLLERDISCOVERY_H
#define CONTROLLERDISCOVERY_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <QTimer>
#include "mdnsdiscothread.h"
#include <memory>
#include "QtZeroConf/qzeroconf.h"
#include "QtZeroConf/qzeroconfservice.h"


class ControllerDiscovery : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int timeoutSec READ timeout WRITE setTimeout)
    Q_PROPERTY(bool isRunning READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(QString foundList READ foundList NOTIFY finished)

public:
    ControllerDiscovery(QObject* parent = 0);
    virtual ~ControllerDiscovery();

    Q_INVOKABLE void discovery();

    int timeout() const;
    void setTimeout(int timeoutSec);

    QString foundList() const;

    bool running() const;
    void setRunning(bool isRunning);
    QZeroConf mZeroConf;
    MDNSDiscoThread mDiscoThread;
public slots:
    void serviceFound(QZeroConfService conf);
    void discovered(const QString& mac, const QString& ip);
signals:
    void foundList(const QString& list);
    void started();
    void finished();
    void runningChanged();
private slots:
    void timedOut();
private:
    int mTimeoutSec;
    bool mIsRunning;
    QStringList mDiscovered;
    QTimer mTimer;

    void stop();
    void onFinished();
};

#endif // CONTROLLERDISCOVERY_H
