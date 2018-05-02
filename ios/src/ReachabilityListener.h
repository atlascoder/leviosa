#ifndef REACHABILITYLISTENER_H
#define REACHABILITYLISTENER_H

#include <QtGlobal>

Q_FORWARD_DECLARE_OBJC_CLASS(ReachabilityListener);

namespace utility
{

enum NetworkStatus
{
	NotReachable = 0,
	ReachableViaWiFi,
	ReachableViaWWAN
};

class ReachabilityDelegate
{
public:
    ReachabilityDelegate();
    virtual ~ReachabilityDelegate();

    virtual void statusChanged(NetworkStatus n) = 0;
    std::string ssid() const;
    std::string bssid_ios() const;
    std::string ipaddr() const;
    
    NetworkStatus status() const;
private:
    ReachabilityListener *listener = nullptr;
};

}

#endif // REACHABILITYLISTENER_H
