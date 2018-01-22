#include "locationmodel.h"
#include <QTextStream>
#include "aws/CognitoSyncAPI.h"
#include <QThread>

using namespace std;

LocationModel::LocationModel(QObject* parent):
    QAbstractListModel(parent),
    SyncableModel("Locations"),
    mDb(DatabaseManager::instance()),
    mLocations(mDb.locationDao.locations()),
    mSelectedLocationIndex(QModelIndex())
{
    mLocalData = (vector<unique_ptr<Syncable>>*)mLocations.get();
    connect(this, &LocationModel::modelUpdated, this, &LocationModel::updateModel);
}

QModelIndex LocationModel::addLocation(const UserLocation& location)
{
    int rowIndex = rowCount();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    unique_ptr<UserLocation> newLocation(new UserLocation(location.name()));
    newLocation->setName(location.name());
    newLocation->setBssid(location.bssid());
    newLocation->setSsid(location.ssid());
    newLocation->setUtcOffset(location.utcOffset());
    newLocation->setPosition(static_cast<int>(mLocations->size()));
    newLocation->setUpdated(true);
    mDb.locationDao.addLocation(*newLocation);
    mLocations->push_back(move(newLocation));
    endInsertRows();
    return index(rowIndex, 0);
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

    const UserLocation location = *mLocations->at(index.row());

    switch(role){
    case Roles::IdRole:
        return location.id();
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
    case Roles::SsidRole:
        return location.ssid();
    case Roles::BssidRole:
        return location.bssid();
    default:
        return QVariant();
    }
}

bool LocationModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(!isIndexValid(index))
        return false;
    UserLocation& location = *mLocations->at(index.row());
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
    case Roles::SsidRole:
        location.setSsid(value.toString());
        break;
    case Roles::BssidRole:
        location.setBssid(value.toString());
        break;
    default:
        return false;
    }
    mDb.locationDao.updateLocation(location);
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
    roles[Roles::IdRole] = "id";
    roles[Roles::NameRole] = "name";
    roles[Roles::UtcOffsetRole] = "utcOffset";
    roles[Roles::PositionRole] = "position";
    roles[Roles::IsOnWlanRole] = "isOnWlan";
    roles[Roles::IsOnlineRole] = "isOnline";
    roles[Roles::SsidRole] = "ssid";
    roles[Roles::BssidRole] = "bssid";
    return roles;
}


void LocationModel::addLocationWithData(const QString &name, int utcOffset, const QString &bssid, const QString &ssid)
{
    UserLocation location(name);
    location.setUtcOffset(utcOffset);
    location.setBssid(bssid);
    location.setSsid(ssid);
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

QModelIndex LocationModel::findLocation(const int id) const
{
    int rows = rowCount();
    int row = 0;
    while(row < rows){
        QModelIndex idx = this->index(row);
        if(idx.data(Roles::IdRole) == id)
            return idx;
        row++;
    }
    return QModelIndex();
}

bool LocationModel::updateLocationWithData(const int id, const QString &name, int utcOffset, const QString &bssid, const QString &ssid, int position)
{
    QModelIndex idx = findLocation(id);
    if(!idx.isValid()) return false;

    UserLocation& location = *mLocations->at(idx.row());
    location.setName(name);
    location.setUtcOffset(utcOffset);
    location.setBssid(bssid);
    location.setSsid(ssid);
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
            setData(this->index(idxs[i]), i, Roles::PositionRole);
        }
        mLocations = mDb.locationDao.locations();
        setSelectedLocationId(location.id());
    }
    else{
        mDb.locationDao.updateLocation(location);
    }
    beginResetModel();
    endResetModel();
    return true;
}

void LocationModel::fireDataChanged()
{
    emit dataChanged(this->index(0), this->index(rowCount()-1));
}

int LocationModel::getSelectedLocationId() const
{
    if(isIndexValid(mSelectedLocationIndex))
        return data(mSelectedLocationIndex, Roles::IdRole).toInt();
    else
        return -1;
}

void LocationModel::setSelectedLocationId(int id)
{
    int row = 0;
    int rows = rowCount();
    mSelectedLocationIndex = QModelIndex();
    while(row<rows){
        if(mLocations->at(row)->id() == id){
            mSelectedLocationIndex = this->index(row);
            break;
        }
        row++;
    }
    emit selectedLocationIdChanged();
    emit selectedLocationNameChanged();
    emit selectedLocationSsidChanged();
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

QString LocationModel::getSelectedLocationSsid() const
{
    return data(mSelectedLocationIndex, Roles::SsidRole).toString();
}

void LocationModel::setSelectedLocationSsid(const QString &ssid)
{
    if(setData(mSelectedLocationIndex, ssid, Roles::SsidRole))
        emit selectedLocationSsidChanged();
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

QString LocationModel::getSelectedTimezoneName() const
{
    int idx = data(mSelectedLocationIndex, Roles::UtcOffsetRole).toInt() / 3600;
    switch (idx) {
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
        if(idx == 0)
            return "UTC";
        else if(idx > 0)
            return "UTC+" + QString::number(idx);
        else
            return "UTC" + QString::number(idx);
    }
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
        while(row < rows){
            if(row == rows-1){
                order << QString("before ").append(data(this->index(row), Roles::NameRole).toString());
            }
            row++;
        }
        if(rows > 1)
            order << "last";
    }
    return order;
}

void LocationModel::mergeUpdates()
{
    int n = mLocalDataCache->size();
    for(int i = 0; i < n; i++){
        if(mLocalDataCache->at(i)->updated()){
            UserLocation loc;
            loc.setId(mUpdates->at(i)->id());
            loc.setSyncContent(mLocalDataCache->at(i)->syncContent());
            loc.setLastModified(mLocalDataCache->at(i)->lastModified());
            loc.setSyncs(mLocalDataCache->at(i)->syncs());
            mDb.locationDao.persistLocation(loc);
        }
    }
    n = mUpdates->size();
    for(int i = 0; i < n; i++){
        if(mUpdates->at(i)->updated()){
            UserLocation loc;
            loc.setId(mUpdates->at(i)->id());
            loc.setSyncContent(mUpdates->at(i)->syncContent());
            loc.setLastModified(mUpdates->at(i)->lastModified());
            loc.setSyncs(mUpdates->at(i)->syncs());
            mDb.locationDao.persistLocation(loc);
        }
    }
    for(int d : *mDeleted.get())
        mDb.locationDao.destroyDeleted(d);
    emit modelUpdated();
    this->SyncableModel::mergeUpdates();
}

void LocationModel::syncModel()
{
    sync();
}

void LocationModel::updateModel()
{
    beginResetModel();
    mLocations = mDb.locationDao.locations();
    mLocalData = (vector<unique_ptr<Syncable>>*)mLocations.get();
    endResetModel();
}

void LocationModel::sync()
{
    mDeleted = mDb.locationDao.deletedIds();
    this->SyncableModel::sync();
}

void LocationModel::markDeleted(int id)
{
    mDb.locationDao.markDeleted(id);
    beginResetModel();
    mLocations = mDb.locationDao.locations();
    endResetModel();
}

void LocationModel::setCurrentBssid(const QString &bssid)
{
    if(bssid == mCurrentBssid) return;
    mCurrentBssid = bssid;
    emit currentBssidChanged();
}

bool LocationModel::setSyncing(bool isSyncing)
{
    if(this->SyncableModel::setSyncing(isSyncing)){
        emit isSyncingChanged();
        return true;
    }
    else
        return false;
}
