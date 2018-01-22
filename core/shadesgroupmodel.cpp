#include "shadesgroupmodel.h"

using namespace std;

ShadesGroupModel::ShadesGroupModel(QObject* parent):
    QAbstractListModel(parent),
    mDb(DatabaseManager::instance()),
    mControllerId(-1),
    mShadesGroups(new vector<unique_ptr<ShadesGroup>>())
{

}

QModelIndex ShadesGroupModel::addShadeGroup(const ShadesGroup &shadesGroup)
{
    int rowsIndex = rowCount();
    beginInsertRows(QModelIndex(), rowsIndex, rowsIndex);
    unique_ptr<ShadesGroup> newGroup(new ShadesGroup(mControllerId));
    newGroup->setName(shadesGroup.name());
    newGroup->setPosition(shadesGroup.position());
    mDb.shadesGroupsDao.addShadesGroup(*newGroup);
    mShadesGroups->push_back(move(newGroup));
    endInsertRows();
    return index(rowsIndex, 0);
}

int ShadesGroupModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mShadesGroups->size();
}

QVariant ShadesGroupModel::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index))
        return QVariant();

    const ShadesGroup group = *mShadesGroups->at(index.row());

    switch (role) {
    case Roles::NameRole:
        return group.name();
    case Roles::PositionRole:
        return group.position();
    default:
        return QVariant();
    }
}

bool ShadesGroupModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!isIndexValid(index))
        return false;

    ShadesGroup& group = *mShadesGroups->at(index.row());

    switch (role) {
    case Roles::NameRole:
        group.setName(value.toString());
        break;
    case Roles::PositionRole:
        group.setPosition(value.toInt());
        break;
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
        mShadesGroups.reset(new vector<unique_ptr<ShadesGroup>>());
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

void ShadesGroupModel::addShadesGroupWithData(int controllerId, const QString &name, int position)
{
    ShadesGroup aGroup(controllerId);
    aGroup.setName(name);
    aGroup.setPosition(position);
    addShadeGroup(aGroup);
}
