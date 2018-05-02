#ifndef CONTROLLERSCAN_H
#define CONTROLLERSCAN_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <QTimer>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <memory>


class ControllerScan : public QThread
{
    Q_OBJECT
    Q_PROPERTY(int timeoutSec READ timeout WRITE setTimeout)
    Q_PROPERTY(bool isRunning READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(QString foundList READ foundList NOTIFY finished)

    void run() override;
public:
    ControllerScan(QObject* parent = 0);
    virtual ~ControllerScan();

    Q_INVOKABLE void discovery();

    int timeout() const;
    void setTimeout(int timeoutSec);

    QString foundList() const;

    bool running() const;
    void setRunning(bool isRunning);
public slots:
    void discovered(const QString& mac, const QString& ip);
signals:
    void foundList(const QString& list);
    void found(const QString& mac, const QString& ip);
    void started();
    void finished();
    void runningChanged();
private slots:
    void timedOut();
    void gotReply(QNetworkReply* reply);
private:

    int mTimeoutSec;
    bool mIsRunning;
    QStringList mDiscovered;
    QTimer mTimer;
    QNetworkAccessManager *mQnam;

    void stop();
    void onFinished();
};

#endif // CONTROLLERSCAN_H
