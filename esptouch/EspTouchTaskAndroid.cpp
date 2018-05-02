#include "EspTouchTaskAndroid.h"

#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroid>
#include <QStringList>

EspTouchTaskAndroid::EspTouchTaskAndroid(QString &ssid, QString &bssid, QString &password, QObject *parent):
    QThread(), mSsid(ssid), mBssid(bssid), mPwd(password), mHostIp("0.0.0.0"), mHostMac("000000000000"), mSuccessful(false)
{
    Q_UNUSED(parent);
}

void EspTouchTaskAndroid::run(){
    QAndroidJniObject java_ssid = QAndroidJniObject::fromString(mSsid);
    QAndroidJniObject java_bssid = QAndroidJniObject::fromString(mBssid);
    QAndroidJniObject java_pwd = QAndroidJniObject::fromString(mPwd);

    QAndroidJniObject java_resp = QtAndroid::androidActivity().callObjectMethod(
                    "runEsptouch",
                    "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
                    java_ssid.object<jstring>(),
                    java_bssid.object<jstring>(),
                    java_pwd.object<jstring>()
                );
    QString resp = java_resp.toString();
    QStringList response = resp.split("/");
    if(response.length() == 2){
        mHostMac = response.at(0);
        mHostIp = response.at(1);
        emit hostConfigured(resp);
    }
}

void EspTouchTaskAndroid::stop()
{
    QtAndroid::androidActivity().callMethod<void>("interruptEsptouch");
    quit();
    wait();
}
