//
//  FBAppEventsWrapper.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 12/19/2557 BE.
//
//

#ifndef __Cheetah__FBAppEventsWrapper__
#define __Cheetah__FBAppEventsWrapper__

#include "cocos2d.h"

enum _FBAppEventName {
    _FBAppEventNameCompletedRegistration,
    _FBAppEventNameViewedContent,
    _FBAppEventNameSearched,
    _FBAppEventNameRated,
    _FBAppEventNameCompletedTutorial,
    _FBAppEventNameAddedToCart,
    _FBAppEventNameAddedToWishlist,
    _FBAppEventNameInitiatedCheckout,
    _FBAppEventNameAddedPaymentInfo,
    _FBAppEventNameAchievedLevel,
    _FBAppEventNameUnlockedAchievement,
    _FBAppEventNameSpentCredits
};

enum _FBAppEventParameterName {
    _FBAppEventParameterNameCurrency,
    _FBAppEventParameterNameRegistrationMethod,
    _FBAppEventParameterNameContentType,
    _FBAppEventParameterNameContentID,
    _FBAppEventParameterNameSearchString,
    _FBAppEventParameterNameSuccess,
    _FBAppEventParameterNameMaxRatingValue,
    _FBAppEventParameterNamePaymentInfoAvailable,
    _FBAppEventParameterNameNumItems,
    _FBAppEventParameterNameLevel,
    _FBAppEventParameterNameDescription,
    _FBAppEventParameterValueYes,
    _FBAppEventParameterValueNo
};

class FBAppEventsWrapper
{
public:
    
    static void logEvent(const char* customEventName);
    
    static void logEvent(const char* customEventName, double valueToSum);
    
    static void logEvent(const char* customEventName, cocos2d::CCDictionary* customParameters);
    
    static void logEvent(const char* customEventName, double valueToSum, cocos2d::CCDictionary* customParameters);
    
    static void logEvent(_FBAppEventName eventName, double valueToSum);
    
    static void logEvent(_FBAppEventName eventName, cocos2d::CCDictionary* predefinedParameters);
    
    static void logEvent(_FBAppEventName eventName, double valueToSum, cocos2d::CCDictionary* predefinedParameters);
    
    static void logPurchase(double amount, const char* currency);
};

#endif /* defined(__Cheetah__FBAppEventsWrapper__) */
