QT += quick svg network
CONFIG += c++11

android {
    QT += androidextras
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    location.cpp \
    esptouchtask.cpp \
    esptouch.cpp \
    networkmonitor.cpp

#linux-g++{
#    INCLUDEPATH += /home/ant/DevTools/aws-sdk-cpp
#    LIBS += -L/usr/local/lib -laws-cpp-sdk-core -laws-cpp-sdk-s3 \
#        -laws-cpp-sdk-cognito-sync -laws-cpp-sdk-cognito-idp -laws-cpp-sdk-cognito-identity \
#        -laws-cpp-sdk-s3
#}

android {
    SOURCES +=
}

RESOURCES += qml.qrc


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    location.h \
    esptouchtask.h \
    esptouch.h \
    networkmonitor.h


DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    ../../../qt/esptouch/EsptouchTaskClient.java \
    data.qmodel

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

STATECHARTS +=
