#include "WlanAPI.h"
#include "shade.h"
#include <QThread>

#include <QTextStream>
WlanAPI::WlanAPI(QObject *parent) : QObject(parent)
{

}

WlanAPI::~WlanAPI()
{

}

void WlanAPI::shadeCmd(const QString &ip, char channel, int cmd)
{
    QString str = QString("http://").append(ip).append("/api?");
    Shade::ShadeState state = static_cast<Shade::ShadeState>(cmd);
    switch (state){
    case Shade::Open:
        str.append("channel=").append(QString::number(channel)).append("&command=up");
        requestGet(str);
        QObject::thread()->usleep(500*1000);
        requestGet(str);
        break;
    case Shade::Close:
        str.append("channel=").append(QString::number(channel)).append("&command=down");
        requestGet(str);
        QObject::thread()->usleep(500*1000);
        requestGet(str);
        break;
    case Shade::Interim:
        str.append("channel=").append(QString::number(channel)).append("&command=stop");
        requestGet(str);
        break;
    case Shade::Up:
        str.append("channel=").append(QString::number(channel)).append("&command=up");
        requestGet(str);
        break;
    case Shade::Down:
        str.append("channel=").append(QString::number(channel)).append("&command=down");
        requestGet(str);
        break;
    case Shade::Opened:
    case Shade::Closed:
    case Shade::FullOpened:
    case Shade::FullClosed:
        return;
    }
}

void WlanAPI::reqFinished()
{
    if(reply->error()){
        emit reqFailed();
    }
    else{
        emit reqSuccessful();
    }
}

void WlanAPI::requestGet(const QString &url)
{
    QTextStream(stdout) << "WLAN CMD: " << url << endl;
    QUrl get_url = QUrl::fromUserInput(url);
    reply = qnam.get(QNetworkRequest(get_url));
    connect(reply, &QNetworkReply::finished, this, &WlanAPI::reqFinished);
}
