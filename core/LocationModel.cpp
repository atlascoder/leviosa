#include "LocationModel.h"
#include <QTextStream>
#include <QThread>
#include <QtConcurrent>

#include "ControllerConnectionsManager.h"
#include "aws/CognitoSyncCommand.h"
#include "TimeZoneModel.h"
#include "UserData.h"

using namespace std;

LocationModel::LocationModel(QObject* parent):
    QAbstractListModel(parent),
    mLocation(new Location),
    mIsNew(true),
    mNeighbours(new vector<shared_ptr<Location>>),
    mControllers(new vector<shared_ptr<Controller>>),
    mCommandsQueue(new queue<pair<int,int>>)
{
    mLocation->setPosition(mNeighbours->size());
    connect(&UserData::instance(), &UserData::dataUpdated, this, &LocationModel::reloadData);
    connect(&ControllerConnectionsManager::instance(), &ControllerConnectionsManager::updated, this, &LocationModel::checkControllers);
}

void LocationModel::saveChanges()
{
    int pos = mLocation->position();
    mLocation->setChanged(true);
    if (mOldPosition != pos) {
        auto curr = find_if(mNeighbours->begin(), mNeighbours->end(), [this](const shared_ptr<Location>& _g)->bool{ return _g->uuid() == this->mLocation->uuid();});
        if (curr != mNeighbours->end())
            mNeighbours->erase(curr);
        mNeighbours->insert(mNeighbours->begin() + pos, mLocation);
        int p = 0;
        while (p < (int)mNeighbours->size()) {
            mNeighbours->at(p)->setPosition(p);
            mNeighbours->at(p)->setChanged(true);
            p++;
        }
        UserData::instance().persistChanges(mNeighbours);
    }
    else {
        UserData::instance().persistChanged(mLocation);
    }
}

void LocationModel::deleteLocation()
{
    size_t p = 0;
    bool after_deleted = false;
    while(p < mNeighbours->size()) {
        if (after_deleted) {
            mNeighbours->at(p)->setPosition(p-1);
            mNeighbours->at(p)->setChanged(true);
        }
        else if (mNeighbours->at(p)->uuid() == mLocation->uuid()) {
            mNeighbours->at(p)->setDeleted(true);
            mNeighbours->at(p)->setChanged(true);
            after_deleted = true;
        }
        p++;
    }
    UserData::instance().persistChanges(mNeighbours, false);
    auto controllers = UserData::instance().controllersForLocation(mLocation->uuid());
    for (auto& c : *controllers) {
        c->setDeleted(true);
        c->setChanged(true);
        auto groups = UserData::instance().shadeGroupsForController(c->mac());
        for (auto& g : *groups) {
            g->setDeleted(true);
            g->setChanged(true);
        }
        UserData::instance().persistChanges(groups, false);
    }
    UserData::instance().persistChanges(controllers);
}

void LocationModel::reloadData()
{
    beginResetModel();
    mControllers = UserData::instance().controllersForLocation(mLocation->uuid());
    endResetModel();
    emit nameChanged();
    emit utcOffsetChanged();
    emit zonesChanged();
    emit positionChanged();
}

int LocationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(mControllers->size());
}

QVariant LocationModel::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index)){
        return QVariant();
    }

    shared_ptr<Controller> controller = mControllers->at(index.row());

    switch(role){
    case Roles::UuidRole:
        return controller->uuid();
    case Roles::NameRole:
        return controller->name();
    case Roles::PositionRole:
        return controller->position();
    case Roles::ControllerStateRole:
        return static_cast<int>(ControllerConnectionsManager::instance().controllerAPI(controller->uuid())->controllerState());
    default:
        return QVariant();
    }
}

void LocationModel::checkControllers()
{
    QVector<int> fields;
    fields.append(Roles::ControllerStateRole);
    for (size_t i = 0; i < mControllers->size(); i++) {
        emit dataChanged(this->index(i), this->index(i), fields);
    }
}

bool LocationModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(!isIndexValid(index))
        return false;
    shared_ptr<Controller> controller = mControllers->at(index.row());
    switch (role) {
    case Roles::NameRole:
        controller->setName(value.toString());
        break;
    case Roles::PositionRole:
        controller->setPosition(value.toInt());
        break;
    case Roles::ControllerStateRole:
        emit dataChanged(index, index);
        return true;
    default:
        return false;
    }
    controller->setChanged(true);
    UserData::instance().persistChanged(controller);
    emit dataChanged(index, index);
    return true;
}

bool LocationModel::isIndexValid(const QModelIndex &index) const
{
    return index.row() >= 0 && index.row() < static_cast<int>(mControllers->size()) && index.isValid();
}

QHash<int, QByteArray> LocationModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::UuidRole] = "uuid";
    roles[Roles::NameRole] = "name";
    roles[Roles::UtcOffsetRole] = "utcOffset";
    roles[Roles::PositionRole] = "position";
    roles[Roles::ControllerStateRole] = "controllerState";
    return roles;
}

QString LocationModel::uuid() const
{
    return mLocation->uuid();
}

void LocationModel::setUuid(const QString &uuid)
{
    if (uuid.isEmpty() || uuid == mLocation->uuid()) return;
    mLocation = UserData::instance().location(uuid);
    mNeighbours = UserData::instance().locations();
    mOldPosition = mLocation->position();
    setNew(false);
    emit uuidChanged();
    emit nameChanged();
    emit bssidChanged();
    emit positionChanged();
    emit utcOffsetChanged();
    reloadData();
}

QString LocationModel::name() const
{
    return mLocation->name();
}

void LocationModel::setName(const QString &name)
{
    if (name == mLocation->name()) return;
    mLocation->setName(name);
    emit nameChanged();
}

QString LocationModel::bssid() const
{
    return mLocation->bssid();
}

void LocationModel::setBssid(const QString &bssid)
{
    if (bssid == mLocation->bssid()) return;
    mLocation->setBssid(bssid);
    emit bssidChanged();
}

int LocationModel::position() const
{
    return mLocation->position();
}

void LocationModel::setPosition(int position)
{
    if (position == mLocation->position()) return;
    mLocation->setPosition(position);
    emit positionChanged();
}

int LocationModel::utcOffset() const
{
    return mLocation->utcOffset();
}

void LocationModel::setUtcOffset(int utcOffset)
{
    if (utcOffset == mLocation->utcOffset()) return;
    mLocation->setUtcOffset(utcOffset);
    emit utcOffsetChanged();
}

QStringList LocationModel::getPositionOrder() const
{
    QStringList order;
    order << "first";
    int row = 1;
    int rows = mNeighbours->size();
    int current_row = 0;
    bool found = false;
    while (current_row < rows) {
        if (mNeighbours->at(current_row)->uuid() == mLocation->uuid()) {
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

bool LocationModel::isNew() const
{
    return mIsNew;
}

void LocationModel::setNew(const bool isNew)
{
    mIsNew = isNew;
    emit isNewChanged();
}

bool LocationModel::single() const
{
    return mNeighbours->size() < 2;
}

void LocationModel::commandShade(const int command)
{
    shared_ptr<vector<shared_ptr<Controller>>> controllers = UserData::instance().controllersForLocation(mLocation->uuid());
    if (NetworkMonitor::instance().isOnWlan() && mLocation->bssid() == NetworkMonitor::instance().getBssid()) {
        for (auto& c : *controllers) {
            shared_ptr<ControllerAPI> api = ControllerConnectionsManager::instance().controllerAPI(c->uuid());
            api->shadeCmd(0, command);
        }
    }
    else {
        QtConcurrent::run(this, &LocationModel::commandAwsLocation, command);
    }
}

void LocationModel::commandAwsLocation(const int command)
{
    unique_ptr<CognitoSyncCommand> commander(new CognitoSyncCommand);
    QString cmd = ControllerAPI::commandCode2String(command);
    commander->sendCommand(mLocation->uuid(), cmd, 0);
}

QString LocationModel::timezoneName() const
{
    return TimeZoneModel::signatureByOffset(mLocation->utcOffset());
}
