#include "ZoneModel.h"

#include "ControllerConnectionsManager.h"
#include "TimeZoneModel.h"

ZoneModel::ZoneModel(QObject *parent):
    QAbstractListModel(parent),
    mController(new Controller),
    mNeighbours(new vector<shared_ptr<Controller>>),
    mShadeGroups(new vector<shared_ptr<ShadeGroup>>),
    mControllerAPI(new ControllerAPI)
{ }

void ZoneModel::saveChanges()
{
    int pos = mController->position();
    mController->setChanged(true);
    if (mOldPosition != pos) {
        auto curr = find_if(mNeighbours->begin(), mNeighbours->end(), [this](const shared_ptr<Controller>& _g)->bool{ return _g->uuid() == this->mController->uuid();});
        if (curr != mNeighbours->end())
            mNeighbours->erase(curr);
        mNeighbours->insert(mNeighbours->begin() + pos, mController);
        int p = 0;
        while (p < (int)mNeighbours->size()) {
            mNeighbours->at(p)->setPosition(p);
            mNeighbours->at(p)->setChanged(true);
            p++;
        }
        UserData::instance().persistChanges(mNeighbours);
    }
    else {
        UserData::instance().persistChanged(mController);
    }
}

void ZoneModel::deleteZone()
{
    size_t p = 0;
    bool after_deleted = false;
    while(p < mNeighbours->size()) {
        if (after_deleted) {
            mNeighbours->at(p)->setPosition(p-1);
            mNeighbours->at(p)->setChanged(true);
        }
        else if (mNeighbours->at(p)->uuid() == mController->uuid()) {
            mNeighbours->at(p)->setDeleted(true);
            mNeighbours->at(p)->setChanged(true);
            after_deleted = true;
        }
        p++;
    }
    auto groups = UserData::instance().shadeGroupsForController(mController->mac());
    for (auto& g : *groups) {
        g->setDeleted(true);
        g->setChanged(true);
    }
    UserData::instance().persistChanges(mNeighbours, false);
    UserData::instance().persistChanges(groups);
}

void ZoneModel::setUuid(const QString &uuid)
{
    if(uuid.isEmpty() || uuid == mController->uuid()) return;
    shared_ptr<Controller> controller = UserData::instance().controller(uuid);
    if (controller.get() == nullptr) {
        mController = shared_ptr<Controller>(new Controller);
    }
    else {
        mController = controller;
        mNeighbours = UserData::instance().controllersForLocation(controller->locationUuid());
        mOldPosition = mController->position();
    }
    mControllerAPI = ControllerConnectionsManager::instance().controllerAPI(uuid);
    mBssid = UserData::instance().locationBssid(mController->locationUuid());
    connect(mControllerAPI.get(), &ControllerAPI::controllerStateChanged, this, &ZoneModel::controllerStateChanged);
    emit controllerChanged();
    emit positionChanged();
    emit nameChanged();
    emit timezoneChanged();
    reloadShadeGroups();
}

QString ZoneModel::uuid() const
{
    return mController->uuid();
}

void ZoneModel::reloadShadeGroups()
{
    beginResetModel();
    mShadeGroups = UserData::instance().shadeGroupsForController(mController->mac());
    endResetModel();
    emit shadeGroupsChanged();
    emit nameChanged();
    emit timezoneChanged();
    emit controllerStatusChanged();
}

int ZoneModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(mShadeGroups->size());
}

QVariant ZoneModel::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index))
        return QVariant();

    shared_ptr<ShadeGroup> group = mShadeGroups->at(index.row());

    switch (role) {
    case Roles::UuidRole:
        return group->uuid();
    case Roles::ChannelRole:
        return group->channel();
    case Roles::NameRole:
        return group->name();
    case Roles::PositionRole:
        return group->position();
    case Roles::OpenAtRole:
        return group->openAt();
    case Roles::CloseAtRole:
        return group->closeAt();
    case Roles::DaysRole:
        return group->days();
    case Roles::OpenAtTextRole:
        return group->openAtText();
    case Roles::CloseAtTextRole:
        return group->closeAtText();
    case Roles::OpenAtUSRole:
        return group->openAtUS();
    case Roles::CloseAtUSRole:
        return group->closeAtUS();
    case Roles::ShadeStateRole:
        return group->state();
    default:
        return QVariant();
    }
}

bool ZoneModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!isIndexValid(index))
        return false;

    shared_ptr<ShadeGroup> group = mShadeGroups->at(index.row());

    switch (role) {
    case Roles::NameRole:
        group->setName(value.toString());
        break;
    case Roles::PositionRole:
        group->setPosition(value.toInt());
        break;
    case Roles::OpenAtRole:
        group->setOpenAt(value.toInt());
        break;
    case Roles::CloseAtRole:
        group->setCloseAt(value.toInt());
        break;
    case Roles::DaysRole:
        group->setDays(value.toInt());
        break;
    case Roles::ShadeStateRole:
        group->setState(static_cast<Shade::ShadeState>(value.toInt()));
        emit dataChanged(index, index);
        return true;
    case Roles::OpenAtUSRole:
        group->setOpenAtUS(value.toFloat());
        break;
    case Roles::CloseAtUSRole:
        group->setCloseAtUS(value.toFloat());
        break;
    case Roles::ChannelRole:
        group->setChannel(value.toChar().toLatin1());
    default:
        return false;
    }
    group->setChanged(true);
    UserData::instance().persistChanges(mShadeGroups);
    emit dataChanged(index, index);
    return true;
}

QHash<int, QByteArray> ZoneModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::UuidRole] = "uuid";
    roles[Roles::ChannelRole] = "channel";
    roles[Roles::NameRole] = "name";
    roles[Roles::PositionRole] = "position";
    roles[Roles::OpenAtRole] = "openAt";
    roles[Roles::CloseAtRole] = "closeAt";
    roles[Roles::DaysRole] = "days";
    roles[Roles::OpenAtTextRole] = "openAtText";
    roles[Roles::CloseAtTextRole] = "closeAtText";
    roles[Roles::ShadeStateRole] = "shadeState";
    roles[Roles::OpenAtUSRole] = "openAtUS";
    roles[Roles::CloseAtUSRole] = "closeAtUS";
    return roles;
}

bool ZoneModel::isIndexValid(const QModelIndex &index) const
{
    return index.row() >= 0 && index.row() < static_cast<int>(mShadeGroups->size()) && index.isValid();
}

QString ZoneModel::locationUuid() const
{
    return mController->locationUuid();
}

QString ZoneModel::title() const
{
    if (mController == nullptr) return "";
    auto location = UserData::instance().location(mController->locationUuid());
    QString locationName = location.get() == nullptr ? "My Home" : location->name();

    if (UserData::instance().locationsCount() > 1 || mNeighbours->size() > 1)
        return locationName + " - " + mController->name();
    else
        return locationName;

}

QString ZoneModel::name() const
{
    return mController->name();
}

void ZoneModel::setName(const QString &name)
{
    if (name == mController->name()) return;
    mController->setName(name);
    emit nameChanged();
}

int ZoneModel::position() const
{
    return mController->position();
}

void ZoneModel::setPosition(const int position)
{
    if (position == mController->position()) return;
    mController->setPosition(position);
    emit positionChanged();
}

QString ZoneModel::controllerMac() const
{
    return mController->mac();
}

QString ZoneModel::locationBssid() const
{
    return mBssid;
}

void ZoneModel::commandController(const QString& groupUuid, int command)
{
    size_t idx = 0;
    if (groupUuid.isEmpty()) {
        // send command to all channels
        mControllerAPI->shadeCmd(0, command);
    }
    else {
        while(idx < mShadeGroups->size()) {
            if (mShadeGroups->at(idx)->uuid()==groupUuid) {
                setData(index(idx), command, Roles::ShadeStateRole);
                mControllerAPI->shadeCmd(mShadeGroups->at(idx)->channel(), command);
                return;
            }
            idx++;
        }
    }
}

void ZoneModel::resetControls()
{
    size_t idx = 0;
    while(idx < mShadeGroups->size()) {
        setData(index(idx), ShadeGroup::Interim, Roles::ShadeStateRole);
        idx++;
    }
}

QString ZoneModel::controllerStatus() const
{
    if (mController == nullptr) return "";
    switch (mControllerAPI->controllerState()) {
    case ControllerAPI::Searching:
        return "Searching..";
    case ControllerAPI::NotFound:
        return "Controller not responding.";
    case ControllerAPI::Wan:
        return "via Internet";
    case ControllerAPI::Wlan:
        return "via WiFi";
    default:
        return "Configuration error.";
    }
}

void ZoneModel::controllerStateChanged(const QString &mac)
{
    if (mac == mController->mac())
        emit controllerStatusChanged();
}

bool ZoneModel::single() const
{
    return UserData::instance().locationsCount() < 2 && UserData::instance().controllersCountForLocation(mController->locationUuid()) < 2;
}

QStringList ZoneModel::positionOrder() const
{
    QStringList order;
    order << "first";
    int row = 1;
    int rows = mNeighbours->size();
    int current_row = 0;
    bool found = false;
    while (current_row < rows) {
        if (mNeighbours->at(current_row)->uuid() == mController->uuid()) {
            found = true;
            break;
        }
        else current_row++;
    }

    if(found){
        // order for existing
        while(row < rows){
            if(row == rows-1){
                order << "last";
            }
            else if(row < current_row){
                order << QString("before ").append(mNeighbours->at(row)->name());
            }
            else {
                order << QString("before ").append(mNeighbours->at(row + 1)->name());
            }
            row++;
        }
    }
    else{
        // order for created
        while(row < rows){
            order << QString("before ").append(mNeighbours->at(row)->name());
            row++;
        }
        if(rows > 1)
            order << "last";
    }
    return order;
}

QString ZoneModel::timezone() const
{
    auto location = UserData::instance().location(mController->locationUuid());
    if (location.get() != nullptr)
        return location->timezone();
    else
        return TimeZoneModel::defaultTimezone();

}
