#ifndef ESPTOUCHTASKANDROID_H
#define ESPTOUCHTASKANDROID_H

#include <QString>
#include <QThread>
#include <QObject>

class EspTouchTaskAndroid: public QThread
{
    Q_OBJECT

    void run() override;

    QString mSsid;
    QString mBssid;
    QString mPwd;

    QString mHostIp;
    QString mHostMac;
    bool mSuccessful;

    void onEsptouchResult();
public:
    EspTouchTaskAndroid(QString &ssid, QString &bssid, QString &password, QObject *parent = 0);

public slots:
    void stop();

signals:
    void hostConfigured(const QString& conf);

    // to remove?
    void finished();
};

#endif // ESPTOUCHTASKANDROID_H
