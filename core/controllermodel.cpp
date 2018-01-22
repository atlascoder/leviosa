#include "controllermodel.h"

using namespace std;

ControllerModel::ControllerModel(QObject* parent):
    QAbstractListModel(parent),
    mDb(DatabaseManager::instance()),
    mLocationId(-1),
    mControllers(new vector<unique_ptr<LocationController>>()),
    mSelectedControllerIndex(QModelIndex())
{
}

QModelIndex ControllerModel::addController(const LocationController& controller)
{
    int rowIndex = rowCount();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    unique_ptr<LocationController> newController(new LocationController(controller.locationId()));
    newController->setName(controller.name());
    newController->setMac(controller.mac());
    newController->setPosition(controller.position());
    mDb.controllerDao.addController(*newController);
    mControllers->push_back(move(newController));
    endInsertRows();
    return index(rowIndex);
}

QModelIndex ControllerModel::findController(int id)
{
    int row = 0;
    int rows = rowCount();
    while(row < rows){
        if(mControllers->at(row)->id() == id)
            return this->index(row);
        row++;
    }
    return QModelIndex();
}

int ControllerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mControllers->size();
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
    case Roles::IdRole:
        return controller.id();
    case Roles::NameRole:
        return controller.name();
    case Roles::MacRole:
        return controller.mac();
    case Roles::PositionRole:
        return controller.position();
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
    default:
        return false;
    }
    mDb.controllerDao.updateController(controller);
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
    roles[Roles::IdRole] = "id";
    roles[Roles::NameRole] = "name";
    roles[Roles::MacRole] = "mac";
    roles[Roles::PositionRole] = "position";
    return roles;
}

void ControllerModel::setLocation(int locationId)
{
    beginResetModel();
    mLocationId = locationId;
    loadControllers(mLocationId);
    endResetModel();
}

void ControllerModel::loadControllers(int locationId)
{
    if(locationId < 0){
        mControllers.reset(new vector<unique_ptr<LocationController>>());
        return;
    }
    mControllers = mDb.controllerDao.controllersForLocation(locationId);
}

int ControllerModel::locationId() const
{
    return mLocationId;
}

void ControllerModel::setLocationId(int locationId)
{
    setLocation(locationId);
}

QString ControllerModel::getFreeName()
{
    int i = 0;
    QString str;
    do {
        i++;
        str = QString("Controller ") + QString::number(i);
    } while(!mDb.controllerDao.validateName(str, mLocationId));

    return str;
}

void ControllerModel::addControllerWithMac(const QString &mac)
{
    if(!mDb.controllerDao.validateMac(mac))
        return;

    LocationController c(mLocationId);
    c.setName(getFreeName());
    c.setMac(mac);
    c.setPosition(mDb.controllerDao.lastPosition(locationId()) + 1);
    addController(c);
}

QString ControllerModel::getLocationName() const
{
    unique_ptr<vector<unique_ptr<UserLocation>>> l(mDb.locationDao.locations());
    vector<unique_ptr<UserLocation>>::iterator i;
    for(i = l->begin(); i != l->end(); ++i)
        if(i->get()->id() == mLocationId)
            return i->get()->name() == NULL ? "" : i->get()->name();
    return "";
}

int ControllerModel::getSelectedControllerId() const
{
    return isIndexValid(mSelectedControllerIndex) ? data(mSelectedControllerIndex, Roles::IdRole).toInt() : -1;
}

void ControllerModel::setSelectedControllerId(int controllerId)
{
    int row = 0;
    int rows = rowCount();
    mSelectedControllerIndex = QPersistentModelIndex();
    while(row<rows){
        if(mControllers->at(row)->id() == controllerId){
            mSelectedControllerIndex = this->index(row);
            break;
        }
        row++;
    }
    emit selectedControllerIdChanged();
    emit selectedControllerMacChanged();
    emit selectedControllerNameChanged();
    emit selectedControllerPositionChanged();
}

QString ControllerModel::getSelectedControllerName() const
{
    if(isIndexValid(mSelectedControllerIndex))
        return data(mSelectedControllerIndex, Roles::NameRole).toString();
    else
        return QString();
}

void ControllerModel::setSelectedControllerName(const QString &name)
{
    if(isIndexValid(mSelectedControllerIndex)){
        setData(mSelectedControllerIndex, name, Roles::NameRole);
        emit selectedControllerNameChanged();
    }
}

QString ControllerModel::getSelectedControllerMac() const
{
    if(isIndexValid(mSelectedControllerIndex))
        return data(mSelectedControllerIndex, Roles::MacRole).toString();
    else
        return QString();
}

void ControllerModel::setSelectedControllerMac(const QString &mac)
{
    if(isIndexValid(mSelectedControllerIndex)){
        setData(mSelectedControllerIndex, mac, Roles::MacRole);
        emit selectedControllerMacChanged();
    }
}

int ControllerModel::getSelectedControllerPosition() const
{
    if(isIndexValid(mSelectedControllerIndex))
        return data(mSelectedControllerIndex, Roles::PositionRole).toInt();
    else
        return 0;
}

void ControllerModel::setSelectedControllerPosition(int position)
{
    if(isIndexValid(mSelectedControllerIndex)){
        setData(mSelectedControllerIndex, position, Roles::PositionRole);
        emit selectedControllerPositionChanged();
    }
}

QModelIndex ControllerModel::indexOfRow(int row) const
{
    return this->index(row);
}
