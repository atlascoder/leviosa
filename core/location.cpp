#include "location.h"
#include <QJsonDocument>
#include <QJsonObject>

Location::Location():
    mUuid(QUuid::createUuid()), mName("My Home"), mUtcOffset(0), mIsOnWlan(false), mIsOnline(false)
{

}

Location::Location(const QUuid & uuid):
    mUuid(uuid), mName(""), mUtcOffset(0), mIsOnWlan(false), mIsOnline(false)
{

}

Location::Location(const QString & uuidStr):
    mUuid(QUuid(uuidStr)), mName(""), mUtcOffset(0), mIsOnWlan(false), mIsOnline(false)
{

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
