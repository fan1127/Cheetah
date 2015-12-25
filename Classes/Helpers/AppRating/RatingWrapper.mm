//
//  RatingWrapper.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 11/12/2557 BE.
//
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)


#ifdef IS_DEBUG_BUILD
    //#define RATE_ALWAYS
#endif



#include "RatingWrapper.h"
#include "Settings.h"
#import "appirater.h"

void RatingWrapper::init()
{
    [Appirater setAppId:@IOS_APP_ID];

    [Appirater setDaysUntilPrompt:2];
    [Appirater setUsesUntilPrompt:5];
    [Appirater setSignificantEventsUntilPrompt:-1];

//#ifdef IS_DEBUG_BUILD
//    [Appirater setDebug:YES];
//#endif

    [Appirater appLaunched:NO];
}

bool RatingWrapper::canRate()
{
#ifdef RATE_ALWAYS
    return true;
#else
    return [Appirater canRate];
#endif
}

void RatingWrapper::rateApp()
{
    [Appirater onRate];
}

void RatingWrapper::dontRateApp()
{
    [Appirater onDontRate];
}

void RatingWrapper::onEnterForeground()
{
    [Appirater appEnteredForeground:NO];
}

#endif
