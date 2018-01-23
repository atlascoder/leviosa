#include "controllermodel.h"
#include <QUuid>

using namespace std;

ControllerModel::ControllerModel(QObject* parent):
    QAbstractListModel(parent),
    mDb(DatabaseManager::instance()),
    mIsOnWlan(false),
    mWlanAPI(this),
    mWanAPI(this),
    mLocationUuid(),
    mControllers(new vector<unique_ptr<LocationController>>()),
    mSelectedControllerIndex(QModelIndex())
{
}

ControllerModel::~ControllerModel()
{

}

QModelIndex ControllerModel::addController(const LocationController& controller)
{
    int rowIndex = rowCount();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    unique_ptr<LocationController> newController(new LocationController(controller.mac()));
    newController->setName(controller.name());
    newController->setPosition(static_cast<int>(mControllers->size()));
    newController->setOpenAt(controller.openAt());
    newController->setCloseAt(controller.closeAt());
    newController->setDays(controller.days());
    newController->setIpAddress(controller.ipAddress());
    mDb.controllersDao.persistItem(*newController);
    mControllers->push_back(move(newController));
    endInsertRows();
    return index(rowIndex);
}

void ControllerModel::updateControllerWithData(const QString& mac, const QString &name, int position)
{
    QModelIndex idx = findController(mac);
    if(!idx.isValid()) return;

    LocationController& controller = *mControllers-> at(idx.row());
    controller.setName(name);
    if(controller.position() != position){
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
        mControllers = mDb.controllersDao.filtered("locationUuid", mLocationUuid);
        setSelectedControllerMac(controller.mac());
    }
    else{
        mDb.controllersDao.persistItem(controller);
    }
}

QModelIndex ControllerModel::findController(const QString& mac)
{
    int row = 0;
    int rows = rowCount();
    while(row < rows){
        if(mControllers->at(row)->mac() == mac)
            return this->index(row);
        row++;
    }
    return QModelIndex();
}

int ControllerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(mControllers->size());
}

int ControllerModel::roleByName(const QString &name) const
{
    QHash<int, QByteArray> rh = roleNames();
    QHash<int, QByteArray>::iterator i;
    for(i = rh.begin(); i != rh.end(); ++i){
        if (i.value().toStdString() == name.toStdString())
            return i.key();
    }
    return -1;
}

QVariant ControllerModel::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index))
        return QVariant();

    const LocationController& controller = *mControllers->at(index.row());

    switch(role){
    case Roles::MacRole:
        return controller.mac();
    case Roles::LocationUuid:
        return controller.locationUuid();
    case Roles::NameRole:
        return controller.name();
    case Roles::PositionRole:
        return controller.position();
    case Roles::OpenAtRole:
        return controller.openAt();
    case Roles::CloseAtRole:
        return controller.closeAt();
    case Roles::DaysRole:
        return controller.days();
    case Roles::IPRole:
        return controller.ipAddress();
    case Roles::IsOnWlanRole:
        return controller.isWlan();
    default:
        return QVariant();
    }
}

bool ControllerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!isIndexValid(index))
        return false;

    LocationController& controller = *mControllers->at(index.row());
    switch (role) {
    case Roles::NameRole:
        controller.setName(value.toString());
        break;
    case Roles::MacRole:
        controller.setMac(value.toString());
        break;
    case Roles::PositionRole:
        controller.setPosition(value.toInt());
        break;
    case Roles::OpenAtRole:
        controller.setOpenAt(value.toInt());
        break;
    case Roles::CloseAtRole:
        controller.setCloseAt(value.toInt());
        break;
    case Roles::DaysRole:
        controller.setDays(value.toInt());
        break;
    case Roles::IPRole:
        controller.setIpAddress(value.toString());
        return true;
    case Roles::IsOnWlanRole:
        controller.setIsWlan(value.toBool());
        emit onWlanChanged();
        return true;
    default:
        return false;
    }
    mDb.controllersDao.persistItem(controller);
    emit dataChanged(index, index);
    return true;
}

bool ControllerModel::isIndexValid(const QModelIndex &index) const
{
    return index.row() >= 0 && index.row() < static_cast<int>(mControllers->size()) && index.isValid();
}

QHash<int, QByteArray> ControllerModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::MacRole] = "mac";
    roles[Roles::LocationUuid] = "locationUuid";
    roles[Roles::NameRole] = "name";
    roles[Roles::PositionRole] = "position";
    roles[Roles::OpenAtRole] = "openAt";
    roles[Roles::CloseAtRole] = "closeAt";
    roles[Roles::DaysRole] = "days";
    roles[Roles::IPRole] = "ipAddress";
    roles[Roles::IsOnWlanRole] = "isOnWlan";
    return roles;
}

void ControllerModel::setLocation(const QString& locationUuid)
{
    beginResetModel();
    mLocationUuid = locationUuid;
    loadControllers(mLocationUuid);
    endResetModel();
    emit locationNameChanged();
}

void ControllerModel::loadControllers(const QString& locationUuid)
{
    if(locationUuid.size() > 0){
        mControllers.reset(new vector<unique_ptr<LocationController>>());
        return;
    }
    mControllers = mDb.controllersDao.filtered("locationUuid", locationUuid);
}

QString ControllerModel::locationUuid() const
{
    return mLocationUuid;
}

void ControllerModel::setLocationUuid(const QString& locationUuid)
{
    setLocation(locationUuid);
}

QString ControllerModel::getFreeName()
{
    int i = 0;
    QString str;
    do {
        i++;
        str = QString("Controller ") + QString::number(i);
    } while(mDb.controllersDao.filtered("name",str)->size() > 0);

    return str;
}

void ControllerModel::addControllerWithMacAndIP(const QString &mac, const QString &ip)
{
    if(mDb.controllersDao.filtered("mac", mac)->size() == 0){
        LocationController c(mac);
        c.setName(getFreeName());
        c.setLocationUuid(mLocationUuid);
        c.setIpAddress(ip);
        c.setPosition(mControllers->size());
        addController(c);
    }
    else{
        int rows = (int)mControllers->size();
        for(int row = 0; row < rows; row++){
            if(mControllers->at(row)->mac() == mac){
                mControllers->at(row)->setIpAddress(ip);
                break;
            }
        }
    }
}

QModelIndex ControllerModel::indexOfRow(int row) const
{
    return this->index(row);
}

QString ControllerModel::getLocationName() const
{
    unique_ptr<vector<unique_ptr<UserLocation>>> l(mDb.locationsDao.items());
    vector<unique_ptr<UserLocation>>::iterator i;
    for(i = l->begin(); i != l->end(); ++i)
        if(i->get()->uuid() == mLocationUuid)
            return i->get()->name() == NULL ? "" : i->get()->name();
    return "";
}

QString ControllerModel::getSelectedControllerMac() const
{
    return isIndexValid(mSelectedControllerIndex) ? data(mSelectedControllerIndex, Roles::MacRole).toString() : "00:00:00:00:00:00";
}

void ControllerModel::setSelectedControllerMac(const QString& controllerMac)
{
    int row = 0;
    int rows = rowCount();
    mSelectedControllerIndex = QPersistentModelIndex();
    while(row<rows){
        if(mControllers->at(row)->mac() == controllerMac){
            mSelectedControllerIndex = this->index(row);
            break;
        }
        row++;
    }
    emit selectedControllerMacChanged();
    emit selectedControllerNameChanged();
    emit selectedControllerPositionChanged();
}

QString ControllerModel::getSelectedControllerName() const
{
    return data(mSelectedControllerIndex, Roles::NameRole).toString();
}

void ControllerModel::setSelectedControllerName(const QString &name)
{
    if(isIndexValid(mSelectedControllerIndex)){
        setData(mSelectedControllerIndex, name, Roles::NameRole);
        emit selectedControllerNameChanged();
    }
}

int ControllerModel::getSelectedControllerPosition() const
{
    return data(mSelectedControllerIndex, Roles::PositionRole).toInt();
}

void ControllerModel::setSelectedControllerPosition(int position)
{
    if(isIndexValid(mSelectedControllerIndex)){
        setData(mSelectedControllerIndex, position, Roles::PositionRole);
        emit selectedControllerPositionChanged();
    }
}

int ControllerModel::getSelectedControllerOpenAt() const
{
    return data(mSelectedControllerIndex, Roles::OpenAtRole).toInt();
}

void ControllerModel::setSelectedControllerOpenAt(int openAt)
{
    if(isIndexValid(mSelectedControllerIndex)){
        setData(mSelectedControllerIndex, openAt, Roles::OpenAtRole);
        emit selectedControllerOpenAtChanged();
    }
}

int ControllerModel::getSelectedControllerCloseAt() const
{
    return data(mSelectedControllerIndex, Roles::CloseAtRole).toInt();
}

void ControllerModel::setSelectedControllerCloseAt(int closeAt)
{
    if(isIndexValid(mSelectedControllerIndex)){
        setData(mSelectedControllerIndex, closeAt, Roles::CloseAtRole);
        emit selectedControllerCloseAtChanged();
    }
}

int ControllerModel::getSelectedControllerDays() const
{
    return data(mSelectedControllerIndex, Roles::DaysRole).toInt();
}

void ControllerModel::setSelectedControllerDays(int days)
{
    if(isIndexValid(mSelectedControllerIndex)){
        setData(mSelectedControllerIndex, days, Roles::DaysRole);
        emit selectedControllerDaysChanged();
    }
}

QStringList ControllerModel::getPositionOrder() const
{
    QStringList order;
    order << "first";
    int row = 1;
    int rows = rowCount();
    if(isIndexValid(mSelectedControllerIndex)){
        // order for creating location
        while(row < rows){
            if(row == rows-1){
                order << "last";
            }
            else if(row < mSelectedControllerIndex.row()){
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

bool ControllerModel::getOnWlan() const
{
    return mIsOnWlan;
}

void ControllerModel::setOnWlan(bool isOnWlan)
{
    mIsOnWlan = isOnWlan;
    emit onWlanChanged();
}

QString ControllerModel::getSelectedControllerIp() const
{
    if(isIndexValid(mSelectedControllerIndex))
        return data(mSelectedControllerIndex, Roles::IPRole).toString();
    else
        return "0.0.0.0";
}

void ControllerModel::setSelectedControllerIp(const QString &ip)
{
    if(isIndexValid(mSelectedControllerIndex))
        setData(mSelectedControllerIndex, ip, Roles::IPRole);
}

void ControllerModel::shadeCmd(const QString& mac, char channel, int cmd)
{
    int rows = mControllers->size();
    for(int row = 0; row < rows; row++){
        if(mControllers->at(row)->mac() == mac){
            if(mIsOnWlan){
                mWlanAPI.shadeCmd(mControllers->at(row)->ipAddress(), channel, cmd);
            }
            else{
                mWanAPI.shadeCmd(mControllers->at(row)->mac(), channel, cmd);
            }
            break;
        }
    }
}
