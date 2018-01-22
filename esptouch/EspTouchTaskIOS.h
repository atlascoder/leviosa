#ifndef ESPTOUCHTASKIOS_H
#define ESPTOUCHTASKIOS_H

#include "../../../esptouch-qt-ios/ESPTouchHolder.h"
#include <QThread>
#include <QString>
#include <QObject>
#include <string>

class EspTouchTaskIOS : public QThread, private esptouch::ESPTouchTaskDelegate
{
    Q_OBJECT

    void run() override;

    std::string mSsid;
    std::string mBssid;
    std::string mPwd;

    QString mHostIp;
    QString mHostMac;
    bool mSuccessful;

    void onEsptouchResult(std::string conf) override;

public:
    EspTouchTaskIOS(QString &ssid, QString &bssid, QString &password, QObject *parent = 0);

public slots:
    void stop();

signals:
    void hostConfigured(const QString& conf);
    void finished();
};

#endif // ESPTOUCHTASKIOS_H
