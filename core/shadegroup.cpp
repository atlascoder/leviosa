#include "shadegroup.h"

ShadeGroup::ShadeGroup(int controllerId):
    Shade(), Schedule(), Syncable(), mControllerId(controllerId), mName("Motor shade")
{

}
ShadeGroup::ShadeGroup():
    Shade(), Schedule(), Syncable(), mControllerId(-1), mName("Motor shade")
{

}

int ShadeGroup::controllerId() const
{
    return mControllerId;
}

void ShadeGroup::setControllerId(int controllerId)
{
    mControllerId = controllerId;
}

QString ShadeGroup::name() const
{
    return mName;
}

void ShadeGroup::setName(const QString &name)
{
    mName = name;
}

char ShadeGroup::channel() const
{
    return mChannel;
}

void ShadeGroup::setChannel(char channel)
{
    mChannel = channel;
}

