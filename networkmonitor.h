#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H
#include <QObject>
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QList>
#include <QNetworkSession>
#include <QPointer>

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
    Q_PROPERTY(QString bssid READ getBssid NOTIFY bssidChanged)
    Q_PROPERTY(QString wlanIp READ getWlanIp NOTIFY wlanIpChanged)

    Q_PROPERTY(bool onWlan READ isOnWlan WRITE setOnWlan NOTIFY connectionStateChanged)

signals:
    void currentStateChanged(const QString &state);
    void connectedChanged();

    void ssidChanged();
    void bssidChanged();
    void wlanIpChanged();

    void connectionStateChanged();
public slots:
    void netChanged(const QNetworkConfiguration &conf);

private slots:
    void setIsOnline(bool online);
public:

    NetworkMonitor(QObject *parent = 0);
    virtual ~NetworkMonitor() {}

    enum ConnectionState { Disconnected, Wifi, NotWifi };
    Q_ENUMS(ConnectionState)

    bool isConnected() const { return mConnectionState != Disconnected; }

    QString getSsid() const;
    QString getBssid() const;
    QString getWlanIp() const;

private:
    QNetworkConfigurationManager mgr;
    QString m_state;
    QPointer<QNetworkSession> session;

    ConnectionState mConnectionState;

#ifdef Q_OS_IOS
    void statusChanged(utility::NetworkStatus n) override;
#endif

    ConnectionState connectionState() const { return mConnectionState; }
    void setConnectionState(ConnectionState connectionState);

    bool isOnWlan() const { return mConnectionState == Wifi; }
    // stub for manual testing
    void setOnWlan(bool onWlan);

};

#endif // NETWORKMONITOR_H
