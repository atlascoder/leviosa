#include "controllerdiscovery.h"
#include "mdnsdiscothread.h"

ControllerDiscovery::ControllerDiscovery(QObject* parent) :
    QObject(parent), mTimeoutSec(10), mIsRunning(false)
{

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

    mThread = std::unique_ptr<MDNSDiscoThread>(new MDNSDiscoThread(this));
    connect(mThread.get(), &MDNSDiscoThread::discovered, this, &ControllerDiscovery::found);
    connect(mThread.get(), &MDNSDiscoThread::started, this, &ControllerDiscovery::started);
    connect(mThread.get(), &MDNSDiscoThread::finished, [this](){ this->onFinished(); });
    mThread->start();
}

void ControllerDiscovery::stop(){
    if(!mIsRunning)
        return;
    mThread->terminate();
}

void ControllerDiscovery::onFinished(){
    mIsRunning = false;
    mThread.release();
    emit finished();
}

ControllerDiscovery::~ControllerDiscovery(){
    if(mThread.get() != NULL && mThread->isRunning()){
        mThread->terminate();
        mThread->wait();
    }
}
