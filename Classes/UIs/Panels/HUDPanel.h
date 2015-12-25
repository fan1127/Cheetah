//
//  HUDPanel.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#ifndef __Cheetah__HUDPanel__
#define __Cheetah__HUDPanel__

#include "cocos2d.h"

#include "../../Extensions/CCPhatButton.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCLoadSpriteURL.h"

#include "../../UIs/EXPProgress.h"
#include "../../UIs/GachaPanel.h"
#include "../../UIs/Dialogs/AchievementDialog.h"
#include "../../UIs/Elements/ResourceProgressButton.h"
#include "../../UIs/Elements/HUDMenuButton.h"
#include "../../UIs/Elements/WinStreak.h"

#define HUD_TRACKPOINT_STR_SIZE 59
#define HUD_OVERRIDE_EXPLORE_BUTTON_BADGE_NUMBER_DISABLE -1

#define kHudPanelBadgeYPos      210.0f

#define kExtraAvatarFrameScale 0.035f

//#define kHudPanelTrackEditWithIcon

class GachaButton;

enum HUDPanelMenu {
    HUDPanelMenuShop,
    HUDPanelMenuGacha,
    HUDPanelMenuAchievement,
    HUDPanelMenuFriends,
    HUDPanelMenuInbox,
    HUDPanelMenuMap,
    HUDPanelMenuHome,
    HUDPanelMenuTotal
};

enum HUDPanelBar {
    HUDPanelBarGems,
    HUDPanelBarFoods,
    HUDPanelBarCoins,
    HUDPanelBarTotal
};

enum HUDButtonDefaultPosition {
    POS_FIXED_DEFAULT,
    POS_BOTTOM_ARRANGE_FROM_LEFT,
    POS_BOTTOM_ARRANGE_FROM_RIGHT
};

class HUDPanel: public cocos2d::CCLayer, public GachaPanelDelegate
{
    bool                    m_dangerButtonsEnabled;
    bool                    m_didPressRaceBtn;

    // Name, Level, Exp
    EXPProgress             *m_expProgress;
    cocos2d::CCLabelBMFont  *m_nameLabel;
    
    // Fan
    cocos2d::CCSprite       *m_fanSprite;
    CCLabelBMFont           *m_fanLabel;
    cocos2d::CCMenu         *m_fanMenu;
    
    // Track
    CCLabelBMFont           *m_trackLabel;
    cocos2d::CCMenu         *m_trackMenu;
    
    // Setting
    cocos2d::CCMenu         *m_settingMenu;
    
    // Main Menu
    cocos2d::CCLayer        *m_mainMenuLayer;
    cocos2d::CCArray        *m_menuItemArray;
    
    // Callback
    void                    *m_close_levelup_effect_sender;
    void                    (*m_close_levelup_effect_callback)(void *sender_);
    
    bool                    m_enableTrackSignBeating;

    cocos2d::CCSprite       *m_signalSignSprite;
    
protected:

    cocos2d::CCSprite*      m_trackStatusSign;
    cocos2d::CCSprite*      m_trackStatusSignIcon;

    static const int        MAX_FIREWORKS = 6;

    struct Firework : public CCObject
    {
        CCParticleSystemQuad*   m_particle;
        //CCSkeletonAnimation*    m_spine;
        HUDPanel*               m_hudPanel;

        void init(HUDPanel* hudPanel);
        void shoot();
        void move();
        void hide();
        void animate();
        static cocos2d::CCPoint randomPosOnScreen();
    };
    friend class Firework;

    Firework                m_levelUpEffectFireworks[MAX_FIREWORKS];

    CCPhatTouchableSprite*  m_levelUpEffectBackground;
    CCSkeletonAnimation*    m_levelUpEffect;
    CCMenu*                 m_levelUpEffectDoneButton;
    CCLabelBMFont*          m_levelUpEffectFont;
    CCLabelBMFont*          m_levelUpEffectBannerFont;

    static bool canShowLevelUpEffect();
    void showButtonInLevelUpEffect();
    void setLevelUpEffectToIdle();
    void shootFirework();
    static void doNothing(void *inst_, void *sender_){}

    bool                    m_showVersionNumber;
    CCLabelBMFont*          m_versionNumber;
    void createVersionLabel(cocos2d::CCLayer* parent);

    int getMargin() const;


public:
    
    cocos2d::CCLayer        *statusLayer;
    cocos2d::CCLayer        *menuPlaceHolderLayer;
    
    AchievementDialog*      achievementDialog;
    
    // Gem, Food, Fan
    ResourceProgressButton  *resourceBars[HUDPanelBarTotal];
    
    cocos2d::CCMenu         *mainMenu;
    
    HUDMenuButton           *menuButtons[HUDPanelMenuTotal];
    
    // Home User
    cocos2d::CCSprite         *photoSprite;
    
    // Streak
    WinStreak               *streakButton;
    

    static CCMenu* createButtonHelper(const char* text, CCObject* target, SEL_MenuHandler selector);
    static CCPhatTouchableSprite* createBlackBackgroundHelper(GLubyte opacity);

    static HUDPanel *create();
    
    bool init();
    
    virtual void update(float delta);
    virtual void onExit();
    
    void updateHUD();
    void updateHUD(bool update_button_);
    void updateHUDTrack(bool track_complete_);
    void updateHUDTrack(bool track_complete_, bool update_sign_);
    
    cocos2d::CCMenu* getSettingMenu() { return m_settingMenu; }

    static void updateHUDLevel(void *inst_, void* sender_);
    void displayLevelUpEffect();
    void onLevelUpEffectDoneButton();
    void hideLevelUpEffect();
    bool isLevelUpEffectShowed();
    void setLevelUpEffectDismissCallback(void *sender_, void (*callback_)(void*));
    void clearLevelUpEffectDismissCallback();
    int getMaxHUDZOrder() const;

    void showIncompleteTrackSign();
    void setEnableTrackSignBeating(bool enable_);
    void trackSignBeating();
    void changeTrackSignToComplete();
    void hideAndReleaseTrackSign(cocos2d::CCObject* sender_);
    void checkTrade();
    
    void updateBadgeAchievement();
    int getOverrideExploreButtonBadgeNumber();
    void setOverrideExploreButtonBadgeNumber(int value_);

    void updateBadgeInbox();
    
    void openIAPStore(RESOURCE_TYPE resourceType);
    void clearUIDialog();
    
    ////////////////////////////////////////////////////////
    // Main Menu Function
    #pragma mark - Main Menu Buttons Functions
    bool addExpBar();
    bool addHomeUserInfo();
    bool addStreakInfo();
    bool addResourceBar(HUDPanelBar resourceBar_, ResourceProgressButton *refResourceBar);
    bool addTrackEditBar();
    bool addTrackEditBar(ResourceProgressButton *refResourceBar);
    
    void setSignalSignDisplayed(bool displayed);
    
    void setVisibilityAll(bool visible_);
    void setVisibilityResourceBar(bool visible_);
    void setVisibilityExpBarAndName(bool visible_);
    void setVisibilityFans(bool visible_);
    void setVisibilityTrackEditBar(bool visible_);
    void setVisibilitySettingButton(bool visible_);
    void setVisibilityMainMenuButton(HUDPanelMenu menuButton_,bool visible_);
    void setVisibilityAllMainMenuButtons(bool visible_);
    
    bool addSettingButton();
    bool addMainMenuButton(HUDPanelMenu menuButton_,
                           HUDMenuButton *refMenuButton,
                           HUDButtonDefaultPosition defaultPosition_);

    void buildMainMenuButtons();
    int getIndexOfMainMenuItemByTag(int tag_);
    void showAllMainMenuButtons(bool show_);
    bool areAllMainMenuButtonsShowed();
    void setDangerButtonsEnabled(bool enabled_);
    bool getDangerButtonsEnabled() { return m_dangerButtonsEnabled; }
    void updateResourceBar(HUDPanelBar bar_);
    void updateAllResourceBars();
    
    ////////////////////////////////////////////////////////
    // Button Callback
    #pragma mark - Button callback functions
    void onExpBarPressed(cocos2d::CCNode *sender);
    void onExpBarFanPressed(cocos2d::CCNode *sender);
    void onGemsBarPressed(cocos2d::CCNode *sender_);
    void onAchievementPressed(cocos2d::CCNode *sender);
    void onFoodsBarPressed(cocos2d::CCNode *sender_);
    void onCoinsBarPressed(cocos2d::CCNode *sender_);
    void onTrackEditPressed(cocos2d::CCObject* sender_);
    void onSettingPressed(cocos2d::CCNode *sender_);
    void onShopButtonPressed(cocos2d::CCNode *sender_);
    void onGachaButtonPressed(cocos2d::CCNode *sender_);
    void onFriendsButtonPressed(cocos2d::CCNode *sender_);
    void onInboxButtonPressed(cocos2d::CCNode *sender_);
    void onMapButtonPressed(cocos2d::CCNode *sender_);
    void onHomeButtonPressed(cocos2d::CCNode *sender_);
    
    ////////////////////////////////////////////////////////
    // callback method
    ////////////////////////////////////////////////////////
    virtual void gachaPanelDidClosing(GachaPanel *gachaPanel);
    static void onMapDataRetrieved(void* sender, std::string resp_json_str);
    
    static void onBackendServiceActive(void* sender);
    
};


#endif /* defined(__Cheetah__HUDPanel__) */
