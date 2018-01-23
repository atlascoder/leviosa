#include "locationcontroller.h"

LocationController::LocationController(const QString& mac):
    mMac(mac), mName("A Controller"), mLastModified(0)
{

}

QString LocationController::locationUuid() const
{
    return mLocationUuid;
}

void LocationController::setLocationUuid(const QString& locationUuid)
{
    mLocationUuid = locationUuid;
}

QString LocationController::name() const
{
    return mName;
}

void LocationController::setName(const QString &name)
{
    mName = name;
}

QString LocationController::mac() const
{
    return mMac;
}

void LocationController::setMac(const QString &mac)
{
    mMac = mac;
}

QString LocationController::ipAddress() const
{
    return mIpAddress;
}

void LocationController::setIpAddress(const QString &ipAddress)
{
    mIpAddress = ipAddress;
}

bool LocationController::isWlan() const
{
    return mIsWLAN;
}

void LocationController::setIsWlan(bool isWlan)
{
    mIsWLAN = isWlan;
}

QJsonObject LocationController::toJson() const
{
    QJsonObject json;
    json.insert("name", mName);
    json.insert("locationUuid", mLocationUuid);
    json.insert("position", QJsonValue(position()));
    return json;
}

void LocationController::withJson(const QJsonObject & json)
{
    if(!json.contains("mac")) return;
    mMac = json.value("mac").toString();

    if(!json.contains("locationUuid")) return;
    mLocationUuid = json.value("locationUuid").toString();

    if(!json.contains("name")) return;
    mName = json.value("name").toString();

    if(!json.contains("position")) return;
    setPosition(json.value("position").toInt());
}

