#include "ControllerConnectionsManager.h"

#include <QDebug>
#include <QMutexLocker>

#include "UserData.h"

ControllerConnectionsManager& ControllerConnectionsManager::instance()
{
    static ControllerConnectionsManager sInstance;
    return sInstance;
}

ControllerConnectionsManager::ControllerConnectionsManager(QObject *parent):
    QObject(parent),
    mAPIs(new vector<shared_ptr<ControllerAPI>>),
    mIsDiscovering(false),
    mDiscovery(new ControllerSSDP)
{
    mControllers = UserData::instance().controllers();
    connect(mDiscovery.get(), &ControllerSSDP::started, this, &ControllerConnectionsManager::onDiscoveryStarted);
    connect(mDiscovery.get(), &ControllerSSDP::found, this, &ControllerConnectionsManager::onDiscovered);
    connect(mDiscovery.get(), &ControllerSSDP::finished, this, &ControllerConnectionsManager::onDiscoveryFinished);
}

ControllerConnectionsManager::~ControllerConnectionsManager()
{ }

shared_ptr<ControllerAPI> ControllerConnectionsManager::controllerAPI(const QString &controllerUuid)
{
    QMutexLocker locker(&mMutex);
    qDebug() << "Controllers count: " << mControllers->size();
    auto c = find_if(mControllers->begin(), mControllers->end(), [&controllerUuid](const shared_ptr<Controller>& _c)->bool{ return _c->uuid() == controllerUuid; });
    if (c != mControllers->end()) {
        auto a = find_if(mAPIs->begin(), mAPIs->end(), [c](const shared_ptr<ControllerAPI>& _a)->bool{ return _a->mac()== c->get()->mac(); });
        shared_ptr<ControllerConfig> config = UserData::instance().controllerConfig(c->get()->mac());
        if (a == mAPIs->end()) {
            shared_ptr<ControllerAPI> api(new ControllerAPI);
            api->setConfig(*config);
            api->setBssid(UserData::instance().locationBssid(c->get()->locationUuid()));
            mAPIs->push_back(api);
            return api;
        }
        else {
            a->get()->setConfig(*config);
            return *a;
        }
    }
    else
        return shared_ptr<ControllerAPI>();
}

void ControllerConnectionsManager::discover()
{
    if (!mDiscovery->running()) {
        mDiscovery->setRunning(true);
        emit updated();
    }
}

inline QString sanitizeMac(const QString& mac)
{
    if (mac.length()>12) {
        QString cmac;
        for (auto c : mac) {
            if (c==":") continue;
            cmac += c;
        }
        return cmac.toUpper();
    }
    else
        return mac.toUpper();
}

void ControllerConnectionsManager::onDiscoveryStarted()
{
    QString bssid = mNetworkMonitor->bssid();
    bool isOnWlan = mNetworkMonitor->isOnWlan();
    for (auto& api : *mAPIs) {
        if (isOnWlan && api->bssid() == bssid) {
            api->setControllerState(ControllerAPI::Searching);
        }
        else {
            api->setControllerState(ControllerAPI::Wan);
        }
    }

}

void ControllerConnectionsManager::onDiscovered(const QString &ip, const QString& configString)
{
    ControllerConfig temp_conf;
    temp_conf.parse(configString);
    if (temp_conf.isValid()) {
        QString controllerUuid = UserData::instance().controllerUuid(temp_conf.mac());
        if (!controllerUuid.isEmpty()) {
            shared_ptr<ControllerAPI> api = controllerAPI(controllerUuid);
            api->setIpAddress(ip);
            api->setBssid(mNetworkMonitor->bssid());
//            api->get()->setConfig(temp_conf);
            api->setControllerState(ControllerAPI::Wlan);
            if (temp_conf != api->currentConfig()) {
                api->postSchedule();
            }
            emit updated();
        }
        else {
            qDebug() << "Found unknown controller " << temp_conf.mac();
        }
//        auto api = find_if(mAPIs->begin(), mAPIs->end(), [&temp_conf](const shared_ptr<ControllerAPI>& _a)->bool{ return _a->mac() == temp_conf.mac(); });
//        if (api == mAPIs->end()) {
//            // create new controller API
//            shared_ptr<ControllerAPI> new_api(new ControllerAPI);
//            new_api->setIpAddress(ip);
//            new_api->setBssid(mNetworkMonitor->bssid());
//            new_api->setConfig(temp_conf);
//            new_api->setControllerState(ControllerAPI::Wlan);
//            mAPIs->push_back(new_api);
//        }
//        else {
//            // update existing controller API
//            api->get()->setIpAddress(ip);
//            api->get()->setBssid(mNetworkMonitor->bssid());
//            if (temp_conf != api->get()->currentConfig()) {
//                api->get()->postSchedule();
//            }
////            api->get()->setConfig(temp_conf);
//            api->get()->setControllerState(ControllerAPI::Wlan);
//        }
    }
    else {
        qDebug() << "Wrong Controller config received: ";
    }
}

void ControllerConnectionsManager::onDiscoveryFinished()
{
    qDebug() << "DISCO FINISHED";
    for (shared_ptr<ControllerAPI>& c : *mAPIs) {
        if (c->controllerState() == ControllerAPI::Searching) {
            c->setControllerState(ControllerAPI::NotFound);
        }
    }
    emit updated();
}

void ControllerConnectionsManager::onConnectionStateChanged()
{
    if (mNetworkMonitor->isOnWlan()) {
        qDebug() << "connection changed to on WLAN ";
        discover();
    }
    else {
        qDebug() << "connection changed to on WAN ";
        for (auto& api : *mAPIs) {
            api->setControllerState(ControllerAPI::Wan);
        }
        emit updated();
    }
}

void ControllerConnectionsManager::setNetworkMonitor(NetworkMonitor *monitor)
{
    mNetworkMonitor = monitor;
    connect(monitor, &NetworkMonitor::connectionStateChanged, this, &ControllerConnectionsManager::onConnectionStateChanged);
}

void ControllerConnectionsManager::check()
{
    onConnectionStateChanged();
}
