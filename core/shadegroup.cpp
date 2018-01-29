#include "shadegroup.h"

ShadeGroup::ShadeGroup(char channel):
    Shade(), Schedule(), mChannel(channel), mControllerMac("00:00:00:00:00:00"), mName("Motor shade")
{

}
ShadeGroup::ShadeGroup():
    Shade(), Schedule(), mChannel(1), mControllerMac("00:00:00:00:00:00"), mName("Motor shade")
{

}

QString ShadeGroup::controllerMac() const
{
    return mControllerMac;
}

void ShadeGroup::setControllerMac(const QString& mac)
{
    mControllerMac = mac;
}

QString ShadeGroup::name() const
{
    return mName;
}

void ShadeGroup::setName(const QString &name)
{
    mName = name;
}

char ShadeGroup::channel() const
{
    return mChannel;
}

void ShadeGroup::setChannel(char channel)
{
    mChannel = channel;
}

QJsonObject ShadeGroup::toJson() const
{
    QJsonObject json;
    json.insert("controllerMac", mControllerMac);
    json.insert("name", mName);
    json.insert("position", mPosition);
    json.insert("openAt", mOpenAt);
    json.insert("closeAt", mCloseAt);
    json.insert("days", mDaysMask);
}

void ShadeGroup::withJson(const QJsonObject &json)
{
    if(!json.contains("controllerMac")) return;
    mControllerMac = json.value("controllerMac").toString();

    if(!json.contains("name")) return;
    mName = json.value("name").toString();

    if(!json.contains("position")) return;
    mPosition = json.value("position").toInt();

    if(!json.contains("openAt")) return;
    mOpenAt = json.value("openAt").toInt();

    if(!json.contains("closeAt")) return;
    mCloseAt = json.value("closeAt").toInt();

    if(!json.contains("days")) return;
    mDaysMask = json.value("days").toInt();
}
