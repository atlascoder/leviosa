#include "mdnsdiscothread.h"
#include <QTextStream>

MDNSDiscoThread::MDNSDiscoThread(QObject *parent):
    QThread(parent)
{

}

void MDNSDiscoThread::run(){
    sleep(1);
    emit discovered("11:22:33:44:55:66", "192.168.1.10");
    QTextStream(stdout) << "discovered 1" << endl;
    sleep(1);
    emit discovered("77:88:99:aa:bb:cc", "192.168.1.11");
    emit discovered("77:88:99:aa:bb:c1", "192.168.1.12");
    emit discovered("77:88:99:aa:bb:c2", "192.168.1.13");
    QTextStream(stdout) << "discovered 2" << endl;
    sleep(2);
}
