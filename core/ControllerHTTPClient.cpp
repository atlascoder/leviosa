#include "ControllerHTTPClient.h"

ControllerHTTPClient::ControllerHTTPClient(QObject *parent):
    QObject(parent),
    cmdReply(nullptr),
    mKeysReply(nullptr),
    mMultipart(nullptr)
{
    mQnam = new QNetworkAccessManager;
}

ControllerHTTPClient::~ControllerHTTPClient()
{
    if(mKeysReply) mKeysReply->deleteLater();
    if(mMultipart) mMultipart->deleteLater();
    mQnam->deleteLater();
}

void ControllerHTTPClient::postKeysAndCert(const QString &ip, const QByteArray &pubKey, const QByteArray &priKey, const QByteArray &cert)
{
    if(mMultipart) mMultipart->deleteLater();
    mMultipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart pubKeyPart;
    pubKeyPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"pubKey\""));
    pubKeyPart.setBody(pubKey);
    mMultipart->append(pubKeyPart);

    QHttpPart priKeyPart;
    priKeyPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"priKey\""));
    priKeyPart.setBody(priKey);
    mMultipart->append(priKeyPart);

    QHttpPart certPart;
    certPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"cert\""));
    certPart.setBody(cert);
    mMultipart->append(certPart);

    mKeysReply = mQnam->post(QNetworkRequest(QUrl::fromUserInput(QString("http://").append(ip).append("/keys"))), mMultipart);
    connect(mKeysReply, &QNetworkReply::finished, this, &ControllerHTTPClient::onPostKeysFinished);
}

void ControllerHTTPClient::onPostKeysFinished()
{
    if(mKeysReply->error() == QNetworkReply::NoError)
        emit keysWasSet();
    else
        emit setKeysFailed(mKeysReply->errorString());
    mKeysReply->deleteLater();
    mKeysReply = nullptr;
    mMultipart->deleteLater();
    mMultipart = nullptr;
}
