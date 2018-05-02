#include "Location.h"
#include <QJsonDocument>
#include <QJsonObject>

Location::Location():
    Syncable(), mName("My Home"), mUtcOffset(0), mIsOnWlan(false), mIsOnline(false)
{
    setUuid(QUuid::createUuid().toString());
}

Location::Location(const QString & uuidStr):
    Syncable(uuidStr), mName(""), mUtcOffset(0), mIsOnWlan(false), mIsOnline(false)
{
    setUuid(uuidStr);
}


QString Location::bssid() const
{
    return mBssid;
}

void Location::setBssid(const QString &bssid)
{
    mBssid = bssid;
}

QString Location::name() const
{
    return mName;
}

void Location::setName(const QString &name)
{
    mName = name;
}

int Location::utcOffset() const
{
    return mUtcOffset;
}

void Location::setUtcOffset(int offset)
{
    mUtcOffset = offset;
}

bool Location::isOnWlan() const
{
    return mIsOnWlan;
}

void Location::setIsOnWlan(bool isOnWlan)
{
    mIsOnWlan = isOnWlan;
}

bool Location::isOnline() const
{
    return mIsOnline;
}

void Location::setOnline(bool isOnline)
{
    mIsOnline = isOnline;
}

QJsonObject Location::toJson() const
{
    QJsonObject json;
    json.insert("name", mName);
    json.insert("bssid", mBssid);
    json.insert("utcOffset", QJsonValue(mUtcOffset));
    json.insert("position", QJsonValue(position()));
    json.insert("item_type", "location");
    return json;
}

void Location::withJson(const QJsonObject & json)
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