#include "controllermodel.h"
#include <QUuid>
#include <QVector>
#include <QMutexLocker>
#include "UserData.h"

using namespace std;

ControllerModel::ControllerModel(QObject* parent):
    QAbstractListModel(parent),
    mIsOnWlan(false),
    mWlanAPI(this),
    mWanAPI(this),
    mLocationUuid(),
    mControllers(UserData::instance().mControllers.get()),
    mSelectedIndex(QModelIndex())
{
    connect(&UserData::instance(), &UserData::dataUpdated, this, &ControllerModel::updateModel);
}

ControllerModel::~ControllerModel()
{

}

void ControllerModel::updateModel()
{
    loadControllers(mLocationUuid);

    long diff = mControllers->size() - rowCount();

    qDebug() << "Update controlles model from rows: " << rowCount();

    if(diff < 0){
        beginRemoveRows(QModelIndex(), mControllers->size(), rowCount()-1);
        endRemoveRows();
    }
    else if(diff > 0){
        beginInsertRows(QModelIndex(), rowCount(), mControllers->size()-1);
        endInsertRows();
    }

    QModelIndex a = this->createIndex(0,0,nullptr);
    QModelIndex b = this->createIndex(mControllers->size(),0,nullptr);
    emit dataChanged(a, b);

}

QModelIndex ControllerModel::addController(const Controller& controller)
{
    int rowIndex = rowCount();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    unique_ptr<Controller> newController(new Controller(controller.mac()));
    newController->setLocationUuid(controller.locationUuid());
    newController->setName(controller.name());
    newController->setPosition(static_cast<int>(mControllers->size()));
    newController->setIpAddress(controller.ipAddress());
    UserData::instance().persistItem(*newController.get(), true);
    mControllers->push_back(move(newController));
    endInsertRows();
    return index(rowIndex);
}

void ControllerModel::updateControllerWithData(const QString& mac, const QString &name, int position)
{
    QModelIndex idx = findController(mac);
    if(!idx.isValid()) return;

    Controller& controller = *mControllers->at(idx.row());
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
            mControllers->at(idxs[i])->setPosition(i);
            mControllers->at(idxs[i])->setSynced(false);
        }
        UserData::instance().persistItems(*mControllers, true);
    }
    else{
        controller.setSynced(false);
        UserData::instance().persistItem(controller, true);
    }
    emit dataChanged(idx, idx);
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

    const Controller& controller = *mControllers->at(index.row());

    switch(role){
    case Roles::MacRole:
        return controller.mac();
    case Roles::LocationUuid:
        return controller.locationUuid();
    case Roles::NameRole:
        return controller.name();
    case Roles::PositionRole:
        return controller.position();
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

    Controller& controller = *mControllers->at(index.row());
    switch (role) {
    case Roles::LocationUuid:
        controller.setLocationUuid(value.toString());
        break;
    case Roles::NameRole:
        controller.setName(value.toString());
        break;
    case Roles::MacRole:
        controller.setMac(value.toString());
        break;
    case Roles::PositionRole:
        controller.setPosition(value.toInt());
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
    UserData::instance().persistItem(controller, true);
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

void ControllerModel::loadControllers(const QString& locationUuid)
{
    UserData::instance().loadControllers(locationUuid);
}

QString ControllerModel::locationUuid() const
{
    return mLocationUuid;
}

void ControllerModel::setLocationUuid(const QString& locationUuid)
{
    if(locationUuid == mLocationUuid) return;
    mLocationUuid = locationUuid;
    beginResetModel();
    loadControllers(mLocationUuid);
    endResetModel();
    emit locationUuidChanged();
    emit locationNameChanged();
}

QString ControllerModel::getFreeName()
{
    int i = 0;
    QString str;
    do {
        i++;
        str = QString("Controller ") + QString::number(i);
    } while(std::find_if(mControllers->begin(), mControllers->end(), [str](unique_ptr<Controller>& lcp)->bool{ return str == lcp->name(); }) != mControllers->end());

    return str;
}

QString ControllerModel::getFreeName(vector<unique_ptr<Controller>>& newItems)
{
    int i = 0;
    QString str;
    do {
        i++;
        str = QString("Controller ") + QString::number(i);
    } while(
            find_if(mControllers->begin(), mControllers->end(), [str](unique_ptr<Controller>& lcp)->bool{ return str == lcp->name(); }) != mControllers->end() ||
            find_if(newItems.begin(), newItems.end(), [str](unique_ptr<Controller>& lcp)->bool{ return str == lcp->name(); }) != newItems.end()
            );

    return str;
}

void ControllerModel::addControllersFromList(const QString &uuid, const QString &list)
{
    QStringList ctrs = list.split(",");
    vector<unique_ptr<Controller>> v;
    for(int i = 0; i < ctrs.size(); i++){
        QStringList mac_ip = ctrs.at(i).split(" ");
        if(UserData::instance().isControllerKnown(mac_ip.at(0))){
            auto ctrlr = find_if(mControllers->begin(), mControllers->end(), [&mac_ip](unique_ptr<Controller>& lcp)->bool{ return mac_ip.at(0)==lcp->mac(); });
            if(ctrlr != mControllers->end())
                ctrlr->get()->setIpAddress(mac_ip.at(1));
        }
        else{
            unique_ptr<Controller> c(new Controller(mac_ip.at(0)));
            c->setName(getFreeName(v));
            c->setLocationUuid(uuid);
            c->setIpAddress(mac_ip.at(1));
            c->setPosition(mControllers->size() + i);
            c->setSynced(false);
            v.push_back(move(c));
        }
    }
    if(v.size() > 0) {
        UserData::instance().persistItems(v, true);
        updateModel();
    }
}


void ControllerModel::addControllerWithMacAndIP(const QString& uuid, const QString &mac, const QString &ip)
{
    auto ctrlr = find_if(mControllers->begin(), mControllers->end(), [mac](unique_ptr<Controller>& lcp)->bool{ return mac==lcp->mac(); });
    if(ctrlr == mControllers->end()){
        Controller c(mac);
        c.setName(getFreeName());
        c.setLocationUuid(uuid);
        c.setIpAddress(ip);
        c.setPosition(mControllers->size());
        addController(c);
    }
    else{
        ctrlr->get()->setIpAddress(ip);
        UserData::instance().persistItem(*ctrlr->get(), true);
    }
}

QModelIndex ControllerModel::indexOfRow(int row) const
{
    return this->index(row);
}

QString ControllerModel::getLocationName() const
{
    auto loc = find_if(UserData::instance().mLocations->begin(), UserData::instance().mLocations->end(), [this](unique_ptr<Location>& loc) -> bool { return loc.get()->uuid() == this->mLocationUuid; });
    if(loc != UserData::instance().mLocations->end())
        return loc->get()->name();
    else
        return "";
}

QString ControllerModel::getSelectedControllerMac() const
{
    return isIndexValid(mSelectedIndex) ? data(mSelectedIndex, Roles::MacRole).toString() : "00:00:00:00:00:00";
}

void ControllerModel::setSelectedControllerMac(const QString& controllerMac)
{
    int row = 0;
    int rows = rowCount();
    mSelectedIndex = QPersistentModelIndex();
    while(row<rows){
        if(mControllers->at(row)->mac() == controllerMac){
            if(row == mSelectedIndex.row()) return;
            mSelectedIndex = this->index(row);
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
    return data(mSelectedIndex, Roles::NameRole).toString();
}

void ControllerModel::setSelectedControllerName(const QString &name)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, name, Roles::NameRole);
        emit selectedControllerNameChanged();
    }
}

int ControllerModel::getSelectedControllerPosition() const
{
    return data(mSelectedIndex, Roles::PositionRole).toInt();
}

void ControllerModel::setSelectedControllerPosition(int position)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, position, Roles::PositionRole);
        emit selectedControllerPositionChanged();
    }
}

int ControllerModel::getSelectedControllerOpenAt() const
{
    return data(mSelectedIndex, Roles::OpenAtRole).toInt();
}

void ControllerModel::setSelectedControllerOpenAt(int openAt)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, openAt, Roles::OpenAtRole);
        emit selectedControllerOpenAtChanged();
    }
}

int ControllerModel::getSelectedControllerCloseAt() const
{
    return data(mSelectedIndex, Roles::CloseAtRole).toInt();
}

void ControllerModel::setSelectedControllerCloseAt(int closeAt)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, closeAt, Roles::CloseAtRole);
        emit selectedControllerCloseAtChanged();
    }
}

int ControllerModel::getSelectedControllerDays() const
{
    return data(mSelectedIndex, Roles::DaysRole).toInt();
}

void ControllerModel::setSelectedControllerDays(int days)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, days, Roles::DaysRole);
        emit selectedControllerDaysChanged();
    }
}

QStringList ControllerModel::getPositionOrder() const
{
    QStringList order;
    order << "first";
    int row = 1;
    int rows = rowCount();
    if(isIndexValid(mSelectedIndex)){
        // order for creating location
        while(row < rows){
            if(row == rows-1){
                order << "last";
            }
            else if(row < mSelectedIndex.row()){
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
    if(isIndexValid(mSelectedIndex))
        return data(mSelectedIndex, Roles::IPRole).toString();
    else
        return "0.0.0.0";
}

void ControllerModel::setSelectedControllerIp(const QString &ip)
{
    if(isIndexValid(mSelectedIndex))
        setData(mSelectedIndex, ip, Roles::IPRole);
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

void ControllerModel::remove(const QString &mac)
{
    Controller c(mac);
    for(int i = 0; i < mControllers->size(); i++)
        if(mControllers->at(i)->mac() == mac){
            beginRemoveRows(QModelIndex(), i, i);
            UserData::instance().removeItem(c, true);
            UserData::instance().loadControllers(mLocationUuid);
            endRemoveRows();
            return;
        }
    // reset whole model if removing item was not found
    beginResetModel();
    UserData::instance().loadControllers(mLocationUuid);
    endResetModel();
}

QString ControllerModel::macByIndex(int idx) const
{
    if(idx < mControllers->size())
        return mControllers->at(idx)->mac();
    else
        return "00:00:00:00:00:00";
}

QString ControllerModel::nameByIndex(int idx) const
{
    if(idx < mControllers->size())
        return mControllers->at(idx)->name();
    else
        return "";
}
