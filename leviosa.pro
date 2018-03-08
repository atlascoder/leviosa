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
    LIBS += -L$$AWS-core/$$ASX -laws-cpp-sdk-core \
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
                -L$$AWS-iam -laws-cpp-sdk-iam \
                -L$$AWS-iot -laws-cpp-sdk-iot \
                -L$$AWS-cognito-idp -laws-cpp-sdk-cognito-idp \
                -L$$AWS-cognito-sync -laws-cpp-sdk-cognito-sync \
                -L$$AWS-access-management -laws-cpp-sdk-access-management \
                -L$$AWS-cognito-identity -laws-cpp-sdk-cognito-identity \
                -L$$AWS-identity-management -laws-cpp-sdk-identity-management \
                -L$$AWS-core -laws-cpp-sdk-core \
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
    aws/IdToken.cpp \
    aws/Jwt.cpp \
    aws/AccessToken.cpp \
    aws/RefreshToken.cpp \
    aws/ClientConfig.cpp \
    CurrentUser.cpp \
    aws/CredentialsRequest.cpp \
    aws/AuthRequest.cpp \
    UserData.cpp \
    core/shadegroup.cpp \
    core/BasicDAO.cpp \
    core/shadegroupmodel.cpp \
    networkmonitor.cpp \
    core/controllerdiscovery.cpp \
    core/controllermodel.cpp \
    core/databasemanager.cpp \
    core/locationmodel.cpp \
    core/mdnsdiscothread.cpp \
    core/schedule.cpp \
    core/shade.cpp \
    core/positioned.cpp \
    main.cpp \
    core/location.cpp \
    core/controller.cpp \
    core/SyncableRecord.tpp \
    core/syncablerecord.cpp \
    core/ControllerConfig.cpp \
    core/ControllerAPI.cpp \
    aws/CognitoSyncCommand.cpp \
    core/ControllersMgr.cpp \
    core/ControllerCfg.cpp \
    aws/ControllerThing.cpp \
    core/ControllerHTTPClient.cpp

# own codebase
HEADERS += \
    EspTouchSetup.h \
    core/WanAPI.h \
    core/User.h \
    core/UserDAO.h \
    aws/CognitoSyncAPI.h \
    aws/IdToken.h \
    aws/Jwt.h \
    aws/AccessToken.h \
    aws/RefreshToken.h \
    aws/ClientConfig.h \
    CurrentUser.h \
    aws/CredentialsRequest.h \
    aws/AuthRequest.h \
    UserData.h \
    core/shadegroup.h \
    core/BasicDAO.h \
    core/shadegroupmodel.h \
    networkmonitor.h \
    core/controllerdiscovery.h \
    core/controllermodel.h \
    core/databasemanager.h \
    core/locationmodel.h \
    core/mdnsdiscothread.h \
    core/schedule.h \
    core/shade.h \
    core/positioned.h \
    core/location.h \
    core/controller.h \
    core/SyncableRecord.h \
    core/syncable.h \
    core/ControllerConfig.h \
    core/scheduleline.h \
    NAM.h \
    core/ControllerHTTPClient.h \
    core/ControllerAPI.h \
    core/timeaux.h \
    aws/CognitoSyncCommand.h \
    core/ControllersMgr.h \
    core/ControllerCfg.h \
    aws/ControllerThing.h

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
