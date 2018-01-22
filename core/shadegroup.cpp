#include "shadesgroup.h"

ShadesGroup::ShadesGroup(int controllerId):
    Shade(-1), mId(-1), mControllerId(controllerId), mName("Motor shade"), mPosition(0)
{

}

int ShadesGroup::id() const
{
    return mId;
}

void ShadesGroup::setId(int id)
{
    mId = id;
}

int ShadesGroup::controllerId() const
{
    return mControllerId;
}

void ShadesGroup::setControllerId(int controllerId)
{
    mControllerId = controllerId;
}

QString ShadesGroup::name() const
{
    return mName;
}

void ShadesGroup::setName(const QString &name)
{
    mName = name;
}

int ShadesGroup::position() const
{
    return mPosition;
}

void ShadesGroup::setPosition(int position)
{
    mPosition = position;
}
