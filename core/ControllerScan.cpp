#include "ControllerScan.h"
#include "mdnsdiscothread.h"
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDebug>

#include "ControllerConfig.h"

using namespace std;

ControllerScan::ControllerScan(QObject* parent) :
    QThread(parent), mTimeoutSec(10), mIsRunning(false), mDiscovered(), mTimer()
{
    connect(&mTimer, &QTimer::timeout, this, &ControllerScan::timedOut);
    mTimer.setInterval(3000);
    mTimer.setSingleShot(true);
}

void ControllerScan::run()
{
    mQnam = new QNetworkAccessManager;
    connect(mQnam, &QNetworkAccessManager::finished, this, &ControllerScan::gotReply);
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    QHostAddress addr;
    QHostAddress mask;
    int mask_len = 0;

    bool got_addr = false;

    for (int nIter=0; nIter<list.count(); nIter++) {
         if((list[nIter].flags() & QNetworkInterface::IsLoopBack) == 0
             && (list[nIter].flags() & QNetworkInterface::IsRunning) != 0
            ){
             for (int nAddr=0; nAddr<list[nIter].addressEntries().count(); nAddr++) {
                 QHostAddress a = list[nIter].addressEntries().at(nAddr).ip();
                 if (   !a.isLoopback()
                        && a.protocol() == QAbstractSocket::IPv4Protocol
                    ) {
                     addr = list[nIter].addressEntries().at(nAddr).ip();
                     mask_len = list[nIter].addressEntries().at(nAddr).prefixLength();
                     mask = list[nIter].addressEntries().at(nAddr).netmask();
                     got_addr = true;
                     break;
                 }
             }
         }
         if (got_addr) break;
    }

    if (got_addr) {

         if(mask_len == 0) return;

         qDebug() << "addr is " << addr << "/" << mask_len;

         unsigned int net_size = (1 << (32 - mask_len)) - 1;

         quint32 net_addr = addr.toIPv4Address() & mask.toIPv4Address();

         for (unsigned int i = 0; i < net_size; i++) {
             QHostAddress a(net_addr + i);
             QString str = QString("http://").append(a.toString()).append("/api");
             QUrl url = QUrl::fromUserInput(str);
             mQnam->get(QNetworkRequest(url));
             qDebug() << "Send request to " << url.toString();
         }

     }
    exec();
    mQnam->deleteLater();
}

void ControllerScan::gotReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString ip = reply->request().url().host();
        qDebug() << "Got reply from " << ip;

        QString content = reply->readAll();
        ControllerConfig conf;
        conf.parse(content);
        QString mac = conf.mac().toUpper();
        for (int i = 1; i < 6; i++)
            mac.insert(12-2*i,':');

        if (conf.isValid()) {
            emit found(mac, ip);
            mDiscovered.append(mac.append(" ").append(ip));
        }
    }
    reply->deleteLater();
}

int ControllerScan::timeout() const
{
    return mTimeoutSec;
}

void ControllerScan::timedOut()
{
    stop();
    emit foundList(mDiscovered.join(","));
}

void ControllerScan::setTimeout(int timeoutSec)
{
    mTimeoutSec = timeoutSec;
}

bool ControllerScan::running() const
{
    return mIsRunning;
}

void ControllerScan::setRunning(bool isRunning)
{
    if(isRunning == mIsRunning) return;
    mIsRunning = isRunning;
    if(mIsRunning)
        discovery();
    else
        stop();

    emit runningChanged();
}

void ControllerScan::discovered(const QString &mac, const QString &ip)
{
    mDiscovered.append(QString(mac).append(" ").append(ip).toUpper());
}

void ControllerScan::discovery(){
    qDebug() << "DISCO START";
    mTimer.start();
    start();
}

void ControllerScan::stop(){
    qDebug() << "DISCO STOP";
    quit();
    onFinished();
}

void ControllerScan::onFinished(){
    mIsRunning = false;
    emit runningChanged();
    emit finished();
}

ControllerScan::~ControllerScan(){
    mTimer.stop();
    quit();
}

QString ControllerScan::foundList() const {
    if(mDiscovered.isEmpty()) return "";
    else return mDiscovered.join(",");
}
