#ifndef ESPTOUCHSETUP_H
#define ESPTOUCHSETUP_H

#include <QObject>
#include <QString>
#include <QThread>

#ifdef Q_OS_IOS
#include "esptouch/EspTouchTaskIOS.h"
#endif

#ifdef Q_OS_ANDROID
#include "esptouch/EspTouchTaskAndroid.h"
#endif

class EspTouchSetup : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString password READ getPassword WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(QString ssid READ getSsid WRITE setSsid NOTIFY ssidChanged)
    Q_PROPERTY(QString bssid READ getBssid WRITE setBssid NOTIFY bssidChanged)
    Q_PROPERTY(QString ipAddress READ getAddress WRITE setAddress NOTIFY addressChanged)

    Q_PROPERTY(QString hostIP READ getHostIP WRITE setHostIP NOTIFY hostIPChanged)
    Q_PROPERTY(QString hostMac READ getHostMac WRITE setHostMac NOTIFY hostMacChanged)

    bool mIsRunning;
    QString mPassword;
    QString mSsid;
    QString mBssid;
    QString mAddress;

    QString mHostIP;
    QString mHostMac;

    void runEspTouch();
    void stopEspTouch();

#ifdef Q_OS_IOS
    EspTouchTaskIOS *mTask;
#endif

#ifdef Q_OS_ANDROID
    EspTouchTaskAndroid *mTask;
#endif

public:
    EspTouchSetup(QObject *parent = 0);

    QString getPassword() const;
    void setPassword(const QString &password);

    bool isRunning() const;
    void setRunning(bool isRunning);

    QString getBssid() const;
    void setBssid(const QString &bssid);

    QString getSsid() const;
    void setSsid(const QString &ssid);

    QString getAddress() const;
    void setAddress(const QString &address);

    QString getHostIP() const;
    void setHostIP(const QString &hostIP);

    QString getHostMac() const;
    void setHostMac(const QString& hostMac);
signals:
    void runningChanged();
    void passwordChanged();
    void ssidChanged();
    void bssidChanged();
    void addressChanged();

    void hostFound(const QString conf);
    void finished();

    void hostIPChanged();
    void hostMacChanged();
    void failed();

public slots:
    void gotHost(const QString& str);
    void stopped();
};

#endif // ESPTOUCHSETUP_H
