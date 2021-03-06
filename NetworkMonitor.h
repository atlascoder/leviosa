#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H
#include <QObject>
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QList>
#include <QNetworkSession>
#include <QPointer>
#include <QTimer>

#ifdef Q_OS_IOS
#include "ios/src/ReachabilityListener.h"
#endif

class NetworkMonitor : public QObject
#ifdef Q_OS_IOS
 , private utility::ReachabilityDelegate
#endif
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectionStateChanged)
    Q_PROPERTY(QString ssid READ getSsid NOTIFY ssidChanged)
    Q_PROPERTY(QString bssid READ bssid NOTIFY bssidChanged)
    Q_PROPERTY(QString wlanIp READ getWlanIp NOTIFY wlanIpChanged)
    Q_PROPERTY(bool onWlan READ isOnWlan NOTIFY connectionStateChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)

signals:
    void currentStateChanged(const QString &state);
    void connectedChanged();

    void ssidChanged();
    void bssidChanged();
    void wlanIpChanged();

    void connectionStateChanged();
    void descriptionChanged();
public slots:
    void netChanged(const QNetworkConfiguration &conf);
    void onApplicationStateChanged(const Qt::ApplicationState state);

private slots:
    void setIsOnline(bool online);
    void checkBssid();

public:

    static NetworkMonitor& instance();

    enum ConnectionState { Disconnected, Wifi, NotWifi };
    Q_ENUMS(ConnectionState)

    bool isConnected() const { return mConnectionState != Disconnected; }

    QString getSsid() const;
    QString bssid() const;
    QString getWlanIp() const;

    bool isOnWlan() const { return mConnectionState == Wifi; }

private:
    QNetworkConfigurationManager mgr;
    QString m_state;
    QPointer<QNetworkSession> session;
    ConnectionState mConnectionState;
    QString mLastBssid;
    QTimer mTimer;

    NetworkMonitor(QObject *parent = 0);

#ifdef Q_OS_IOS
    void statusChanged(utility::NetworkStatus n) override;
#endif

#ifdef Q_OS_ANDROID
    int getConnectionType() const;
#endif

    QString getBssid() const;

    QString description() const;

    ConnectionState connectionState() const { return mConnectionState; }
    void setConnectionState(ConnectionState connectionState);

};

#endif // NETWORKMONITOR_H
