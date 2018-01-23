#ifndef USERDATA_H
#define USERDATA_H

#include <QObject>
#include <QThread>
#include "core/databasemanager.h"
#include "CurrentUser.h"
#include "core/userlocation.h"
#include "core/locationcontroller.h"
#include "core/shadegroup.h"

class UserData : public QThread
{
    Q_OBJECT

    Q_PROPERTY(bool isSyncing READ isSyncing NOTIFY syncStateChanged)

    void run() override;

    UserData() :
        mDb(DatabaseManager::instance()),
        mUser(CurrentUser::instance()),
        mLocations(mDb.locationsDao.items()),
        mControllers(mDb.controllersDao.items()),
        mShadeGroups(mDb.shadeGroupsDao.items()),
        mSyncState(Clean)
    {
//        inspectData();
    }
public:
    static UserData& instance();
    virtual ~UserData();

    enum SyncState { Clean, Dirty, Editing, Syncing, Failed, Cancelled };
    Q_ENUMS(SyncState)

    std::unique_ptr<std::vector<std::unique_ptr<UserLocation>>> mLocations;
    std::unique_ptr<std::vector<std::unique_ptr<LocationController>>> mControllers;
    std::unique_ptr<std::vector<std::unique_ptr<ShadeGroup>>> mShadeGroups;

public slots:
    void sync();
    void dataChanged();

signals:
    void dataUpdated();

    void syncStateChanged();

private:
    DatabaseManager& mDb;
    CurrentUser& mUser;

    SyncState mSyncState;

    // check for local data changes and mark set state as Clean or Dirty
//    void inspectData();

    Syncable locationsSyncable() const;
    Syncable controllersSyncable() const;
    Syncable shadeGroupsSyncable() const;

    void mergeUpdates(Syncable & locations, Syncable & controllers, Syncable & shadeGroups);

    bool isSyncing() const { return mSyncState == Syncing; }
    void setState(SyncState state);

};

#endif // USERDATA_H
