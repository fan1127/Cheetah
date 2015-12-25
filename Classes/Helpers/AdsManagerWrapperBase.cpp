//
//  AdsManagerWrapper.cpp
//  Cheetah
//
//  Created by Venca
//
//

#include "AdsManagerWrapper.h"
#include "../Settings.h"
#include "../Entities/PlayerState.h"
#include "../Services/GameContentService.h"

USING_NS_CC;

extern unsigned int currentTime;
extern bool hasPurchasedDuringThisSession;

static inline long millisecondNow()
{
    struct cc_timeval now;
    CCTime::gettimeofdayCocos2d(&now, NULL);
    return (now.tv_sec * 1000 + now.tv_usec / 1000);
}

void AdsManagerWrapper::increaseSessionCount()
{
    CCLog_("increaseSessionCount currentSession:%d",getSessionCount());

    CCUserDefault::sharedUserDefault()->setIntegerForKey(SESSION_COUNT_KEY, getSessionCount()+1);

    if (!getFirstPlayingDay()) {
        CCLog_("first played -- getFirstPlayingDay():%lf",getFirstPlayingDay());

        CCUserDefault::sharedUserDefault()->setDoubleForKey(FIRST_PLAYING_DAY_KEY, (double)millisecondNow());
    } else {
        CCLog_("already played -- getFirstPlayingDay():%lf",getFirstPlayingDay());
    }
}

int AdsManagerWrapper::getSessionCount()
{
    return CCUserDefault::sharedUserDefault()->getIntegerForKey(SESSION_COUNT_KEY);
}

double AdsManagerWrapper::getFirstPlayingDay()
{
    CCLog_("getFirstPlayingDay():%lf",CCUserDefault::sharedUserDefault()->getDoubleForKey(FIRST_PLAYING_DAY_KEY));

    return CCUserDefault::sharedUserDefault()->getDoubleForKey(FIRST_PLAYING_DAY_KEY);
}

bool AdsManagerWrapper::meetAdsDisplayingRequirements()
{
    //These 3 conditions must be met
    //1. Play the game on the 5th day or later after logs in
    //2. Play the game at least 20 sessions
    //3. Player hasn't purchased anything (get the values from server)
    
    double minPlayingDaysRequirement = GameContentService::getInstance()->LiveGameConfig["ADS_MIN_DAYS"];
    double minPlayingSessionsRequirement = GameContentService::getInstance()->LiveGameConfig["ADS_MIN_SESSIONS"];
    
    //data not yet loaded or set to 0
    if (minPlayingDaysRequirement <= 0.0f ||
        minPlayingSessionsRequirement <= 0.0f)
    {
        return false;
    }
    
    PlayerState *_playerState = PlayerState::getInstance();

    //must know about PurchaseStat data, otherwise don't display the ads
    if (!_playerState->PurchaseStat.dataReceived)
    {
        CCLog_("_playerState->PurchaseStat.dataReceived is false");
        return false;
    }

    bool userHasNotPurchasedAnything = (_playerState->PurchaseStat.total_purchase <= 0);

    if (hasPurchasedDuringThisSession)
    {
        userHasNotPurchasedAnything = false;
    }

    CCLog_("userHasNotPurchasedAnything:%d",userHasNotPurchasedAnything);

    CCLog_("meetAdsDisplayingRequirements currentSession:%d",getSessionCount());

    double targetDay = (getFirstPlayingDay() + minPlayingDaysRequirement*CONVERT_DAY_TO_MILLISECONDS);
    double currentDay = (double)millisecondNow();

    CCLog_("targetDay:%lf",targetDay);
    CCLog_("currentDay:%lf",currentDay);

    bool playingDays = targetDay <= currentDay;
    bool sessionCount = (getSessionCount() >= minPlayingSessionsRequirement);

    return playingDays && sessionCount && userHasNotPurchasedAnything;
}
