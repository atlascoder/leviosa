#include "networkmonitor.h"
#include <QDebug>
#include <QNetworkInterface>

#define CON_STATE_NAME (mConnectionState == Disconnected ? "offline" : (mConnectionState==Wifi ? "WLAN" : "WAN"))

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroid>
#endif

NetworkMonitor::NetworkMonitor(QObject *parent) :
    QObject(parent),
    #ifdef Q_OS_IOS
        ReachabilityDelegate(),
    #endif
    mConnectionState(Disconnected)
{
#ifdef Q_OS_IOS
    if(status() == utility::NotReachable){
        setConnectionState(Disconnected);
    } else if(status() == utility::ReachableViaWiFi){
        setConnectionState(Wifi);
    }
    else{
        setConnectionState(NotWifi);
    }
#elif defined Q_OS_ANDROID
    connect(&mgr, &QNetworkConfigurationManager::onlineStateChanged, this, &NetworkMonitor::setIsOnline);
    connect(&mgr, &QNetworkConfigurationManager::configurationChanged, this, &NetworkMonitor::netChanged);
    connect(&mgr, &QNetworkConfigurationManager::configurationAdded, this, &NetworkMonitor::netChanged);
    if(!mgr.isOnline()){
        setConnectionState(Disconnected);
    }
    else if(mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN){
        setConnectionState(Wifi);
    }
    else{
        setConnectionState(NotWifi);
    }
#else
    connect(&mgr, &QNetworkConfigurationManager::onlineStateChanged, this, &NetworkMonitor::setIsOnline);
    connect(&mgr, &QNetworkConfigurationManager::configurationChanged, this, &NetworkMonitor::netChanged);
    setIsOnline(mgr.isOnline());
#endif
}

void NetworkMonitor::setIsOnline(bool online){
    qDebug() << "isOnline triggered: " << online;
    if(online){
        if(mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN)
            setConnectionState(Wifi);
        else
            setConnectionState(NotWifi);
    }
    else
        setConnectionState(Disconnected);
}

void NetworkMonitor::netChanged(const QNetworkConfiguration &conf)
{
    qDebug() << "netChanges triggered: " << conf.bearerTypeName();
    if(mgr.isOnline()){
        if(conf.bearerType() == QNetworkConfiguration::BearerWLAN)
            setConnectionState(Wifi);
        else
            setConnectionState(NotWifi);
    }
    else{
        setConnectionState(Disconnected);
    }
}

#ifdef Q_OS_IOS
void NetworkMonitor::statusChanged(utility::NetworkStatus n)
{
    if(n == utility::NotReachable){
        setConnectionState(Disconnected);
    }
    else if(n == utility::ReachableViaWiFi){
        setConnectionState(Wifi);
    }
    else{
        setConnectionState(NotWifi);
    }
    qDebug() << "NETWORK " << CON_STATE_NAME;
    if(mConnectionState == Wifi)
        qDebug() << "SSID " << getSsid() << " / " << QString::fromStdString(bssid()) << " / " << getWlanIp();
}
#endif

QString NetworkMonitor::getSsid() const
{
    if(mConnectionState == Wifi){
#ifdef Q_OS_IOS
        return QString::fromStdString(ssid());
#elif defined Q_OS_ANDROID
        QAndroidJniObject j_ssid = QtAndroid::androidActivity().callObjectMethod("getSsid", "()Ljava/lang/String;");
        return j_ssid.toString();
#else
    return mgr.defaultConfiguration().name();
#endif
    }
    else return mgr.defaultConfiguration().name();
}

QString NetworkMonitor::getBssid() const
{
    QString _bssid;
    if(mConnectionState == Wifi){
#ifdef Q_OS_IOS
        _bssid = QString::fromStdString(bssid());
#elif defined Q_OS_ANDROID
        QAndroidJniObject j_bssid = QtAndroid::androidActivity().callObjectMethod(
                    "getBssid",
                    "()Ljava/lang/String;"
                    );
        _bssid = j_bssid.toString();
#else
    QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
    for(int i = 0; i < ifs.size(); i++){
        QString cname = mgr.defaultConfiguration().name();
        if(ifs.at(i).name() == cname){
            _bssid = QString::fromStdString(ifs.at(i).hardwareAddress().toStdString());
            break;
        }
    }
#endif
    }
    qDebug() << "bssid: " << _bssid;
    return _bssid;
}

QString NetworkMonitor::getWlanIp() const
{
    if(mConnectionState == Wifi){
#ifdef Q_OS_IOS
    return QString::fromStdString(ipaddr());
#elif defined Q_OS_ANDROID
    QAndroidJniObject j_wlanIp = QtAndroid::androidActivity().callObjectMethod("getWlanIP", "()Ljava/lang/String;");
    return j_wlanIp.toString();
#else
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for(int i = 0; i < addresses.size(); i++)
        if(!addresses[i].isLoopback())
            return addresses[i].toString();

    return QString("0.0.0.0");
#endif
    }
    else return QString("0.0.0.0");
}

void NetworkMonitor::setConnectionState(ConnectionState connectionState)
{
    if(mConnectionState == connectionState){
        qDebug() << "ConnectionState not changed " << CON_STATE_NAME;
        return;
    }
    mConnectionState = connectionState;
    qDebug() << "ConnectionState changed to " << CON_STATE_NAME;
    emit connectionStateChanged();
}

void NetworkMonitor::setOnWlan(bool onWlan){
    if(mgr.isOnline()){
        if(onWlan)
            setConnectionState(Wifi);
        else
            setConnectionState(NotWifi);
    }
    else
        setConnectionState(Disconnected);
}
