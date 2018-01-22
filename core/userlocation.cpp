#include "userlocation.h"
#include <QJsonDocument>
#include <QJsonObject>

UserLocation::UserLocation(const QString &name):
    Syncable(), mName(name), mUtcOffset(0), mIsOnWlan(false), mIsOnline(false)
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

QString UserLocation::ssid() const
{
    return mSsid;
}

void UserLocation::setSsid(const QString &ssid)
{
    mSsid = ssid;
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

QString UserLocation::syncContent() const
{
    QJsonObject json;
    json.insert("name", mName);
    json.insert("bssid", mBssid);
    json.insert("utcOffset", QJsonValue(mUtcOffset));
    json.insert("position", QJsonValue(position()));
    return QString(QJsonDocument(json).toJson());
}

void UserLocation::setSyncContent(const QString &syncContent)
{
    setUpdated(false);
    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(QByteArray::fromStdString(syncContent.toStdString()), &error);
    if(error.error != error.NoError || !json.isObject()) return;
    QJsonObject obj = json.object();

    if(!obj.contains("name")) return;
    mName = obj.value("name").toString();

    if(!obj.contains("bssid")) return;
    mBssid = obj.value("bssid").toString();

    if(!obj.contains("utcOffset")) return;
    mUtcOffset = obj.value("utcOffset").toInt();

    if(!obj.contains("position")) return;
    setPosition(obj.value("position").toInt());

    setUpdated(true);
}
