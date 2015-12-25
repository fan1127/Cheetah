//
//  AnalyticsClientWrapper.mm
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 1/8/2558 BE.
//
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "cocos-ext.h"
#include "AnalyticsClientWrapper.h"
#include "../Settings.h"
#include "../Entities/PlayerState.h"
#include "support/zip_support/ZipUtils.h"
#import <ifaddrs.h>
#import <arpa/inet.h>
#include <ctime>
#include "AnalyticsHelper.h"
#include "StringHelper.h"

USING_NS_CC;

#pragma mark - Instance Handling

#if IS_RELEASE_BUILD
#define ANALYTICS_URL "http://analytics.pocketplaylab.com/events"
#else
#define ANALYTICS_URL "http://analytics.pocketplaylab.com/events?debug=true"
#endif

#define ANALYTICS_HTTP_RETRIES (1)


// Get IP Address
bool AnalyticsClient::usingWifi()
{
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    NSString *wifiAddress = nil;
    NSString *cellAddress = nil;
    
    // retrieve the current interfaces - returns 0 on success
    if(!getifaddrs(&interfaces)) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            sa_family_t sa_type = temp_addr->ifa_addr->sa_family;
            if(sa_type == AF_INET || sa_type == AF_INET6) {
                NSString *name = [NSString stringWithUTF8String:temp_addr->ifa_name];
                NSString *addr = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)]; // pdp_ip0
                //NSLog(@"NAME: \"%@\" addr: %@", name, addr); // see for yourself
                
                if([name isEqualToString:@"en0"]) {
                    // Interface is the wifi connection on the iPhone
                    wifiAddress = addr;
                } else if([name isEqualToString:@"pdp_ip0"]) {
                    // Interface is the cell connection on the iPhone
                    cellAddress = addr;
                }
                
            }
            temp_addr = temp_addr->ifa_next;
        }
        // Free memory
        freeifaddrs(interfaces);
    }
//    NSString *addr = wifiAddress ? wifiAddress : cellAddress;
    
    CCLog_("wifiAddress:%s", [wifiAddress UTF8String]);
    CCLog_("cellAddress:%s", [cellAddress UTF8String]);
    
    bool usingWifi = wifiAddress;
    
    return usingWifi;
}

std::string AnalyticsClient::generateUUID()
{
    CFUUIDRef uuidRef = CFUUIDCreate(NULL);
    CFStringRef uuidStringRef = CFUUIDCreateString(NULL, uuidRef);
    CFRelease(uuidRef);
    NSString *uuid = [(NSString *)uuidStringRef autorelease];
    return [uuid UTF8String];
}

void AnalyticsClient::sendData()
{
    std::string compressedJson = getCompressedData();

    if (compressedJson.empty())
    {
        return;
    }

    std::string size = StringHelper::toString(compressedJson.length());

    NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
    [request setURL:[NSURL URLWithString:@ANALYTICS_URL]];
    [request setHTTPMethod:@"POST"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    [request setValue: [NSString stringWithCString:size.c_str() encoding:[NSString defaultCStringEncoding]] forHTTPHeaderField:@"Content-Length"];
    [request setValue:@"deflate" forHTTPHeaderField:@"Content-Encoding"];
    NSMutableData *data = [NSMutableData data];
    [data appendBytes:compressedJson.c_str() length:compressedJson.length()];
    [request setHTTPBody:data];

    NSHTTPURLResponse* response = nil;
    NSError* error = nil;
    NSData* connection = [NSURLConnection sendSynchronousRequest:request
                                          returningResponse:&response
                                                      error:&error];
    if (error == nil)
    {
        NSString* dataString = [[[NSString alloc] initWithData:connection encoding:NSUTF8StringEncoding] autorelease];
        onResponded(response.statusCode, [dataString UTF8String]);
    }
    else
    {
        CCLog_("AnalyticsClient request failed: %d", error.code);
    }
}

void AnalyticsClient::appDidEnterBackground()
{
	logEvent(EVENT_SESSION_END);

    UIApplication * application = [UIApplication sharedApplication];

    if([[UIDevice currentDevice] respondsToSelector:@selector(isMultitaskingSupported)])
    {
        NSLog(@"Multitasking Supported");

        __block UIBackgroundTaskIdentifier background_task;
        background_task = [application beginBackgroundTaskWithExpirationHandler:^ {

            //Clean up code. Tell the system that we are done.
            [application endBackgroundTask: background_task];
            background_task = UIBackgroundTaskInvalid;
        }];

        //To make the code block asynchronous
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{

            //### background task starts
            NSLog(@"Running in the background\n");

            sendData();

            //Clean up code. Tell the system that we are done.
            [application endBackgroundTask: background_task];
            background_task = UIBackgroundTaskInvalid;
        });
    }
    else
    {
        NSLog(@"Multitasking Not Supported");
    }
}

#endif
