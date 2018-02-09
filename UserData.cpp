#include "UserData.h"
#include "aws/CredentialsRequest.h"
#include "CurrentUser.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

#include <QDebug>

using namespace std;

UserData& UserData::instance()
{
    static UserData sInstance;
    return sInstance;
}

UserData::UserData() :
    mDb(DatabaseManager::instance()),
    mUser(CurrentUser::instance()),
    mLocations(new std::vector<std::unique_ptr<Location>>),
    mControllers(new std::vector<std::unique_ptr<Controller>>),
    mShadeGroups(new std::vector<std::unique_ptr<ShadeGroup>>),
    mSyncState(Clean),
    mCancelled(false),
    mSyncer(new CognitoSyncAPI)
{
    loadLocations();
}

void UserData::stopRequests()
{
    qDebug() << "UserData: stop requests";
    if(mCancelled) return;
    mCancelled = true;
    CurrentUser::instance().stopRequests();
    mSyncer->cancelRequests();
}

void UserData::run()
{
    if(mSyncState == Syncing || mSyncState == Editing || mCancelled)
        return;
    else
        setState(Syncing);

    CurrentUser & u = CurrentUser::instance();
    if(!mCancelled){
        u.requestCredentials();
        if(mCancelled) return;
        if(u.isAuthenticated() && u.hasCredentials()){
            Aws::Auth::AWSCredentials creds;
            u.fillCredentials(creds);
            SyncableRecord<Location> locations(*mDb.locationsDao.items().get(), mUser.locationsModified(), mUser.locationsSyncCount(), mUser.locationsSynced());
            SyncableRecord<Controller> controllers(*mDb.controllersDao.items().get(), mUser.controllersModified(), mUser.controllersSyncCount(), mUser.controllersSynced());
            SyncableRecord<ShadeGroup> shadeGroups(*mDb.shadeGroupsDao.items().get(), mUser.shadeGroupsModified(), mUser.shadeGroupsSyncCount(), mUser.shadeGroupsSynced());
            if(!mCancelled){
                mSyncer->resetWithCredentials(creds);
                mSyncer->sync(creds, "config", locations, controllers, shadeGroups);
                mergeUpdates(locations, controllers, shadeGroups);
            }
        }
    }
    setState(Clean);
}

UserData::~UserData()
{
    stopRequests();
    if(isRunning()){
        this->quit();
        this->wait();
    }
}

void UserData::mergeUpdates(SyncableRecord<Location> &locations, SyncableRecord<Controller> &controllers, SyncableRecord<ShadeGroup> &shadeGroups)
{
    bool updated = false;
    // locations
    if(locations.updated()){
        vector<unique_ptr<Location>>& items = locations.items();
        qDebug() << "Merging locations update // syncCount: " <<  locations.syncCount();
        for(vector<unique_ptr<Location>>::iterator item = items.begin(); item != items.end(); item++){
            item->get()->setSynced(true);
            mDb.locationsDao.persistItem(*item->get(), false);
        }
        updated = true;
    }
    else{
        qDebug() << "Merging locations not required";
    }

    if(locations.synced()){
        updated = true;
        mUser.setLocationsModified(locations.lastModified());
        mUser.setLocationsSyncCount(locations.syncCount());
        mUser.setLocationsSynced(true);
    }

    size_t to_del = locations.deletedIdx().size();

    if(to_del > 0){
        qDebug() << "Deleting " << to_del << "locations";
        for(int idx : locations.deletedIdx())
            mDb.locationsDao.destroy(*locations.items().at(idx), false);
        updated = true;
    }

    // constrollers
    if(controllers.updated()){
        vector<unique_ptr<Controller>>& items = controllers.items();
        qDebug() << "Merging controllers update // syncCount: " <<  controllers.syncCount();
        for(vector<unique_ptr<Controller>>::iterator item = items.begin(); item != items.end(); item++){
            item->get()->setSynced(true);
            mDb.controllersDao.persistItem(*item->get(), false);
        }
        updated = true;
    }
    else{
        qDebug() << "Merging controllers not required";
    }

    if(controllers.synced()){
        updated = true;
        mUser.setControllersModified(controllers.lastModified());
        mUser.setControllersSyncCount(controllers.syncCount());
        mUser.setControllersSynced(true);
    }

    to_del = controllers.deletedIdx().size();

    if(to_del > 0){
        qDebug() << "Deleting " << to_del << " controllers";
        for(int idx : controllers.deletedIdx())
            mDb.controllersDao.destroy(*controllers.items().at(idx), false);
        updated = true;
    }

    // shadeGroups
    if(shadeGroups.updated()){
        vector<unique_ptr<ShadeGroup>>& items = shadeGroups.items();
        qDebug() << "Merging shadeGroups update // syncCount: " <<  shadeGroups.syncCount();
        for(vector<unique_ptr<ShadeGroup>>::iterator item = items.begin(); item != items.end(); item++){
            item->get()->setSynced(true);
            mDb.shadeGroupsDao.persistItem(*item->get(), false);
        }
        updated = true;
    }
    else{
        qDebug() << "Merging shadeGroups not required";
    }

    if(shadeGroups.synced()){
        updated = true;
        mUser.setShadeGroupsModified(shadeGroups.lastModified());
        mUser.setShadeGroupsSyncCount(shadeGroups.syncCount());
        mUser.setShadeGroupsSynced(true);
    }

    to_del = shadeGroups.deletedIdx().size();

    if(to_del > 0){
        qDebug() << "Deleting " << to_del << " shades groups";
        for(int idx : shadeGroups.deletedIdx())
            mDb.shadeGroupsDao.destroy(*shadeGroups.items().at(idx), false);
        updated = true;
    }

    if(updated){
        mUser.persistUserDataModified();
        emit dataUpdated();
    }
}

void UserData::sync()
{
    if(!isRunning()){
        this->start();
    }
}

void UserData::dataChanged()
{
    emit dataUpdated();
    sync();
}

void UserData::setState(SyncState state)
{
    if(state == mSyncState) return;
    mSyncState = state;
    emit syncStateChanged();
}

/*
 *
 *  Locations iterface for models
 *
 */
void UserData::loadLocations()
{
    mLocations->clear();
    mDb.locationsDao.loadItems(*mLocations.get());
}

void UserData::persistItem(Location& location, bool toSync)
{
    mDb.locationsDao.persistItem(location, false);
    if(toSync){
        CurrentUser::instance().setLocationsModified(QDateTime::currentSecsSinceEpoch());
        CurrentUser::instance().setLocationsSynced(false);
        CurrentUser::instance().persistUserDataModified();
        sync();
    }
}

void UserData::persistItems(std::vector<std::unique_ptr<Location>> &locations, bool toSync)
{

    for (auto ci = locations.begin(); ci != locations.end(); ci++){
        mDb.locationsDao.persistItem(*ci->get(), false);
    }
    if(toSync){
        CurrentUser::instance().setLocationsModified(QDateTime::currentSecsSinceEpoch());
        CurrentUser::instance().setLocationsSynced(false);
        CurrentUser::instance().persistUserDataModified();
        sync();
    }
}

void UserData::removeItem(Location &location, bool toSync)
{
    mDb.locationsDao.destroy(location, false);
    if(toSync){
        CurrentUser::instance().setLocationsModified(QDateTime::currentSecsSinceEpoch());
        CurrentUser::instance().setLocationsSynced(false);
        CurrentUser::instance().persistUserDataModified();
        sync();
    }
}

/*
 *
 * Controllers interface for models
 *
 */

void UserData::loadControllers(const QString &locationUuid)
{
    QMap<QString, QString> mac_ip;
    for(vector<unique_ptr<Controller>>::iterator i = mControllers->begin(); i != mControllers->end(); i++){
        if(i->get()->ipAddress() != "0.0.0.0")
            mac_ip.insert(i->get()->mac(), i->get()->ipAddress());
    }
    mControllers->clear();
    unique_ptr<vector<unique_ptr<Controller>>> new_c(new vector<unique_ptr<Controller>>);

    mDb.controllersDao.loadFiltered(*new_c.get(), "locationUuid", locationUuid);
    for(vector<unique_ptr<Controller>>::iterator i = new_c->begin(); i !=new_c->end(); i++){
        if(mac_ip.contains(i->get()->mac())){
            i->get()->setIpAddress(mac_ip[i->get()->mac()]);
            i->get()->api().setControllerState(ControllerAPI::ControllerState::Wlan);
        }
        mControllers->push_back(move(*i));
    }
}

void UserData::persistItem(Controller& controller, bool toSync)
{
    mDb.controllersDao.persistItem(controller, false);
    if(toSync){
        CurrentUser::instance().setControllersModified(QDateTime::currentSecsSinceEpoch());
        CurrentUser::instance().setControllersSynced(false);
        CurrentUser::instance().persistUserDataModified();
        sync();
    }
}

void UserData::persistItems(std::vector<std::unique_ptr<Controller>> &controllers, bool toSync)
{

    for (auto ci = controllers.begin(); ci != controllers.end(); ci++){
        mDb.controllersDao.persistItem(*ci->get(), false);
    }
    if(toSync){
        CurrentUser::instance().setControllersModified(QDateTime::currentSecsSinceEpoch());
        CurrentUser::instance().setControllersSynced(false);
        CurrentUser::instance().persistUserDataModified();
        sync();
    }
}

void UserData::removeItem(Controller &controller, bool toSync)
{
    mDb.controllersDao.destroy(controller, false);
    if(toSync){
        CurrentUser::instance().setControllersModified(QDateTime::currentSecsSinceEpoch());
        CurrentUser::instance().setControllersSynced(false);
        CurrentUser::instance().persistUserDataModified();
        sync();
    }
}

/*
 *
 * ShadeGroups interface for models
 *
 */

void UserData::loadShadeGroups(const QString& mac)
{
    mShadeGroups->clear();
    mDb.shadeGroupsDao.loadFiltered(*mShadeGroups.get(), "controllerMac", mac);
}

void UserData::persistItem(ShadeGroup& shadeGroup, bool toSync)
{
    mDb.shadeGroupsDao.persistItem(shadeGroup, false);
    if(toSync){
        CurrentUser::instance().setShadeGroupsModified(QDateTime::currentSecsSinceEpoch());
        CurrentUser::instance().setShadeGroupsSynced(false);
        CurrentUser::instance().persistUserDataModified();
        sync();
    }
}

void UserData::persistItems(std::vector<std::unique_ptr<ShadeGroup>> &shadeGroups, bool toSync)
{

    for (auto ci = shadeGroups.begin(); ci != shadeGroups.end(); ci++){
        mDb.shadeGroupsDao.persistItem(*ci->get(), false);
    }
    if(toSync){
        CurrentUser::instance().setShadeGroupsModified(QDateTime::currentSecsSinceEpoch());
        CurrentUser::instance().setShadeGroupsSynced(false);
        CurrentUser::instance().persistUserDataModified();
        sync();
    }
}

void UserData::removeItem(ShadeGroup &shadeGroup, bool toSync)
{
    mDb.shadeGroupsDao.destroy(shadeGroup, false);
    if(toSync){
        CurrentUser::instance().setShadeGroupsModified(QDateTime::currentSecsSinceEpoch());
        CurrentUser::instance().persistUserDataModified();
        sync();
    }
}

bool UserData::isControllerKnown(const QString &mac) const
{
    auto filtered = mDb.controllersDao.filtered("mac", mac);
    return filtered->size() > 0;
}

unique_ptr<vector<unique_ptr<ShadeGroup>>> UserData::shadeGroupsForController(const QString &mac) const
{
    unique_ptr<vector<unique_ptr<ShadeGroup>>> v(new vector<unique_ptr<ShadeGroup>>);
    mDb.shadeGroupsDao.loadFiltered(*v.get(), "controllerMac", mac);
    return v;
}

void UserData::createDefaultLocation()
{
    Location l;
    l.setName("My Home");
    l.setSynced("false");
    QDateTime local(QDateTime::currentDateTime());
    QDateTime utc(local.toUTC());
    QDateTime dt(utc.date(), utc.time(), Qt::LocalTime);
    l.setUtcOffset(dt.secsTo(local));
    l.setPosition(0);
    persistItem(l, true);
}

void UserData::updateLocationWithBssid(const QString &uuid, const QString &bssid)
{
    unique_ptr<vector<unique_ptr<Location>>> loc = mDb.locationsDao.filtered("uuid", uuid);
    for(vector<unique_ptr<Location>>::iterator i = loc->begin(); i != loc->end(); i++){
        i->get()->setBssid(bssid);
        mDb.locationsDao.persistItem(*i->get(), true);
    }
    CurrentUser::instance().setLocationsSynced(false);
}

QString UserData::locationBssid(const QString &uuid)
{
    auto l = mDb.locationsDao.filtered("uuid", uuid);
    if(l->size() == 0)
        return "";
    else
        return l->at(0)->bssid();
}

int UserData::locationUtcOffset(const QString &uuid)
{
    auto l = mDb.locationsDao.filtered("uuid", uuid);
    if(l->size() == 0)
        return 0;
    else
        return l->at(0)->utcOffset();
}
