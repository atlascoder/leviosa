#include "ShadeGroup.h"

ShadeGroup::ShadeGroup(const QString &uuid):
    Syncable(uuid), Shade(), Schedule(), mChannel(1), mControllerMac("00:00:00:00:00:00"), mName("New Shade")
{

}

ShadeGroup::ShadeGroup(char channel):
    mChannel(channel), mControllerMac("00:00:00:00:00:00"), mName("New Shade")
{

}
ShadeGroup::ShadeGroup():
    mChannel(1), mControllerMac("00:00:00:00:00:00"), mName("New Shade")
{

}

QString ShadeGroup::controllerMac() const
{
    return mControllerMac;
}

void ShadeGroup::setControllerMac(const QString& mac)
{
    mControllerMac = mac.toUpper();
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
    json.insert("channel", mChannel);
    json.insert("item_type", "group");
    return json;
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

    if(!json.contains("channel")) return;
    mChannel = json.value("channel").toInt();

}

