#include "esptouch.h"
#include "esptouchtask.h"

bool Esptouch::isRunning()
{
    return running;
}

Esptouch::Esptouch(QObject *parent) : QObject(parent)
{

}

void Esptouch::interrupt()
{
    if(pTask) pTask->interrupt();
}

void Esptouch::execute(const QString &apSsid, const QString &apBssid, const QString &apPassword)
{
    if(running)
        return;
    else
        running = true;
    pTask = new EspTouchTask(apSsid, apBssid, apPassword);
    pTask->connect(pTask, &EspTouchTask::finished, this, &Esptouch::completed);
    pTask->connect(pTask, &EspTouchTask::finished, pTask, &EspTouchTask::deleteLater);
    pTask->start();
}
