#include "userlocation.h"
#include <QJsonDocument>
#include <QJsonObject>

UserLocation::UserLocation():
    mUuid(QUuid::createUuid()), mName("My Home"), mUtcOffset(0), mIsOnWlan(false), mIsOnline(false)
{

}

UserLocation::UserLocation(const QUuid & uuid):
    mUuid(uuid), mName(""), mUtcOffset(0), mIsOnWlan(false), mIsOnline(false)
{

}

UserLocation::UserLocation(const QString & uuidStr):
    mUuid(QUuid(uuidStr)), mName(""), mUtcOffset(0), mIsOnWlan(false), mIsOnline(false)
{

}


QString UserLocation::bssid() const
{
    return mBssid;
}

void UserLocation::setBssid(const QString &bssid)
{
    mBssid = bssid;
}

QString UserLocation::name() const
{
    return mName;
}

void UserLocation::setName(const QString &name)
{
    mName = name;
}

int UserLocation::utcOffset() const
{
    return mUtcOffset;
}

void UserLocation::setUtcOffset(int offset)
{
    mUtcOffset = offset;
}

bool UserLocation::isOnWlan() const
{
    return mIsOnWlan;
}

void UserLocation::setIsOnWlan(bool isOnWlan)
{
    mIsOnWlan = isOnWlan;
}

bool UserLocation::isOnline() const
{
    return mIsOnline;
}

void UserLocation::setOnline(bool isOnline)
{
    mIsOnline = isOnline;
}

QJsonObject UserLocation::toJson() const
{
    QJsonObject json;
    json.insert("name", mName);
    json.insert("bssid", mBssid);
    json.insert("utcOffset", QJsonValue(mUtcOffset));
    json.insert("position", QJsonValue(position()));
    return json;
}

void UserLocation::withJson(const QJsonObject & json)
{
    if(!json.contains("name")) return;
    mName = json.value("name").toString();

    if(!json.contains("bssid")) return;
    mBssid = json.value("bssid").toString();

    if(!json.contains("utcOffset")) return;
    mUtcOffset = json.value("utcOffset").toInt();

    if(!json.contains("position")) return;
    setPosition(json.value("position").toInt());
}
