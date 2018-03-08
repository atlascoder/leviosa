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
    QNetworkReply* mKeysReply;
    QHttpMultiPart* mMultipart;
signals:
    void requestFinished(QNetworkReply* reply);
    void failed();

    void keysWasSet();
    void setKeysFailed(const QString& msg);

public slots:
    void get(const QString& url) {
        cmdReply = mQnam->get(QNetworkRequest(QUrl::fromUserInput(url)));
        connect(cmdReply, &QNetworkReply::finished, this, &ControllerHTTPClient::finishedGet);
    }

    void postKeysAndCert(const QString& ip, const QByteArray& pubKey, const QByteArray& priKey, const QByteArray& cert);
private slots:
    void finishedGet() { if(cmdReply) emit requestFinished(cmdReply); }
    void onPostKeysFinished();
public:
    ControllerHTTPClient(QObject *parent=nullptr);
    virtual ~ControllerHTTPClient();
};

#endif // CONTROLLERHTTPCLIENT_H
