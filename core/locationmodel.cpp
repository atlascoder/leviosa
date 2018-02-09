#include "locationmodel.h"
#include <QTextStream>
#include <QThread>
#include "UserData.h"

using namespace std;

LocationModel::LocationModel(QObject* parent):
    QAbstractListModel(parent),
    mLocations(UserData::instance().mLocations.get()),
    mSelectedLocationIndex(QModelIndex())
{
    connect(&UserData::instance(), &UserData::dataUpdated, this, &LocationModel::updateModel);
}

QModelIndex LocationModel::addLocation(const Location& location)
{
    int rowIndex = rowCount();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    unique_ptr<Location> newLocation(new Location(location.uuid()));
    newLocation->setName(location.name());
    newLocation->setBssid(location.bssid());
    newLocation->setUtcOffset(location.utcOffset());
    newLocation->setPosition(static_cast<int>(mLocations->size()));
    UserData::instance().persistItem(*newLocation.get(), true);
    mLocations->push_back(move(newLocation));
    endInsertRows();
    return this->index(rowIndex);
}

int LocationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(mLocations->size());
}

QVariant LocationModel::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index)){
        return QVariant();
    }

    const Location location = *mLocations->at(index.row());

    switch(role){
    case Roles::UuidRole:
        return location.uuid().toString();
    case Roles::NameRole:
        return location.name();
    case Roles::UtcOffsetRole:
        return location.utcOffset();
    case Roles::PositionRole:
        return location.position();
    case Roles::IsOnWlanRole:
        return location.isOnWlan();
    case Roles::IsOnlineRole:
        return location.isOnline();
    case Roles::BssidRole:
        if(location.bssid().isEmpty()) return QString("<Not set>");
        else return location.bssid();
    case Roles::TimezoneRole:
        return timezoneName(location.utcOffset());
    default:
        return QVariant();
    }
}

bool LocationModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(!isIndexValid(index))
        return false;
    Location& location = *mLocations->at(index.row());
    switch (role) {
    case Roles::NameRole:
        location.setName(value.toString());
        break;
    case Roles::UtcOffsetRole:
        location.setUtcOffset(value.toInt());
        break;
    case Roles::PositionRole:
        location.setPosition(value.toInt());
        break;
    case Roles::IsOnWlanRole:
        location.setIsOnWlan(value.toBool());
        emit dataChanged(index, index);
        return true;
    case Roles::IsOnlineRole:
        location.setOnline(value.toBool());
        emit dataChanged(index, index);
        return true;
    case Roles::BssidRole:
        location.setBssid(value.toString());
        break;
    default:
        return false;
    }
    location.setSynced(false);
    UserData::instance().persistItem(location, true);
    emit dataChanged(index, index);
    return true;
}

bool LocationModel::isIndexValid(const QModelIndex &index) const
{
    return index.row() >= 0 && index.row() < static_cast<int>(mLocations->size()) && index.isValid();
}

QHash<int, QByteArray> LocationModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::UuidRole] = "uuid";
    roles[Roles::NameRole] = "name";
    roles[Roles::UtcOffsetRole] = "utcOffset";
    roles[Roles::PositionRole] = "position";
    roles[Roles::IsOnWlanRole] = "isOnWlan";
    roles[Roles::IsOnlineRole] = "isOnline";
    roles[Roles::BssidRole] = "bssid";
    roles[Roles::TimezoneRole] = "timezone";
    return roles;
}


void LocationModel::addLocationWithData(const QString &name, int utcOffset, const QString &bssid)
{
    Location location;
    location.setName(name);
    location.setUtcOffset(utcOffset);
    location.setBssid(bssid);
    location.setPosition(static_cast<int>(mLocations->size()));
    addLocation(location);
}

int LocationModel::roleByName(const QString &name) const
{
    QHash<int, QByteArray> rh = roleNames();
    QHash<int, QByteArray>::iterator i;
    for(i = rh.begin(); i != rh.end(); ++i){
        if (i.value().toStdString() == name.toStdString())
            return i.key();
    }
    return -1;
}

QModelIndex LocationModel::findLocation(const QString &uuid) const
{
    int rows = rowCount();
    int row = 0;
    while(row < rows){
        QModelIndex idx = this->index(row);
        if(idx.data(Roles::UuidRole) == uuid)
            return idx;
        row++;
    }
    return QModelIndex();
}

void LocationModel::updateLocationWithData(const QString &uuid, const QString &name, int utcOffset, const QString &bssid, int position)
{
    QModelIndex idx = findLocation(uuid);
    if(!idx.isValid()) return;

    Location& location = *mLocations->at(idx.row());
    location.setName(name);
    location.setUtcOffset(utcOffset);
    location.setBssid(bssid);
    if(location.position() != position){
        vector<int> idxs;
        int pos = 0;
        int rows = rowCount();
        int row = 0;
        while(pos < rows){
            if(position == pos){
                idxs.push_back(idx.row());
                pos++;
            }
            else if(row == idx.row()){
                row++;
                continue;
            }
            else {
                idxs.push_back(row);
                row++;
                pos++;
            }
        }
        for(int i = 0; i < rows; i++){
            mLocations->at(idxs[i])->setPosition(i);
            mLocations->at(idxs[i])->setSynced(false);
        }
        UserData::instance().persistItems(*mLocations, true);
    }
    else{
        location.setSynced(false);
        UserData::instance().persistItem(location, true);
    }
}

QString LocationModel::getSelectedLocationUuid() const
{
    if(isIndexValid(mSelectedLocationIndex))
        return data(mSelectedLocationIndex, Roles::UuidRole).toString();
    else
        return "";
}

void LocationModel::setSelectedLocationUuid(const QString &uuid)
{
    int row = 0;
    int rows = rowCount();
    mSelectedLocationIndex = QModelIndex();
    while(row<rows){
        if(mLocations->at(row)->uuid() == uuid){
            mSelectedLocationIndex = this->index(row);
            break;
        }
        row++;
    }
    emit selectedLocationUuidChanged();
    emit selectedLocationNameChanged();
    emit selectedLocationBssidChanged();
    emit selectedLocationPositionChanged();
    emit selectedLocationUtcOffsetChanged();
}

QString LocationModel::getSelectedLocationName() const
{
    return data(mSelectedLocationIndex, Roles::NameRole).toString();
}

void LocationModel::setSelectedLocationName(const QString &name)
{
    if(setData(mSelectedLocationIndex, name, Roles::NameRole))
        emit selectedLocationNameChanged();
}

QString LocationModel::getSelectedLocationBssid() const
{
    return data(mSelectedLocationIndex, Roles::BssidRole).toString();
}

void LocationModel::setSelectedLocationBssid(const QString &bssid)
{
    if(setData(mSelectedLocationIndex, bssid, Roles::BssidRole))
        emit selectedLocationBssidChanged();
}

int LocationModel::getSelectedLocationPosition() const
{
    return data(mSelectedLocationIndex, Roles::PositionRole).toInt();
}

void LocationModel::setSelectedLocationPosition(int position)
{
    if(setData(mSelectedLocationIndex, position, Roles::PositionRole))
        emit selectedLocationPositionChanged();
}

int LocationModel::getSelectedLocationUtcOffset() const
{
    return data(mSelectedLocationIndex, Roles::UtcOffsetRole).toInt();
}

void LocationModel::setSelectedLocationUtcOffset(int utcOffset)
{
    if(setData(mSelectedLocationIndex, utcOffset, Roles::UtcOffsetRole))
        emit selectedLocationUtcOffsetChanged();
}

QModelIndex LocationModel::indexOfRow(int row) const
{
    return this->index(row);
}

QString LocationModel::timezoneName(int utcOffset) const
{
    switch (utcOffset / 3600) {
    case -5:
        return "EST";
    case -6:
        return "CST";
    case -7:
        return "MST";
    case -8:
        return "PST";
    case -9:
        return "AKST";
    case -10:
        return "HAST";
    default:
        if(utcOffset == 0)
            return "UTC";
        else if(utcOffset > 0)
            return "UTC+" + QString::number(utcOffset/3600);
        else
            return "UTC" + QString::number(utcOffset/3600);
    }
}

QString LocationModel::getSelectedTimezoneName() const
{
    return timezoneName(data(mSelectedLocationIndex, Roles::UtcOffsetRole).toInt());
}

void LocationModel::setSelectedTimezoneName(const QString &timezoneName)
{
    Q_UNUSED(timezoneName);
}

QStringList LocationModel::getPositionOrder() const
{
    QStringList order;
    order << "first";
    int row = 1;
    int rows = rowCount();
    if(isIndexValid(mSelectedLocationIndex)){
        // order for creating location
        while(row < rows){
            if(row == rows-1){
                order << "last";
            }
            else if(row < mSelectedLocationIndex.row()){
                order << QString("before ").append(data(this->index(row), Roles::NameRole).toString());
            }
            else{
                order << QString("before ").append(data(this->index(row+1), Roles::NameRole).toString());
            }
            row++;
        }
    }
    else{
        // order for editing selected location
        while(row < rows-1){
            order << QString("before ").append(data(this->index(row), Roles::NameRole).toString());
            row++;
        }
        if(rows > 1)
            order << "last";
    }
    return order;
}

void LocationModel::updateModel()
{
    qDebug() << "LocationsModel: updated";
    beginResetModel();
    UserData::instance().loadLocations();
    endResetModel();
}

void LocationModel::remove(const QString &uuid)
{
    Location loc(uuid);
    UserData::instance().removeItem(loc, true);
}

void LocationModel::setCurrentBssid(const QString &bssid)
{
    if(bssid == mCurrentBssid) return;
    mCurrentBssid = bssid;
    emit currentBssidChanged();
}

QString LocationModel::uuidByIndex(int index) const
{
    if(index >=0 && index < static_cast<int>(mLocations->size()))
        return mLocations->at(index)->uuid().toString();
    else
        return "";
}

QString LocationModel::bssidByIndex(int index) const
{
    if(index >=0 && index < static_cast<int>(mLocations->size()))
        return mLocations->at(index)->bssid();
    else
        return QString();
}
