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
#include <queue>
#include "ControllerConfig.h"
#include "ControllerHTTPClient.h"

using namespace std;

class ControllerAPI : public QThread
{
    Q_OBJECT

    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)

    QUrl url;
    QNetworkReply* mCommandReply;
    QNetworkReply* mScheduleReply;
    QStringList mScheduleCommands;
    ControllerConfig mControllerConfig;
    QString mIpAddress;

    bool mIsSyncing;

    QString mMac;
    QString mBssid;

    QNetworkAccessManager* mQnam;

    QFuture<void> mCommandRequest;

    QByteArray mPubKey;
    QByteArray mPriKey;
    QByteArray mCert;

    void run() override;
public:
    ControllerAPI(QObject *parent = 0);
    virtual ~ControllerAPI();

    enum ControllerState { Wan, Searching, NotFound, Wlan };
    Q_ENUMS(ControllerState)

    void setIpAddress(const QString& ipAddress);
    QString ipAddress() const { return mIpAddress; }

    void shadeCmd(char channel, int cmd);

    void setBssid(const QString& bssid);
    QString bssid() const;

    void setConfig(const ControllerConfig& config);
    ControllerConfig& currentConfig();

    ControllerState controllerState() const { return mControllerState; }
    void setControllerState(ControllerState state);

    QString mac() const { return mMac; }
    void setMac(const QString& mac) { mMac = mac; }

    void pushKeysAndCert(const QByteArray& pubKey, const QByteArray& priKey, const QByteArray& cert);

    bool onWlan() const { return mControllerState == Wlan; }

    void sendAwsCommand(const QString& command, int channel);

    static QString commandCode2String(int command);

    void updateSchedule(const QString& scheduleJson);

signals:
    void ipAddressChanged();

    void commandSuccessful();
    void commandFailed();
    void scheduleSet();
    void scheduleFailed();

    void sendCommand(int channel, int command);
    void httpPost(const QString& url, const QByteArray& data);
    void controllerStateChanged(const QString& mac);

    void sendCloudCommand(const QString& mac, const QString& command, int channel);

    void cloudCommandSent();
    void cloudCommandFailed();

private slots:
    void commandFinished(QNetworkReply *reply);
    void scheduleFinished();

    void commandHTTPRequest(int channel, int command);
    void commandAWSRequest(int channel, int command);

private:
    ControllerState mControllerState;

    void sendSchedule();
    void httpCommand(const QString& url);
};

#endif // CONTROLLERAPI_H
