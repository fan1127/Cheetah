//
//  AdsManagerWrapper.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 12/23/2557 BE.
//
//

#ifndef __Cheetah__AdsManagerWrapper__
#define __Cheetah__AdsManagerWrapper__

#include "cocos2d.h"

#define SESSION_COUNT_KEY "SessionCount"
#define FIRST_PLAYING_DAY_KEY "FirstPlayingDay"
#define CONVERT_DAY_TO_MILLISECONDS (86400.0*1000.0)

class AdsManagerWrapper
{
public:
    
    static void increaseSessionCount();
    static int getSessionCount();
    static double getFirstPlayingDay();
    
    static bool meetAdsDisplayingRequirements();
    static void showInterstitial();
    static void cacheInterstitial();
};

#endif /* defined(__Cheetah__AdsManagerWrapper__) */
