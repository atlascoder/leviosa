#include "ControllerAPI.h"
#include "shade.h"
#include <QThread>

#include <QApplication>
#include <QtConcurrent>

#include "NAM.h"
#include <QDebug>

ControllerAPI::ControllerAPI(QObject *parent)
    : QThread(parent), mCommandReply(nullptr), mConfigReply(nullptr), mScheduleReply(nullptr), mTZoneReply(nullptr), mScheduleCommands(), mControllerConfig(), mIpAddress("0.0.0.0"), mOnWlan(false), mControllerState(Wan)
{
    mQnam = new QNetworkAccessManager;
    this->start();
}

ControllerAPI::~ControllerAPI()
{
    if(mConfigReply != nullptr) mConfigReply->abort();
    if(mCommandReply != nullptr) mCommandReply->abort();
    if(mScheduleReply != nullptr) mScheduleReply->abort();
    if(mScheduleReply != nullptr) mTZoneReply->abort();
    this->quit();
    this->wait();
    delete mQnam;
}

void ControllerAPI::run(){
    ControllerHTTPClient* client = new ControllerHTTPClient;
    connect(this, &ControllerAPI::httpGet, client, &ControllerHTTPClient::get);
    connect(client, &ControllerHTTPClient::requestFinished, this, &ControllerAPI::commandFinished);
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
    QString str = QString("http://").append(mIpAddress).append("/api?");
    Shade::ShadeState state = static_cast<Shade::ShadeState>(cmd);
    switch (state){
    case Shade::Open:
        str.append("channel=").append(QString::number(channel)).append("&command=up");
        httpCommand(str);
        QObject::thread()->usleep(2000);
        httpCommand(str);
        break;
    case Shade::Close:
        str.append("channel=").append(QString::number(channel)).append("&command=down");
        httpCommand(str);
        QObject::thread()->usleep(2000);
        httpCommand(str);
        break;
    case Shade::Interim:
        str.append("channel=").append(QString::number(channel)).append("&command=stop");
        httpCommand(str);
        break;
    case Shade::Up:
        str.append("channel=").append(QString::number(channel)).append("&command=up");
        httpCommand(str);
        break;
    case Shade::Down:
        str.append("channel=").append(QString::number(channel)).append("&command=down");
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

void ControllerAPI::fetchConfig()
{
    mScheduleCommands.clear();
    setControllerState(Configuring);
    QString str = QString("http://").append(mIpAddress).append("/api");
    QUrl url = QUrl::fromUserInput(str);
    mConfigReply = mQnam->get(QNetworkRequest(url));
    connect(mConfigReply, &QNetworkReply::finished, this, &ControllerAPI::configFetchFinished);
}

void ControllerAPI::commandFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        emit commandSuccessful();
    }
    else{
        emit commandFailed();
    }
    reply->deleteLater();
    reply = nullptr;
}

void ControllerAPI::configFetchFinished()
{
    if(mConfigReply->error() == QNetworkReply::NoError){
        QString content = mConfigReply->readAll();
        mControllerConfig.parse(content);
        if(mControllerConfig.isValid()){
            qDebug() << "Valid config received for " << mIpAddress;
            setControllerState(Wlan);
            emit configFetched();
        }
        else{
            qDebug() << "Config failed for " << mIpAddress;
            setControllerState(ConfigFailed);
            emit configFailed();
        }
    }
    else{
        qDebug() << "Config failed for " << mIpAddress;
        setControllerState(ConfigFailed);
        emit configFailed();
    }
    mConfigReply->deleteLater();
    mConfigReply = nullptr;
    mIsSyncing = false;
}

void ControllerAPI::configPushFinished()
{
    if(mConfigReply->error() == QNetworkReply::NoError){
        QString content = mConfigReply->readAll();
        mControllerConfig.parse(content);
        if(mControllerConfig.isValid())
            emit configPushed();
    }
    mConfigReply->deleteLater();
    mConfigReply = nullptr;
}

void ControllerAPI::httpCommand(const QString &url)
{
    qDebug() << "WLAN CMD: " << url;
    emit httpGet(url);
}

void ControllerAPI::pushConfig()
{
    mScheduleCommands.clear();
    QString base = "http://" + mIpAddress + "/api?command=";
    for(int i = 0; i < 12; i++){
        mScheduleCommands.append(base + "rmv_sch&sch_index=0");
    }

    for(std::vector<std::unique_ptr<ScheduleLine>>::iterator i = mControllerConfig.scheduleLines().begin(); i != mControllerConfig.scheduleLines().end(); i++){
        if(i->get()->dayEU != 0)
        mScheduleCommands.append(base + "add_sch&" + i->get()->asQueryParams());
    }

    sendSchedule();
}

void ControllerAPI::sendSchedule()
{
    qDebug() << "qnam thread " << mQnam->thread() << QApplication::instance()->thread();
    if(mScheduleCommands.size() > 0){
        QUrl url = QUrl::fromUserInput(mScheduleCommands.first());
        qDebug() << "Sending: " << url;
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

ControllerConfig& ControllerAPI::currentConfig()
{
    return mControllerConfig;
}

void ControllerAPI::setControllerState(ControllerState state)
{
    if(mControllerState == state) return;
    mControllerState = state;
    emit controllerStateChanged(mMac);
}

void ControllerAPI::commandAWSRequest(int channel, int command)
{
    qDebug() << " command WAN to channel[" << channel << "] => " << command;
}

void ControllerAPI::setOnWlan(bool onWlan)
{
    if(onWlan == mOnWlan) return;
    mOnWlan = onWlan;
    if(mOnWlan)
        setControllerState(Searching);
    else
        setControllerState(Wan);
}

void ControllerAPI::setTimezone(const QString &timezone)
{
    QString str = QString("http://").append(mIpAddress).append("/api?command=set_tz&timezone=").append(timezone);
    QUrl url = QUrl::fromUserInput(str);
    mTZoneReply = mQnam->get(QNetworkRequest(url));
    connect(mTZoneReply, &QNetworkReply::finished, this, &ControllerAPI::timezoneFinished);
}

void ControllerAPI::timezoneFinished()
{
    mTZoneReply->deleteLater();
}

