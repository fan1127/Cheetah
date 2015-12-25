//
//  Settings.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/12/2556 BE.
//
//

#ifndef Cheetah_Settings_h
#define Cheetah_Settings_h

#include "cocos2d.h"
#include "user.h"
#include "BufferUtils.h"

// app id on ios store
#define IOS_APP_ID                  "855124397"

#define kAnalyticsClientAppID       "ranchrun"

#if IS_DEBUG_BUILD
#define kAnalyticsClientAppEnvironment  "development"
#else
#if IS_RELEASE_BUILD
#define kAnalyticsClientAppEnvironment  "test"
#else
#define kAnalyticsClientAppEnvironment  "production"
#endif
#endif

#define kBuildingUpdateRoutine      1.0f
#define kEnterBackgroundTimeMax     15 //second
#define kIsoMapSaveInterval         15

#define kEmailAddress               "support+RR@playlab.com"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define kEmailSubject               "Feedback%20-%20iOS"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define kEmailSubject               "Feedback%20-%20Android"
#endif

#define kDefaultFont                "dimbo"

#define kDialogTabletSmall          cocos2d::CCPoint(0.6, 0.4)
#define kDialogTabletMedium         cocos2d::CCPoint(0.6, 0.6)
#define kDialogTabletLarge          cocos2d::CCPoint(0.6, 0.6)

#define kDialogPhoneSmall           cocos2d::CCPoint(0.5, 0.5)
#define kDialogPhoneMedium          cocos2d::CCPoint(0.75, 0.75)
#define kDialogPhoneLarge           cocos2d::CCPoint(0.6, 0.6)

#define kDialogFontColour           ccc3(6.0f, 57.0f, 94.0f)

//#define kDialogSmallSize            cocos2d::CCSize(842.0f, 486.0f)
#define kDialogSmallSize            cocos2d::CCSize(1000.0f, 600.0f)
#define kDialogMediumSize           cocos2d::CCSize(1228.0f, 820.0f)
#define kDialogLargeSize            cocos2d::CCSize(1474.0f, 984.0f)

#define kPopOverDialogNarmalWidth   520.0f
#define kPopOverDialogSpacing       20.0f
#define kPopOverDialogPadding       cocos2d::CCSize(64.0f, 64.0f)

//#define kDialogFooterTextEnabled

#define kDefaultAvatarSize          cocos2d::CCSize(102.0f, 102.0f)

#define kXLargeMultiplier           1.000f
#define kLargeMultiplier            0.500f
#define kMediumMultiplier           0.585f
#define kSmallMultiplier            0.300f

#define kDialogFontLabelSize        48.0f
#define kDialogFontLabelSize2       56.0f
#define kDialogFontTitleSize        64.0f

#define kDialogPadding              32.0f

//@PlusPingya
#define kMiniDialogSize1                cocos2d::CCSize(532.0f, 600.0f)
#define kMiniDialogSize2                cocos2d::CCSize(532.0f, 720.0f)
#define kMiniDialogFontTitleSize        60.0f
#define kMiniDialogFontLabelSize        44.0f
#define kMiniDialogFontCLabelSize       60.0f
#define kMiniDialogFontSmallLabelSize   30.0f

#define kProgressBar4BuildingInMapSize      cocos2d::CCSize(264.0f, 57.0f)
#define kProgressBar4BuildingInMapOffset    cocos2d::CCPoint(11.0f, 13.0f)

#define kMapLabelSize                   40.0f

#define kBlackColour                ccc3( 51.0f, 51.0f, 51.0f)
#define kBlueColour                 ccc3( 55.0f,134.0f,196.0f)
#define kGreenColour                ccc3(  0.0f,166.0f, 81.0f)
#define kDarkGreenColour            ccc3(  0.0f, 68.0f,  0.0f)
#define kDarkGreyColour              ccc3( 76.0f,117.0f, 93.0f)

#define kTintLock cocos2d::ccc3(100, 100, 100)
#define kTintUnlock cocos2d::ccc3(255, 255, 255)

#define kTutorialRaceSrcDir                     "common/race_tutorial/"
#define kTutorialCharacterSrcDir                "common/character_tutorial/"
#define kTutorialEffectSrcDir                   "common/tutorial/"
#define kPetSrcPath                             "common/pet/"
#define kCharacterSrcDir                        "common/character/"
#define kDecorationSrcDir                       "common/decoration/"

#define ARC4RANDOM_MAX      0x100000000

#define HUD_MENU_TOUCH_PRIORITY 100

#define ANDROID_TOUCH_MOVE_THRESHOLD 10.0f

#define AppStoreLink "https://itunes.apple.com/au/app/hoof-it/id855124397"
#define AndroidStoreLink "market://details?id=ppl.cocos2dx.ranchrun"


//======================== FEATURE SETTING =============================
// true = ON, false = OFF
#define FEATURE_COINS_FROM_PET true
#define BUILD_HABITAT_WITH_GEMS true


//======================================================================

enum SKILL {
    SKILL_NONE = 0,
    SKILL_DASH,
    SKILL_TRAP,
    SKILL_STOMP,
    SKILL_SHOCKWAVE1,
    SKILL_SHOCKWAVE2,
    SKILL_COUNT_PLUS_ONE //needs to minus one to get actuall skill count
};

enum RESOURCE_TYPE {
    RESOURCE_TYPE_GEM,
    RESOURCE_TYPE_COIN,
    RESOURCE_TYPE_FOOD
};


//======================================================================


#define IS_DEBUG_BUILD 1
#if IS_DEBUG_BUILD //DEBUG ONLY FLAGS

#define DEBUG_ENABLE_PROFILERS 1

//#define DEBUG_ALL_ITEMS_NEEDED_TO_FUSE_AND_EVOLVE

//#define LOAD_CUSTOM_UUID "c7ddb0c1-97d8-4560-9b86-ee3219ccc2d4" //Keeper
//#define LOAD_CUSTOM_UUID "f8ff00c5-b565-4956-8991-969b0749884f" //Top
//#define LOAD_CUSTOM_UUID "c7ddb0c1-97d8-4560-9b86-ee3219ccc2d4" //Keeper
//#define LOAD_CUSTOM_UUID "f8ff00c5-b565-4956-8991-969b0749884f" //Top
//#define LOAD_CUSTOM_UUID "475a130b-5cde-4f8c-9943-8695e0dd49f2" //jakob
//#define LOAD_CUSTOM_UUID "6dbda23d-6801-4047-aaee-1e6621893e11" //Will
//#define LOAD_CUSTOM_UUID "56cff521-1ba4-462e-9298-3fab8716575a" //Wam: Local Server: Sound Test
//#define LOAD_CUSTOM_UUID "e74c3728-e805-47e9-9c17-6f76368a186f" //Wam: Balancing

//#define LOAD_CUSTOM_UUID "38c40b43-b719-4699-a98f-eded602ca9fc" //Wam: Live Server

//UUID to Load the Farm
//#define LOAD_CUSTOM_UUID "c12080c7-600a-4712-84f3-0855548e80b3" //Eddie
//#define LOAD_CUSTOM_UUID "edadf594-ed76-470d-9fb9-55005ef6e368" // Venca
//#define LOAD_CUSTOM_UUID "2bfb6aaf-a3fb-4081-9756-06465eb73086" // Jarda
//#define LOAD_CUSTOM_UUID "a21c890e-39fe-4473-a360-e0aa80b93e5b" //Eddie
//#define DEBUG_SELECT_UUID "96c88e12-f47b-4323-ac45-cd614b4a0544" //Eddie

//#define LOAD_CUSTOM_UUID "" //Eddie
//#define DEBUG_SELECT_UUID "a21c890e-39fe-4473-a360-e0aa80b93e5b" //Eddie
//#define LOAD_CUSTOM_UUID "dace04e6-2788-49a5-a19d-6060c943f7d2" //Vencent1

//#define DEBUG_SELECT_UUID "3fbd5639-028e-476b-8343-3ee02367a3fc" //Testa
//#define DEBUG_SELECT_UUID "edadf594-ed76-470d-9fb9-55005ef6e368" // Venca
//#define DEBUG_SELECT_UUID "2bfb6aaf-a3fb-4081-9756-06465eb73086" // Jarda
//#define DEBUG_SELECT_UUID "b3534f51-a34f-4b16-b830-7a5cc429a53c" // ((:-)
//#define DEBUG_SELECT_UUID "cf74fcca-66c6-47ae-8c47-4de10dcc5880"
//#define DEBUG_SELECT_UUID "c6e1626b-dd66-40cd-aa20-48e46d48abe6" //Anuj
//#define DEBUG_SELECT_UUID "ebb995f8-e104-44c6-8bdf-0583024e78db" //VincentGoh1988
//#define DEBUG_SELECT_UUID "050e6637-5dc7-4e19-9c39-2f997146e481"
//#define DEBUG_SELECT_UUID "8103baeb-2e9d-49ab-9ebf-766feead1338"

//Top-----
//#define LOAD_CUSTOM_UUID "f8ff00c5-b565-4956-8991-969b0749884f" //Load Nami
//#define LOAD_CUSTOM_UUID "62af5e4f-a5e2-4f1c-9758-663ae0fbdfc2" //Load Top
//#define DEBUG_SELECT_UUID "f8ff00c5-b565-4956-8991-969b0749884f" //Race Nami
//#define LOAD_CUSTOM_UUID "99662327-b14d-4082-8f36-1e3bd5f85b41" //Dee
//#define LOAD_CUSTOM_UUID "c7ddb0c1-97d8-4560-9b86-ee3219ccc2d4" //Keeper
//#define DEBUG_SELECT_UUID "c7a97c65-f37d-4603-8cbf-0865119122e1" //Stamford Ranch III


#define DEBUG_RACE_PLAYERS_EVEN_IF_ONLINE


//#define DEBUG_SELECT_UUID_FOR_VISIT "f287d08a-727d-4cdd-a8ee-9c7e0fed2d34"
//#define DEBUG_SKIP_ALL_TUTS
//#define DEBUG_SKIP_ASK_NAME
//#define DEBUG_SKIP_FIRST_RACE
//#define DEBUG_SKIP_SECOND_RACE
//#define DEBUG_SKIP_GACHA //SKIP_FIRST_RACE doesn't work without SKIP_GACHA

//#define DEBUG_AI
//#define DEBUG_USE_HELPER_FOREVER
//#define DEBUG_HELPER_USE_SKILL_FOREVER
//#define DEBUG_RUN_FOREVER 1//100000 //set number of laps
//#define DEBUG_TEST_STOMP_AREA
//#define DEBUG_TEST_STOMP_AREA2
//#define DEBUG_TEST_SHOCKWAVE_STRAIGHT
//#define DEBUG_INFINITEENERGY 1000000
//#define DEBUG_ADDITIONALENERGY  30
//#define DEBUG_LOADING

//#define DEBUG_WORKING_ON_TUTFIRSTRACE

//#define DEBUG_TRACK

#define DEBUG_ANIMAL_TRADING_ON_VISIT
//#define DEBUG_ANIMAL_TRADING__OPTION_DISABLE_TRADE



//#define EDIT_MODE
#ifdef EDIT_MODE
#define DECORATION_MOVABLE true
#else
#define DECORATION_MOVABLE false
#endif

#endif //#endif for #if IS_DEBUG_BUILD //DEBUG ONLY FLAGS

//======================================================================

#if IS_DEBUG_BUILD //DEBUG VERSION

#define kFacebookAppID      @"617873771639723"//@"679256135501486"
#define kFacebookAppIDChar  "617873771639723"//"679256135501486"
//
#define FBAppNameSpace      @"ranchrun"//@"hoofit-test-a"
#define FBAppNameSpaceChar  "ranchrun"//"hoofit-test-a"

#else //RELEASE VERSION && ADHOC VERSION

#define kFacebookAppID      @"617873771639723"
#define kFacebookAppIDChar  "617873771639723"

#define FBAppNameSpace      @"ranchrun"
#define FBAppNameSpaceChar  "ranchrun"

#endif //#endif for #if IS_DEBUG_BUILD //DEBUG VERSION

//======================================================================

#if IS_RELEASE_BUILD

#define CCLog_(format, ...)      do {} while (0)
#define NSLog_(format, ...)      do {} while (0)

#else

#define CCLog_(format, ...)      cocos2d::CCLog(format, ##__VA_ARGS__)
#define NSLog_(format, ...)      NSLog(format, ##__VA_ARGS__)

#endif

//======================================================================

#define IS_STRING_EQUAL(STRING1, STRING2) (strcmp(STRING1, STRING2) == 0)

#define DEBUG_FLOAT(NAME) CCLog_("float:%.2f",NAME);
#define DEBUG_XY(NAME) CCLog_("x:%.2f, y:%.2f",NAME->x, NAME->y);
#define DEBUG_POINT_XY(NAME) CCLog_("x:%.2f, y:%.2f",NAME.x, NAME.y);
#define DEBUG_POINT_WH(NAME) CCLog_("width:%.2f, height:%.2f",NAME.width, NAME.width);
#define DEBUG_RECT(NAME) CCLog_("origin.x:%.2f, origin.y:%.2f, size.width:%.2f, size.height:%.2f",NAME.origin.x, NAME.origin.y, NAME.size.width, NAME.size.height);

#define TITLE_OFFSET 80.0f
#define SETPOS_TITLE_OFFSET_RIGHT(__title__) setPosition(ccp(__title__->getPositionX()+(__title__->getContentSize().width*0.5f + TITLE_OFFSET*sizeMultiplier), __title__->getPositionY()))
#define SETPOS_TITLE_OFFSET_LEFT(__title__) setPosition(ccp(__title__->getPositionX()-(__title__->getContentSize().width*0.5f + TITLE_OFFSET*sizeMultiplier), __title__->getPositionY()))

#endif
