//
//  CustomEventsHelper.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 11/10/2557 BE.
//
//

#ifndef __Cheetah__CustomEventsHelper__
#define __Cheetah__CustomEventsHelper__

#include <stdio.h>
#include "cocos2d.h"
#include "UserData.h"

#define kCustomEventsIdleAnimationVersion "idle_animation_version"
#define kCustomEventsIdleAnimation "idle_animation"

#define kIdleAnimationTapVill "idle_animation_tap_vill"
#define kIdleAnimationTapVillTimesUsedMax 1

#define kIdleAnimationFreeItem "idle_animation_free_item"
#define kIdleAnimationFreeItemTimesUsedMax 1

#define kIdleAnimationRaceButton "idle_animation_race_button"
#define kIdleAnimationRaceButtonTimesUsedMax 3 //TODO: get this value from GameData server

#define kIdleAnimationConnectFacebook "idle_animation_connect_facebook"

#define kIdleAnimationMapButton "idle_animation_map_button"
#define kIdleAnimationMapButtonTimesUsedMax 1
#define kIdleAnimationMapButtonBeginTime "idle_animation_map_button_begin_time"
#define kIdleAnimationMapButtonTimeUntilEnabled 3.0f

#define kIdleAnimationChoosePet "idle_animation_choose_pet"

#define kCustomEventsPetsAreSleeping "custom_event_pets_are_sleeping"
#define kCustomEventsBuildingUpgrading "custom_event_building_upgrading"

#define kDefaultIdleAnimationDelay 3.0f
#define kDefaultIdleAnimationScaleDifference 0.07f

#define kIAPGemPromotionKey       "custom_event_gem_promotion"
#define kIAPGemPromotionTimerKey  "custom_event_gem_promotion_timer"
#define kIAPGatchaOneTimeOfferKey "custom_event_gatcha_one_time_offer"
#define kIAPGatchaOneTimeOfferCountKey "custom_event_gatcha_one_time_offer_count"
#define kIAPGatchaOneTimeOfferVisited         "custom_event_gatcha_one_time_offer_visited"
#define kIAPGatchaOneTimeOfferVisitedPremium  "custom_event_gatcha_one_time_offer_visited_premium"
#define kIAPGatchaOneTimeOfferCancelNum   "custom_event_gatcha_cancel_num"
#define kIAPGatchaOneTimeOfferCancelTimer "custom_event_gatcha_cancel_timer"

#define kIAPTimeAfterTutorialKey "custom_event_time_after_tutorial"
#define kIAPRacesWonKey          "custom_event_races_won"


class CustomEventsHelper
{
private:

    Dictionary<double> doubleData;
    Dictionary<int> intData;
    Dictionary<bool> boolData;

    void setDouble(const char* key, double value);
    double getDouble(const char*key, double def) const;
    void setInt(const char* key, int value);
    int  getInt(const char*key, int def) const;
    void setBool(const char* key, bool value);
    bool getBool(const char*key, bool def) const;

    CustomEventsHelper();

    // this instance is non-copyable!
    CustomEventsHelper(const CustomEventsHelper &);
    CustomEventsHelper &operator=(const CustomEventsHelper &);

    bool m_raced;

public:

    static CustomEventsHelper& Instance()
    {
        static CustomEventsHelper instance;
        return instance;
    }

    void doWhenTutorialsBegins();
    void doAfterTutorialsEnds();
    
    bool shouldAnimateIdle(std::string idleAnimationName);
    bool shouldAnimateIdleWithCurrentTimesUsed(std::string idleAnimationName, int timesUsedMax);
    bool shouldAnimateIdleWithCurrentTimeAfterBeginTime(std::string beginTimeKey, float timeUntilEnabled);
    void recordBeginTimeFor(std::string beginTimeKey);
    void checkAndIncreaseTimesUsedFor(std::string idleAnimationName);
    
    cocos2d::CCAction* getIdleAnimation();
    cocos2d::CCAction* getIdleAnimation(float delay, float scaleDifference);
    cocos2d::CCAction* getIdleAnimation(float delayBefore, float delayAfter, float scaleDifference);
    cocos2d::CCAction* getIdleAnimationWithOriginalScale(float originalScale,
                                                         float delayBefore,
                                                         float delayAfter,
                                                         float scaleDifference);
    cocos2d::CCAction* getIdleAnimationAnimateOnce();

    bool isTutorialFinished() const;

    void sleepingCheck();
    bool isSleepingCheckedFinished();

    void upgradingCheck();
    bool isUpgradingCheckedFinished();
    
    void resetAllSettings();

    void onEnterRaceScene() { m_raced = true; }
    void onRaceWon();
    int getNumberOfWins() const;
    int getTimeFromTutorial() const;

    // ------------------------
    // IAP PURCHASES
    // ------------------------
    void setGemPromotionWithTimer(int timerSeconds);
    void setGemPromotion(bool status);
    bool isGemPromotion();
    int  getGemPromotionTimer();
    void updateGemPromotion();

    bool isOneTimeGatchaOffer();
    void onOneTimeGatchaOffer();
    void onVisitedGatchaScreenWithoutPromotion(bool premium);
    void updateOneTimeGatchaOffer();
    void onCancelGatchaOneTimeOffer();
    bool isTimeFromGatchaCancel() const;
};

#endif /* defined(__Cheetah__AnalyticsHelper__) */
