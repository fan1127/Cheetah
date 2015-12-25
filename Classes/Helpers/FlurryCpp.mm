//
//  FlurryCpp.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 7/16/2557 BE.
//
//

#include "FlurryCpp.h"
#include <UIKit/UIKit.h>
#include "Flurry.h"
#include "Settings.h"

USING_NS_CC;

const char* FlurryEvent::RACE_START               = "001_RaceStart_v2";
const char* FlurryEvent::RACE_END                 = "002_RaceEnd_v2";
const char* FlurryEvent::RACE_END_PLAYER_WON      = "003_RaceEnd_PlayerWon_v2";
const char* FlurryEvent::RACE_END_PLAYER_GAVEUP   = "004_RaceEnd_PlayerGaveUp_v2";
const char* FlurryEvent::RACE_END_AI_WON          = "005_RaceEnd_AIWon_v2";

const char* FlurryEvent::PLAYER_LEVEL_UP          = "101_PlayerLevelUp_v2";
const char* FlurryEvent::PET_LEVEL_UP             = "102_PetLevelUp_v2";

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
FlurryEvent::FlurryEvent()
{
}
#endif

//Note: this is copied pasted to FBAppEventsWrapper
NSDictionary* getNSDictionaryFromCCDictionaryWithAllStrings(cocos2d::CCDictionary* ccDict)
{
    NSMutableDictionary *nsDict = [NSMutableDictionary dictionary];
    
    if (nsDict == NULL)
        return nil;
    
    CCDictElement* pElement = NULL;
    CCDICT_FOREACH(ccDict, pElement)
    {
        CCString* obj = (CCString*)pElement->getObject();
        std::string strKey = pElement->getStrKey();
        
        NSString *strObj = [NSString stringWithUTF8String:obj->getCString()];
        
        [nsDict setObject:strObj forKey:[NSString stringWithUTF8String:strKey.c_str()]];
    }
    
    return nsDict;
}

void FlurryCpp::setUserID(const char* userID)
{
#if IS_RELEASE_BUILD
    [Flurry setUserID:[NSString stringWithUTF8String:userID]];
#endif
}

void setUserAge(int age)
{
#if IS_RELEASE_BUILD
    [Flurry setAge:age];
#endif
}

void setUserGender(const char* gender)
{
#if IS_RELEASE_BUILD
    //Allowable values are @c @"m" or @c @"f"
    [Flurry setGender:[NSString stringWithUTF8String:gender]];
#endif
}

void FlurryCpp::setAppVersion(const char* version)
{
#if IS_RELEASE_BUILD
    [Flurry setAppVersion:[NSString stringWithUTF8String:version]];
#endif
}

const char* FlurryCpp::getFlurryAgentVersion()
{
    return [[Flurry getFlurryAgentVersion] UTF8String];
}

void FlurryCpp::setShowErrorInLogEnabled(bool value)
{
    [Flurry setShowErrorInLogEnabled:value];
}

void FlurryCpp::setDebugLogEnabled(bool value)
{
    [Flurry setDebugLogEnabled:(FlurryLogLevel)value];
}

void FlurryCpp::setLogLevel(FlurryLogLevelCpp value)
{
    [Flurry setLogLevel:(FlurryLogLevel)value];
}

void FlurryCpp::setSessionContinueSeconds(int seconds)
{
    [Flurry setSessionContinueSeconds:seconds];
}

void FlurryCpp::setCrashReportingEnabled(bool value)
{
    [Flurry setCrashReportingEnabled:value];
}

void FlurryCpp::startSession(const char* apiKey)
{
    [Flurry startSession:[NSString stringWithUTF8String:apiKey]];
}

void FlurryCpp::pauseBackgroundSession()
{
    [Flurry pauseBackgroundSession];
}

void FlurryCpp::addOrigin(const char* originName, const char* originVersion)
{
    [Flurry addOrigin:[NSString stringWithUTF8String:originName] withVersion:[NSString stringWithUTF8String:originVersion]];
}

void FlurryCpp::logEvent(const char* eventName)
{
#if IS_RELEASE_BUILD
    [Flurry logEvent:[NSString stringWithUTF8String:eventName]];
#endif
}

void FlurryCpp::logEvent(const char* eventName, cocos2d::CCDictionary* parameters)
{
#if IS_RELEASE_BUILD
    [Flurry logEvent:[NSString stringWithUTF8String:eventName] withParameters:getNSDictionaryFromCCDictionaryWithAllStrings(parameters)];
#endif
}

void FlurryCpp::logEvent(const char* eventName, bool timed)
{
#if IS_RELEASE_BUILD
    [Flurry logEvent:[NSString stringWithUTF8String:eventName] timed:timed];
#endif
}

void FlurryCpp::logEvent(const char* eventName, cocos2d::CCDictionary* parameters, bool timed)
{
#if IS_RELEASE_BUILD
    [Flurry logEvent:[NSString stringWithUTF8String:eventName] withParameters:getNSDictionaryFromCCDictionaryWithAllStrings(parameters) timed:timed];
#endif
}

void FlurryCpp::endTimedEvent(const char* eventName, cocos2d::CCDictionary* parameters)
{
#if IS_RELEASE_BUILD
    [Flurry endTimedEvent:[NSString stringWithUTF8String:eventName] withParameters:getNSDictionaryFromCCDictionaryWithAllStrings(parameters)];
#endif
}

void FlurryCpp::setEventLoggingEnabled(bool value)
{
    [Flurry setEventLoggingEnabled:value];
}







