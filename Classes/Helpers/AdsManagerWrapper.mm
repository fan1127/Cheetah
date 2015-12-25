//
//  AdsManagerWrapper.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 12/23/2557 BE.
//
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "AdsManagerWrapper.h"
#import <Chartboost/Chartboost.h>


void AdsManagerWrapper::showInterstitial()
{
//    if ([Chartboost hasInterstitial:CBLocationGameScreen]) //forces display, only when the ads are cached
//    {
        [Chartboost showInterstitial:CBLocationGameScreen];
//    }
}

void AdsManagerWrapper::cacheInterstitial()
{
    [Chartboost cacheInterstitial:CBLocationGameScreen];
}

#endif
