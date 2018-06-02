#include "LocationsModel.h"
#include "UserData.h"
#include "TimeZoneModel.h"
#include "ControllerConnectionsManager.h"
#include "aws/CognitoSyncCommand.h"


#include <QMutexLocker>
#include <QTime>
#include <QDate>
#include <QTimeZone>
#include <QStringList>
#include <QtConcurrent>
#include <memory>

using namespace std;

LocationsModel::LocationsModel(QObject* parent):
    QAbstractListModel(parent),
    mIsActive(false),
    mLocations(new vector<shared_ptr<Location>>)
{
//    mTimer = new QTimer(this);
//    mTimer->setSingleShot(true);
//    connect(mTimer, &QTimer::timeout, this, &LocationsModel::onTimer);
    connect(&UserData::instance(), &UserData::dataUpdated, this, &LocationsModel::reloadData);
    reloadData();
}

void LocationsModel::reloadData()
{
    beginResetModel();
    mLocations = UserData::instance().locations();
    endResetModel();
    emit singleChanged();
}

int LocationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(mLocations->size());
}

QVariant LocationsModel::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index))
        return QVariant();

    shared_ptr<Location> location = mLocations->at(index.row());

    switch (role) {
    case Roles::UuidRole:
        return location->uuid();
    case Roles::NameRole:
        return location->name();
    case Roles::PositionRole:
        return location->position();
    case Roles::BssidRole:
        return location->bssid();
    case Roles::TimeZoneRole:
        return location->timezone();
    case Roles::LocationTime: {
        QString s = location->timezone();
        QTimeZone tz(s.toLatin1());
        QDateTime utc = QDateTime::currentDateTimeUtc();
        return utc.toTimeZone(tz).toString("hh:mm ") + location->timezone();
    }
    default:
        return QVariant();
    }
}

bool LocationsModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if(!isIndexValid(index)) return false;
    shared_ptr<Location> location = mLocations->at(index.row());
    switch (role) {
    case Roles::NameRole:
        location->setName(value.toString());
        break;
    default:
        return false;
    }
    location->setChanged(true);
    UserData::instance().persistChanged(location);
    emit dataChanged(index, index);
    return true;
}

bool LocationsModel::isIndexValid(const QModelIndex &index) const
{
    return index.row() >= 0 && index.row() < static_cast<int>(mLocations->size()) && index.isValid();
}

QHash<int, QByteArray> LocationsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::UuidRole] = "uuid";
    roles[Roles::NameRole] = "name";
    roles[Roles::TimeZoneRole] = "timezone";
    roles[Roles::PositionRole] = "position";
    roles[Roles::BssidRole] = "bssid";
    roles[Roles::LocationTime] = "locationTime";
    return roles;
}

bool LocationsModel::single() const
{
    return mLocations->size() < 2;
}

bool LocationsModel::active() const
{
    return mIsActive;
}

void LocationsModel::setActive(bool isActive)
{
    if (isActive == mIsActive) return;
    mIsActive = isActive;
//    if (mIsActive) {
//        updateTime();
//        mTimer->setInterval(60000 - QTime::currentTime().msecsSinceStartOfDay() % 60000);
//        mTimer->start();
//    }
//    else {
//        mTimer->stop();
//    }
    emit activeChanged();
}

//void LocationsModel::onTimer()
//{
//    updateTime();
//    mTimer->setInterval(60000 - QTime::currentTime().msecsSinceStartOfDay() % 60000);
//    if (mIsActive) mTimer->start();
//}

//void LocationsModel::updateTime()
//{
//    QVector<int> roles;
//    roles.append(Roles::LocationTime);
//    emit dataChanged(index(0), index(rowCount()-1), roles);
//}

void LocationsModel::commandShades(const int command)
{
    QtConcurrent::run(this, &LocationsModel::sendCommandToShades, command);
}

void LocationsModel::sendCommandToShades(const int command)
{
    auto locations = UserData::instance().locations();

    QStringList locations_wan;

    for (auto& location: *locations) {
        if (NetworkMonitor::instance().isOnWlan() && location->bssid() == NetworkMonitor::instance().bssid()) {
            // send via http
            auto controllers = UserData::instance().controllers();
            for (auto& controller: *controllers) {
                auto api = ControllerConnectionsManager::instance().controllerAPI(controller->uuid());
                if (api->onWlan()) {
                    api->shadeCmd(0, command);
                }
                else {
                    qDebug() << "Controller is not discovered";
                }
            }
        }
        else {
            // prepare to send via WAN
            locations_wan.append(location->uuid());
        }
    }

    if (!locations_wan.empty()) {
        // send via WAN
        unique_ptr<CognitoSyncCommand> commander(new CognitoSyncCommand);
        QString cmd = ControllerAPI::commandCode2String(command);
        QString loc_list = locations_wan.join(",");
        commander->sendCommand(loc_list, cmd, 0);
    }
}
