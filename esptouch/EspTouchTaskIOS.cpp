#include "EspTouchTaskIOS.h"

EspTouchTaskIOS::EspTouchTaskIOS(QString &ssid, QString &bssid, QString &password, QObject *parent):
    QThread(parent), mHostIp(), mHostMac(), mSuccessful(false)
{
    mSsid = ssid.toStdString();
    mBssid = bssid.toStdString();
    mPwd = password.toStdString();
}

void EspTouchTaskIOS::run()
{
    startEspTouch(mSsid, mBssid, mPwd);
}

void EspTouchTaskIOS::stop(){
    stopEspTouch();
    quit();
    wait();
}

void EspTouchTaskIOS::onEsptouchResult(std::string conf)
{
    QString astr(QString::fromStdString(conf));
    emit hostConfigured(astr);
}
