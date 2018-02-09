#ifndef CONTROLLERHTTPCLIENT_H
#define CONTROLLERHTTPCLIENT_H

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ControllerHTTPClient : public QObject
{
    Q_OBJECT

    QNetworkAccessManager* mQnam;
    QNetworkReply* reply;
signals:
    void requestFinished(QNetworkReply* reply);
    void failed();

public slots:
    void get(const QString& url) {
        reply = mQnam->get(QNetworkRequest(QUrl::fromUserInput(url)));
        connect(reply, &QNetworkReply::finished, this, &ControllerHTTPClient::finishedGet);
    }

private slots:
    void finishedGet() { if(reply) emit requestFinished(reply); }
public:
    ControllerHTTPClient(QObject *parent=0): QObject(parent), reply(nullptr)
    {
        mQnam = new QNetworkAccessManager;
    }
    virtual ~ControllerHTTPClient() {
        delete mQnam;
    }
};

#endif // CONTROLLERHTTPCLIENT_H
