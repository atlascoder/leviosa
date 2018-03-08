#include "controllermodel.h"
#include <QUuid>
#include <QVector>
#include <QMutexLocker>
#include "UserData.h"
#include "ControllerAPI.h"
#include "shadegroup.h"
#include "timeaux.h"
#include <memory>

using namespace std;

ControllerModel::ControllerModel(QObject* parent):
    QAbstractListModel(parent),
    mIsOnWlan(false),
    mLocationUuid(),
    mCurrentIp("0.0.0.0"),
    mControllers(nullptr),
    mSelectedIndex(QModelIndex()),
    mCurrentBssid(""),
    mLocationBssid(""),
    mAPIs(new map<QString, unique_ptr<ControllerAPI>>),
    mDiscovering(false),
    mIsDataLoaded(false)
{
    UserData::instance().loadLocations();
    connect(&UserData::instance(), &UserData::dataUpdated, this, &ControllerModel::updateModel);
}

ControllerModel::~ControllerModel()
{

}

void ControllerModel::updateModel()
{

    int rows = rowCount();

    loadControllers(mLocationUuid);

    long diff = mControllers->size() - rows;

    qDebug() << "Update controlles model, changing rows: " << diff;

    setControllersAPI();

    if(diff < 0){
        beginRemoveRows(QModelIndex(), mControllers->size(), rows-1);
        endRemoveRows();
    }
    else if(diff > 0){
        beginInsertRows(QModelIndex(), rows, mControllers->size()-1);
        endInsertRows();
    }

    emit dataChanged(index(0,0), index(rowCount()-1,0));
    emit selectedControllerStatusChanged();
    emit countChanged();
    emit locationNameChanged();
    emit locationBssidChanged();
}

QModelIndex ControllerModel::addController(const Controller& controller)
{
    int rowIndex = rowCount();
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    unique_ptr<Controller> newController(new Controller(controller.mac()));
    newController->setLocationUuid(controller.locationUuid());
    newController->setName(controller.name());
    newController->setPosition(static_cast<int>(mControllers->size()));
    if(mAPIs->find(controller.mac()) == mAPIs->end()){
        unique_ptr<ControllerAPI> api(new ControllerAPI);
        api->setMac(controller.mac());
        mAPIs->insert(make_pair(controller.mac(), move(api)));
    }
    UserData::instance().persistItem(*newController.get(), true);
    mControllers->push_back(move(newController));
    endInsertRows();
    emit countChanged();
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
    return mControllers == nullptr ? 0 : static_cast<int>(mControllers->size());
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
        if(mAPIs->find(controller.mac()) == mAPIs->end())
            return "0.0.0.0";
        else
            return mAPIs->at(controller.mac())->ipAddress();
    case Roles::IsOnWlanRole:
        if(mAPIs->find(controller.mac()) == mAPIs->end())
            return false;
        else
            return mAPIs->at(controller.mac())->onWlan();
    case Roles::ControllerStateRole:
        if(mAPIs->find(controller.mac()) == mAPIs->end())
            return ControllerAPI::ControllerState::NotFound;
        else
            return mAPIs->at(controller.mac())->controllerState();
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
        if(mAPIs->find(controller.mac()) != mAPIs->end()){
            mAPIs->at(controller.mac())->setIpAddress(value.toString());
        }
        else{
            unique_ptr<ControllerAPI> api(new ControllerAPI);
            api->setMac(controller.mac());
            api->setIpAddress(value.toString());
            mAPIs->insert(make_pair(controller.mac(), move(api)));
        }
        return true;
    case Roles::IsOnWlanRole:
        if(mAPIs->find(controller.mac()) == mAPIs->end()){
            unique_ptr<ControllerAPI> api(new ControllerAPI);
            api->setMac(controller.mac());
            mAPIs->insert(make_pair(controller.mac(), move(api)));
        }
        if(value.toBool() && mAPIs->at(controller.mac())->controllerState()==ControllerAPI::ControllerState::Wan)
            mAPIs->at(controller.mac())->setControllerState(ControllerAPI::ControllerState::Searching);
        else
            mAPIs->at(controller.mac())->setControllerState(ControllerAPI::ControllerState::Wlan);
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
    roles[Roles::ControllerStateRole] = "controllerState";
    return roles;
}

void ControllerModel::loadControllers(const QString& locationUuid)
{
    qDebug() << "Location UUID to model: " << locationUuid;
    if(mControllers == nullptr) mControllers = UserData::instance().mControllers.get();
    UserData::instance().loadControllers(locationUuid);
    setLocationBssid(UserData::instance().locationBssid(locationUuid));
}

QString ControllerModel::locationUuid() const
{
    return mLocationUuid;
}

void ControllerModel::setLocationUuid(const QString& locationUuid)
{
    if(locationUuid == mLocationUuid) return;
    mLocationUuid = locationUuid;
    setDataLoaded(false);
    beginResetModel();
    loadControllers(mLocationUuid);
    setControllersAPI();
    endResetModel();
    setDataLoaded(true);
    emit locationUuidChanged();
    emit locationNameChanged();
    emit locationBssidChanged();
    emit locationStatusChanged();
    emit countChanged();
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

//void ControllerModel::addControllersFromList(const QString &uuid, const QString &list)
//{
//    Q_UNUSED(uuid)
//    Q_UNUSED(list)
//    if(!mIsOnWlan) return;

//    for(auto i = mAPIs->begin(); i != mAPIs->end(); i++){
//        if(i->second->controllerState() == ControllerAPI::ControllerState::Searching){
//            i->second->setControllerState(ControllerAPI::NotFound);
//        }
//    }
//    emit dataChanged(this->index(0), this->index(mControllers->size()-1));
//    emit selectedControllerStatusChanged();

//}


void ControllerModel::addControllerWithMacAndIP(const QString& uuid, const QString &mac, const QString &ip)
{
    auto ctrlr = find_if(mControllers->begin(), mControllers->end(), [mac](unique_ptr<Controller>& lcp)->bool{ return mac==lcp->mac(); });
    if(ctrlr == mControllers->end()){
        Controller c(mac);
        c.setName(getFreeName());
        c.setLocationUuid(uuid);
        c.setPosition(mControllers->size());
        addController(c);
        unique_ptr<ControllerAPI> api(new ControllerAPI);
        api->setMac(mac);
        api->setIpAddress(ip);
        mAPIs->insert(make_pair(mac, move(api)));
    }
    else{
        if(mAPIs->find(mac) != mAPIs->end())
            mAPIs->at(mac)->setIpAddress(ip);
        else{
            unique_ptr<ControllerAPI> api(new ControllerAPI);
            api->setMac(mac);
            api->setIpAddress(ip);
            mAPIs->insert(make_pair(mac, move(api)));
        }
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
    emit selectedControllerStatusChanged();
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
    if(mIsOnWlan == isOnWlan) return;
    mIsOnWlan = isOnWlan;
    if(!mIsOnWlan)
        for(auto i = mAPIs->begin(); i != mAPIs->end(); i++)
            i->second->setControllerState(ControllerAPI::ControllerState::Wan);
    emit onWlanChanged();
    emit locationStatusChanged();
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

void ControllerModel::remove(const QString &mac)
{
    bool removed = false;
    vector<unique_ptr<Controller>> v;
    for(size_t i = 0; i < mControllers->size(); i++){
        if(mControllers->at(i)->mac() == mac){
            UserData::instance().removeItem(*mControllers->at(i).get(), false);
            removed = true;
        }
        else if(removed){
            mControllers->at(i)->setPosition(i-1);
            v.push_back(move(mControllers->at(i)));
        }
        else{
            v.push_back(move(mControllers->at(i)));
        }
    }
    UserData::instance().persistItems(v, true);
}

QString ControllerModel::macByIndex(int idx) const
{
    if(idx < static_cast<int>(mControllers->size()))
        return mControllers->at(idx)->mac();
    else
        return "00:00:00:00:00:00";
}

QString ControllerModel::nameByIndex(int idx) const
{
    if(idx < static_cast<int>(mControllers->size()))
        return mControllers->at(idx)->name();
    else
        return "";
}

void ControllerModel::setSelectedControllerIndex(int index)
{
    if(index >=0 && index < static_cast<int>(mControllers->size())){
        setSelectedControllerMac(mControllers->at(index)->mac());
    }
}

void ControllerModel::checkIP(const QString &mac, const QString &ip)
{
    if(!mIsOnWlan) return;
    qDebug() << "CHECK CONTROLLER: " << mac + "/" + ip;
    if(mAPIs->find(mac) != mAPIs->end()){
        connect(mAPIs->at(mac).get(), &ControllerAPI::controllerStateChanged, this, &ControllerModel::controllerStatusUpdated);
        mAPIs->at(mac)->setIpAddress(ip);
        mAPIs->at(mac)->fetchConfig();
    }
    emit selectedControllerStatusChanged();
}

void ControllerModel::controllerStatusUpdated(const QString& mac)
{
    vector<unique_ptr<Controller>>::iterator c = find_if(mControllers->begin(), mControllers->end(), [&mac](const unique_ptr<Controller>& f)->bool{ return f->mac()==mac; });
    if(c == mControllers->end()) return;

    if(mAPIs->find(mac) != mAPIs->end() && mAPIs->at(mac)->controllerState() == ControllerAPI::ControllerState::Wlan){
        checkConfiguration(*mAPIs->at(mac).get());
    }

    if(isIndexValid(mSelectedIndex)){
        if(mControllers->at(mSelectedIndex.row())->mac() == mac){
            emit selectedControllerStatusChanged();
        }
        emit dataChanged(mSelectedIndex, mSelectedIndex);
    }
}

QString ControllerModel::selectedControllerStatus()
{
    if(isIndexValid(mSelectedIndex))
    {
        Controller* c = mControllers->at(mSelectedIndex.row()).get();
        if(mAPIs->find(c->mac()) == mAPIs->end()) return "Not found.";
        switch(mAPIs->at(c->mac())->controllerState()){
        case ControllerAPI::Wan:
            return "on Cloud";
        case ControllerAPI::NotFound:
            return "Not found.";
        case ControllerAPI::Searching:
            return "Searching...";
        case ControllerAPI::Configuring:
            return "Configuring";
        case ControllerAPI::ConfigFailed:
            return "Not responding!";
        case ControllerAPI::Wlan:
            return QString(mAPIs->at(c->mac())->currentConfig().timezone()).append(" ").append(mAPIs->at(c->mac())->currentConfig().dateTime().toString("h:mm ap"));
        default:
            return "wrong controller state";
        }
    }
    else return "";
}

void ControllerModel::commandShade(const QString &mac, int channel, int command)
{
    if(mAPIs->find(mac) != mAPIs->end())
        mAPIs->at(mac)->shadeCmd(channel, command);
}

QString ControllerModel::ipByMac(const QString &mac)
{
    if(mAPIs->find(mac) == mAPIs->end())
        return "127.0.0.1";
    else
        return mAPIs->at(mac)->ipAddress();
}

void ControllerModel::setCurrentBssid(const QString &bssid){
    if(mCurrentBssid == bssid) return;
    mCurrentBssid = bssid;
    emit currentBssidChanged();
    emit locationStatusChanged();
}

void ControllerModel::checkConfiguration(ControllerAPI& api){

    unique_ptr<vector<unique_ptr<ShadeGroup>>> shadeGroups(UserData::instance().shadeGroupsForController(api.currentConfig().mac()));

    int scheduledChannels = 0;
    ControllerConfig& config = api.currentConfig();

    for_each(shadeGroups->begin(), shadeGroups->end(), [&scheduledChannels](const unique_ptr<ShadeGroup>& g)->void{ if(g->days()!=0) scheduledChannels++; });

    QString locTimezone = timeaux::utcOffsetToTimezone(UserData::instance().locationUtcOffset(mLocationUuid));

    if(locTimezone != config.timezone())
        api.setTimezone(locTimezone);

    bool config_match = true;
    if(scheduledChannels == config.scheduledChannels()) {
        for(vector<unique_ptr<ShadeGroup>>::iterator g = shadeGroups->begin(); g != shadeGroups->end(); g++){
            ShadeGroup *pg = g->get();
            if (pg->days() != config.days(pg->channel())
                    || (pg->days() != 0
                        && (pg->openAt() != config.openAt(pg->channel())
                            || pg->closeAt() != config.closeAt(pg->channel())))){
                config_match = false;
                break;
            }
        }
    }
    else config_match = false;

    if(!config_match){
        config.setTimezone(locTimezone);
        config.clearSchedule();
        for(vector<unique_ptr<ShadeGroup>>::iterator g = shadeGroups->begin(); g != shadeGroups->end(); g++)
            config.addSchedule(g->get()->channel(), g->get()->days(), g->get()->openAt(), g->get()->closeAt());
        api.pushConfig();
    }
}

void ControllerModel::setDiscovering(bool isDiscovering)
{
    qDebug() << "DISCOVERING: " << mDiscovering << " => " << isDiscovering;
    if(mDiscovering == isDiscovering) return;
    mDiscovering = isDiscovering;
    if(mDiscovering){
        for(auto a = mAPIs->begin(); a != mAPIs->end(); a++){
            a->second->setIpAddress("0.0.0.0");
            a->second->setControllerState(ControllerAPI::Searching);
        }
    }
    else{
        for(auto i = mAPIs->begin(); i != mAPIs->end(); i++){
            if(i->second->controllerState() == ControllerAPI::ControllerState::Searching){
                i->second->setControllerState(ControllerAPI::NotFound);
            }
        }
    }
    emit discoveringChanged();
    emit dataChanged(this->index(0), this->index(rowCount()-1));
    emit selectedControllerStatusChanged();
}

void ControllerModel::setDataLoaded(bool isLoaded)
{
    if(mIsDataLoaded == isLoaded) return;
    mIsDataLoaded = isLoaded;
    emit dataLoadedChanged();
}

int ControllerModel::selectedControllerState() const
{
    if(isIndexValid(mSelectedIndex))
        return static_cast<int>(mAPIs->at(mControllers->at(mSelectedIndex.row())->mac())->controllerState());
    else return 0;
}

void ControllerModel::setLocationBssid(const QString &locationBssid)
{
    if(mLocationBssid == locationBssid) return;
    qDebug() << "Location BSSID changing: " << mLocationBssid << " -> " << locationBssid;
    mLocationBssid = locationBssid;
    emit locationBssidChanged();
}

bool ControllerModel::isCurrentLocation() const
{
    return mIsOnWlan && !mLocationBssid.isEmpty() && mCurrentBssid == mLocationBssid;
}

bool ControllerModel::isNewLocation() const
{
    return mLocationBssid.isEmpty();
}

QString ControllerModel::locationStatusText() const
{
    if(mIsOnWlan){
        if(mLocationBssid.isEmpty())
            return "Setup controller";
        else if(mLocationBssid == mCurrentBssid)
            return "Via Current location, WiFi";
        else
            return "Via Internet";
    }
    else{
        return "Via Internet";
    }
}

void ControllerModel::setControllersAPI()
{
    for(auto c = mControllers->begin(); c != mControllers->end(); c ++){
        if(mAPIs->find(c->get()->mac()) == mAPIs->end()){
            unique_ptr<ControllerAPI> api(new ControllerAPI);
            api->setMac(c->get()->mac());
            mAPIs->insert(make_pair(c->get()->mac(), move(api)));
        }
    }
}

void ControllerModel::searchController(const QString &mac)
{
    if(mAPIs->find(mac) != mAPIs->end()){
        mAPIs->at(mac)->setControllerState(ControllerAPI::ControllerState::Searching);
        emit selectedControllerStatusChanged();
        emit dataChanged(mSelectedIndex, mSelectedIndex);
    }
}
