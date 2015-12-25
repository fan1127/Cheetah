//
// Created by Vaclav Samec on 10/20/14 AD.
//

#pragma  once

#include <string>
#include <vector>
#include "../AppMacros.h"

namespace cocos2d { class CCSprite; class CCSpriteFrame; namespace extension { class CCScale9Sprite; } }

class UIHelper
{
public:
    
    static void setTwoNodesToMiddlePosition(cocos2d::CCNode* nodeLeft, cocos2d::CCNode* nodeRight, float margin, cocos2d::CCPoint position);
    static void setTwoNodesToMiddlePosition(float widthNodeLeft, float widthNodeRight, cocos2d::CCNode* nodeLeft, cocos2d::CCNode* nodeRight, float margin, cocos2d::CCPoint position);

    //! cache all UI sprite lists for fast access
    static void cacheSprites();

    //! try to open all sprites before the game starts
    static void checkSprites();

    //! get sprite from cache by name
    static cocos2d::CCSprite* getSpriteByName(std::string name)
    {
        return cocos2d::CCSprite::createWithSpriteFrameName(name.c_str());
    }

    //! all scalable sprites
    enum ScalableSprites
    {
        Header,
        ProgressBarBg,
        ProgressBarColor,
        AchievementBG,
        AchievementBGSolid,
        Ribbon,
        RibbonSmall,
        RibbonGrey,
        ScreenForSlice,
        TabActive,
        TabInactive,
        TrackMenuBg,
        PaperBg,

        RaceElementsBarGreen,
        RaceElementsBarYellow,
        RaceElementsBarWhite,

        ProgressBarBgHUD,

        ScalableSpritesEnd,
    };

    //! get scalable sprite as scale8sprite
    static cocos2d::extension::CCScale9Sprite* getScalableSprite(ScalableSprites sprite);

    //! get scalable sprite as normal sprite
    static cocos2d::CCSprite* getScalableSpriteAsNormal(ScalableSprites sprite)
    {
        CCAssert(sprite < ScalableSpritesEnd, "Invalid sprite name!");
        return getSpriteByName(scalableSprites[sprite]);
    }

    //! all non-scalable ui sprites
    enum UISprites
    {
        AbilitiesAbilitiesButton,
        AbilitiesAbilitiesButtonBg,
        AbilitiesPowerup1,
        AbilitiesPowerup2,
        AbilitiesPowerup3,
        AbilitiesPowerup4,
        AbilitiesPowerup5,
        AbilitiesStanIcon,
        AbilitiesButtonBgDisabled,
        AbilitiesLock,

        AbilitiesButtonDisabled,
        AbilitiesPowerup1Disabled,
        AbilitiesPowerup2Disabled,
        AbilitiesPowerup3Disabled,
        AbilitiesPowerup4Disabled,
        AbilitiesPowerup5Disabled,

        AchievementsClaimButton,
        AchievementsClaimGem,
        AchievementsGameCenter,
        AchievementsStar,
        AchievementsStarEmpty,
        AchievementsRewardGem,

        AnimalStats_iconTrackAsphalt,
        AnimalStats_iconTrackDirt,
        AnimalStats_iconTrackGrass,
        AnimalStats_iconTrackSand,
        AnimalStats_iconTrackSnow,
        AnimalStatsEnergyIcon,
        AnimalStatsFromBarnIcon,
        AnimalStatsInBarnIcon,
        AnimalStatsLeaderIcon,
        AnimalStatsLikeIcon,
        AnimalStatsMobilityIcon,
        AnimalStatsSpeedIcon,
        AnimalStatsSellIcon,

        BuildBuildingsIcon,
        BuildCrTreeIcon,
        BuildDescriptionBg,
        BuildArrow,

        FacebookConnectionDialogueBox,
        FacebookConnectionFacebookButton,
        FacebookConnectionNoThanksButton,
        FacebookConnectionClaimGemsButton,

        GachaPromotionStroke,
        GachaPromotionButton,
        GachaPromotionGlow,
        GachaPromotionMarker,
        GachaPromotionGem,
        GachaTitleText,

        HUDGatchaButton,
        HUDAchievements,
        HUDAdd,
        HUDBuildButton,
        HUDCoin,
        HUDCounterBig,
        HUDCounterBigColor,
        HUDCounterMedalsNTrace,
        HUDExpirienceBar,
        HUDExpirienceBarColor,
        HUDFood,
        HUDFriends,
        HUDGem,
        HUDInbox,
        HUDMapButton,
        HUDHomeButton,
        HUDNewMarkerBig,
        HUDNewMarkerSmall,
        HUDSettings,
        HUDTrace,
        HUDCompleteTrack,
        HUDIncompleteTrack,
        HUDCompleteIcon,
        HUDIncompleteIcon,
        HUDCheckmarkBig,
        HUDCheckmarkSmall,
        HUDIconCoinNoShadow,
        HUDIconFoodNoShadow,
        HUDSignalSign,
        
        HUDPaperArrow,
        HUDPaperArrowLeft,
        
        NSClaimGem,
        NSClaimGemBlue,
        
        NSClassesE,
        NSClassesD,
        NSClassesC,
        NSClassesB,
        NSClassesA,
        NSClassesS,
        NSClassesSS,

        NSClassesRibbonFriend,
        NSClassesRibbonEnemy,

        NSFriendsIcon,
        NSBuyCoinIcon,
        NSBuyGemIcon,
        NSCloseButton,
        NSHelpButton,
        NSMedal,
        NSRewardGem,
        NSWorldIcon,

        PetShop_uiHabitat,
        PetShop_uiHabitatQuestionmark,
        PossibleRewardsChangeButton,
        PossibleRewardsChangeIcon,
        PossibleRewardsPaper,
        PossibleRewardsSlider,
        PossibleRewardsVisitButton,

        PurchaseCoinIcon,
        PurchaseCoins1,
        PurchaseCoins2,
        PurchaseCoins3,
        PurchaseFood1,
        PurchaseFood2,
        PurchaseFood3,
        PurchaseFoodIcon,
        PurchaseGemIcon,
        PurchaseGems1,
        PurchaseGems2,
        PurchaseGems3,
        PurchaseGems4,
        PurchaseGems5,
        PurchaseItemBg,
        PurchaseMostPopular,
        PurchaseLight,
        PurchasePromotionCrossout,
        PurchasePromotionMarker,
        PurchasePromotionTime,

        RaceElementsArrows,
        RaceElementsBackButton,
        RaceElementsBarBorder,

        RaceElementsEnemyPetBg,
        RaceElementsGiveUpButton1,
        RaceElementsGiveUpButton2,
        RaceElementsGoButton,
        RaceElementsLeaderBg,
        RaceElementsPowerIcon,
        RaceElementsSpeedometer,
        RaceElementsSpeedometerArrow,
        RaceElementsStartButton,
        RaceElementsYourPet,
        RaceElementsWakeUpButton,
        RaceElementsFeedButton,
        RaceElementsFoodIcon,
        RaceElementsActiveAnimal,
        RaceElementsInactiveAnimal,
        RaceElementsBarPattern,
        RaceElementsPowerBarWhite,
        RaceElementsPowerBarBorder,
        RaceElementsButtonHelperIsFriend,
        RaceElementsButtonHelperIsNotFriend,

        RankingVisitButton,
        RankingScoreCounter,

        SettingsOffButton,
        SettingsOnButton,
        SettingsSupportButton,

        TraceBuildingsOptionsAddButton,
        TraceBuildingsOptionsCancelButton,
        TraceBuildingsOptionsDeleteButton,
        TraceBuildingsOptionsInfoButton,
        TraceBuildingsOptionsOkButton,
        TraceBuildingsOptionsOverviewButton,
        TraceBuildingsOptionsRotateButton,
        TraceBuildingsOptionsUpgradeButton,
        TraceBuildingsOptionsStartButton,
        TraceBuildingsOptionsReverseButton,
        TraceBuildingsOptionsAxeButton,
        TraceBuildingsOptionsCollectCoinsButton,
        TraceBuildingsOptionsCollectCoinsInactiveButton,
        TraceBuildingsOptionsCollectFoodButton,
        TraceBuildingsOptionsCollectFoodInactiveButton,
        TraceBuildingsOptionsPackButton,
        TraceBuildingsOptionsSetAsLeaderButton,
        TraceBuildingsOptionsSpeedUpButton,
        TraceBuildingsOptionsBuildIcon,
        TraceBuildingsOptionsSleepIcon,
        TraceBuildingsOptionsUpgradingIcon,
        TraceBuildingsOptionsHungryEmotion,
        TraceBuildingsOptionsSleepyEmotion,
        TraceBuildingsOptionsMerchantBuyIcon,
        TraceBuildingsOptionsPowerUpSmallIcon1,
        TraceBuildingsOptionsPowerUpSmallIcon2,
        TraceBuildingsOptionsPowerUpSmallIcon3,
        TraceBuildingsOptionsPowerUpSmallIcon4,
        TraceBuildingsOptionsPowerUpSmallIcon5,
        TraceBuildingsOptionsGemIconForAnimation,
        TraceBuildingsOptionsBubbleEmotion,

        UpgradePlusBg,
        UpgradeBuildingBg,
        UpgradeClockIcon,
        UpgradeFood1,
        UpgradeFood2,
        UpgradeRanchIcon,
        UpgradeUpArrow,
        UpgradeInstantBuyButton,
        UpgradeEnergyIcon,
        UpgradeCoinsStorage,
        UpgradeItemCapacity,
        UpgradeTickets,
        UpgradeMaxBonus,
        UpgradeAnimalRoom,
        UpgradeDistance,
        UpgradeTrackpoint,
        UpgradeTradableAnimal,

        UpgradeAndTradeItemsForAnimalArrow,
        UpgradeAndTradeItemsForAnimalBuyButton,

        WakeUpCloseButton,
        WakeUpPaper,

        FriendsIcon,
        WorldIcon,

        FriendsHelpStartButton,
        FriendsHelpStunIcon,
        FriendsHelpBg,
        FriendsHelpBgSelected,
        FriendsHelpFacebookButtonNoIcon,
        FriendsHelpFacebookIcon,

        InboxClaimButton,
        InboxCoin,
        InboxGem,
        InboxLevelIcon,
        InboxMedalIcon,
        InboxRevengeButton,
        InboxNewStar,

        AvatarBorder,
        AvatarDefault,

        NotificationYes,
        NotificationNo,
        
        MerchantObjectButton,
        MerchantObjectLight,
        MerchantCoinsButton,
        MerchantGemsButton,
        MerchantObjectSoldoutButton,
        MerchantSoldoutButton,
        
        PlusButton,
        MinusButton,
        
        ReplayHUD1x,
        ReplayHUD2x,
        ReplayHUDPause,
        ReplayHUDPlaySmall,
        ReplayHUDPlayBig,

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        GooglePlayIcon,
        GooglePlayButton,
#endif
        
        UISpritesEnd,
    };

    //! get non-scalable sprite
    static cocos2d::CCSprite* getSprite(UISprites sprite)
    {
        CCAssert(sprite < UISpritesEnd, "Invalid sprite name!");
        return getSpriteByName(uiSprites[sprite]);
    }

    static cocos2d::CCSpriteFrame* getSpriteFrame(UISprites sprite);

    static cocos2d::CCSpriteFrame* getSpriteFrame(ScalableSprites sprite);

    //! get sprite path in plist
    static const char* const getSpritePath(UISprites sprite)
    {
        return uiSprites[sprite];
    }

    //! get scalable sprite path in plist
    static const char* const getSpritePath(ScalableSprites sprite)
    {
        return scalableSprites[sprite];
    }

    enum UIAnimations
    {
        AnimationHUDGatcha,
        AnimationHUDSignalSign,
        UIAnimationsEnd,
    };

    //! returns array of sprites
    static std::vector<cocos2d::CCSpriteFrame*> getAnimationFrames(UIAnimations animation);

private:

    //! definition of all scalable sprites
    static const char* const scalableSprites[ScalableSpritesEnd];

    //! definition of all non-scalable sprites
    static const char* const uiSprites[UISpritesEnd];
};
