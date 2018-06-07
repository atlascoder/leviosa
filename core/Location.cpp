#include "Location.h"
#include <QJsonDocument>
#include <QJsonObject>

Location::Location():
    Syncable(), mName("Home"), mTimezone("EST"), mIsOnWlan(false), mIsOnline(false)
{
    setUuid(QUuid::createUuid().toString());
}

Location::Location(const QString & uuidStr):
    Syncable(uuidStr), mName(""), mIsOnWlan(false), mIsOnline(false)
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

QString Location::timezone() const
{
    return mTimezone;
}

void Location::setTimezone(const QString &tzone)
{
    mTimezone = tzone;
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
    json.insert("timezone", mTimezone);
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

    if(!json.contains("position")) return;
    setPosition(json.value("position").toInt());

    if (!json.contains("timezone")) return;
    mTimezone = json.value("timezone").toString();
}
