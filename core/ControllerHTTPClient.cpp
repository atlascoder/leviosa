#include "ControllerHTTPClient.h"

ControllerHTTPClient::ControllerHTTPClient(QObject *parent):
    QObject(parent),
    cmdReply(nullptr),
    mSetupReply(nullptr),
    mMultipart(nullptr)
{
    mQnam = new QNetworkAccessManager;
}

ControllerHTTPClient::~ControllerHTTPClient()
{
    if(mSetupReply) mSetupReply->deleteLater();
    if(mMultipart) mMultipart->deleteLater();
    mQnam->deleteLater();
}

void ControllerHTTPClient::postSetup(const QString &ip, const QByteArray &pubKey, const QByteArray &priKey, const QByteArray &cert, const int controllerId, const QByteArray& schedule)
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

    QNetworkRequest req = QNetworkRequest(QUrl::fromUserInput(QString("http://").append(ip).append("/setup")));
    req.setRawHeader("com.leviosashades.controllerId", QString::number(controllerId).toLatin1());
    req.setRawHeader("com.leviosashades.schedule", schedule);

    mSetupReply = mQnam->post(req, mMultipart);
    connect(mSetupReply, &QNetworkReply::finished, this, &ControllerHTTPClient::onPostKeysFinished);
}

void ControllerHTTPClient::onPostKeysFinished()
{
    if(mSetupReply->error() == QNetworkReply::NoError) {
        qDebug() << "Keys was set";
        emit keysWasSet();
    }
    else {
        qDebug() << "Keys setting error: " << mSetupReply->errorString();
        emit setKeysFailed(mSetupReply->errorString());
    }
    mSetupReply->deleteLater();
    mSetupReply = nullptr;
    mMultipart->deleteLater();
    mMultipart = nullptr;
}

void ControllerHTTPClient::post(const QString &url, const QByteArray& data)
{
    cmdReply = mQnam->post(QNetworkRequest(QUrl::fromUserInput(url)), data);
    connect(cmdReply, &QNetworkReply::finished, this, &ControllerHTTPClient::finishedPost);
}
