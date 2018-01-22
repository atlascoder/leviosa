#include "locationcontroller.h"

LocationController::LocationController(const int locationId):
    mId(-1), mLocationId(locationId), mName("A Controller"), mPosition(0)
{

}

int LocationController::id() const
{
    return mId;
}

void LocationController::setId(const int id)
{
    mId = id;
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

int LocationController::position() const
{
    return mPosition;
}

void LocationController::setPosition(int position)
{
    mPosition = position;
}
