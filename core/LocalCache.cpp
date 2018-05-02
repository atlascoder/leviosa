#include "LocalCache.h"

#include <QDebug>
#include <QStringList>
#include <QJsonParseError>

#include "Location.h"
#include "Controller.h"
#include "ShadeGroup.h"


template<>
bool LocalCache<Controller>::findByKey(const QString &key, Controller** pFound) const
{
    auto found = find_if(mItems->begin(), mItems->end(), [key](const unique_ptr<Controller>& i)->bool{ return key==i->mac(); });
    if(found != mItems->end()){
        *pFound = found->get();
        return true;
    }
    else return false;
}

template<>
bool LocalCache<ShadeGroup>::findByKey(const QString &key, ShadeGroup** pFound) const
{
    QStringList mac_c = key.split("-");
    QString mac = mac_c.at(0);
    int channel = mac_c.at(1).toInt();
    auto found = find_if(mItems->begin(), mItems->end(), [channel, &mac](const unique_ptr<ShadeGroup>& i)->bool{ return channel==i->channel() && mac==i->controllerMac(); });
    if(found != mItems->end()){
        *pFound = found->get();
        return true;
    }
    else return false;
}
