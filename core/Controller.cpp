#include "Controller.h"

Controller::Controller(const QString& uuid):
    Syncable(uuid), mName("Zone 1")
{

}

QString Controller::locationUuid() const
{
    return mLocationUuid;
}

void Controller::setLocationUuid(const QString& locationUuid)
{
    mLocationUuid = locationUuid;
}

QString Controller::name() const
{
    return mName;
}

void Controller::setName(const QString &name)
{
    mName = name;
}

QString Controller::mac() const
{
    return mMac;
}

void Controller::setMac(const QString &mac)
{
    mMac = mac;
}

QJsonObject Controller::toJson() const
{
    QJsonObject json;
    json.insert("mac", mMac);
    json.insert("locationUuid", mLocationUuid);
    json.insert("name", mName);
    json.insert("position", QJsonValue(position()));
    json.insert("item_type", "controller");
    return json;
}

void Controller::withJson(const QJsonObject & json)
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
