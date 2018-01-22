#include "locationcontroller.h"

LocationController::LocationController(const int locationId):
    Syncable(), mLocationId(locationId), mName("A Controller")
{

}

int LocationController::locationId() const
{
    return mLocationId;
}

void LocationController::setLocationId(int locationId)
{
    mLocationId = locationId;
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
