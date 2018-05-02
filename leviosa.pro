QT += quick svg network sql concurrent
CONFIG += c++11

DEV_LIBS_PATH = /Users/anton.titkov/DevLibs


# Android specifics
android {
    QT += androidextras
    DISTFILES += \
        android/AndroidManifest.xml \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradlew \
        android/res/values/libs.xml \
        android/build.gradle \
        android/gradle/wrapper/gradle-wrapper.properties \
        android/src/com/atlascoder/

    # EspTouch backport
    HEADERS += esptouch/EspTouchTaskAndroid.h
    SOURCES += esptouch/EspTouchTaskAndroid.cpp

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    # ZeroConf
    LIBS += -L$$PWD/../../qt-zero-conf/build-Android-arm/android-build/lib -lQtZeroConf
    INCLUDEPATH += $$PWD/../qt-zero-conf/QtZeroConf

    # AWS libs
    AWS = $$DEV_LIBS_PATH/aws-sdk-cpp/android/aws-cpp-sdk
    LIBS += \
                -L$$AWS-iam -laws-cpp-sdk-iam \
                -L$$AWS-iot -laws-cpp-sdk-iot \
                -L$$AWS-cognito-idp -laws-cpp-sdk-cognito-idp \
                -L$$AWS-cognito-sync -laws-cpp-sdk-cognito-sync \
                -L$$AWS-access-management -laws-cpp-sdk-access-management \
                -L$$AWS-cognito-identity -laws-cpp-sdk-cognito-identity \
                -L$$AWS-identity-management -laws-cpp-sdk-identity-management \
                -L$$AWS-logs -laws-cpp-sdk-logs \
                -L$$AWS-core -laws-cpp-sdk-core \
                -L/Users/anton.titkov/DevLibs/aws-sdk-cpp/android/external/curl/lib -lcurl \
                -L/Users/anton.titkov/DevLibs/aws-sdk-cpp/android/external/openssl/lib -lssl -lcrypto \
#                -L/Users/anton.titkov/DevLibs/openssl/android -lssl -lcrypto

    INCLUDEPATH += \
                Users/anton.titkov/DevLibs/aws-sdk-cpp/android/external/openssl/include


    # Crypto++
    INCLUDEPATH += $$DEV_LIBS_PATH/cryptopp/android/include
    LIBS += -L$$DEV_LIBS_PATH/cryptopp/android/lib -lcryptopp

    ANDROID_EXTRA_LIBS = \
        $$PWD/android/libs/openssl/libcrypto.so \
        $$PWD/android/libs/openssl/libssl.so
}

# iOS specifics
ios {
    app_launch_images.files = $$PWD/ios/SplashScreen.xib $$PWD/ios/leviosa-logo.png
    QMAKE_BUNDLE_DATA = app_launch_images
    QMAKE_INFO_PLIST = $$PWD/ios/Info.plist

    #ZeroConf
    include(QtZeroConf/qtzeroconf.pri)
    DEFINES= QZEROCONF_STATIC

    # EspTouch backport
    SC_IOS = $$PWD/../../esptouch-qt-ios

    # Backports to native NetworkReachability and ESPTouch components
    OBJECTIVE_HEADERS += \
        $$PWD/ios/src/Reachability.h \
        $$PWD/ios/src/ReachabiltyListener.h \
        $$SC_IOS/ESPDataCode.h \
        $$SC_IOS/ESPDatumCode.h \
        $$SC_IOS/ESPGuideCode.h \
        $$SC_IOS/ESPTouchGenerator.h \
        $$SC_IOS/ESPTouchResult.h \
        $$SC_IOS/ESPTouchTaskParameter.h \
        $$SC_IOS/ESPUDPSocketClient.h \
        $$SC_IOS/ESPUDPSocketServer.h \
        $$SC_IOS/ESPVersionMacro.h \
        $$SC_IOS/ESP_ByteUtil.h \
        $$SC_IOS/ESP_CRC8.h \
        $$SC_IOS/ESP_NetUtil.h \
        $$SC_IOS/ESP_WifiUtil.h \
        $$SC_IOS/ESPTouchTask.h \
        $$SC_IOS/ESPTouchDelegate.h \
        $$SC_IOS/ESPTouchHolder.h


    OBJECTIVE_SOURCES += \
        $$PWD/ios/src/Reachability.mm \
        $$PWD/ios/src/ReachabilityListener.mm \
        $$SC_IOS/ESPDataCode.mm \
        $$SC_IOS/ESPDatumCode.mm \
        $$SC_IOS/ESPGuideCode.mm \
        $$SC_IOS/ESPTouchGenerator.mm \
        $$SC_IOS/ESPTouchResult.mm \
        $$SC_IOS/ESPTouchTaskParameter.mm \
        $$SC_IOS/ESPUDPSocketClient.mm \
        $$SC_IOS/ESPUDPSocketServer.mm \
        $$SC_IOS/ESP_ByteUtil.mm \
        $$SC_IOS/ESP_CRC8.mm \
        $$SC_IOS/ESP_NetUtil.mm \
        $$SC_IOS/ESP_WifiUtil.mm \
        $$SC_IOS/ESPTouchTask.mm \
        $$SC_IOS/ESPTouchHolder.mm

    HEADERS += esptouch/EspTouchTaskIOS.h
    SOURCES += esptouch/EspTouchTaskIOS.cpp

    LIBS += -framework SystemConfiguration

    # AWS
    AWS = $$DEV_LIBS_PATH/aws-sdk-cpp/xcode/aws-cpp-sdk
    ASX = Release-iphoneos
    LIBS += \
                -L$$AWS-core/$$ASX -laws-cpp-sdk-core \
                -L$$AWS-iam/$$ASX -laws-cpp-sdk-iam \
                -L$$AWS-access-management/$$ASX -laws-cpp-sdk-access-management \
                -L$$AWS-cognito-identity/$$ASX -laws-cpp-sdk-cognito-identity \
                -L$$AWS-iot/$$ASX -laws-cpp-sdk-iot \
                -L$$AWS-cognito-idp/$$ASX -laws-cpp-sdk-cognito-idp \
                -L$$AWS-cognito-sync/$$ASX -laws-cpp-sdk-cognito-sync \
                -L$$AWS-identity-management/$$ASX -laws-cpp-sdk-identity-management \
                -L$$DEV_LIBS_PATH/libcurl/lib -lcurl

    # Crypto++
    INCLUDEPATH += $$DEV_LIBS_PATH/cryptopp/ios/include
    LIBS += -L$$DEV_LIBS_PATH/cryptopp/ios -lcryptopp
}

# macOS specifics
macos {

    # import QtZeroConf https://github.com/jbagg/QtZeroConf
    include(QtZeroConf/qtzeroconf.pri)
    DEFINES= QZEROCONF_STATIC

    AWS = $$DEV_LIBS_PATH/aws-sdk-cpp/host/aws-cpp-sdk

    LIBS += \
                -L$$AWS-core -laws-cpp-sdk-core \
                -L$$AWS-iam -laws-cpp-sdk-iam \
                -L$$AWS-access-management -laws-cpp-sdk-access-management \
                -L$$AWS-cognito-identity -laws-cpp-sdk-cognito-identity \
                -L$$AWS-iot -laws-cpp-sdk-iot \
                -L$$AWS-cognito-idp -laws-cpp-sdk-cognito-idp \
                -L$$AWS-cognito-sync -laws-cpp-sdk-cognito-sync \
                -L$$AWS-identity-management -laws-cpp-sdk-identity-management \
                -lcurl

    #import Crypto++ lib
    INCLUDEPATH += $$DEV_LIBS_PATH/cryptopp/build-host/include
    LIBS += -L$$DEV_LIBS_PATH/cryptopp/build-host/lib -lcryptopp
}

# AWS headers same for all platforms
AWS_H = $$DEV_LIBS_PATH/aws-sdk-cpp/src/aws-sdk-cpp/aws-cpp-sdk
INCLUDEPATH += $$AWS_H-core/include \
                $$AWS_H-logs/include \
                $$AWS_H-iam/include \
                $$AWS_H-access-management/include \
                $$AWS_H-cognito-identity/include \
                $$AWS_H-cognito-idp/include \
                $$AWS_H-iot/include \
                $$AWS_H-cognito-sync/include \
                $$AWS_H-identity-management/include


SOURCES += \
    EspTouchSetup.cpp \
    core/WanAPI.cpp \
    core/User.cpp \
    core/UserDAO.cpp \
    aws/CognitoSyncAPI.cpp \
    aws/ClientConfig.cpp \
    CurrentUser.cpp \
    aws/AuthRequest.cpp \
    UserData.cpp \
    core/mdnsdiscothread.cpp \
    main.cpp \
    core/ControllerConfig.cpp \
    core/ControllerAPI.cpp \
    aws/CognitoSyncCommand.cpp \
    aws/ControllerThing.cpp \
    core/ControllerHTTPClient.cpp \
    core/ControllerScan.cpp \
    aws/PersistentCredentialsProviderSQL.cpp \
    core/CredentialsDAO.cpp \
    aws/AwsApi.cpp \
    aws/CognitoCredentialsProvider.cpp \
    aws/IdToken.cpp \
    aws/Jwt.cpp \
    core/LocalCache.cpp \
    core/LocalCache.tpp \
    CognitoSyncTests.cpp \
    core/CognitoSyncDAO.cpp \
    core/Location.cpp \
    core/Controller.cpp \
    core/ShadeGroup.cpp \
    core/ShadeGroupModel.cpp \
    core/DatabaseManager.cpp \
    core/LocationModel.cpp \
    core/ControllerDiscovery.cpp \
    core/Positioned.cpp \
    core/Schedule.cpp \
    core/Shade.cpp \
    NetworkMonitor.cpp \
    UserData.tpp \
    core/ZoneModel.cpp \
    core/ControllerConnectionsManager.cpp \
    core/LocationsModel.cpp \
    core/TimeZoneModel.cpp \
    core/ControllerSSDP.cpp

# own codebase
HEADERS += \
    EspTouchSetup.h \
    core/WanAPI.h \
    core/User.h \
    core/UserDAO.h \
    aws/CognitoSyncAPI.h \
    aws/ClientConfig.h \
    CurrentUser.h \
    aws/AuthRequest.h \
    UserData.h \
    core/mdnsdiscothread.h \
    core/Syncable.h \
    core/ControllerConfig.h \
    NAM.h \
    core/ControllerHTTPClient.h \
    core/ControllerAPI.h \
    core/timeaux.h \
    aws/CognitoSyncCommand.h \
    aws/ControllerThing.h \
    core/ControllerScan.h \
    aws/PersistentCredentialsProviderSQL.h \
    core/CredentialsDAO.h \
    aws/AwsApi.h \
    aws/CognitoCredentialsProvider.h \
    aws/IdToken.h \
    aws/Jwt.h \
    core/LocalCache.h \
    CognitoSyncTests.h \
    core/CognitoSyncDAO.h \
    core/Jsonized.h \
    core/Location.h \
    core/Controller.h \
    core/ShadeGroup.h \
    core/ShadeGroupModel.h \
    core/DatabaseManager.h \
    core/LocationModel.h \
    core/Positioned.h \
    core/Schedule.h \
    core/ScheduleLine.h \
    core/Shade.h \
    core/ControllerDiscovery.h \
    NetworkMonitor.h \
    core/ZoneModel.h \
    core/ControllerConnectionsManager.h \
    core/LocationsModel.h \
    core/TimeZoneModel.h \
    core/ControllerSSDP.h

# Boost lib for multiprecision ints
INCLUDEPATH += $$DEV_LIBS_PATH/boost-cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
#QML_IMPORT_PATH = styles

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
