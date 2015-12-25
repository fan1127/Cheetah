//
//  AppsFlyerWrapper.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 12/22/2557 BE.
//
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "AppsFlyerWrapper.h"
#include "Settings.h"
#import "AppsFlyerTracker.h"

USING_NS_CC;

extern std::string uuid;

void AppsFlyerWrapper::init()
{
    [AppsFlyerTracker sharedTracker].customerUserID = [NSString stringWithUTF8String:uuid.c_str()];
    [AppsFlyerTracker sharedTracker].appleAppID = [NSString stringWithUTF8String:IOS_APP_ID];
    [AppsFlyerTracker sharedTracker].appsFlyerDevKey = [NSString stringWithUTF8String:APP_FLYERS_DEV_KEY];
}

void AppsFlyerWrapper::setCurrencyCode(const char* currencyCode)
{
    [AppsFlyerTracker sharedTracker].currencyCode = [NSString stringWithUTF8String:currencyCode];
}

void AppsFlyerWrapper::trackEvent(const char* eventName)
{
    AppsFlyerWrapper::trackPurchaseEvent(eventName, "0");
}

void AppsFlyerWrapper::trackPurchaseEvent(const char* eventName, const char* eventValue)
{
    [[AppsFlyerTracker sharedTracker] trackEvent:[NSString stringWithUTF8String:eventName]
                                       withValue:[NSString stringWithUTF8String:eventValue]];
}

void AppsFlyerWrapper::startSession()
{
    // for iOS this is handled from AppController.mm
    //
    // [[AppsFlyerTracker sharedTracker] trackAppLaunch];
    //
}

#endif
