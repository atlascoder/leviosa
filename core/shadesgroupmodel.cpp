#include "shadesgroupmodel.h"
#include <QSet>

using namespace std;

ShadesGroupModel::ShadesGroupModel(QObject* parent):
    QAbstractListModel(parent),
    mDb(DatabaseManager::instance()),
    mControllerId(-1),
    mSelectedIndex(QModelIndex()),
    mShadesGroups(new vector<unique_ptr<ShadeGroup>>())
{

}

QModelIndex ShadesGroupModel::addShadeGroup(const ShadeGroup &shadesGroup)
{
    int rowsIndex = rowCount();
    beginInsertRows(QModelIndex(), rowsIndex, rowsIndex);
    unique_ptr<ShadeGroup> newGroup(new ShadeGroup(shadesGroup.controllerId()));
    newGroup->setName(shadesGroup.name());
    newGroup->setPosition(static_cast<int>(mShadesGroups->size()));
    newGroup->setOpenAt(shadesGroup.openAt());
    newGroup->setCloseAt(shadesGroup.closeAt());
    newGroup->setDays(shadesGroup.days());
    newGroup->setChannel(shadesGroup.channel());
    mDb.shadesGroupsDao.addShadesGroup(*newGroup);
    mShadesGroups->push_back(move(newGroup));
    endInsertRows();
    return index(rowsIndex, 0);
}

void ShadesGroupModel::removeShadesGroup(int shadesGroupId)
{
    int row = 0;
    int rows = rowCount();
    QModelIndex idx = QModelIndex();
    while(row < rows){
        idx = this->index(row);
        if(data(idx, Roles::IdRole) == shadesGroupId)
            break;
        else
            row++;
    }
    if(isIndexValid(idx)){
        beginRemoveRows(idx.parent(), row, row);
        mDb.shadesGroupsDao.removeShadesGroup(shadesGroupId);
        endRemoveRows();
    }
    else{
        beginResetModel();
        mDb.shadesGroupsDao.removeShadesGroup(shadesGroupId);
        endResetModel();
    }
}

int ShadesGroupModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(mShadesGroups->size());
}

QVariant ShadesGroupModel::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index))
        return QVariant();

    const ShadeGroup& group = *mShadesGroups->at(index.row());

    switch (role) {
    case Roles::IdRole:
        return group.id();
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
    case Roles::ChannelRole:
        return group.channel();
    default:
        return QVariant();
    }
}

bool ShadesGroupModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!isIndexValid(index))
        return false;

    ShadeGroup& group = *mShadesGroups->at(index.row());

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
    mDb.shadesGroupsDao.updateShadesGroup(group);
    emit dataChanged(index, index);
    return true;
}

bool ShadesGroupModel::isIndexValid(const QModelIndex &index) const
{
    return index.row() >= 0 && index.row() < static_cast<int>(mShadesGroups->size()) && index.isValid();
}

QHash<int, QByteArray> ShadesGroupModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::IdRole] = "id";
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
    roles[Roles::ChannelRole] = "channel";
    return roles;
}

void ShadesGroupModel::setController(int controllerId)
{
    beginResetModel();
    mControllerId = controllerId;
    loadGroupsShades(controllerId);
    endResetModel();
}

void ShadesGroupModel::loadGroupsShades(int controllerId)
{
    if(controllerId < 0){
        mShadesGroups.reset(new vector<unique_ptr<ShadeGroup>>());
        return;
    }
    mShadesGroups = mDb.shadesGroupsDao.shadesGroupsForController(controllerId);
}

int ShadesGroupModel::controllerId() const
{
    return mControllerId;
}

void ShadesGroupModel::setControllerId(int controllerId)
{
    setController(controllerId);
}

void ShadesGroupModel::addShadesGroupWithData(int controllerId, const QString &name)
{
    ShadeGroup aGroup(controllerId);
    aGroup.setName(name);
    aGroup.setPosition(static_cast<int>(mShadesGroups->size()));
    aGroup.setChannel(findFreeChannel(controllerId));
    addShadeGroup(aGroup);
}

int ShadesGroupModel::getSelectedGroupId() const
{
    return isIndexValid(mSelectedIndex) ? data(mSelectedIndex, Roles::IdRole).toInt() : -1;
}

void ShadesGroupModel::setSelectedGroupId(int id)
{
    int row= 0;
    int rows = rowCount();
    mSelectedIndex = QModelIndex();
    while(row < rows){
        if(mShadesGroups->at(row)->id() == id){
            mSelectedIndex = this->index(row);
            break;
        }
        row++;
    }
    emit selectedGroupIdChanged();
    emit selectedGroupNameChanged();
    emit selectedGroupPositionChanged();
    emit selectedGroupOpenAtChanged();
    emit selectedGroupCloseAtChanged();
    emit selectedGroupDaysChanged();
}

QString ShadesGroupModel::getSelectedGroupName() const
{
    return data(mSelectedIndex, Roles::NameRole).toString();
}

void ShadesGroupModel::setSelectedGroupName(const QString &name)
{
    setData(mSelectedIndex, name, Roles::NameRole);
}

int ShadesGroupModel::getSelectedGroupPosition() const
{
    return data(mSelectedIndex, Roles::PositionRole).toInt();
}

void ShadesGroupModel::setSelectedGroupPosition(int position)
{
    ShadeGroup& group = *mShadesGroups->at(mSelectedIndex.row());
    if(group.position() != position){
        vector<int> idxs;
        int pos = 0;
        int rows = rowCount();
        int row = 0;
        while(pos < rows){
            if(position == pos){
                idxs.push_back(mSelectedIndex.row());
                pos++;
            }
            else if(row == mSelectedIndex.row()){
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
        loadGroupsShades(mControllerId);
    }
}

int ShadesGroupModel::roleByName(const QString& name) const
{
    QHash<int, QByteArray> rh = roleNames();
    QHash<int, QByteArray>::iterator i;
    for(i = rh.begin(); i != rh.end(); ++i){
        if (i.value().toStdString() == name.toStdString())
            return i.key();
    }
    return -1;
}

int ShadesGroupModel::getSelectedGroupOpenAt() const
{
    return data(mSelectedIndex, Roles::OpenAtRole).toInt();
}

void ShadesGroupModel::setSelectedGroupOpenAt(int openAt)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, openAt, Roles::OpenAtRole);
        emit selectedGroupOpenAtChanged();
    }
}

int ShadesGroupModel::getSelectedGroupCloseAt() const
{
    return data(mSelectedIndex, Roles::CloseAtRole).toInt();
}

void ShadesGroupModel::setSelectedGroupCloseAt(int closeAt)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, closeAt, Roles::CloseAtRole);
        emit selectedGroupCloseAtChanged();
    }
}

int ShadesGroupModel::getSelectedGroupDays() const
{
    return data(mSelectedIndex, Roles::DaysRole).toInt();
}

void ShadesGroupModel::setSelectedGroupDays(int days)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, days, Roles::DaysRole);
        emit selectedGroupDaysChanged();
    }
}

QString ShadesGroupModel::getSelectedOpenAtText() const
{
    return data(mSelectedIndex, Roles::OpenAtTextRole).toString();
}

QString ShadesGroupModel::getSelectedCloseAtText() const
{
    return data(mSelectedIndex, Roles::CloseAtTextRole).toString();
}

Shade::ShadeState ShadesGroupModel::getSelectedShadeState() const
{
    return static_cast<Shade::ShadeState>(data(mSelectedIndex, Roles::ShadeStateRole).toInt());
}

void ShadesGroupModel::setSelectedShadeState(Shade::ShadeState state)
{
    if(isIndexValid(mSelectedIndex)){
        setData(mSelectedIndex, state, Roles::ShadeStateRole);
        emit selectedShadeStateChanged();
    }
}

void ShadesGroupModel::setStateToAll(const Shade::ShadeState state)
{
    int row = 0;
    int rows = rowCount();
    while(row < rows){
        setData(this->index(row), static_cast<int>(state), Roles::ShadeStateRole);
    }
}

void ShadesGroupModel::setStateToAll(const int state)
{
    int row = 0;
    int rows = rowCount();
    while(row < rows){
        setData(this->index(row++), state, Roles::ShadeStateRole);
    }
}

float ShadesGroupModel::getSelectedOpenAtUS() const
{
    return data(mSelectedIndex, Roles::OpenAtUSRole).toFloat();
}

void ShadesGroupModel::setSelectedOpenAtUS(const float openAtUS)
{
    setData(mSelectedIndex, openAtUS, Roles::OpenAtUSRole);
}

float ShadesGroupModel::getSelectedCloseAtUS() const
{
    return data(mSelectedIndex, Roles::CloseAtUSRole).toFloat();
}

void ShadesGroupModel::setSelectedCloseAtUS(const float closeAtUS)
{
    setData(mSelectedIndex, closeAtUS, Roles::CloseAtUSRole);
}

void ShadesGroupModel::setScheduleForCurrentController(int days, float openAtUS, float closeAtUS)
{
    int row = 0;
    int rows = rowCount();
    QModelIndex idx;
    while(row < rows){
        idx = this->index(row);
        setData(idx, days, Roles::DaysRole);
        setData(idx, openAtUS, Roles::OpenAtUSRole);
        setData(idx, closeAtUS, Roles::CloseAtUSRole);
        row++;
    }
}

QStringList ShadesGroupModel::getPositionOrder() const
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

char ShadesGroupModel::getSelectedChannel() const
{
    return (char)data(mSelectedIndex, Roles::ChannelRole).toInt();
}

void ShadesGroupModel::setSelectedChannel(char channel)
{
    setData(mSelectedIndex, channel, Roles::ChannelRole);
}

char ShadesGroupModel::findFreeChannel(int controllerId) const
{
    QSet<char> channels;
    if(controllerId){
        int l = mShadesGroups->size();
        for(int i = 0; i < l; i++)
            channels.insert(mShadesGroups->at(i)->channel());
    }
    else{
        std::unique_ptr<std::vector<std::unique_ptr<ShadeGroup>>> shadesGroups = mDb.shadesGroupsDao.shadesGroupsForController(controllerId);
        int l = shadesGroups->size();
        for(int i = 0; i < l; i++)
            channels.insert(shadesGroups->at(i)->channel());
    }

    char ch = 1;
    while(channels.contains(ch)){ ch++; }
    return ch;
}
