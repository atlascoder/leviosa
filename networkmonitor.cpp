#include "networkmonitor.h"

void NetworkMonitor::changeState(bool isConnected){
    m_state = isConnected ? ("Online " + mgr.defaultConfiguration().bearerTypeName()) : "Offline";
    qInfo() << m_state;
    emit currentStateChanged(m_state);
}

void NetworkMonitor::isOnline(const bool connected)
{
    changeState(connected);
}

NetworkMonitor::NetworkMonitor(QObject *parent) : QObject(parent)
{
    connect(&mgr, &QNetworkConfigurationManager::onlineStateChanged, this, &NetworkMonitor::isOnline);
    m_state = mgr.isOnline() ? ("Online " + mgr.defaultConfiguration().bearerTypeName()) : "Offline";
}
