#ifndef USERDATA_H
#define USERDATA_H

#include <QObject>
#include <QThread>
#include <QFuture>

#include "core/DatabaseManager.h"
#include "CurrentUser.h"
#include "core/Location.h"
#include "core/Controller.h"
#include "core/ShadeGroup.h"
#include "core/LocalCache.h"
#include "aws/CognitoSyncAPI.h"
#include "aws/ControllerThing.h"
#include "core/ControllerHTTPClient.h"

#define VERSION "0.1"

using namespace std;

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

    shared_ptr<vector<shared_ptr<Location>>> mLocations;
    shared_ptr<vector<shared_ptr<Controller>>> mControllers;
    shared_ptr<vector<shared_ptr<ShadeGroup>>> mShadeGroups;
public:
    static UserData& instance();
    virtual ~UserData();

    enum SyncState { Clean, Dirty, Editing, Syncing, Failed, Cancelled };
    Q_ENUMS(SyncState)

    void loadItems();

    QString locationBssid(const QString& uuid);

    template <class T>
    void persistChanges(const shared_ptr<vector<shared_ptr<T>>>& items, bool doReload = true);

    template <class T>
    void persistChanged(const shared_ptr<T>& item, bool doReload = true);

    int locationUtcOffset(const QString& uuid);

    shared_ptr<Location> locationOfGroup(const QString& groupUuid) const;
    shared_ptr<Location> locationOfController(const QString& controllerUuid) const;
    shared_ptr<Controller> controllerOfGroup(const QString& groupUuid) const;
    shared_ptr<vector<shared_ptr<Controller>>> controllersForLocation(const QString& locationUuid) const;
    shared_ptr<vector<shared_ptr<ShadeGroup>>> shadeGroupsForController(const QString& mac) const;

    shared_ptr<Location> location(const QString& uuid) const;
    shared_ptr<Controller> controller(const QString& uuid) const;
    shared_ptr<ShadeGroup> shadeGroup(const QString& uuid) const;

    shared_ptr<vector<shared_ptr<Location>>> locations() const;
    shared_ptr<vector<shared_ptr<Controller>>> controllers() const;
    shared_ptr<vector<shared_ptr<ShadeGroup>>> shadeGroups() const;

    void stopRequests();

    bool isControllerKnown(const QString& mac) const;

    int locationsCount();

    QString defaultLocationName() const;

    Q_INVOKABLE void createDefaultLocation();
    Q_INVOKABLE void addFirstController(const QString& mac, const QString& bssid);
    Q_INVOKABLE void createControllerInLocation(const QString& mac, const QString& bssid);

    Q_INVOKABLE void setupController(const QString& mac, const QString& bssid);

    Q_INVOKABLE void setupControllerKeys(const QString& mac, const QString& ip);
    Q_INVOKABLE bool isLocationEmpty(const QString& uuid);
    Q_INVOKABLE bool isBssidKnown(const QString& bssid);
    Q_INVOKABLE QString controllerMac(const QString& uuid) const;
    Q_INVOKABLE QString firstLocationUuid() const;
    Q_INVOKABLE int controllersCountForLocation(const QString& locationUuid) const;
    Q_INVOKABLE QString firstControllerInLocation(const QString& locationUuid) const;
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

    LocalCache<Location> locationsSyncable() const;
    LocalCache<Controller> controllersSyncable() const;
    LocalCache<ShadeGroup> shadeGroupsSyncable() const;

    void persistUpdates(LocalCache<Location> & locations, LocalCache<Controller> & controllers, LocalCache<ShadeGroup> & shadeGroups);

    bool isSyncing() const { return mSyncState == Syncing; }
    void setState(SyncState state);

    QString firstLocationUuid();

    QString email() const { return mUser.email(); }

    QString version() const { return VERSION; }

    void createThingKeys(const QString mac, const QString& ip);

};
#include "UserData.tpp"
#endif // USERDATA_H
