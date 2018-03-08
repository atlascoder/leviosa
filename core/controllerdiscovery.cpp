#include "controllerdiscovery.h"
#include "mdnsdiscothread.h"
#include <QTextStream>
#include <QDebug>

using namespace std;

ControllerDiscovery::ControllerDiscovery(QObject* parent) :
    QObject(parent), mTimeoutSec(10), mIsRunning(false), mDiscovered(), mTimer()
{
    connect(&mZeroConf, &QZeroConf::serviceAdded, this, &ControllerDiscovery::serviceFound);
    connect(&mDiscoThread, &MDNSDiscoThread::discovered, this, &ControllerDiscovery::discovered);
    connect(&mTimer, &QTimer::timeout, this, &ControllerDiscovery::timedOut);
    mTimer.setInterval(3000);
    mTimer.setSingleShot(true);
}

int ControllerDiscovery::timeout() const
{
    return mTimeoutSec;
}

void ControllerDiscovery::timedOut()
{
    stop();
    emit foundList(mDiscovered.join(","));
}

void ControllerDiscovery::setTimeout(int timeoutSec)
{
    mTimeoutSec = timeoutSec;
}

bool ControllerDiscovery::running() const
{
    return mIsRunning;
}

void ControllerDiscovery::setRunning(bool isRunning)
{
    if(isRunning == mIsRunning) return;
    mIsRunning = isRunning;
    if(mIsRunning)
        discovery();
    else
        stop();

    emit runningChanged();
}

void ControllerDiscovery::discovered(const QString &mac, const QString &ip)
{
    mDiscovered.append(QString(mac).append(" ").append(ip).toUpper());
}

void ControllerDiscovery::discovery(){
    qDebug() << "DISCO START";
    mDiscovered.clear();
    mDiscoThread.start();
    mTimer.start();
    mZeroConf.startBrowser("_http._tcp");
}

void ControllerDiscovery::stop(){
    qDebug() << "DISCO STOP";
    mZeroConf.stopBrowser();
    mDiscoThread.quit();
    onFinished();
}

void ControllerDiscovery::onFinished(){
    mIsRunning = false;
    emit runningChanged();
    emit finished();
}

ControllerDiscovery::~ControllerDiscovery(){
    mTimer.stop();
    mDiscoThread.quit();
    mDiscoThread.wait();
    if(mZeroConf.browserExists())
        mZeroConf.stopBrowser();
}

void ControllerDiscovery::serviceFound(QZeroConfService conf)
{
    QTextStream(stdout) << "Domain: " << conf.domain() << " name: " << conf.name() << " ip: " << conf.ip().toString() << " type: " << conf.type() << " host: " << conf.host() << endl;

    QRegExp rx("^(WiShade-([\\da-fA-F]{2})([\\da-fA-F]{2})([\\da-fA-F]{2})).*$");
    int pos = rx.indexIn(conf.host());
    if(pos >=0){
        QStringList hosts = rx.capturedTexts();
        QString mac = "24:0A:C4:" + hosts.at(2) + ":" + hosts.at(3) + ":" + hosts.at(4);
        emit found(mac.toUpper(), conf.ip().toString());
        mDiscovered.append(mac.append(" ").append(conf.ip().toString()).toUpper());
    }
}

QString ControllerDiscovery::foundList() const {
    if(mDiscovered.isEmpty()) return "";
    else return mDiscovered.join(",");
}
