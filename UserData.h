#ifndef USERDATA_H
#define USERDATA_H

#include <QObject>
#include <QThread>
#include "core/databasemanager.h"
#include "CurrentUser.h"
#include "core/location.h"
#include "core/controller.h"
#include "core/shadegroup.h"
#include "core/SyncableRecord.h"
#include "aws/CognitoSyncAPI.h"

class UserData : public QThread
{
    Q_OBJECT

    Q_PROPERTY(QString email READ email NOTIFY emailChanged)
    Q_PROPERTY(bool isSyncing READ isSyncing NOTIFY syncStateChanged)

    void run() override;

    UserData();
    DatabaseManager& mDb;
    CurrentUser& mUser;
public:
    static UserData& instance();
    virtual ~UserData();

    enum SyncState { Clean, Dirty, Editing, Syncing, Failed, Cancelled };
    Q_ENUMS(SyncState)

    std::unique_ptr<std::vector<std::unique_ptr<Location>>> mLocations;
    std::unique_ptr<std::vector<std::unique_ptr<Controller>>> mControllers;
    std::unique_ptr<std::vector<std::unique_ptr<ShadeGroup>>> mShadeGroups;

    void loadLocations();
    void persistItem(Location& location, bool toSync);
    void removeItem(Location& location, bool toSync);
    void persistItems(std::vector<std::unique_ptr<Location>>& locations, bool toSync);

    void loadControllers(const QString& locationUuid);
    void persistItem(Controller &controller, bool toSync);
    void removeItem(Controller &controller, bool toSync);
    void persistItems(std::vector<std::unique_ptr<Controller>>& controllers, bool toSync);

    void loadShadeGroups(const QString& controllerMac);
    void persistItem(ShadeGroup &controller, bool toSync);
    void removeItem(ShadeGroup &controller, bool toSync);
    void persistItems(std::vector<std::unique_ptr<ShadeGroup>>& shadeGroups, bool toSync);

    void stopRequests();

    bool isControllerKnown(const QString& mac) const;

public slots:
    void sync();
    void dataChanged();

signals:
    void dataUpdated();

    void syncStateChanged();
    void emailChanged();

private:
    SyncState mSyncState;
    bool mCancelled;
    std::unique_ptr<CognitoSyncAPI> mSyncer;

    // flag means that data were changed while sync processed, and resync is required

    SyncableRecord<Location> locationsSyncable() const;
    SyncableRecord<Controller> controllersSyncable() const;
    SyncableRecord<ShadeGroup> shadeGroupsSyncable() const;

    void mergeUpdates(SyncableRecord<Location> & locations, SyncableRecord<Controller> & controllers, SyncableRecord<ShadeGroup> & shadeGroups);

    bool isSyncing() const { return mSyncState == Syncing; }
    void setState(SyncState state);

    QString email() const { return mUser.email(); }

};

#endif // USERDATA_H
