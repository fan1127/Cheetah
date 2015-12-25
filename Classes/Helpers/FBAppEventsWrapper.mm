//
//  FBAppEventsWrapper.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 12/19/2557 BE.
//
//


#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "FBAppEventsWrapper.h"
#import <FacebookSDK/FacebookSDK.h>

USING_NS_CC;

NSString *getNSStringFromEventName(_FBAppEventName eventName)
{
    switch (eventName) {
        case _FBAppEventNameCompletedRegistration:
            return FBAppEventNameCompletedRegistration;
            break;
            
        case _FBAppEventNameViewedContent:
            return FBAppEventNameViewedContent;
            break;
            
        case _FBAppEventNameSearched:
            return FBAppEventNameSearched;
            break;
            
        case _FBAppEventNameRated:
            return FBAppEventNameRated;
            break;
            
        case _FBAppEventNameCompletedTutorial:
            return FBAppEventNameCompletedTutorial;
            break;
            
        case _FBAppEventNameAddedToCart:
            return FBAppEventNameAddedToCart;
            break;
            
        case _FBAppEventNameAddedToWishlist:
            return FBAppEventNameAddedToWishlist;
            break;
            
        case _FBAppEventNameInitiatedCheckout:
            return FBAppEventNameInitiatedCheckout;
            break;
            
        case _FBAppEventNameAddedPaymentInfo:
            return FBAppEventNameAddedPaymentInfo;
            break;
            
        case _FBAppEventNameAchievedLevel:
            return FBAppEventNameAchievedLevel;
            break;
            
        case _FBAppEventNameUnlockedAchievement:
            return FBAppEventNameUnlockedAchievement;
            break;
            
        case _FBAppEventNameSpentCredits:
            return FBAppEventNameSpentCredits;
            break;
            
        default:
            break;
    }
    
    return @"error";
}

NSString *getNSStringFromParameterName(_FBAppEventParameterName parameterName)
{
    switch (parameterName) {
        case _FBAppEventParameterNameCurrency:
            return FBAppEventParameterNameCurrency;
            break;
            
        case _FBAppEventParameterNameRegistrationMethod:
            return FBAppEventParameterNameRegistrationMethod;
            break;
            
        case _FBAppEventParameterNameContentType:
            return FBAppEventParameterNameContentType;
            break;
            
        case _FBAppEventParameterNameContentID:
            return FBAppEventParameterNameContentID;
            break;
            
        case _FBAppEventParameterNameSearchString:
            return FBAppEventParameterNameSearchString;
            break;
            
        case _FBAppEventParameterNameSuccess:
            return FBAppEventParameterNameSuccess;
            break;
            
        case _FBAppEventParameterNameMaxRatingValue:
            return FBAppEventParameterNameMaxRatingValue;
            break;
            
        case _FBAppEventParameterNamePaymentInfoAvailable:
            return FBAppEventParameterNamePaymentInfoAvailable;
            break;
            
        case _FBAppEventParameterNameNumItems:
            return FBAppEventParameterNameNumItems;
            break;
            
        case _FBAppEventParameterNameLevel:
            return FBAppEventParameterNameLevel;
            break;
            
        case _FBAppEventParameterNameDescription:
            return FBAppEventParameterNameDescription;
            break;
            
        case _FBAppEventParameterValueYes:
            return FBAppEventParameterValueYes;
            break;
            
        case _FBAppEventParameterValueNo:
            return FBAppEventParameterValueNo;
            break;
            
        default:
            break;
    }
    
    return @"error";
}

//Note: this is a direct copy paste from FlurryCpp
NSDictionary* getNSDictionaryFromCCDictionaryWithAllStrings2(cocos2d::CCDictionary* customParameters)
{
    NSMutableDictionary *nsDict = [NSMutableDictionary dictionary];
    
    if (nsDict == NULL)
        return nil;
    
    CCDictElement* pElement = NULL;
    CCDICT_FOREACH(customParameters, pElement)
    {
        CCString* obj = (CCString*)pElement->getObject();
        std::string strKey = pElement->getStrKey();
        
        NSString *strObj = [NSString stringWithUTF8String:obj->getCString()];
        
        [nsDict setObject:strObj forKey:[NSString stringWithUTF8String:strKey.c_str()]];
    }
    
    return nsDict;
}

NSDictionary* getNSDictionaryFromCCDictionaryWithFBParameters(cocos2d::CCDictionary* predefinedParameters)
{
    NSMutableDictionary *nsDict = [NSMutableDictionary dictionary];
    
    if (nsDict == NULL)
        return nil;
    
    CCDictElement* pElement = NULL;
    CCDICT_FOREACH(predefinedParameters, pElement)
    {
        CCString* obj = (CCString*)pElement->getObject();
        std::string strKey = pElement->getStrKey();
        NSString *strKey2 = [NSString stringWithUTF8String:strKey.c_str()];
        int intKey = [strKey2 intValue];
        _FBAppEventParameterName enumKey = (_FBAppEventParameterName)intKey;
        NSString *strKey3 = getNSStringFromParameterName(enumKey);
        
        NSString *strObj = [NSString stringWithUTF8String:obj->getCString()];
        
        [nsDict setObject:strObj forKey:strKey3];
    }
    
    return nsDict;
}

void FBAppEventsWrapper::logEvent(const char* customEventName)
{
    [FBAppEvents logEvent:[NSString stringWithUTF8String:customEventName]];
}

void FBAppEventsWrapper::logEvent(const char* customEventName, double valueToSum)
{
    [FBAppEvents logEvent:[NSString stringWithUTF8String:customEventName]
               valueToSum:valueToSum];
}

void FBAppEventsWrapper::logEvent(const char* customEventName, cocos2d::CCDictionary* customParameters)
{
    [FBAppEvents logEvent:[NSString stringWithUTF8String:customEventName]
               parameters:getNSDictionaryFromCCDictionaryWithAllStrings2(customParameters)];
}

void FBAppEventsWrapper::logEvent(const char* customEventName, double valueToSum, cocos2d::CCDictionary* customParameters)
{
    [FBAppEvents logEvent:[NSString stringWithUTF8String:customEventName]
               valueToSum:valueToSum
               parameters:getNSDictionaryFromCCDictionaryWithAllStrings2(customParameters)];
}

void FBAppEventsWrapper::logEvent(_FBAppEventName eventName, double valueToSum)
{
    [FBAppEvents logEvent:getNSStringFromEventName(eventName)
               valueToSum:valueToSum];
}

void FBAppEventsWrapper::logEvent(_FBAppEventName eventName, cocos2d::CCDictionary* predefinedParameters)
{
    [FBAppEvents logEvent:getNSStringFromEventName(eventName)
               parameters:getNSDictionaryFromCCDictionaryWithFBParameters(predefinedParameters)];
}

void FBAppEventsWrapper::logEvent(_FBAppEventName eventName, double valueToSum, cocos2d::CCDictionary* predefinedParameters)
{
    [FBAppEvents logEvent:getNSStringFromEventName(eventName)
               valueToSum:valueToSum
               parameters:getNSDictionaryFromCCDictionaryWithFBParameters(predefinedParameters)];
}

void FBAppEventsWrapper::logPurchase(double amount, const char* currency)
{
    [FBAppEvents logPurchase:amount
                    currency:[NSString stringWithUTF8String:currency]];
}


#endif
