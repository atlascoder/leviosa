#include "userlocation.h"


UserLocation::UserLocation(const QString &name):
    mId(-1), mName(name), mUtcOffset(0), mIsLocal(false), mIsOnline(false)
{
}

int UserLocation::id() const {
    return mId;
}

void UserLocation::setId(const int id)
{
    mId = id;
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

int UserLocation::position() const
{
    return mPosition;
}

void UserLocation::setPosition(int position)
{
    mPosition = position;
}

bool UserLocation::isLocal() const
{
    return mIsLocal;
}

void UserLocation::setLocal(bool isLocal)
{
    mIsLocal = isLocal;
}

bool UserLocation::isOnline() const
{
    return mIsOnline;
}

void UserLocation::setOnline(bool isOnline)
{
    mIsOnline = isOnline;
}
