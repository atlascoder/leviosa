#include "ControllerAPI.h"
#include "Shade.h"
#include <QThread>

#include <QApplication>
#include <QtConcurrent>

#include <QHttpMultiPart>
#include <memory>

#include "aws/CognitoSyncCommand.h"

#include "NAM.h"
#include <QDebug>

ControllerAPI::ControllerAPI(QObject *parent):
    QThread(parent),
    mCommandReply(nullptr),
    mScheduleReply(nullptr),
    mScheduleCommands(),
    mControllerConfig(),
    mIpAddress("0.0.0.0"),
    mControllerState(Wan)
{
    mQnam = new QNetworkAccessManager;
    this->start();
}

ControllerAPI::~ControllerAPI()
{
    if(mCommandReply != nullptr) mCommandReply->abort();
    if(mScheduleReply != nullptr) mScheduleReply->abort();
    this->quit();
    this->wait();
    delete mQnam;
    mCommandRequest.cancel();
    mCommandRequest.waitForFinished();
}

void ControllerAPI::run(){

    ControllerHTTPClient* client = new ControllerHTTPClient;

    connect(this, &ControllerAPI::httpGet, client, &ControllerHTTPClient::post);
    connect(this, &ControllerAPI::postKeysAndCertificate, client, &ControllerHTTPClient::postKeysAndCert);
    connect(client, &ControllerHTTPClient::requestFinished, this, &ControllerAPI::commandFinished);
    connect(client, &ControllerHTTPClient::keysWasSet, this, &ControllerAPI::onKeysAndCertSet);
    connect(client, &ControllerHTTPClient::setKeysFailed, this, &ControllerAPI::onKeysAndCertSetFailed);

    exec();
    client->deleteLater();
}

void ControllerAPI::setIpAddress(const QString &ipAddress)
{
    if(mIpAddress == ipAddress) return;
    mIpAddress = ipAddress;
    emit ipAddressChanged();
}

void ControllerAPI::commandHTTPRequest(int channel, int cmd)
{
    QString str = QString("http://").append(mIpAddress).append("/command/");
    Shade::ShadeState state = static_cast<Shade::ShadeState>(cmd);
    switch (state){
    case Shade::Open:
        str.append("open/").append(QString::number(channel));
        httpCommand(str);
        break;
    case Shade::Close:
        str.append("close/").append(QString::number(channel));
        httpCommand(str);
        break;
    case Shade::Interim:
        str.append("stop/").append(QString::number(channel));
        httpCommand(str);
        break;
    case Shade::Up:
        str.append("up/").append(QString::number(channel));
        httpCommand(str);
        break;
    case Shade::Down:
        str.append("down/").append(QString::number(channel));
        httpCommand(str);
        break;
    case Shade::Opened:
    case Shade::Closed:
    case Shade::FullOpened:
    case Shade::FullClosed:
        return;
    }
}

void ControllerAPI::shadeCmd(char channel, int cmd)
{
    if(mControllerState == Wlan)
        commandHTTPRequest(channel, cmd);
    else
        commandAWSRequest(channel, cmd);
}

void ControllerAPI::commandFinished(QNetworkReply *reply)
{
    if(reply != nullptr){
        if (reply->error() == QNetworkReply::NoError)
            emit commandSuccessful();
        else
            emit commandFailed();
        reply->deleteLater();
    }
    else{
        emit commandFailed();
    }
}

void ControllerAPI::httpCommand(const QString &url)
{
    qDebug() << "WLAN CMD: " << url;
    emit httpGet(url);
}

void ControllerAPI::sendSchedule()
{
    if(mScheduleCommands.size() > 0){
        QUrl url = QUrl::fromUserInput(mScheduleCommands.first());
        mScheduleCommands.pop_front();
        mScheduleReply = mQnam->get(QNetworkRequest(url));
        connect(mScheduleReply, &QNetworkReply::finished, this, &ControllerAPI::scheduleFinished);
    }
    else
        emit scheduleSet();
}

void ControllerAPI::scheduleFinished()
{
    if(mScheduleReply->error() == QNetworkReply::NoError){
        mScheduleReply->deleteLater();
        mScheduleReply=nullptr;
        sendSchedule();
    }
    else
        emit scheduleFailed();
}

void ControllerAPI::setConfig(const ControllerConfig &config)
{
    mControllerConfig = config;
    setControllerState(Wlan);
}

ControllerConfig& ControllerAPI::currentConfig()
{
    return mControllerConfig;
}

void ControllerAPI::setControllerState(ControllerState state)
{
    qDebug() << "Set controller state [" << mMac << "] : " << mControllerState << " => " << state;
//    if(mControllerState == state) return;
    mControllerState = state;
    emit controllerStateChanged(mMac);
}

QString ControllerAPI::commandCode2String(int command)
{
    Shade::ShadeState state = static_cast<Shade::ShadeState>(command);
    switch (state){
    case Shade::Open:
        return "open";
    case Shade::Close:
        return "close";
    case Shade::Up:
        return "up";
    case Shade::Down:
        return "down";
    case Shade::Interim:
        return "stop";
    default:
        return "";
    }
}

void ControllerAPI::commandAWSRequest(int channel, int command)
{

    QString cmd = commandCode2String(command);
    if (cmd.isEmpty()) return;

    qDebug() << " command WAN to channel[" << channel << "] => " << command;

    if (mCommandRequest.isCanceled() || mCommandRequest.isFinished()) {
        mCommandRequest = QtConcurrent::run(this, &ControllerAPI::sendAwsCommand, cmd, channel);
    }
}

void ControllerAPI::sendAwsCommand(const QString& command, int channel)
{
    std::unique_ptr<CognitoSyncCommand> commandCall(new CognitoSyncCommand);
    commandCall->sendCommand(mMac, command, channel);
    if (commandCall->isSuccessful()) {
        qDebug() << "Command sent trough AWS";
        emit cloudCommandSent();
    }
    else {
        qDebug() << "Sending trough AWS failed";
        emit cloudCommandFailed();
    }
}

void ControllerAPI::pushKeysAndCert(const QByteArray &pubKey, const QByteArray &priKey, const QByteArray &cert)
{
    mPubKey = pubKey;
    mPriKey = priKey;
    mCert = cert;
    emit postKeysAndCertificate(mIpAddress, mPubKey, mPriKey, mCert);
}

void ControllerAPI::onKeysAndCertSet()
{
    emit keysWasSet();
    mPubKey.clear();
    mPriKey.clear();
    mCert.clear();
}

void ControllerAPI::onKeysAndCertSetFailed(const QString &msg)
{
    emit setKeysFailed(msg);
    mPubKey.clear();
    mPriKey.clear();
    mCert.clear();
}

void ControllerAPI::setBssid(const QString &bssid)
{
    mBssid = bssid;
}

QString ControllerAPI::bssid() const
{
    return mBssid;
}
