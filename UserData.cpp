#include "UserData.h"
#include "CurrentUser.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QtConcurrent>
#include <QString>

#include <QDebug>

#include "aws/ControllerThing.h"
#include "core/TimeZoneModel.h"
#include "core/ControllerSchedule.h"

using namespace std;

UserData& UserData::instance()
{
    static UserData sInstance;
    return sInstance;
}

UserData::UserData() :
    mDb(DatabaseManager::instance()),
    mUser(CurrentUser::instance()),
    mLocations(new vector<shared_ptr<Location>>),
    mControllers(new vector<shared_ptr<Controller>>),
    mShadeGroups(new vector<shared_ptr<ShadeGroup>>),
    mSyncState(Clean),
    mCancelled(false),
    mSyncer(new CognitoSyncAPI)
{
    loadItems();
}

template <class T>
void putItem(const shared_ptr<vector<shared_ptr<T>>>& items, QString& uuid, QJsonObject& obj, long long syncCount) {
    auto i = find_if(items->begin(), items->end(), [&uuid](const shared_ptr<T>& _i)->bool{ return _i->uuid() == uuid; });
    if (i != items->end()) {
        i->get()->withJson(obj);
        i->get()->setSyncCount(static_cast<int>(syncCount));
    }
    else {
        shared_ptr<T> t(new T(uuid));
        t->withJson(obj);
        t->setSyncCount(static_cast<int>(syncCount));
        items->push_back(t);
    }
}

template <class T>
void sortOrdered(const shared_ptr<vector<shared_ptr<T>>>& items){
    sort(items->begin(), items->end(), [](const shared_ptr<T>& a, const shared_ptr<T>&b)->bool{ return a->position() < b->position(); });
}

QString sanitizeMac(const QString& mac)
{
    QString cmac;
    if (mac.length()>12) {
        for (auto c : mac) {
            if (c==":") continue;
            cmac += c;
        }
    }
    else{
        cmac = mac;
    }
    return cmac.toUpper();
}

void UserData::loadItems()
{
    shared_ptr<vector<shared_ptr<Syncable>>> items = mDb.cognitoSyncDAO.items();
    mLocations->clear();
    mControllers->clear();
    mShadeGroups->clear();
    for (auto& item : *items) {
        // ignore deleted item
        if (item->isDeleted()) continue;

        QString json_string(item->value());
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(json_string.toStdString()), &error);
        if (error.error == QJsonParseError::NoError) {
            QJsonObject obj = doc.object();
            if (obj.contains("item_type")) {
                QString item_type = obj["item_type"].toString();
                QString uuid = item->uuid();
                if (item_type == "location") {
                    putItem<Location>(mLocations, uuid, obj, item->syncCount());
                }
                else if (item_type == "controller") {
                    putItem<Controller>(mControllers, uuid, obj, item->syncCount());
                }
                else if (item_type == "group") {
                    putItem<ShadeGroup>(mShadeGroups, uuid, obj, item->syncCount());
                }
                else {
                    qDebug() << "JSON item has unknown `item_type` => " << item_type;
                }
            }
            else {
                qDebug() << "JSON item does not contain `item_type` memeber: " << json_string;
            }
        }
        else {
            qDebug() << "failed to parse JSON content: " << json_string;
        }
    }

    sortOrdered<Location>(mLocations);
    sortOrdered<Controller>(mControllers);
    sortOrdered<ShadeGroup>(mShadeGroups);
    if (mUser.authState() == CurrentUser::Authenticated) {
        sync();
    }
}

void UserData::stopRequests()
{
    qDebug() << "UserData: stop requests";
    if(mCancelled) return;
    mCancelled = true;
    this->quit();
    this->wait();
    CurrentUser::instance().stopRequests();
}

void UserData::run()
{
    if(mSyncState == Syncing || mSyncState == Editing || mCancelled)
        return;
    else
        setState(Syncing);

    shared_ptr<vector<shared_ptr<Syncable>>> items = mDb.cognitoSyncDAO.items();
    if(mCancelled) return;
    mSyncer->syncData(items);

    if (mSyncer->isSuccessful()) {
        // update all syncCounts and reset isChanged
        mDb.cognitoSyncDAO.persistSynced(items);
        setState(Clean);
    }
    else
        setState(Failed);
}

UserData::~UserData()
{
    stopRequests();
    if(isRunning()){
        this->quit();
        this->wait();
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
    if(mSyncState == SyncState::Clean) {
        loadItems();
        if (mSyncer->isUpdated()) {
            emit dataUpdated();
        }
        emit synced();
    }
    emit syncStateChanged();
}

bool UserData::isControllerKnown(const QString &mac) const
{
    QString cmac = sanitizeMac(mac);
    auto c = find_if(mControllers->begin(), mControllers->end(), [&cmac](const shared_ptr<Controller>& _c)->bool{ return cmac == _c->mac(); });
    return c != mControllers->end();
}

QString UserData::firstLocationUuid() const
{
    if (mLocations->empty())
        return "";
    else
        return mLocations->at(0)->uuid();
}

int UserData::controllersCountForLocation(const QString &locationUuid) const
{
    int count = 0;
    for_each(mControllers->begin(), mControllers->end(), [&locationUuid, &count](const shared_ptr<Controller>& _g)->void{ if (_g->locationUuid()==locationUuid) count++; });
    return count;
}

QString UserData::firstControllerInLocation(const QString &locationUuid) const
{
    auto c = find_if(mControllers->begin(), mControllers->end(), [&locationUuid](const shared_ptr<Controller>& _c)->bool{ return _c->locationUuid() == locationUuid; });
    if (c == mControllers->end())
        return "";
    else {
        return c->get()->uuid();
    }
}

QString UserData::controllerMac(const QString& uuid) const
{
    auto i = find_if(mControllers->begin(), mControllers->end(), [&uuid](const shared_ptr<Controller>& _i)->bool{ return _i->uuid() == uuid; });
    if (i == mControllers->end())
        return "";
    else
        return i->get()->mac();
}

QString UserData::controllerUuid(const QString& mac) const
{
    auto i = find_if(mControllers->begin(), mControllers->end(), [&mac](const shared_ptr<Controller>& _i)->bool{ return _i->mac() == mac; });
    if (i == mControllers->end())
        return "";
    else
        return i->get()->uuid();
}

QString UserData::controllerTimezone(const QString& uuid) const
{
    auto i = find_if(mControllers->begin(), mControllers->end(), [&uuid](const shared_ptr<Controller>& _i)->bool{ return _i->uuid() == uuid; });
    if (i == mControllers->end())
        return TimeZoneModel::defaultTimezone();
    else {
        QString locationUuid = i->get()->locationUuid();
        auto l = find_if(mLocations->begin(), mLocations->end(), [&locationUuid](const shared_ptr<Location>& _l)->bool{ return _l->uuid() == locationUuid; });
        return l == mLocations->end() ? TimeZoneModel::defaultTimezone() : l->get()->timezone();
    }
}

shared_ptr<vector<shared_ptr<ShadeGroup>>> UserData::shadeGroupsForController(const QString& mac) const
{
    QString cmac = sanitizeMac(mac);
    shared_ptr<vector<shared_ptr<ShadeGroup>>> groups(new vector<shared_ptr<ShadeGroup>>);
    for (auto& g : *mShadeGroups) {
        if (g->controllerMac() == mac) {
            groups->push_back(g);
        }
    }
    return groups;
}

shared_ptr<vector<shared_ptr<Controller>>> UserData::controllersForLocation(const QString& locationUuid) const
{
    shared_ptr<vector<shared_ptr<Controller>>> controllers(new vector<shared_ptr<Controller>>);
    for (auto& c : *mControllers) {
        if (c->locationUuid() == locationUuid) {
            controllers->push_back(c);
        }
    }
    return controllers;
}

shared_ptr<vector<shared_ptr<Location>>> UserData::locations() const
{
    return mLocations;
}

shared_ptr<vector<shared_ptr<Controller>>> UserData::controllers() const
{
    return mControllers;
}

shared_ptr<vector<shared_ptr<ShadeGroup>>> UserData::shadeGroups() const
{
    return mShadeGroups;
}

void UserData::createDefaultLocation()
{
    shared_ptr<Location> l;
    l->setTimezone(TimeZoneModel::defaultTimezone());
    l->setPosition(0);
    mLocations->push_back(l);
    persistChanges(mLocations);
}

void UserData::updateLocationWithBssid(const QString &uuid, const QString &bssid)
{
    auto l = find_if(mLocations->begin(), mLocations->end(), [&uuid](const shared_ptr<Location>& _l)->bool{ return _l->uuid()==uuid;});
    if (l != mLocations->end()) {
        l->get()->setBssid(bssid);
        l->get()->setChanged(true);
        persistChanges(mLocations);
    }
    else {
        qDebug() << "location with UUID " << uuid << " not found to update with BSSID " << bssid;
    }
}

QString UserData::locationBssid(const QString &uuid)
{
    auto l = find_if(mLocations->begin(), mLocations->end(), [uuid](const shared_ptr<Location>& _l)->bool{ return uuid == _l->uuid(); });
    if(l == mLocations->end())
        return "";
    else
        return l->get()->bssid();
}

void UserData::clear()
{
    mLocations->clear();
    mControllers->clear();
    mShadeGroups->clear();
}

int UserData::locationsCount()
{
    return mLocations->size();
}

QString UserData::firstLocationUuid()
{
    return mLocations->empty() ? "" : mLocations->at(0)->uuid();
}

void UserData::createControllerInLocation(const QString &mac, const QString &locationUuid)
{
    QString cmac = sanitizeMac(mac);
    auto l = find_if(mLocations->begin(), mLocations->end(), [&locationUuid](const shared_ptr<Location>& _l)->bool{ return locationUuid == _l->uuid(); });

    if (l == mLocations->end()) {
        qDebug() << "Location " << locationUuid << " not found to add controller with mac " << cmac;
        return;
    }

    shared_ptr<Controller> c;
    c->setMac(cmac);
    c->setLocationUuid(l->get()->uuid());

    int n = 1;

    while (
           find_if(
               mControllers->begin(),
               mControllers->end(),
               [&n, &locationUuid](const shared_ptr<Controller>& _c)->bool{ return _c->locationUuid()==locationUuid && _c->name()==QString("Zone ").append(QString::number(n)); }) != mControllers->end()
            ) {
        n++;
    }

    mControllers->push_back(c);
    persistChanges(mControllers);
}

void UserData::addFirstController(const QString &mac, const QString &bssid)
{
    if (!mLocations->empty()) {
        qDebug() << "Can't add first controller with location - locations isn't empty";
        return;
    }

    shared_ptr<Location> l(new Location);
    l->setName("My Home");
    l->setBssid(bssid);
    l->setTimezone(TimeZoneModel::defaultTimezone());
    l->setPosition(0);
    mLocations->push_back(l);

    QString cmac = sanitizeMac(mac);

    shared_ptr<Controller> c(new Controller);
    c->setMac(cmac);
    c->setLocationUuid(mLocations->at(0)->uuid());
    mControllers->push_back(c);

    shared_ptr<ShadeGroup> g(new ShadeGroup);
    g->setControllerMac(cmac);
    g->setName("Group 1");
    g->setChannel(1);
    mShadeGroups->push_back(g);
    persistChanges(mLocations);
    persistChanges(mControllers);
    persistChanges(mShadeGroups);
    UserData::instance().sync();
}

bool UserData::isLocationEmpty(const QString &uuid)
{
    auto c = find_if(mControllers->begin(), mControllers->end(), [&uuid](const shared_ptr<Controller>& _c)->bool{ return _c->uuid()==uuid; });
    return c == mControllers->end();
}

bool UserData::isBssidKnown(const QString &bssid)
{
    auto l = find_if(mLocations->begin(), mLocations->end(), [&bssid](const shared_ptr<Location>& _l)->bool{ return _l->bssid()==bssid; });
    return l != mLocations->end();
}

void UserData::setupControllerKeys(const QString &mac, const QString& ip, const QString& bssid)
{
    QString cmac = sanitizeMac(mac);
    mKeysAndCertResp.cancel();
    if(mControllerClient){
        mControllerClient->deleteLater();
        mControllerClient = nullptr;
    }
    mControllerClient = new ControllerHTTPClient;
    connect(this, &UserData::installKeysAndCert, mControllerClient, &ControllerHTTPClient::postSetup);
    connect(mControllerClient, &ControllerHTTPClient::keysWasSet, this, &UserData::setupControllerSuccessful);
    connect(mControllerClient, &ControllerHTTPClient::setKeysFailed, this, &UserData::setupControllerFailed);
    if(mKeysAndCertResp.isCanceled() || mKeysAndCertResp.isFinished()){
        mKeysAndCertResp = QtConcurrent::run(this, &UserData::createThingKeys, cmac, ip, bssid);
    }
}

void UserData::createThingKeys(const QString mac, const QString& ip, const QString& bssid)
{
    ControllerThing *thing = new ControllerThing;
    if(!mCancelled){
        if(mCancelled) {
            delete thing;
            return;
        }
        thing->setupController(mac);
        QString cmac = sanitizeMac(mac);
        if(thing->isSuccessful()) {
            int controller_id = 1;
            ControllerSchedule cs;
            auto loc = find_if(mLocations->begin(), mLocations->end(), [&bssid](const shared_ptr<Location>& _l)->bool{ return _l->bssid() == bssid; });
            if (loc == mLocations->end())
                cs.timezone = TimeZoneModel::defaultTimezone();
            else
                cs.timezone = loc->get()->timezone();

            auto c = find_if(mControllers->begin(), mControllers->end(), [&cmac](const shared_ptr<Controller>& _c)->bool{ return _c->mac() == cmac; });
            if (c == mControllers->end()){
                while (find_if(mControllers->begin(), mControllers->end(), [&controller_id](const shared_ptr<Controller>& _c)->bool{ return _c->id() == controller_id; }) != mControllers->end()) {
                    controller_id++;
                }
            }
            else {
                controller_id = c->get()->id();
            }
            sleep(5);
            emit installKeysAndCert(ip, thing->pubKey(), thing->priKey(), thing->cert(), controller_id, cs.json().toLatin1());
        }
        else
            emit setupControllerFailed("Setup controller failed");
    }
    delete thing;
}

void UserData::setupControllerSuccessful()
{
    emit controllerSetupSuccessful();
    if(mControllerClient){
        mControllerClient->deleteLater();
        mControllerClient = nullptr;
    }
}

void UserData::setupControllerFailed(const QString& msg)
{
    emit controllerSetupFailed(msg);
    if(mControllerClient){
        mControllerClient->deleteLater();
        mControllerClient = nullptr;
    }
}

shared_ptr<Location> UserData::locationOfGroup(const QString &groupUuid) const
{
    auto controller = controllerOfGroup(groupUuid);
    return controller.get() == nullptr ? shared_ptr<Location>() : locationOfController(controller->uuid());
}

shared_ptr<Location> UserData::locationOfController(const QString &controllerUuid) const
{
    auto c = find_if(mControllers->begin(), mControllers->end(), [&controllerUuid](const shared_ptr<Controller>& _c)->bool{ return _c->uuid() == controllerUuid; });
    if (c != mControllers->end()) {
        auto l = find_if(mLocations->begin(), mLocations->end(), [&c](const shared_ptr<Location>& _l)->bool{ return _l->uuid() == c->get()->locationUuid(); });
        if (l != mLocations->end()) {
            return *l;
        }
    }
    return shared_ptr<Location>();
}

shared_ptr<Controller> UserData::controllerOfGroup(const QString &groupUuid) const
{
    auto g = find_if(mShadeGroups->begin(), mShadeGroups->end(), [&groupUuid](const shared_ptr<ShadeGroup>& _g)->bool{ return _g->uuid() == groupUuid; });
    if (g != mShadeGroups->end()) {
        auto c = find_if(mControllers->begin(), mControllers->end(), [&g](const shared_ptr<Controller>& _c)->bool{ return _c->mac()==g->get()->controllerMac(); });
        if (c != mControllers->end())
            return *c;
    }
    return shared_ptr<Controller>();
}

shared_ptr<Controller> UserData::controller(const QString &uuid) const
{
    auto c = find_if(mControllers->begin(), mControllers->end(), [&uuid](const shared_ptr<Controller>& _c)->bool{ return _c->uuid()==uuid; });
    if (c == mControllers->end())
        return shared_ptr<Controller>();
    else
        return *c;
}

shared_ptr<Location> UserData::location(const QString &uuid) const
{
    auto l = find_if(mLocations->begin(), mLocations->end(), [&uuid](const shared_ptr<Location>& _l)->bool{ return _l->uuid()==uuid; });
    if (l == mLocations->end())
        return shared_ptr<Location>();
    else
        return *l;
}

shared_ptr<ShadeGroup> UserData::shadeGroup(const QString &uuid) const
{
    auto g = find_if(mShadeGroups->begin(), mShadeGroups->end(), [&uuid](const shared_ptr<ShadeGroup>& _g)->bool{ return _g->uuid()==uuid; });
    if (g == mShadeGroups->end())
        return shared_ptr<ShadeGroup>();
    else
        return *g;
}

void UserData::setupController(const QString &mac, const QString &bssid)
{
    QString cmac = sanitizeMac(mac);
    QString location_uuid;
    auto loc = find_if(mLocations->begin(), mLocations->end(), [&bssid](const shared_ptr<Location>& _l)->bool{ return _l->bssid() == bssid; });
    if (loc == mLocations->end()) {
        // create location with bssid
        shared_ptr<Location> new_location(new Location);
        new_location->setBssid(bssid);
        new_location->setPosition(static_cast<int>(mLocations->size()));
        new_location->setTimezone(TimeZoneModel::defaultTimezone());
        new_location->setName(defaultLocationName());
        mLocations->push_back(new_location);
        location_uuid = new_location->uuid();
        persistChanged(new_location, false);
    }
    else {
        location_uuid = loc->get()->uuid();
    }
    // create controller for existing location
    auto c = find_if(mControllers->begin(), mControllers->end(), [&cmac](const shared_ptr<Controller>& _c)->bool{ return _c->mac() == cmac; });
    if (c == mControllers->end()) {
        shared_ptr<Controller> new_controller(new Controller);
        new_controller->setLocationUuid(location_uuid);
        auto controllers = controllersForLocation(location_uuid);
        new_controller->setPosition(static_cast<int>(controllers->size()));
        new_controller->setMac(cmac);
        int controller_id = 1;
        while (find_if(mControllers->begin(), mControllers->end(), [&controller_id](const shared_ptr<Controller>& _c)->bool{ return _c->id() == controller_id; }) != mControllers->end()) {
            controller_id++;
        }
        new_controller->setId(controller_id);
        int n = 1;
        QString controller_name;
        do {
            controller_name = QString("Zone ").append(QString::number(n));
            n++;
        } while (find_if(controllers->begin(), controllers->end(), [&controller_name, &location_uuid](const shared_ptr<Controller>& _c)->bool{ return _c->name() == controller_name && _c->locationUuid() == location_uuid; }) != controllers->end());
        new_controller->setName(controller_name);
        mControllers->push_back(new_controller);
        persistChanged(new_controller, false);

        shared_ptr<ShadeGroup> new_group(new ShadeGroup);
        new_group->setControllerMac(cmac);
        new_group->setName("Group 1");
        persistChanged(new_group);
    }
    else {
        // just finish do nothing
        c->get()->setLocationUuid(location_uuid);
        c->get()->setChanged(true);
        persistChanged(*c);
    }
}

QString UserData::defaultLocationName() const
{
    if (mLocations->empty()) return "My Home";
    int n = 1;
    QString name;
    do {
        name = QString("Location ").append(QString::number(n++));
    } while (find_if(mLocations->begin(), mLocations->end(), [&name](const shared_ptr<Location>& _l)->bool{ return _l->name() == name; }) != mLocations->end());

    return name;
}

shared_ptr<ControllerConfig> UserData::controllerConfig(const QString &controllerMac) const
{
    shared_ptr<ControllerConfig> config(new ControllerConfig);
    QString uuid = controllerUuid(controllerMac);
    if (!uuid.isEmpty()) {
        config->setTimezone(controllerTimezone(uuid));
        config->setMac(controllerMac);
        for (shared_ptr<ShadeGroup> group : *mShadeGroups) {
            if (group->controllerMac() == controllerMac)
                config->setGroupSchedule(group->channel()-1, group->days(), group->openAt(), group->closeAt());
        }
        config->setValid(true);
    }
    return config;
}
