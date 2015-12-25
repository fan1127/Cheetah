//
//  AppsFlyerWrapper.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 12/22/2557 BE.
//
//

#ifndef __Cheetah__AppsFlyerWrapper__
#define __Cheetah__AppsFlyerWrapper__

#include "cocos2d.h"

#define APP_FLYERS_DEV_KEY "U7ZrxGxQ3mWGFoktUCYGu9"

#define EVENT_TUTORIAL_COMPLETED "Completed Tutorial"
#define EVENT_PLAYER_LEVEL_10 "Ranch Level 10"
#define EVENT_PLAYER_LEVEL_20 "Ranch Level 20"
#define EVENT_PLAYER_LEVEL_30 "Ranch Level 30"
#define EVENT_PLAYER_LEVEL_40 "Ranch Level 40"
#define EVENT_RACE_WON "Race Won"
#define EVENT_RACE_GAVEUP "Race Gave Up"
#define EVENT_RACE_LOST "Race Lost"
#define EVENT_USER_OPEN_IAP_STORE "User Open IAP Store"
#define EVENT_TUT_FIRST_SPEECH_TAPPED "Tut: First Speech Tapped"
#define EVENT_TUT_FIRST_RACE_ENDED "Tut: First Race Ended"

class AppsFlyerWrapper
{
public:
    
    static void init();
    
    static void setCurrencyCode(const char* currencyCode);
    
    static void trackEvent(const char* eventName);
    
    static void trackPurchaseEvent(const char* eventName, const char* eventValue);

    static void startSession();
    
};

#endif /* defined(__Cheetah__AppsFlyerWrapper__) */
