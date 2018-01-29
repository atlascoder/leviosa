#include "SyncableRecord.h"
#include <QStringList>
#include "location.h"
#include "controller.h"
#include "shadegroup.h"

template<>
QString SyncableRecord<Location>::recordKey(const Location& location) const
{
    return location.uuid().toString();
}

template<>
std::unique_ptr<Location> SyncableRecord<Location>::fromJson(const QString& key, const QJsonObject &json) const
{
    std::unique_ptr<Location> loc(new Location(key));
    loc->withJson(json);
    return loc;
}

template<>
QString SyncableRecord<Controller>::recordKey(const Controller& controller) const
{
    return controller.mac();
}

template<>
std::unique_ptr<Controller> SyncableRecord<Controller>::fromJson(const QString& key, const QJsonObject &json) const
{
    std::unique_ptr<Controller> loc(new Controller(key));
    loc->withJson(json);
    return loc;
}

template<>
QString SyncableRecord<ShadeGroup>::recordKey(const ShadeGroup& shadeGroup) const
{
//    return QString::number(shadeGroup.channel());
    return shadeGroup.controllerMac().append("-").append(QString::number(shadeGroup.channel()));
}

template<>
std::unique_ptr<ShadeGroup> SyncableRecord<ShadeGroup>::fromJson(const QString& key, const QJsonObject &json) const
{
    QStringList mac_c = key.split("-");
    std::unique_ptr<ShadeGroup> loc(new ShadeGroup(mac_c.at(1).toInt()));
    loc->withJson(json);
    loc->setControllerMac(mac_c.at(0));
    return loc;
}


template<>
bool SyncableRecord<Location>::findByKey(const QString &key, Location** pFound) const
{
    auto found = find_if(mItems->begin(), mItems->end(), [key](const unique_ptr<Location>& i)->bool{ return key==i->uuid().toString(); });
    if(found != mItems->end()){
        *pFound = found->get();
        return true;
    }
    else return false;
}

template<>
bool SyncableRecord<Controller>::findByKey(const QString &key, Controller** pFound) const
{
    auto found = find_if(mItems->begin(), mItems->end(), [key](const unique_ptr<Controller>& i)->bool{ return key==i->mac(); });
    if(found != mItems->end()){
        *pFound = found->get();
        return true;
    }
    else return false;
}

template<>
bool SyncableRecord<ShadeGroup>::findByKey(const QString &key, ShadeGroup** pFound) const
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
