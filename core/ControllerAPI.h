#ifndef CONTROLLERAPI_H
#define CONTROLLERAPI_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>
#include <QFuture>
#include <QByteArray>

#include "ControllerConfig.h"
#include "ControllerHTTPClient.h"

class ControllerAPI : public QThread
{
    Q_OBJECT

    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)

    QUrl url;
    QNetworkReply* mCommandReply;
    QNetworkReply* mConfigReply;
    QNetworkReply* mScheduleReply;
    QNetworkReply* mTZoneReply;
    QStringList mScheduleCommands;
    ControllerConfig mControllerConfig;
    QString mIpAddress;

    bool mIsSyncing;

    QString mMac;

    QNetworkAccessManager* mQnam;

    QFuture<void> mCommandRequest;

    QByteArray mPubKey;
    QByteArray mPriKey;
    QByteArray mCert;

    void run() override;
public:
    ControllerAPI(QObject *parent = 0);
    virtual ~ControllerAPI();

    enum ControllerState { Wan, Searching, NotFound, Configuring, ConfigFailed, Wlan };
    Q_ENUMS(ControllerState)

    void setIpAddress(const QString& ipAddress);
    QString ipAddress() const { return mIpAddress; }

    void shadeCmd(char channel, int cmd);

    void fetchConfig();
    void pushConfig();
    void setTimezone(const QString& timezone);

    void setConfig(const ControllerConfig& config);
    ControllerConfig& currentConfig();

    ControllerState controllerState() const { return mControllerState; }
    void setControllerState(ControllerState state);

    void setMac(const QString& mac) { mMac = mac; }

    void pushConfigAndWait();

    void pushKeysAndCert(const QByteArray& pubKey, const QByteArray& priKey, const QByteArray& cert);

    bool onWlan() const { return mControllerState != Wan; }

signals:
    void ipAddressChanged();

    void commandSuccessful();
    void commandFailed();
    void configFetched();
    void configFailed();
    void configPushed();
    void scheduleSet();
    void scheduleFailed();
    void keysWasSet();
    void setKeysFailed(const QString& msg);

    void postKeysAndCertificate(const QString& ip, const QByteArray& pubKey, const QByteArray& priKey, const QByteArray& cert);
    void sendCommand(int channel, int command);
    void httpGet(const QString& url);
    void controllerStateChanged(const QString& mac);

    void sendCloudCommand(const QString& mac, const QString& command, int channel);
private slots:
    void commandFinished(QNetworkReply *reply);
    void configFetchFinished();
    void configPushFinished();
    void scheduleFinished();
    void timezoneFinished();

    void commandHTTPRequest(int channel, int command);
    void commandAWSRequest(int channel, int command);

    void cloudCommandSent();
    void cloudCommandFailed();

    void onKeysAndCertSet();
    void onKeysAndCertSetFailed(const QString& msg);
private:
    ControllerState mControllerState;

    void sendSchedule();
    void httpCommand(const QString& url);
};

#endif // CONTROLLERAPI_H
