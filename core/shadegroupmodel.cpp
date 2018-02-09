#include "shadegroupmodel.h"
#include <QSet>

#include "UserData.h"
#include "ControllerAPI.h"

using namespace std;

ShadeGroupModel::ShadeGroupModel(QObject* parent):
    QAbstractListModel(parent),
    mDb(DatabaseManager::instance()),
    mControllerMac("00:00:00:00:00:00"),
    mSelectedIndex(QModelIndex()),
    mShadeGroups(UserData::instance().shadeGroupsForController(mControllerMac)),
    mControllerAPI(new ControllerAPI),
    mIpAddress("0.0.0.0")
{
    connect(&UserData::instance(), &UserData::dataUpdated, this, &ShadeGroupModel::updateModel);
    connect(mControllerAPI.get(), &ControllerAPI::scheduleSet, this, &ShadeGroupModel::onScheduleSet);
    connect(mControllerAPI.get(), &ControllerAPI::scheduleFailed, [this]()->void{ emit scheduleSetFailed(); });
}

void ShadeGroupModel::loadShadeGroups()
{
    unique_ptr<vector<unique_ptr<ShadeGroup>>> loaded = UserData::instance().shadeGroupsForController(mControllerMac);
    for(vector<unique_ptr<ShadeGroup>>::iterator i = loaded->begin(); i != loaded->end(); i++){
        vector<unique_ptr<ShadeGroup>>::iterator g = find_if(mShadeGroups->begin(), mShadeGroups->end(), [i](const unique_ptr<ShadeGroup>& g)->bool{ return i->get()->controllerMac()==g->controllerMac(); });
        if(g != mShadeGroups->end()){
            i->get()->setMatchPrevious(*g->get() == *i->get());
        }
        else
            i->get()->setMatchPrevious(false);
    }
    mShadeGroups.swap(loaded);
}

void ShadeGroupModel::updateModel(){
    loadShadeGroups();

    long diff = mShadeGroups->size() - rowCount();

    qDebug() << "Update shadeGroups model from rows: " << rowCount();

    if(diff < 0){
        beginRemoveRows(QModelIndex(), mShadeGroups->size(), rowCount()-1);
        endRemoveRows();
    }
    else if(diff > 0){
        beginInsertRows(QModelIndex(), rowCount(), mShadeGroups->size()-1);
        endInsertRows();
    }
    for(size_t i = 0; i < mShadeGroups->size(); i++){
        if(!mShadeGroups->at(i)->matchPrevious()){
            QModelIndex a = this->createIndex(i,0,nullptr);
            QModelIndex b = this->createIndex(i,0,nullptr);
            emit dataChanged(a, b);
        }
    }
}

QModelIndex ShadeGroupModel::addShadeGroup(const ShadeGroup &shadesGroup)
{
    int rowsIndex = rowCount();
    unique_ptr<ShadeGroup> newGroup(new ShadeGroup(shadesGroup.channel()));
    newGroup->setControllerMac(shadesGroup.controllerMac());
    newGroup->setName(shadesGroup.name());
    newGroup->setPosition(static_cast<int>(mShadeGroups->size()));
    newGroup->setOpenAt(shadesGroup.openAt());
    newGroup->setCloseAt(shadesGroup.closeAt());
    newGroup->setDays(shadesGroup.days());
    UserData::instance().persistItem(*newGroup.get(), true);
    mShadeGroups->push_back(move(newGroup));
    beginInsertRows(QModelIndex(), rowsIndex, rowsIndex);
    loadShadeGroups();
    endInsertRows();
    return this->index(rowsIndex);
}

void ShadeGroupModel::updateShadeGroupsWithData(int channel, const QString& name, int position, float openAtUS, float closeAtUS, int days, bool scheduleForController)
{
    QModelIndex idx = find(channel);
    if(!idx.isValid()) return;

    ShadeGroup& shadeGroup = *mShadeGroups->at(idx.row());
    shadeGroup.setName(name);

    if(scheduleForController){
        for(auto i = mShadeGroups->begin(); i != mShadeGroups->end(); i++){
            i->get()->setOpenAtUS(openAtUS);
            i->get()->setCloseAtUS(closeAtUS);
            i->get()->setDays(days);
        }
    }
    else{
        shadeGroup.setOpenAtUS(openAtUS);
        shadeGroup.setCloseAtUS(closeAtUS);
        shadeGroup.setDays(days);
    }

    if(shadeGroup.position() != position){
        vector<int> idxs;
        int pos = 0;
        int rows = mShadeGroups->size();
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
            mShadeGroups->at(idxs[i])->setPosition(i);
            mShadeGroups->at(idxs[i])->setSynced(false);
        }
    }
    else{
        shadeGroup.setSynced(false);
    }
    if(mControllerAPI->controllerState() == ControllerAPI::ControllerState::Wlan){
        setScheduleToController();
    }
    else{
        UserData::instance().persistItems(*mShadeGroups.get(), true);
        emit scheduleSet();
    }
}

QModelIndex ShadeGroupModel::find(int channel) const
{
    int row = 0;
    int rows = rowCount();
    while(row < rows){
        if(mShadeGroups->at(row)->channel() == channel)
            return this->index(row);
        row++;
    }
    return QModelIndex();
}


void ShadeGroupModel::removeShadeGroup(const QString& mac, char channel)
{
    ShadeGroup c(channel);
    c.setControllerMac(mac);
    UserData::instance().removeItem(c, true);

    beginResetModel();
    loadShadeGroups();
    endResetModel();
}

int ShadeGroupModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(mShadeGroups->size());
}

QVariant ShadeGroupModel::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index))
        return QVariant();

    const ShadeGroup& group = *mShadeGroups->at(index.row());

    switch (role) {
    case Roles::ChannelRole:
        return group.channel();
    case Roles::NameRole:
        return group.name();
    case Roles::PositionRole:
        return group.position();
    case Roles::OpenAtRole:
        return group.openAt();
    case Roles::CloseAtRole:
        return group.closeAt();
    case Roles::DaysRole:
        return group.days();
    case Roles::OpenAtTextRole:
        return group.openAtText();
    case Roles::CloseAtTextRole:
        return group.closeAtText();
    case Roles::OpenAtUSRole:
        return group.openAtUS();
    case Roles::CloseAtUSRole:
        return group.closeAtUS();
    case Roles::ShadeStateRole:
        return group.state();
    default:
        return QVariant();
    }
}

bool ShadeGroupModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!isIndexValid(index))
        return false;

    ShadeGroup& group = *mShadeGroups->at(index.row());

    switch (role) {
    case Roles::NameRole:
        group.setName(value.toString());
        break;
    case Roles::PositionRole:
        group.setPosition(value.toInt());
        break;
    case Roles::OpenAtRole:
        group.setOpenAt(value.toInt());
        break;
    case Roles::CloseAtRole:
        group.setCloseAt(value.toInt());
        break;
    case Roles::DaysRole:
        group.setDays(value.toInt());
        break;
    case Roles::ShadeStateRole:
        group.setState(static_cast<Shade::ShadeState>(value.toInt()));
        emit dataChanged(index, index);
        return true;
    case Roles::OpenAtUSRole:
        group.setOpenAtUS(value.toFloat());
        break;
    case Roles::CloseAtUSRole:
        group.setCloseAtUS(value.toFloat());
        break;
    case Roles::ChannelRole:
        group.setChannel(value.toChar().toLatin1());
    default:
        return false;
    }
    UserData::instance().persistItem(group, true);
    loadShadeGroups();
    emit dataChanged(index, index);
    return true;
}

bool ShadeGroupModel::isIndexValid(const QModelIndex &index) const
{
    return index.row() >= 0 && index.row() < static_cast<int>(mShadeGroups->size()) && index.isValid();
}

QHash<int, QByteArray> ShadeGroupModel::roleNames() const
{
    QHash<int, QByteArray> roles;
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

QString ShadeGroupModel::controllerMac() const
{
    return mControllerMac;
}

void ShadeGroupModel::setControllerMac(const QString& controllerMac)
{
    beginResetModel();
    mControllerMac = controllerMac;
    loadShadeGroups();
    endResetModel();
}

void ShadeGroupModel::addShadesGroupWithData(const QString& controllerMac, const QString &name)
{
    ShadeGroup aGroup(findFreeChannel(controllerMac));
    aGroup.setControllerMac(controllerMac);
    aGroup.setName(name);
    aGroup.setPosition(static_cast<int>(mShadeGroups->size()));
    addShadeGroup(aGroup);
}

int ShadeGroupModel::getSelectedChannel() const
{
    return isIndexValid(mSelectedIndex) ? data(mSelectedIndex, Roles::ChannelRole).toInt() : -1;
}

void ShadeGroupModel::setSelectedChannel(int channel)
{
    int row= 0;
    int rows = rowCount();
    mSelectedIndex = QModelIndex();
    while(row < rows){
        if(mShadeGroups->at(row)->channel() == channel){
            mSelectedIndex = this->index(row);
            break;
        }
        row++;
    }
    emit selectedChannelChanged();
    emit selectedGroupNameChanged();
    emit selectedGroupPositionChanged();
    emit selectedGroupOpenAtChanged();
    emit selectedGroupCloseAtChanged();
    emit selectedGroupDaysChanged();
}

QString ShadeGroupModel::getSelectedGroupName() const
{
    return data(mSelectedIndex, Roles::NameRole).toString();
}

void ShadeGroupModel::setSelectedGroupName(const QString &name)
{
    setData(mSelectedIndex, name, Roles::NameRole);
}

int ShadeGroupModel::getSelectedGroupPosition() const
{
    return data(mSelectedIndex, Roles::PositionRole).toInt();
}

int ShadeGroupModel::roleByName(const QString& name) const
{
    QHash<int, QByteArray> rh = roleNames();
    QHash<int, QByteArray>::iterator i;
    for(i = rh.begin(); i != rh.end(); ++i){
        if (i.value().toStdString() == name.toStdString())
            return i.key();
    }
    return -1;
}

int ShadeGroupModel::getSelectedGroupOpenAt() const
{
    return data(mSelectedIndex, Roles::OpenAtRole).toInt();
}

void ShadeGroupModel::setSelectedGroupOpenAt(int openAt)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, openAt, Roles::OpenAtRole);
        emit selectedGroupOpenAtChanged();
    }
}

int ShadeGroupModel::getSelectedGroupCloseAt() const
{
    return data(mSelectedIndex, Roles::CloseAtRole).toInt();
}

void ShadeGroupModel::setSelectedGroupCloseAt(int closeAt)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, closeAt, Roles::CloseAtRole);
        emit selectedGroupCloseAtChanged();
    }
}

int ShadeGroupModel::getSelectedGroupDays() const
{
    return data(mSelectedIndex, Roles::DaysRole).toInt();
}

void ShadeGroupModel::setSelectedGroupDays(int days)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, days, Roles::DaysRole);
        emit selectedGroupDaysChanged();
    }
}

QString ShadeGroupModel::getSelectedOpenAtText() const
{
    return data(mSelectedIndex, Roles::OpenAtTextRole).toString();
}

QString ShadeGroupModel::getSelectedCloseAtText() const
{
    return data(mSelectedIndex, Roles::CloseAtTextRole).toString();
}

Shade::ShadeState ShadeGroupModel::getSelectedShadeState() const
{
    return static_cast<Shade::ShadeState>(data(mSelectedIndex, Roles::ShadeStateRole).toInt());
}

void ShadeGroupModel::setSelectedShadeState(Shade::ShadeState state)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, state, Roles::ShadeStateRole);
        emit selectedShadeStateChanged();
    }
}

void ShadeGroupModel::setStateToAll(const Shade::ShadeState state)
{
    int row = 0;
    int rows = rowCount();
    while(row < rows){
        setData(this->index(row++), static_cast<int>(state), Roles::ShadeStateRole);
    }
}

void ShadeGroupModel::setStateToAll(const int state)
{
    int row = 0;
    int rows = rowCount();
    while(row < rows){
        setData(this->index(row++), state, Roles::ShadeStateRole);
    }
}

float ShadeGroupModel::getSelectedOpenAtUS() const
{
    return data(mSelectedIndex, Roles::OpenAtUSRole).toFloat();
}

void ShadeGroupModel::setSelectedOpenAtUS(const float openAtUS)
{
    setData(mSelectedIndex, openAtUS, Roles::OpenAtUSRole);
}

float ShadeGroupModel::getSelectedCloseAtUS() const
{
    return data(mSelectedIndex, Roles::CloseAtUSRole).toFloat();
}

void ShadeGroupModel::setSelectedCloseAtUS(const float closeAtUS)
{
    setData(mSelectedIndex, closeAtUS, Roles::CloseAtUSRole);
}

QStringList ShadeGroupModel::getPositionOrder() const
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

char ShadeGroupModel::findFreeChannel(const QString& controllerMac) const
{
    QSet<char> channels;
    if(isIndexValid(mSelectedIndex)){
        int l = mShadeGroups->size();
        for(int i = 0; i < l; i++)
            channels.insert(mShadeGroups->at(i)->channel());
    }
    else{
        std::unique_ptr<std::vector<std::unique_ptr<ShadeGroup>>> shadesGroups = mDb.shadeGroupsDao.filtered("controllerMac", controllerMac);
        int l = shadesGroups->size();
        for(int i = 0; i < l; i++)
            channels.insert(shadesGroups->at(i)->channel());
    }

    char ch = 1;
    while(channels.contains(ch)){ ch++; }
    return ch;
}

void ShadeGroupModel::setScheduleToController()
{
    // update controller if on wlan
    if(mControllerAPI->controllerState() == ControllerAPI::ControllerState::Wlan){
        mControllerAPI->currentConfig().clearSchedule();
        for(vector<unique_ptr<ShadeGroup>>::iterator i = mShadeGroups->begin(); i != mShadeGroups->end(); i++)
            mControllerAPI->currentConfig().addSchedule(i->get()->channel(), i->get()->days(), i->get()->openAt(), i->get()->closeAt());
        mControllerAPI->pushConfig();
    }
    else emit scheduleSetFailed();
}

void ShadeGroupModel::onScheduleSet()
{
    UserData::instance().persistItems(*mShadeGroups.get(), true);
    emit scheduleSet();
}

void ShadeGroupModel::setIpAddress(const QString &ipAddress)
{
    mIpAddress = ipAddress;

    if(mIpAddress == "0.0.0.0")
        mControllerAPI->setControllerState(ControllerAPI::ControllerState::Wan);
    else{
        mControllerAPI->setIpAddress(ipAddress);
        mControllerAPI->fetchConfig();
    }

    emit ipAddressChanged();
}
