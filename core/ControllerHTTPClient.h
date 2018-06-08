#ifndef CONTROLLERHTTPCLIENT_H
#define CONTROLLERHTTPCLIENT_H

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QByteArray>

class ControllerHTTPClient : public QObject
{
    Q_OBJECT

    QNetworkAccessManager* mQnam;
    QNetworkReply* cmdReply;
    QNetworkReply* mSetupReply;
    QHttpMultiPart* mMultipart;
signals:
    void requestFinished(QNetworkReply* reply);
    void failed();

    void keysWasSet();
    void setKeysFailed(const QString& msg);

public slots:
    void post(const QString& url, const QByteArray& data);

    void postSetup(const QString& ip, const QByteArray& pubKey, const QByteArray& priKey, const QByteArray& cert, const int controllerId, const QByteArray& schedule);
private slots:
    void finishedPost() { if(cmdReply) emit requestFinished(cmdReply); }
    void postFailed(QNetworkReply::NetworkError code);
    void onPostKeysFinished();
public:
    ControllerHTTPClient(QObject *parent=nullptr);
    virtual ~ControllerHTTPClient();
};

#endif // CONTROLLERHTTPCLIENT_H
