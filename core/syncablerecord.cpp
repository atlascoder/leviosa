#include "SyncableRecord.h"
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
    return QString::number(shadeGroup.channel());
}

template<>
std::unique_ptr<ShadeGroup> SyncableRecord<ShadeGroup>::fromJson(const QString& key, const QJsonObject &json) const
{
    std::unique_ptr<ShadeGroup> loc(new ShadeGroup(key.toInt()));
    loc->withJson(json);
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
    int channel = key.toInt();
    auto found = find_if(mItems->begin(), mItems->end(), [channel](const unique_ptr<ShadeGroup>& i)->bool{ return channel==i->channel(); });
    if(found != mItems->end()){
        *pFound = found->get();
        return true;
    }
    else return false;
}
