#include "esptouchtask.h"

#ifdef Q_OS_DARWIN

void EspTouchTask::run()
{
    emit finished(response);
}

void EspTouchTask::interrupt()
{

}

#else

#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroid>

void EspTouchTask::run()
{
    QAndroidJniObject java_apSsid = QAndroidJniObject::fromString(apSsid);
    QAndroidJniObject java_apBssid = QAndroidJniObject::fromString(apBssid);
    QAndroidJniObject java_apPass = QAndroidJniObject::fromString(apPassword);

    QAndroidJniObject java_resp = QtAndroid::androidActivity().callObjectMethod("runEsptouch",
                                                                                        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
                                                                                        java_apSsid.object<jstring>(),
                                                                                        java_apBssid.object<jstring>(),
                                                                                        java_apPass.object<jstring>()
                                                                                   );
    response = java_resp.toString();
    emit finished(response);
}

void EspTouchTask::interrupt()
{
    QtAndroid::androidActivity().callMethod<void>("interruptEsptouch");
}

#endif // Q_OS_DARWIN
