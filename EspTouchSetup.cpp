#include "EspTouchSetup.h"

#include <QTextStream>

EspTouchSetup::EspTouchSetup(QObject *parent): QObject(parent), mIsRunning(false)
{

}

bool EspTouchSetup::isRunning() const
{
    return mIsRunning;
}

void EspTouchSetup::setRunning(bool isRunning)
{
    if(isRunning == mIsRunning)
        return;
    if(isRunning){
        runEspTouch();
    }
    else{
        stopEspTouch();
    }
}

QString EspTouchSetup::getPassword() const
{
    return mPassword;
}

void EspTouchSetup::setPassword(const QString &password)
{
    mPassword = password;
    emit passwordChanged();
}

QString EspTouchSetup::getSsid() const
{
    return mSsid;
}

void EspTouchSetup::setSsid(const QString &ssid)
{
    mSsid = ssid;
    emit ssidChanged();
}

QString EspTouchSetup::getBssid() const
{
    return mBssid;
}

void EspTouchSetup::setBssid(const QString &bssid)
{
    mBssid = bssid;
    emit bssidChanged();
}

QString EspTouchSetup::getAddress() const
{
    return mAddress;
}

void EspTouchSetup::setAddress(const QString &address)
{
    mAddress = address;
    emit addressChanged();
}

QString EspTouchSetup::getHostIP() const
{
    return mHostIP;
}

void EspTouchSetup::setHostIP(const QString &hostIP)
{
    mHostIP = hostIP;
    emit hostIPChanged();
}

QString EspTouchSetup::getHostMac() const
{
    return mHostMac;
}

void EspTouchSetup::setHostMac(const QString &hostMac)
{
    mHostMac = hostMac;
    emit hostMacChanged();
}

void EspTouchSetup::gotHost(const QString& str)
{
    QString astr(str);
    QTextStream(stdout) << "EspTouchSetup CONFIGURED: " << astr << endl;
    QStringList list = astr.split("/");
    if(list.length() == 2){
        setHostMac(list.at(0));
        setHostIP(list.at(1));
        emit hostFound(astr);
    }
    else
        emit failed();
}

void EspTouchSetup::stopped()
{
    mIsRunning = false;
    QTextStream(stdout) << "EspTouchSetup STOPPED" << endl;
    emit finished();
}

#if defined(Q_OS_IOS)
void EspTouchSetup::runEspTouch()
{
    if(mIsRunning) return;

    mIsRunning = true;
    mTask = new EspTouchTaskIOS(mSsid, mBssid, mPassword);
    connect(mTask, &EspTouchTaskIOS::finished, this, &EspTouchSetup::stopped);
    connect(mTask, &EspTouchTaskIOS::finished, mTask, &EspTouchTaskIOS::deleteLater);
    connect(mTask, &EspTouchTaskIOS::hostConfigured, this, &EspTouchSetup::gotHost);
    mTask->start();
    QTextStream(stdout) << "EspTouchSetup STARTED" << endl;
}

void EspTouchSetup::stopEspTouch()
{
    QTextStream(stdout) << "EspTouchSetup CANCELING..." << endl;
    mIsRunning = false;
    mTask->stop();
}

#elif defined(Q_OS_ANDROID)
void EspTouchSetup::runEspTouch()
{
    if(mIsRunning) return;

    mIsRunning = true;
    mTask = new EspTouchTaskAndroid(mSsid, mBssid, mPassword);
    connect(mTask, &EspTouchTaskAndroid::finished, this, &EspTouchSetup::stopped);
    connect(mTask, &EspTouchTaskAndroid::finished, mTask, &EspTouchTaskAndroid::deleteLater);
    connect(mTask, &EspTouchTaskAndroid::hostConfigured, this, &EspTouchSetup::gotHost);
    mTask->start();
    QTextStream(stdout) << "EspTouchSetup STARTED" << endl;
}

void EspTouchSetup::stopEspTouch()
{
    QTextStream(stdout) << "EspTouchSetup CANCELING..." << endl;
    mIsRunning = false;
    mTask->stop();
}
#else

void EspTouchSetup::runEspTouch()
{

}

void EspTouchSetup::stopEspTouch()
{

}
#endif
