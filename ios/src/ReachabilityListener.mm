#include <cassert>
#include <mutex>
#include <string>

#include <Foundation/Foundation.h>

#include "ReachabilityListener.h"
#include "Reachability.h"
#import <SystemConfiguration/CaptiveNetwork.h>

#include <ifaddrs.h>
#include <arpa/inet.h>

@interface  ReachabilityListener : NSObject
@end

@implementation ReachabilityListener
{
    utility::ReachabilityDelegate *delegate;
    Reachability *reachability;
}

- (instancetype)initWithDelegate:(utility::ReachabilityDelegate *) aDelegate
{
    assert(aDelegate);

    if (self = [super init]) {
        delegate = aDelegate;
        reachability = [Reachability reachabilityForInternetConnection];
        [[NSNotificationCenter defaultCenter] addObserver : self selector : @selector(reachabilityStatusChanged:) name : kReachabilityChangedNotification object : nil];
        [reachability startNotifier];
    }

    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [reachability stopNotifier];
    [reachability release];

    [super dealloc];
}

- (void) reachabilityStatusChanged : (Reachability *) notUsed
{
#pragma unused(notUsed)
    assert(delegate);
    delegate->statusChanged(([reachability currentReachabilityStatus]));
}

- (utility::NetworkStatus) status
{
    return [reachability currentReachabilityStatus];
}

- (std::string) ssid
{
    //https://stackoverflow.com/questions/31555640/how-to-get-wifi-ssid-in-ios9-after-captivenetwork-is-deprecated-and-calls-for-wi
    NSString *wifiName = @"";
    NSArray *interFaceNames = (__bridge_transfer id)CNCopySupportedInterfaces();
    
    for (NSString *name in interFaceNames) {
        NSDictionary *info = (__bridge_transfer id)CNCopyCurrentNetworkInfo((__bridge CFStringRef)name);
        
        if (info[@"SSID"]) {
            wifiName = info[@"SSID"];
        }
    }
    return std::string([wifiName UTF8String]);
}

- (std::string) bssid
{
    // sourced https://stackoverflow.com/questions/31555640/how-to-get-wifi-ssid-in-ios9-after-captivenetwork-is-deprecated-and-calls-for-wi
    NSString *wifiName = @"";
    NSArray *interFaceNames = (__bridge_transfer id)CNCopySupportedInterfaces();
    
    for (NSString *name in interFaceNames) {
        NSDictionary *info = (__bridge_transfer id)CNCopyCurrentNetworkInfo((__bridge CFStringRef)name);
        
        if (info[@"SSID"]) {
            wifiName = info[@"BSSID"];
        }
    }
    return std::string([wifiName UTF8String]);
}

- (std::string)ipaddr
{
    // sourced https://stackoverflow.com/questions/6807788/how-to-get-ip-address-of-iphone-programmatically
    NSString *address = @"0.0.0.0";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"]) {
                    // Get NSString from C String
                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                    
                }
                
            }
            
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return std::string([address UTF8String]);
}

@end

namespace utility {

ReachabilityDelegate::ReachabilityDelegate()
{
    listener = [[ReachabilityListener alloc] initWithDelegate:this];
}

ReachabilityDelegate::~ReachabilityDelegate()
{
    [listener release];
}

NetworkStatus ReachabilityDelegate::status() const
{
    return [listener status];
}
    
std::string ReachabilityDelegate::ssid() const
{
    return [listener ssid];
}

std::string ReachabilityDelegate::bssid_ios() const
{
    return [listener bssid];
}

std::string ReachabilityDelegate::ipaddr() const
{
    return [listener ipaddr];
}
}
