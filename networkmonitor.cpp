#include "networkmonitor.h"
#include <QTextStream>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroid>
#endif

void NetworkMonitor::changeState(bool isConnected){
    mIsOnline = isConnected;
    m_state = isConnected ? ("Online " + mgr.defaultConfiguration().bearerTypeName()) : "Offline";
    qInfo() << m_state;
    emit currentStateChanged(m_state);
}

void NetworkMonitor::isOnline(bool connected)
{
    QTextStream(stdout) << "NETWORK -> " << (connected ? "online" : "offline") << endl;
    changeState(connected);
    emit connectedChanged();
}

NetworkMonitor::NetworkMonitor(QObject *parent) :
    QObject(parent),
    #ifdef Q_OS_IOS
        ReachabilityDelegate(),
    #endif
    mIsOnline(false),
    mIsWLAN(false)
{
#ifdef Q_OS_IOS
    if(status() == utility::NotReachable){
        mIsOnline = false;
    } else {
        mIsOnline = true;
        mIsWLAN = status() == utility::ReachableViaWiFi;
    }
#else
    connect(&mgr, &QNetworkConfigurationManager::onlineStateChanged, this, &NetworkMonitor::isOnline);
    connect(&mgr, &QNetworkConfigurationManager::configurationChanged, this, &NetworkMonitor::netChanged);
    mIsOnline = mgr.isOnline();
    mIsWLAN = mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN;
#endif
    m_state = mIsOnline ? ("Online " + mgr.defaultConfiguration().bearerTypeName()) : "Offline";
    QTextStream(stdout) << "NetConfig name: " << mgr.defaultConfiguration().name() << endl;
}

bool NetworkMonitor::isConnected() const
{
    return mIsOnline;
}

void NetworkMonitor::netChanged(const QNetworkConfiguration &conf)
{
    QTextStream(stdout) << "NETWORK CHANGED to " << conf.name() << (mgr.isOnline() ? " online" : " offline") <<  endl;
}

#ifdef Q_OS_IOS
void NetworkMonitor::statusChanged(utility::NetworkStatus n)
{
    if(n == utility::NotReachable){
        mIsOnline = false;
    }
    else{
        mIsOnline = true;
        mIsWLAN = n == utility::ReachableViaWiFi;
    }
    QTextStream(stdout) << "NETWORK " << (mIsOnline ? (mIsWLAN ? "online via WLAN" : "online via WAN") : "offline") << endl;
    if(mIsOnline && mIsWLAN)
        QTextStream(stdout) << "SSID " << getSsid() << " / " << getBssid() << " / " << getWlanIp() << endl;
    emit connectedChanged();
    emit wlanIpChanged();
    emit onWlanChanged();
}
#endif

QString NetworkMonitor::getSsid() const
{
#ifdef Q_OS_IOS
    if(mIsWLAN)
        return QString::fromStdString(ssid());
#endif
#ifdef Q_OS_ANDROID
    if(mIsWLAN){
        QAndroidJniObject j_ssid = QtAndroid::androidActivity().callObjectMethod(
                    "getSsid",
                    "()Ljava/lang/String;"
                    );
        return j_ssid.toString();
    }
#endif
    return QString();
}

QString NetworkMonitor::getBssid() const
{
#ifdef Q_OS_IOS
    if(mIsWLAN)
        return QString::fromStdString(bssid());
#endif
#ifdef Q_OS_ANDROID
    if(mIsWLAN){
        QAndroidJniObject j_bssid = QtAndroid::androidActivity().callObjectMethod(
                    "getBssid",
                    "()Ljava/lang/String;"
                    );
        return j_bssid.toString();
    }
#endif
    return QString();
}

QString NetworkMonitor::getWlanIp() const
{
#ifdef Q_OS_IOS
    return QString::fromStdString(ipaddr());
#endif

#ifdef Q_OS_ANDROID
    if(mIsWLAN){
        QAndroidJniObject j_wlanIp = QtAndroid::androidActivity().callObjectMethod("getWlanIP", "()Ljava/lang/String;");
        return j_wlanIp.toString();
    }
#endif
    return QString();
}

bool NetworkMonitor::getOnWlan() const
{
    return mIsOnline ;//&& mIsWLAN;
}
