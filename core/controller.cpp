#include "controller.h"

Controller::Controller(const QString& mac):
    mMac(mac), mName("Controller 1")//, mControllerAPI(new ControllerAPI)
{
//    mControllerAPI->setMac(mMac);
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
//    mControllerAPI->setMac(mac);
}

//QString Controller::ipAddress() const
//{
//    return mControllerAPI->ipAddress();
//}

//void Controller::setIpAddress(const QString &ipAddress)
//{
//    mControllerAPI->setIpAddress(ipAddress);
//}

//bool Controller::onWlan() const
//{
//    return mControllerAPI->onWlan();
//}

//void Controller::setOnWlan(bool isWlan)
//{
//    mControllerAPI->setOnWlan(isWlan);
//}

QJsonObject Controller::toJson() const
{
    QJsonObject json;
    json.insert("name", mName);
    json.insert("locationUuid", mLocationUuid);
    json.insert("position", QJsonValue(position()));
    return json;
}

void Controller::withJson(const QJsonObject & json)
{
    if(!json.contains("locationUuid")) return;
    mLocationUuid = json.value("locationUuid").toString();

    if(!json.contains("name")) return;
    mName = json.value("name").toString();

    if(!json.contains("position")) return;
    setPosition(json.value("position").toInt());
}
