#ifndef CONTROLLERCONNECTIONSMANAGER_H
#define CONTROLLERCONNECTIONSMANAGER_H

#include <QObject>
#include <QMutex>
#include <memory>

#include "Controller.h"
#include "ControllerAPI.h"
#include "ControllerConfig.h"
#include "ControllerSSDP.h"
#include "NetworkMonitor.h"


using namespace std;

class ControllerConnectionsManager : public QObject
{
    Q_OBJECT
public:
    static ControllerConnectionsManager& instance();

    shared_ptr<ControllerAPI> controllerAPI(const QString& controllerUuid);

    void setNetworkMonitor(NetworkMonitor* monitor);

    Q_INVOKABLE void check();

signals:
    void updated();

public slots:
    void discover();
private slots:
    void onDiscoveryStarted();
    void onDiscovered(const QString& ip, const QString& configString);
    void onDiscoveryFinished();

    void onConnectionStateChanged();
private:

    shared_ptr<vector<shared_ptr<Controller>>> mControllers;
    shared_ptr<vector<shared_ptr<ControllerAPI>>> mAPIs;
    bool mIsDiscovering;
    unique_ptr<ControllerSSDP> mDiscovery;
    NetworkMonitor* mNetworkMonitor;
    QMutex mMutex;

    ControllerConnectionsManager(QObject *parent=nullptr);
    ~ControllerConnectionsManager();
};

#endif // CONTROLLERCONNECTIONSMANAGER_H
