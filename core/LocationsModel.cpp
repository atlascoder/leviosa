#include "LocationsModel.h"
#include "UserData.h"
#include "TimeZoneModel.h"

using namespace std;

LocationsModel::LocationsModel(QObject* parent):
    QAbstractListModel(parent),
    mLocations(new vector<shared_ptr<Location>>)
{
    connect(&UserData::instance(), &UserData::dataUpdated, this, &LocationsModel::reloadData);
    reloadData();
}

void LocationsModel::reloadData()
{
    beginResetModel();
    mLocations = UserData::instance().locations();
    endResetModel();
    emit singleChanged();
}

int LocationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(mLocations->size());
}

QVariant LocationsModel::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index)){
        return QVariant();
    }

    shared_ptr<Location> location = mLocations->at(index.row());

    switch(role){
    case Roles::UuidRole:
        return location->uuid();
    case Roles::NameRole:
        return location->name();
    case Roles::PositionRole:
        return location->position();
    case Roles::BssidRole:
        return location->bssid();
    case Roles::UtcOffsetRole:
        return location->utcOffset();
    case Roles::TimeZoneRole:
        return TimeZoneModel::signatureByOffset(location->utcOffset());
    default:
        return QVariant();
    }
}

bool LocationsModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(!isIndexValid(index)) return false;

    shared_ptr<Location> location = mLocations->at(index.row());
    switch (role) {
    case Roles::NameRole:
        location->setName(value.toString());
        break;
    default:
        return false;
    }
    location->setChanged(true);
    UserData::instance().persistChanged(location);
    emit dataChanged(index, index);
    return true;
}

bool LocationsModel::isIndexValid(const QModelIndex &index) const
{
    return index.row() >= 0 && index.row() < static_cast<int>(mLocations->size()) && index.isValid();
}

QHash<int, QByteArray> LocationsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::UuidRole] = "uuid";
    roles[Roles::NameRole] = "name";
    roles[Roles::UtcOffsetRole] = "utcOffset";
    roles[Roles::TimeZoneRole] = "timezone";
    roles[Roles::PositionRole] = "position";
    roles[Roles::BssidRole] = "bssid";
    return roles;
}

bool LocationsModel::single() const
{
    return mLocations->size() < 2;
}
