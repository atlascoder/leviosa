#include "NetworkMonitor.h"
#include <QDebug>
#include <QNetworkInterface>

#define CON_STATE_NAME (mConnectionState == Disconnected ? "offline" : (mConnectionState==Wifi ? "WLAN" : "WAN"))

NetworkMonitor& NetworkMonitor::instance()
{
    static NetworkMonitor sInstance;
    return sInstance;
}

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroid>
#endif

NetworkMonitor::NetworkMonitor(QObject *parent) :
    QObject(parent),
    #ifdef Q_OS_IOS
        ReachabilityDelegate(),
    #endif
    mConnectionState(Disconnected),
    mLastBssid("")
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
    if(!mgr.isOnline() && mConnectionState != Disconnected){
        setConnectionState(Disconnected);
    }
    else if(mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN && (mConnectionState != Wifi || mLastBssid != bssid())){
        mLastBssid = bssid();
        setConnectionState(Wifi);
    }
    else if(mConnectionState != NotWifi){
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
        if(mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN){
            mLastBssid = bssid();
            setConnectionState(Wifi);
        }
        else
            setConnectionState(NotWifi);
    }
    else
        setConnectionState(Disconnected);
}

void NetworkMonitor::netChanged(const QNetworkConfiguration &conf)
{
    qDebug() << "netChanges triggered: " << conf.bearerTypeName();
    if (mgr.isOnline()) {
        if (mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN) {
            const QString curr_bssid = bssid();
            if (curr_bssid != mLastBssid) {
                mLastBssid = curr_bssid;
                setConnectionState(Wifi);
                emit bssidChanged();
            }
        }
        else {
            mLastBssid = "";
            setConnectionState(NotWifi);
        }
    }
    else {
        mLastBssid = "";
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
        qDebug() << "SSID " << getSsid() << " / " << QString::fromStdString(bssid_ios()) << " / " << getWlanIp();
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
    return mLastBssid;
}

QString NetworkMonitor::bssid() const
{
    QString _bssid;
    if(mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN){
#ifdef Q_OS_IOS
        _bssid = QString::fromStdString(bssid_ios());
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
    mConnectionState = connectionState;
    qDebug() << "ConnectionState changed to " << CON_STATE_NAME;
    emit connectionStateChanged();
}

void NetworkMonitor::setOnWlan(bool onWlan){
    mLastBssid = "";
    if(mgr.isOnline()){
        if(onWlan){
            mLastBssid = bssid();
            setConnectionState(Wifi);
        }
        else
            setConnectionState(NotWifi);
    }
    else
        setConnectionState(Disconnected);
}
