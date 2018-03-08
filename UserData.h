#ifndef USERDATA_H
#define USERDATA_H

#include <QObject>
#include <QThread>
#include <QFuture>
#include "core/databasemanager.h"
#include "CurrentUser.h"
#include "core/location.h"
#include "core/controller.h"
#include "core/shadegroup.h"
#include "core/SyncableRecord.h"
#include "aws/CognitoSyncAPI.h"
#include "aws/ControllerThing.h"
#include "core/ControllerHTTPClient.h"

#define VERSION "0.1"

class UserData : public QThread
{
    Q_OBJECT

    Q_PROPERTY(QString email READ email NOTIFY emailChanged)
    Q_PROPERTY(bool isSyncing READ isSyncing NOTIFY syncStateChanged)
    Q_PROPERTY(int locationsCount READ locationsCount NOTIFY locationsCountChanged)
    Q_PROPERTY(QString firstLocationUuid READ firstLocationUuid NOTIFY firstLocationUuidChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)

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

    QString locationBssid(const QString& uuid);
    int locationUtcOffset(const QString& uuid);

    void loadControllers(const QString& locationUuid);
    void persistItem(Controller &controller, bool toSync);
    void removeItem(Controller &controller, bool toSync);
    void persistItems(std::vector<std::unique_ptr<Controller>>& controllers, bool toSync);

    std::unique_ptr<std::vector<std::unique_ptr<ShadeGroup>>> shadeGroupsForController(const QString& mac) const;
    void loadShadeGroups(const QString& controllerMac);
    void persistItem(ShadeGroup &controller, bool toSync);
    void removeItem(ShadeGroup &controller, bool toSync);
    void persistItems(std::vector<std::unique_ptr<ShadeGroup>>& shadeGroups, bool toSync);

    void stopRequests();

    bool isControllerKnown(const QString& mac) const;

    Q_INVOKABLE void createDefaultLocation();
    Q_INVOKABLE void addFirstController(const QString& mac, const QString& bssid);
    Q_INVOKABLE void addControllerToLocation(const QString& mac, const QString& bssid);
    Q_INVOKABLE void setupController(const QString& mac, const QString& ip);
    Q_INVOKABLE bool isLocationEmpty(const QString& uuid);
    Q_INVOKABLE bool isBssidKnown(const QString& bssid);
    SyncState syncState() const { return mSyncState; }

    void updateLocationWithBssid(const QString& uuid, const QString& bssid);

    void clear();

public slots:
    void sync();
    void dataChanged();

private slots:
    void setupControllerSuccessful();
    void setupControllerFailed(const QString &msg);
signals:
    void dataUpdated();

    void synced();

    void syncStateChanged();
    void emailChanged();
    void locationsCountChanged();
    void firstLocationUuidChanged();
    void versionChanged();

    void installKeysAndCert(const QString &ip, const QByteArray &pubKey, const QByteArray &priKey, const QByteArray &cert);
    void controllerSetupSuccessful();
    void controllerSetupFailed(const QString& msg);
private:
    SyncState mSyncState;
    bool mCancelled;
    std::unique_ptr<CognitoSyncAPI> mSyncer;
    QFuture<void> mKeysAndCertResp;
    ControllerHTTPClient *mControllerClient;


    // flag means that data were changed while sync processed, and resync is required

    SyncableRecord<Location> locationsSyncable() const;
    SyncableRecord<Controller> controllersSyncable() const;
    SyncableRecord<ShadeGroup> shadeGroupsSyncable() const;

    void mergeUpdates(SyncableRecord<Location> & locations, SyncableRecord<Controller> & controllers, SyncableRecord<ShadeGroup> & shadeGroups);

    bool isSyncing() const { return mSyncState == Syncing; }
    void setState(SyncState state);

    int locationsCount();
    QString firstLocationUuid();

    QString email() const { return mUser.email(); }

    QString version() const { return VERSION; }

    void createThingKeys(const QString mac, const QString& ip);

};

#endif // USERDATA_H
