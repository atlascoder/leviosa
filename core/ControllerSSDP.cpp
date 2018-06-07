#include "ControllerSSDP.h"
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDebug>

#include "ControllerConfig.h"

using namespace std;

ControllerSSDP::ControllerSSDP(QObject* parent) :
    QObject(parent), mTimeoutSec(10), mIsRunning(false), mTimer(new QTimer), mGroup("239.255.255.250"), mPort(1900), mSock(nullptr)
{
    connect(mTimer.get(), &QTimer::timeout, this, &ControllerSSDP::timedOut);
}

void ControllerSSDP::gotDatagram()
{
    QByteArray buffer;
    QHostAddress sender;
    quint16 senderPort;
    while(mSock->hasPendingDatagrams())
    {
        buffer.resize(mSock->pendingDatagramSize());
        mSock->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
        QString txt(buffer);
        QStringList lines = txt.split("\r\n");
        QString ip;
        QString config;
        for (QString& line : lines) {
            if (line.startsWith("LOCATION: http://")){
                QRegExp rx("^.*\\/\\/(\\d*\\.\\d*\\.\\d*\\.\\d*).*$");
                if (rx.indexIn(line) >= 0) {
                    ip = rx.cap(1);
                }
            }
            else if (line.startsWith("leviosa.wiShadeController.config: ")) {
                QRegExp rx("^.*\\s(\\{.*\\})$");
                if (rx.indexIn(line) >= 0) {
                    config = rx.cap(1);
                }
            }
        }
        if (!ip.isEmpty() && !config.isEmpty()) {
            emit found(ip, config);
        }
    }
}

void ControllerSSDP::sendSearch()
{
    QByteArray Data;
    Data.append("M-SEARCH * HTTP/1.1\r\n");
    Data.append("HOST: 239.255.255.250:1900\r\n");
    Data.append("ST: urn:leviosa:device:wiShadeController:1\r\n");
    Data.append("MAN: \"ssdp:discover\"\r\n");
    Data.append("MX: 2\r\n\r\n");
    qDebug() << "Sending M-SEARCH";
    mSock->writeDatagram(Data, mGroup, mPort);
}

int ControllerSSDP::timeout() const
{
    return mTimeoutSec;
}

void ControllerSSDP::timedOut()
{
    mIsRunning = false;
    emit onFinished();
}

void ControllerSSDP::setTimeout(int timeoutSec)
{
    mTimeoutSec = timeoutSec;
}

bool ControllerSSDP::running() const
{
    return mIsRunning;
}

void ControllerSSDP::setRunning(bool isRunning)
{
    if(isRunning == mIsRunning) return;
    mIsRunning = isRunning;
    if(mIsRunning)
        discovery();
    else
        stop();

    emit runningChanged();
}

void ControllerSSDP::discovery(){
    qDebug() << "DISCO START";
    emit started();
    mDiscovered.clear();
    if (mSock != nullptr) {
        mSock->close();
        mSock->deleteLater();
    }
    mSock = new QUdpSocket;
    if(mSock->bind(QHostAddress::AnyIPv4, mPort, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint)) {
        qDebug() << "Socket set up";
        if (mSock->joinMulticastGroup(mGroup)) {
            qDebug() << "Joined to multicast";
            connect(mSock, &QUdpSocket::readyRead, this, &ControllerSSDP::gotDatagram);
            connect(mSock, &QUdpSocket::disconnected, this, &ControllerSSDP::onSocketClosed);
            sendSearch();
            mTimer->setSingleShot(true);
            mTimer->setInterval(2000);
            mTimer->start();
            return;
        }
    }
    onSocketClosed();
}

void ControllerSSDP::onSocketClosed()
{
    if (mSock == nullptr) return;
    if (mSock->isOpen()) mSock->close();
    mSock->deleteLater();
    mSock = nullptr;
}

void ControllerSSDP::stop(){
    qDebug() << "DISCO STOP";
    onSocketClosed();
    onFinished();
}

void ControllerSSDP::onFinished(){
    mIsRunning = false;
    emit runningChanged();
    emit finished();
}

QStringList& ControllerSSDP::foundList()
{
    return mDiscovered;
}

ControllerSSDP::~ControllerSSDP(){
    onSocketClosed();
}
