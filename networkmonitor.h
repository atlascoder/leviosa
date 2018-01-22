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
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString onlineState READ getState NOTIFY currentStateChanged)
    Q_PROPERTY(QString ssid READ getSsid NOTIFY ssidChanged)
    Q_PROPERTY(QString bssid READ getBssid NOTIFY bssidChanged)
    Q_PROPERTY(QString wlanIp READ getWlanIp NOTIFY wlanIpChanged)

    Q_PROPERTY(bool onWlan READ getOnWlan NOTIFY onWlanChanged)

    QNetworkConfigurationManager mgr;
    QString m_state;
    QPointer<QNetworkSession> session;
    QString getState() const { return m_state; }
    void changeState(bool isConnected);

    QString mSsid;
    QString mBssid;

    bool mIsOnline;
    bool mIsWLAN;
#ifdef Q_OS_IOS
    void statusChanged(utility::NetworkStatus n) override;
#endif
signals:
    void currentStateChanged(const QString &state);
    void connectedChanged();

    void ssidChanged();
    void bssidChanged();
    void wlanIpChanged();

    void onWlanChanged();
public slots:
    void isOnline(bool connected);
    void netChanged(const QNetworkConfiguration &conf);

public:
    NetworkMonitor(QObject *parent = 0);

    bool isConnected() const;

    QString getSsid() const;
    QString getBssid() const;
    QString getWlanIp() const;

    bool getOnWlan() const;

};

#endif // NETWORKMONITOR_H
