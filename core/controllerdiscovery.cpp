#include "controllerdiscovery.h"
#include "mdnsdiscothread.h"
#include <QTextStream>

ControllerDiscovery::ControllerDiscovery(QObject* parent) :
    QObject(parent), mTimeoutSec(10), mIsRunning(false)
{
    connect(&mZeroConf, &QZeroConf::serviceAdded, this, &ControllerDiscovery::serviceFound);
}

int ControllerDiscovery::timeout() const
{
    return mTimeoutSec;
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

    if(isRunning)
        discovery();
    else
        stop();

    emit runningChanged();
}

void ControllerDiscovery::discovery(){
    if(mIsRunning)
        return;
    else
        mIsRunning = true;
    mZeroConf.startBrowser("_http._tcp");
}

void ControllerDiscovery::stop(){
    if(!mIsRunning)
        return;
    mZeroConf.stopBrowser();
    mIsRunning = false;
}

void ControllerDiscovery::onFinished(){
    mIsRunning = false;
    emit finished();
}

ControllerDiscovery::~ControllerDiscovery(){
    if(mZeroConf.browserExists())
        mZeroConf.stopBrowser();
}

void ControllerDiscovery::serviceFound(QZeroConfService conf)
{
    QTextStream(stdout) << "Domain: " << conf.domain() << " name: " << conf.name() << " ip: " << conf.ip().toString() << " type: " << conf.type() << " host: " << conf.host() << endl;

    QRegExp rx("^(WiShade-([\\da-fA-F]{2})([\\da-fA-F]{2})).*$");
    int pos = rx.indexIn(conf.host());
    if(pos >=0){
        QStringList hosts = rx.capturedTexts();
        QString mac = "24:0A:C4:03:" + hosts.at(2) + ":" + hosts.at(3);
        emit found(mac, conf.ip().toString());
    }
}
