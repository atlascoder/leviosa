#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtConcurrent>

#include "NetworkMonitor.h"
#include "core/LocationModel.h"
#include "core/ControllerModel.h"
#include "core/ShadeGroupModel.h"
#include "core/ControllerDiscovery.h"
#include "core/ControllerScan.h"
#include "core/Shade.h"
#include "core/ControllerAPI.h"
#include "core/ZoneModel.h"
#include "core/LocationsModel.h"
#include "core/TimeZoneModel.h"
#include "EspTouchSetup.h"

#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/Aws.h>
#include <sys/utsname.h>
#include <memory>

#include "UserData.h"
#include "core/ControllerConnectionsManager.h"
#include "CognitoSyncTests.h"
#include "aws/AwsApi.h"

int main(int argc, char *argv[])
{

    Aws::SDKOptions awsOptions;
    awsOptions.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
    Aws::InitAPI(awsOptions);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QQmlContext* ctx = engine.rootContext();

    qmlRegisterType<LocationsModel>("com.atlascoder.LocationsModel", 1, 0, "LocationsModel");
    qmlRegisterType<LocationModel>("com.atlascoder.LocationModel", 1, 0, "LocationModel");
    qmlRegisterType<ZoneModel>("com.atlascoder.ZoneModel", 1, 0, "ZoneModel");
    qmlRegisterType<TimeZoneModel>("com.atlascoder.TimeZoneModel", 1, 0, "TimeZoneModel");
    qmlRegisterType<ShadeGroupModel>("com.atlascoder.ShadeGroupModel", 1, 0, "ShadeGroupModel");
    qmlRegisterType<EspTouchSetup>("com.atlascoder.EspTouch", 1, 0, "EspTouch");
    qmlRegisterType<ControllerAPI>("com.atlascoder.ControllerAPI", 1, 0, "ControllerAPI");

    qmlRegisterType<Shade>("com.atlascoder.Shade", 1, 0, "Shade");

    UserData& userData(UserData::instance());
    CurrentUser& currentUser(CurrentUser::instance());
    ControllerConnectionsManager& controllerAPIs(ControllerConnectionsManager::instance());

    NetworkMonitor& netMon(NetworkMonitor::instance());
    controllerAPIs.setNetworkMonitor(&netMon);

    ctx->setContextProperty("qGuiApp", &app);
#ifdef Q_OS_IOS
    QObject::connect(&app, &QGuiApplication::applicationStateChanged, &netMon, &NetworkMonitor::onApplicationStateChanged);
#endif
    ctx->setContextProperty("userData", &userData);
    ctx->setContextProperty("currentUser", &currentUser);
    ctx->setContextProperty("netMonitor", &netMon);
    ctx->setContextProperty("controllerAPIs", &controllerAPIs);


    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    int app_result = app.exec();

    UserData::instance().stopRequests();
    AwsApi::instance().stopRequests();
    Aws::ShutdownAPI(awsOptions);
    return app_result;
}
