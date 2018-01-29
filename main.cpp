#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "networkmonitor.h"
#include "core/locationmodel.h"
#include "core/controllermodel.h"
#include "core/shadegroupmodel.h"
#include "core/controllerdiscovery.h"
#include "core/shade.h"
#include "EspTouchSetup.h"
#include "UserLogin.h"

#include <aws/core/utils/memory/stl/AWSString.h>


#include "UserData.h"

#include <aws/core/Aws.h>

int main(int argc, char *argv[])
{
    Aws::SDKOptions awsOptions;
    Aws::InitAPI(awsOptions);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<NetworkMonitor>("com.atlascoder.NetMonitor", 1, 0, "NetMon");
    qmlRegisterType<LocationModel>("com.atlascoder.LocationsModel", 1, 0, "LocationsModel");
    qmlRegisterType<ControllerModel>("com.atlascoder.ControllersModel", 1, 0, "ControllersModel");
    qmlRegisterType<ShadeGroupModel>("com.atlascoder.ShadesGroupsModel", 1, 0, "ShadesGroupsModel");
    qmlRegisterType<ControllerDiscovery>("com.atlascoder.ControllerDiscovery", 1, 0, "ControllerDiscovery");
    qmlRegisterType<EspTouchSetup>("com.atlascoder.EspTouch", 1, 0, "EspTouch");

    qmlRegisterType<Shade>("com.atlascoder.Shade", 1, 0, "Shade");
    qmlRegisterType<UserLogin>("com.atlascoder.UserLogin", 1, 0, "UserLogin");

    NetworkMonitor netMonitor;
    UserData& userData(UserData::instance());

    QQmlApplicationEngine engine;

    QQmlContext* ctx = engine.rootContext();

    ctx->setContextProperty("netMonitor", &netMonitor);
    ctx->setContextProperty("qGuiApp", &app);
    ctx->setContextProperty("userData", &userData);

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    int app_result = app.exec();

    UserData::instance().stopRequests();

    Aws::ShutdownAPI(awsOptions);
    return app_result;
}
