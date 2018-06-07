#include "NetworkMonitor.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QStringList>

#define CON_STATE_NAME (mConnectionState == Disconnected ? "offline" : (mConnectionState==Wifi ? "WLAN" : "WAN"))

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroid>
#endif

NetworkMonitor& NetworkMonitor::instance()
{
    static NetworkMonitor sInstance;
    return sInstance;
}

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
        mLastBssid = getBssid();
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
    else if(mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN && (mConnectionState != Wifi || mLastBssid != getBssid())){
        mLastBssid = getBssid();
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
    mTimer.setInterval(1000);
    mTimer.setSingleShot(false);
    connect(&mTimer, &QTimer::timeout, this, &NetworkMonitor::checkBssid);
}

void NetworkMonitor::setIsOnline(bool online){
    qDebug() << "isOnline triggered: " << online;
    if (online) {
        if (mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN) {
            mLastBssid = getBssid();
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
//    qDebug() << "netChanges triggered: " << conf.bearerTypeName();
#ifdef Q_OS_ANDROID
    int conn_state = getConnectionType();
    if (conn_state == 2) {
        const QString curr_bssid = getBssid();
        if (curr_bssid != mLastBssid) {
            mLastBssid = getBssid();
            setConnectionState(Wifi);
            emit bssidChanged();
        }
    }
    else if (conn_state == 1) {
        if (!mLastBssid.isEmpty()) mLastBssid = "";
        setConnectionState(NotWifi);
    }
    else {
        if (!mLastBssid.isEmpty()) mLastBssid = "";
        setConnectionState(Disconnected);
    }
#else
    if (mgr.isOnline()) {
        if (mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN) {
            const QString curr_bssid = getBssid();
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
#endif
    emit descriptionChanged();
}

#ifdef Q_OS_IOS
void NetworkMonitor::statusChanged(utility::NetworkStatus n)
{
    mTimer.stop();
    if(n == utility::NotReachable){
        setConnectionState(Disconnected);
    }
    else if(n == utility::ReachableViaWiFi){
        mLastBssid = getBssid();
        setConnectionState(Wifi);
        mTimer.start();
    }
    else{
        setConnectionState(NotWifi);
    }
    qDebug() << "NETWORK " << CON_STATE_NAME;
    if(mConnectionState == Wifi)
        qDebug() << "SSID " << getSsid() << " / " << getBssid() << " / " << getWlanIp();
}

#endif

void NetworkMonitor::checkBssid()
{
    qDebug() << "check bssid";
#ifdef Q_OS_IOS
    if (getBssid() == mLastBssid) return;
    mLastBssid = getBssid();
    emit connectionStateChanged();
#elif defined Q_OS_ANDROID
    QNetworkConfiguration conf = mgr.defaultConfiguration();
    netChanged(conf);
#endif
}

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

QString NetworkMonitor::bssid() const
{
#ifdef Q_OS_IOS
    return getBssid();
#else
    return mLastBssid;
#endif
}

QString NetworkMonitor::getBssid() const
{
    QString _bssid;
#ifdef Q_OS_IOS
        _bssid = QString::fromStdString(bssid_ios());
        if (_bssid.length() != 17) {
            QStringList bssid_vals = _bssid.split(":");
            QString __bssid;
            for (int i = 0; i < bssid_vals.length(); i++) {
                if (bssid_vals[i].length() == 1)
                    bssid_vals[i] = "0" + bssid_vals[i];
            }
            _bssid = bssid_vals.join(":");
        }
#elif defined Q_OS_ANDROID
    if(mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN){
        QAndroidJniObject j_bssid = QtAndroid::androidActivity().callObjectMethod(
                    "getBssid",
                    "()Ljava/lang/String;"
                    );
        _bssid = j_bssid.toString();
    }
#else
    if(mgr.defaultConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN){
        QList<QNetworkInterface> ifs = QNetworkInterface::allInterfaces();
        for(int i = 0; i < ifs.size(); i++){
            QString cname = mgr.defaultConfiguration().name();
            if(ifs.at(i).name() == cname){
                _bssid = QString::fromStdString(ifs.at(i).hardwareAddress().toStdString());
                break;
            }
        }
    }
#endif
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

void NetworkMonitor::onApplicationStateChanged(const Qt::ApplicationState state)
{
    if (state == Qt::ApplicationActive) {
        mTimer.setInterval(1000);
#ifdef Q_OS_IOS
        if (isOnWlan()) checkBssid();
        mTimer.setSingleShot(false);
#elif defined Q_OS_ANDROID
        QNetworkConfiguration conf = mgr.defaultConfiguration();
        netChanged(conf);
        mTimer.setSingleShot(false);
#endif
        mTimer.start();
    }
    else {
        mTimer.stop();
    }
}

QString NetworkMonitor::description() const
{
    QString bearerTypes[] = { "BearerUnknown",
                                     "BearerEthernet",
                                     "BearerWLAN",
                                     "Bearer2G",
                                     "BearerCDMA2000",
                                     "BearerWCDMA",
                                     "BearerHSPA",
                                     "BearerBluetooth",
                                     "BearerWiMAX",
                                     "BearerEVDO",
                                     "BearerLTE",
                                     "Bearer3G",
                                     "Bearer4G"};
    return "Bearer: " + bearerTypes[mgr.defaultConfiguration().bearerType()] +
            ", online: " +
            (mgr.isOnline() ? "YES" : "NO");
}


#ifdef Q_OS_ANDROID
int NetworkMonitor::getConnectionType() const
{
    return QtAndroid::androidActivity().callMethod<jint>("getConnectionState");
}
#endif
