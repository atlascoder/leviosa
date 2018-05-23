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
        if (a == mAPIs->end()) {
            shared_ptr<ControllerAPI> api(new ControllerAPI);
            mAPIs->push_back(api);
            api->setMac(c->get()->mac());
            api->setBssid(UserData::instance().locationBssid(c->get()->locationUuid()));
            return api;
        }
        else {
            return *a;
        }
    }
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
        auto api = find_if(mAPIs->begin(), mAPIs->end(), [&temp_conf](const shared_ptr<ControllerAPI>& _a)->bool{ return _a->mac() == temp_conf.mac(); });
        if (api == mAPIs->end()) {
            // create new controller API
            shared_ptr<ControllerAPI> new_api(new ControllerAPI);
            new_api->setIpAddress(ip);
            new_api->setConfig(temp_conf);
            new_api->setBssid(mNetworkMonitor->bssid());
            mAPIs->push_back(new_api);
        }
        else {
            // update existing controller API
            api->get()->setIpAddress(ip);
            api->get()->setConfig(temp_conf);
            api->get()->setBssid(mNetworkMonitor->bssid());
        }
    }
    else {
        qDebug() << "Wrong Controller config received: ";
    }
    emit updated();
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
