//
//  CustomEventsHelper.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 11/10/2557 BE.
//
//

#include "CustomEventsHelper.h"
#include "../Entities/Tutorials/CTutAchievement.h"

#include "../Entities/PlayerState.h"

#include "../Settings.h"
#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

extern double currentTimeClient;

USING_NS_CC;


void CustomEventsHelper::setDouble(const char* key, double value)
{
    doubleData.set(key, value);
    CCUserDefault::sharedUserDefault()->setDoubleForKey(key, value);
}

double CustomEventsHelper::getDouble(const char*key, double def) const
{
    return doubleData.get(key, def);
}

void CustomEventsHelper::setInt(const char* key, int value)
{
    intData.set(key, value);
    CCUserDefault::sharedUserDefault()->setIntegerForKey(key, value);
}

int CustomEventsHelper::getInt(const char*key, int def) const
{
    return intData.get(key, def);
}

void CustomEventsHelper::setBool(const char* key, bool value)
{
    boolData.set(key, value);
    CCUserDefault::sharedUserDefault()->setBoolForKey(key, value);
}

bool CustomEventsHelper::getBool(const char*key, bool def) const
{
    return boolData.get(key, def);
}

CustomEventsHelper::CustomEventsHelper() : m_raced(false)
{
    //! DEBUG, reset all settings
#ifdef IS_DEBUG_BUILD
    //resetAllSettings();
#endif

    setInt(kCustomEventsIdleAnimationVersion, CCUserDefault::sharedUserDefault()->getIntegerForKey(kCustomEventsIdleAnimationVersion, 0));

    setBool(kCustomEventsPetsAreSleeping, CCUserDefault::sharedUserDefault()->getBoolForKey(kCustomEventsPetsAreSleeping, false));
    setBool(kCustomEventsBuildingUpgrading, CCUserDefault::sharedUserDefault()->getBoolForKey(kCustomEventsBuildingUpgrading, false));

    setInt(kIdleAnimationConnectFacebook, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIdleAnimationConnectFacebook, 0.0));
    setInt(kIdleAnimationRaceButton, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIdleAnimationRaceButton, 0.0));
    setInt(kIdleAnimationMapButton, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIdleAnimationMapButton, 0.0));
    setDouble(kIdleAnimationMapButtonBeginTime, CCUserDefault::sharedUserDefault()->getDoubleForKey(kIdleAnimationMapButtonBeginTime, 0.0));
    setInt(kIdleAnimationFreeItem, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIdleAnimationFreeItem, 0.0));;
    setInt(kIdleAnimationTapVill, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIdleAnimationTapVill, 0.0));

    setInt(kIAPGemPromotionTimerKey, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIAPGemPromotionTimerKey, 0));
    setBool(kIAPGemPromotionKey, CCUserDefault::sharedUserDefault()->getBoolForKey(kIAPGemPromotionKey, false));
    setBool(kIAPGatchaOneTimeOfferKey, CCUserDefault::sharedUserDefault()->getBoolForKey(kIAPGatchaOneTimeOfferKey, false));
    setInt(kIAPGatchaOneTimeOfferCountKey, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIAPGatchaOneTimeOfferCountKey, 0));
    setBool(kIAPGatchaOneTimeOfferVisited, CCUserDefault::sharedUserDefault()->getBoolForKey(kIAPGatchaOneTimeOfferVisited, false));
    setBool(kIAPGatchaOneTimeOfferVisitedPremium, CCUserDefault::sharedUserDefault()->getBoolForKey(kIAPGatchaOneTimeOfferVisitedPremium, false));
    setInt(kIAPGatchaOneTimeOfferCancelNum, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIAPGatchaOneTimeOfferCancelNum, 0));
    setInt(kIAPGatchaOneTimeOfferCancelTimer, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIAPGatchaOneTimeOfferCancelTimer, 0));
    setInt(kIAPTimeAfterTutorialKey, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIAPTimeAfterTutorialKey, 0));
    setInt(kIAPRacesWonKey, CCUserDefault::sharedUserDefault()->getIntegerForKey(kIAPRacesWonKey, 0));

//// !!!!!!! DEBUG !!!!!!!
//   setInt(kIAPGatchaOneTimeOfferCountKey, 0);
//// !!!!!!!
}

bool CustomEventsHelper::shouldAnimateIdle(std::string idleAnimationName)
{
#if !(IS_DEBUG_BUILD)
    int idle_animation_version = getInt(kCustomEventsIdleAnimationVersion, 0);
    if (idle_animation_version != 1)
        return false;
#endif
    
    if (idleAnimationName == kIdleAnimationTapVill) {
        
        CCLog_("_isTutorialFinished:%d",isTutorialFinished());
        CCLog_("shouldAnimateIdleWithCurrentTimesUsed:%d",shouldAnimateIdleWithCurrentTimesUsed(kIdleAnimationTapVill, kIdleAnimationTapVillTimesUsedMax));
        
        return shouldAnimateIdleWithCurrentTimesUsed(kIdleAnimationTapVill, kIdleAnimationTapVillTimesUsedMax) && isTutorialFinished();
    }
    
    if (idleAnimationName == kIdleAnimationFreeItem) {
        return shouldAnimateIdleWithCurrentTimesUsed(kIdleAnimationFreeItem, kIdleAnimationFreeItemTimesUsedMax);
    }
    
    if (idleAnimationName == kIdleAnimationRaceButton) {
        return shouldAnimateIdleWithCurrentTimesUsed(kIdleAnimationRaceButton, kIdleAnimationRaceButtonTimesUsedMax);
    }
    
    if (idleAnimationName == kIdleAnimationConnectFacebook) {
        return shouldAnimateIdleWithCurrentTimesUsed(kIdleAnimationConnectFacebook, 1);
    }
    
    if (idleAnimationName == kIdleAnimationMapButton) {
        bool shouldTimesUsed = shouldAnimateIdleWithCurrentTimesUsed(kIdleAnimationMapButton, kIdleAnimationMapButtonTimesUsedMax);
        bool shouldTimeAfterBeginTime = shouldAnimateIdleWithCurrentTimeAfterBeginTime(kIdleAnimationMapButtonBeginTime, kIdleAnimationMapButtonTimeUntilEnabled);
        
        return (shouldTimesUsed && shouldTimeAfterBeginTime);
    }
    
    if (idleAnimationName == kIdleAnimationChoosePet) {
        return PlayerState::getPlayer()->player.racing.level < 3;
    }
    
    return false;
}


bool CustomEventsHelper::shouldAnimateIdleWithCurrentTimesUsed(std::string idleAnimationName, int timesUsedMax)
{
    int timesUsed = getInt(idleAnimationName.c_str(), 0);

    return (timesUsed < timesUsedMax);
}

void CustomEventsHelper::checkAndIncreaseTimesUsedFor(std::string idleAnimationName)
{
    if (!CustomEventsHelper::shouldAnimateIdle(idleAnimationName))
        return;
    
    int timesUsed = getInt(idleAnimationName.c_str(), 0);
    
    setInt(idleAnimationName.c_str(), timesUsed+1);
}

bool CustomEventsHelper::shouldAnimateIdleWithCurrentTimeAfterBeginTime(std::string beginTimeKey, float timeUntilEnabled)
{
    double beginTime = getDouble(beginTimeKey.c_str(), 0.0);

    if (beginTime == 0) //value isn't set
        return false;
    
    double timePassed = currentTimeClient - beginTime;
    
    return (timePassed > timeUntilEnabled);
}

void CustomEventsHelper::recordBeginTimeFor(std::string beginTimeKey)
{
    //note: the function takes int, but currentTime is unsign int
    setDouble(beginTimeKey.c_str(), (double)currentTimeClient);
}

void CustomEventsHelper::doAfterTutorialsEnds()
{
    CustomEventsHelper::recordBeginTimeFor(kIdleAnimationMapButtonBeginTime);
    setInt(kIAPTimeAfterTutorialKey, (int)currentTimeClient);

    g_sceneManager->cleanAfterTutorial();
}

void CustomEventsHelper::doWhenTutorialsBegins()
{
    //this is so that we can target idle animations to specific user who has just completed tutorial with this version
    setInt(kCustomEventsIdleAnimationVersion, 1);
}

cocos2d::CCAction* CustomEventsHelper::getIdleAnimation()
{
    return getIdleAnimation(kDefaultIdleAnimationDelay, kDefaultIdleAnimationScaleDifference);
}

cocos2d::CCAction* CustomEventsHelper::getIdleAnimation(float delay, float scaleDifference)
{
    return CCRepeatForever::create(CCSequence::create(CCDelayTime::create(delay),
                                                      CCScaleTo::create(0.15f, 1.0 + scaleDifference),
                                                      CCScaleTo::create(0.15f, 1.0 - scaleDifference),
                                                      CCScaleTo::create(0.15f, 1.0 + scaleDifference),
                                                      CCScaleTo::create(0.15f, 1.0),
                                                      NULL));
}

cocos2d::CCAction* CustomEventsHelper::getIdleAnimation(float delayBefore, float delayAfter, float scaleDifference)
{
    return getIdleAnimationWithOriginalScale(1.0f, delayBefore, delayAfter, scaleDifference);
}

cocos2d::CCAction* CustomEventsHelper::getIdleAnimationWithOriginalScale(float originalScale, float delayBefore, float delayAfter, float scaleDifference)
{
    return CCRepeatForever::create(CCSequence::create(CCDelayTime::create(delayBefore),
                                                      CCScaleTo::create(0.15f, originalScale + scaleDifference),
                                                      CCScaleTo::create(0.15f, originalScale - scaleDifference),
                                                      CCScaleTo::create(0.15f, originalScale + scaleDifference),
                                                      CCScaleTo::create(0.15f, originalScale),
                                                      CCDelayTime::create(delayAfter),
                                                      NULL));
}

cocos2d::CCAction* CustomEventsHelper::getIdleAnimationAnimateOnce()
{
    float scaleDifference = 0.10f;
    return CCRepeatForever::create(CCSequence::create(CCDelayTime::create(3.0f),
                                                      CCScaleTo::create(0.15f, 1.0 + scaleDifference),
                                                      CCScaleTo::create(0.15f, 1.0 - scaleDifference),
                                                      CCScaleTo::create(0.01f, 1),
                                                      CCDelayTime::create(2.0f),
                                                      NULL));
}

bool CustomEventsHelper::isTutorialFinished() const
{
    return PlayerState::getPlayer()->Checklist[TutAchievement::key()];
}

void CustomEventsHelper::sleepingCheck()
{
    setBool(kCustomEventsPetsAreSleeping, true);
}

bool CustomEventsHelper::isSleepingCheckedFinished()
{
    return getBool(kCustomEventsPetsAreSleeping, false);
}

void CustomEventsHelper::upgradingCheck()
{
    setBool(kCustomEventsBuildingUpgrading, true);
}

bool CustomEventsHelper::isUpgradingCheckedFinished()
{
    return getBool(kCustomEventsBuildingUpgrading, false);
}

void CustomEventsHelper::resetAllSettings()
{
    setInt(kCustomEventsIdleAnimationVersion, 0);
    
    setBool(kCustomEventsPetsAreSleeping, false);
    setBool(kCustomEventsBuildingUpgrading, false);
    
    setInt(kIdleAnimationRaceButton, 0.0);
    setInt(kIdleAnimationConnectFacebook, 0.0);
    setInt(kIdleAnimationMapButton, 0.0);
    setInt(kIdleAnimationFreeItem, 0.0);
    setInt(kIdleAnimationTapVill, 0.0);
    setDouble(kIdleAnimationMapButtonBeginTime, 0.0);

    setInt(kIAPGemPromotionTimerKey, 0);
    setBool(kIAPGemPromotionKey, false);
    setBool(kIAPGatchaOneTimeOfferKey, false);
    setInt(kIAPGatchaOneTimeOfferCountKey, 0);
    setBool(kIAPGatchaOneTimeOfferVisited, false);
    setBool(kIAPGatchaOneTimeOfferVisitedPremium, false);
    setInt(kIAPGatchaOneTimeOfferCancelNum, 0);
    setInt(kIAPGatchaOneTimeOfferCancelTimer, 0);
    setInt(kIAPTimeAfterTutorialKey, 0);
    setInt(kIAPRacesWonKey, 0);
}


// ------------------------
// IAP PURCHASES
// ------------------------
void CustomEventsHelper::setGemPromotionWithTimer(int timerSeconds)
{
    if (timerSeconds > 0)
    {
        setInt(kIAPGemPromotionTimerKey, (int) currentTimeClient + timerSeconds);
        setBool(kIAPGemPromotionKey, true);
    }
}

void CustomEventsHelper::setGemPromotion(bool status)
{
    setBool(kIAPGemPromotionKey, status);
}

bool CustomEventsHelper::isGemPromotion()
{
    updateGemPromotion();
    return getBool(kIAPGemPromotionKey, true);
}

void CustomEventsHelper::updateGemPromotion()
{
    int timer = (int)(getInt(kIAPGemPromotionTimerKey, 0) - currentTimeClient);

    if (timer < 0)
    {
        setBool(kIAPGemPromotionKey, false);
    }
}

int  CustomEventsHelper::getGemPromotionTimer()
{
    updateGemPromotion();
    return (int)(getInt(kIAPGemPromotionTimerKey, 0) - currentTimeClient);
}

bool CustomEventsHelper::isOneTimeGatchaOffer()
{
    updateOneTimeGatchaOffer();
    return getBool(kIAPGatchaOneTimeOfferKey, false);
}

void CustomEventsHelper::onOneTimeGatchaOffer()
{
    setInt(kIAPGatchaOneTimeOfferCountKey, getInt(kIAPGatchaOneTimeOfferCountKey, 0)+1);
    updateOneTimeGatchaOffer();

    CCLog_("CustomEventsHelper::onOneTimeGatchaOffer %d", getInt(kIAPGatchaOneTimeOfferCountKey, 0));
}

void CustomEventsHelper::onRaceWon()
{
    setInt(kIAPRacesWonKey, getNumberOfWins()+1);
    CCLog_("CustomEventsHelper::onRaceWon %d", getNumberOfWins());
}

int CustomEventsHelper::getNumberOfWins() const
{
    return getInt(kIAPRacesWonKey, 0);
}

int CustomEventsHelper::getTimeFromTutorial() const
{
    if (isTutorialFinished())
    {
        CCLog_("CustomEventsHelper::getTimeFromTutorial %d", (int)currentTimeClient - getInt(kIAPTimeAfterTutorialKey, 0));
        return (int)currentTimeClient - getInt(kIAPTimeAfterTutorialKey, 0);
    }

    return 0;
}

bool CustomEventsHelper::isTimeFromGatchaCancel() const
{
    int timeFromCancel = (int)currentTimeClient - getInt(kIAPGatchaOneTimeOfferCancelTimer, 0);
    int num = getInt(kIAPGatchaOneTimeOfferCancelNum, 0);

    bool isTime = timeFromCancel > num * 72*60*60;

    CCLog_("CustomEventsHelper::isTimeFromGatchaCancel %d %d %d", timeFromCancel, num, isTime);

    return isTime;
}

void CustomEventsHelper::updateOneTimeGatchaOffer()
{
    setBool(kIAPGatchaOneTimeOfferKey, false);

    int numberOfAcceptedOffers = getInt(kIAPGatchaOneTimeOfferCountKey, 0);

    CCLog_("CustomEventsHelper::updateOneTimeGatchaOffer %d", numberOfAcceptedOffers);

    if (numberOfAcceptedOffers < 2)
    {
        bool premium = getBool(kIAPGatchaOneTimeOfferVisitedPremium, false);

        if (premium)
        {
            if (numberOfAcceptedOffers == 0)
            {
                setInt(kIAPGatchaOneTimeOfferCountKey, 1);
            }

            // premium stuff
            bool oneTimeVisitOfGatchaPremium = getBool(kIAPGatchaOneTimeOfferVisitedPremium, false);
            bool cancelTimeout = isTimeFromGatchaCancel();
            CCLog_("Premium gacha offers: %d %d", oneTimeVisitOfGatchaPremium, cancelTimeout);
            setBool(kIAPGatchaOneTimeOfferKey, oneTimeVisitOfGatchaPremium && cancelTimeout);
        }
        else
        {
            if (numberOfAcceptedOffers == 0)
            {
                bool atLeast30MinutesOfPlay = getTimeFromTutorial() > 30*60;
                bool oneTimeVisitOfGatcha = getBool(kIAPGatchaOneTimeOfferVisited, false);
                bool atLeast5RacesWon = getInt(kIAPRacesWonKey, 0) >= 7;
                bool cancelTimeout = isTimeFromGatchaCancel();

                CCLog_("Normal gacha offers: %d %d %d %d", atLeast30MinutesOfPlay, oneTimeVisitOfGatcha, atLeast5RacesWon, cancelTimeout);

                setBool(kIAPGatchaOneTimeOfferKey, atLeast30MinutesOfPlay && oneTimeVisitOfGatcha && atLeast5RacesWon && cancelTimeout);
            }
        }
    }
}

void CustomEventsHelper::onCancelGatchaOneTimeOffer()
{
    setInt(kIAPGatchaOneTimeOfferCancelNum, getInt(kIAPGatchaOneTimeOfferCancelNum, 0)+1);
    setInt(kIAPGatchaOneTimeOfferCancelTimer, (int)currentTimeClient);

    CCLog_("CustomEventsHelper::onCancelGatchaOneTimeOffer: %d %d", getInt(kIAPGatchaOneTimeOfferCancelNum, 0), getInt(kIAPGatchaOneTimeOfferCancelTimer, 0));
}

void CustomEventsHelper::onVisitedGatchaScreenWithoutPromotion(bool premium)
{
    if (isTutorialFinished())
    {
        if (premium)
        {
            if (!getBool(kIAPGatchaOneTimeOfferVisitedPremium, false))
            {
                setInt(kIAPGatchaOneTimeOfferCancelNum, 0);
            }

            setBool(kIAPGatchaOneTimeOfferVisitedPremium, true);
        }
        else
        {
            setBool(kIAPGatchaOneTimeOfferVisited, true);
        }

        CCLog_("CustomEventsHelper::onVisitedGatchaScreenWithoutPromotion: premium:[%d] %d %d", premium, getBool(kIAPGatchaOneTimeOfferVisitedPremium, false), getBool(kIAPGatchaOneTimeOfferVisited, false));
    }
}
