#include "WanAPI.h"
#include <QDebug>

WanAPI::WanAPI(QObject *parent): QObject(parent)
{

}

WanAPI::~WanAPI()
{

}

void WanAPI::shadeCmd(const QString &mac, char channel, int cmd)
{
    qDebug() << "Sending WAN command " << cmd << " to channel " << channel << " of " << mac;
}

void WanAPI::reqFinished()
{

}
