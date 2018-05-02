#ifndef CONTROLLERSSDP_H
#define CONTROLLERSSDP_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <QTimer>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUdpSocket>
#include <memory>

#include "ControllerConfig.h"

using namespace std;

class ControllerSSDP : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int timeoutSec READ timeout WRITE setTimeout)
    Q_PROPERTY(bool isRunning READ running WRITE setRunning NOTIFY runningChanged)
public:
    ControllerSSDP(QObject* parent = 0);
    virtual ~ControllerSSDP();


    int timeout() const;
    void setTimeout(int timeoutSec);

    QStringList& foundList();

    bool running() const;
    void setRunning(bool isRunning);

signals:
    void found(const QString& ip, const QString& configString);
    void started();
    void finished();
    void doDiscover();
    void runningChanged();
private slots:
    void sendSearch();
    void timedOut();
    void gotDatagram();
private slots:
    void onSocketClosed();
private:

    int mTimeoutSec;
    bool mIsRunning;
    QStringList mDiscovered;
    unique_ptr<QTimer> mTimer;

    const QHostAddress mGroup;
    const int mPort;

    QUdpSocket* mSock;

    void discovery();
    void stop();
    void onFinished();
};

#endif // CONTROLLERSSDP_H
