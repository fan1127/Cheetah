//
//  RacePanel.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/23/56 BE.
//
//

#ifndef __Cheetah__RacePanel__
#define __Cheetah__RacePanel__

#include "cocos2d.h"

#define RACEPANEL_MENU_MOVING_SPEED     0.5f
#define RACEPANEL_MENU_EASING_SPEED     0.5f

#define RACEPANEL_LBL_ROW_TOP_1         screenSize.height-(47.0f*sizeMultiplier)
#define RACEPANEL_LBL_ROW_TOP_2         screenSize.height-(110.0f*sizeMultiplier)
#define RACEPANEL_LBL_ROW_TOP_3         screenSize.height-(185.0f*sizeMultiplier)
#define RACEPANEL_LBL_COL_CENTER        screenSize.width*0.5f
#define RACEPANEL_LBL_COL_CENTER_ROW_1  screenSize.height-(77.0f*sizeMultiplier)
#define RACEPANEL_LBL_COL_CENTER_ROW_2  screenSize.height-(182.0f*sizeMultiplier)
#define RACEPANEL_LBL_COL_TOP_1         screenSize.width-(420.0f*sizeMultiplier)
#define RACEPANEL_LBL_COL_TOP_2         screenSize.width-(300.0f*sizeMultiplier)
#define RACEPANEL_LBL_COL_TOP_3         screenSize.width-(126.0f*sizeMultiplier)
#define RACEPANEL_LBL_ROW_TOP_1_FONTSIZE 45.0f
#define RACEPANEL_LBL_ROW_TOP_2_FONTSIZE 55.0f
#define RACEPANEL_LBL_FONTSIZE_BIG 100.0f

#define SPD_PGB_CLR_R_1 196.0f
#define SPD_PGB_CLR_G_1 78.0f
#define SPD_PGB_CLR_B_1 78.0f
#define SPD_PGB_CLR_R_2 230.0f
#define SPD_PGB_CLR_G_2 204.0f
#define SPD_PGB_CLR_B_2 50.0f
#define SPD_PGB_CLR_R_3 111.0f
#define SPD_PGB_CLR_G_3 222.0f
#define SPD_PGB_CLR_B_3 70.0f
#define SPD_PGB_SIZE_0      14.0f
#define SPD_PGB_SIZE_100    468.0f
#define SPD_PGB_SIZE_FORMAT(__percentDecimal__)  CCSizeMake((SPD_PGB_SIZE_0+(__percentDecimal__*(SPD_PGB_SIZE_100-SPD_PGB_SIZE_0)))*sizeMultiplier, 52.0f*sizeMultiplier)
//468.0f-14.0f ~ 100%-0%
#define SPD_PGB_GREEN_AT_ABOVE_PERCENT 0.7f //70%

#define OLDER_COST_INIT_VALUE -100

#define DEFAULT_GROUP_ENERGY_POSITION (\
ccp(this->getContentSize().width-marginWidth-m_bgEnergy->getContentSize().width*0.6f,\
600.0f * sizeMultiplier\
+m_skillCostLabel->getContentSize().height*0.5f))
//lastButton->getPositionY()+lastButton->getContentSize().height*0.5f

#define POWERUPS_COUNT 5
#define ENERGY_COLOR_RED ccc3(245, 90, 32)
#define ENERGY_COLOR_NORMAL ccc3(255, 255, 255)

#define POS_X_btnGetHelp (200.0f*sizeMultiplier)
#define POS_Y_btnGetHelp (134.0f*sizeMultiplier)


#include "../Extensions/CCPhatButton.h"
#include "../Extensions/CCLabelHelper.h"
#include "../Extensions/CCLoadSpriteURL.h"
#include "../UIs/GaugePanel.h"
#include "../UIs/Rank.h"

#include "../Settings.h"
#include "../VOs/PlayerHelperPetVO.h"
#include "../Extensions/CCPhatTouchableSprite.h"

#define kRacePanelButtonMax (SKILL_COUNT_PLUS_ONE-1)

#define kHelperPetSpineOffsetToHelperButton ccp(-83.0f*sizeMultiplier,-48.0f*sizeMultiplier)

#define ZORDER_BELOW_POWERUPBUTTONS 999
#define ZORDER_POWERUPBUTTONS 1000
#define ZORDER_ABOVE_POWERUPBUTTONS 1001

enum RacePanelState {
    RPS_STANDBY,
    RPS_COUNTDOWN,
    RPS_LAUNCH,
    RPS_RUNNING,
};

typedef enum {
    BallEffectTypeFromPlayer,
    BallEffectTypeFromOpponent
}BallEffectType;

class CCNikButton;

class RacePanel: public cocos2d::CCLayer {
    
    cocos2d::CCSprite*  m_uiTraffic;
    cocos2d::CCPoint    m_uiTrafficShowPos;
    cocos2d::CCPoint    m_uiTrafficHidePos;
    bool                m_uiTrafficShow;
    
    cocos2d::CCSprite*  m_lightRed;
    cocos2d::CCSprite*  m_lightYellow;
    cocos2d::CCSprite*  m_lightGreen;
    
    bool                m_btnGoShow;
    
    CCPhatButton*       m_btnExit;
    
    ///////////////////////////////////////////////////////
    // Start running logic properties
    RacePanelState      m_racePanelState;
    int                 m_countDown;
    int                 m_randCount;
    
    int                 m_olderCost; //for check if cost is updated
    int x;
    int y;
    
    CCLabelBMFont       *m_skillCostLabel;
    CCSprite            *_energyIcon;
    bool                canTouch;
    
    int                 m_currentLap;

    CCMenu*     m_btnReplay1x;
    CCMenu*     m_btnReplay2x;
    //CCMenu*     m_btnReplay4x;
    CCMenu*     m_btnReplayPlay;
    CCMenu*     m_btnReplayPause;
    CCMenu*     m_btnReplayAgain;
    CCMenu*     m_btnGoHome;
    CCPhatTouchableSprite* m_blackBackground;

    CCLabelBMFont* m_replayLabel;
    CCLabelBMFont* m_replayTimerLabel;
    CCLabelBMFont* m_replayTimer;

    void onReplayPlayButtonPressed(CCObject *sender);
    void onReplayPauseButtonPressed(CCObject *sender);
    void onReplaySetSpeed1XButtonPressed(CCObject *sender);
    void onReplaySetSpeed2XButtonPressed(CCObject *sender);
    //void onReplaySetSpeed4XButtonPressed(CCObject *sender);
    void onReplayAgainButtonPressed(CCObject *sender);
    void onGoHomeButtonPressed(CCObject *sender);
    void setReplayPlaybackSpeed(float speed);
    CCMenu* createReplayButton(const char* spriteName, float buttonScale, SEL_MenuHandler selector);

    bool                m_forceVeryEasyGauge;
    bool                m_forceHideGiveUpButton;
    bool                m_forceHideGetHelpButton;
    bool                m_disableTouchDownGaugeFunction;

    bool                m_showVersionNumber;
    CCLabelBMFont*      m_versionNumber;
    void createVersionLabel(cocos2d::CCLayer* parent);

public:
    
    SKILL               m_playerUsableSkills[kRacePanelButtonMax];
    
    CCLabelBMFont       *m_tutorialLabel;
    
    CCNikButton*        m_btnGo;
    CCNikButton*        m_btnGiveUp;
    CCNikButton*        m_btnGetHelp;
    cocos2d::extension::CCSkeletonAnimation *m_helperPetSpine = NULL;
    CCSprite*           m_helperPhotoSprite;
    
    bool                m_btnGetHelpIsHiding;
    
    GaugePanel          *gaugePanel                        = NULL;
    
    Rank                *rankPopup;
    
    CCNode*             m_groupPowerupButtons;
        
    CCLabelBMFont*      m_lblLapTitle;
    CCLabelBMFont*      m_lblLap;
    CCLabelBMFont*      m_lblTimerTitle;
    CCLabelBMFont*      m_lblTimer;
    CCLabelBMFont*      m_lblTimerEffect;
    CCLabelBMFont*      m_lblPosTitle;
    CCLabelBMFont*      m_lblPos;
    CCNode*             m_groupTimeAdded;
    CCLabelBMFont*      m_lblTimeAdded;
    CCScale9Sprite*     m_bgTimeAdded;
    CCNode*             m_groupEnergy;
    CCScale9Sprite*     m_bgEnergy;
    CCSprite*           m_signSprite;
    CCSprite*           m_signBgSprite;
    
    CCLabelBMFont*      m_lblSpeed;
    CCNode*             m_groupSpeed;
    CCSprite*           m_speedCircle;
    CCSprite*           m_speedHand;
    
    CCNode *            m_hintGroup;
    
    CCSprite*           m_bg;
//    CCNikButton*        m_bg;
    
//    cocos2d::extension::CCScale9Sprite*     m_speedProgressBar;
    
//    CCSprite                            *cursorSprite;
    
    virtual bool init();
    virtual void update(float delta);
    CREATE_FUNC(RacePanel);
    
    ~RacePanel();

    void initReplayUI();
    void showReplayAgainButton();
    void setReplayTime(float time);

    void updateSpeedProgressBar(float percentDecimal);
    
    void addSpotlightBG();
    void addSpotlightBG(int zOrder);
    void touchSpotlightBG();
    void removeSpotlightBG();
    
    void createGroupEnergy();
    void showGroupEnergy(bool animated_);
    void hideGroupEnergy(bool animated_);
    void setAllPowerupButtonsCostVisible(bool costVisible);
    void forceEnabledPowerupButton(SKILL skill, bool forceEnable);
    void turnOffAllForceEnabledPowerupButtons();
    void showPowerUpButtons(SKILL *skills_, bool animated_);
    void hidePowerUpButtons(bool animated);
    
//    void resetAllSkillsCost();
    void checkSkillCost(int newCost);
    
    void noEnergyUIFeedback();
    void resetEnergyColor();
    
    void addGlowEffect(CCNode *node);
    void addGlowEffectWithDelay(CCNode *node, float delay);
    
    void setCost(CCString *costString);
    void updateCostOnButton(CCObject *button, int cost);
    void useDash(CCObject *sender);
    void useTrap(CCObject *sender);
    void useStomp(CCObject *sender);
    void useShockwaveStraight1(CCObject *sender);
    void useShockwaveStraight2(CCObject *sender);
    
    void stopAnimatedCursor();
    
    void showBtn(float delay);
    void hideBtn(float delay);
    
    void showTrafficLight(float delay);
    void hideTrafficLight(float delay);
    
    void showGauge();
    void animateCursor();
    void showGauge(float delay);
    void hideGauge(float delay);
    
    void showHUD();
    void hideHUD();
    
    void showFinalLap();
    void showTimeUp();
    void showSign(const char* signPng, float animationTime);
    
//    bool hintIsActive()
//    {
//        if (m_hintGroup)
//            return m_hintGroup->isVisible();
//        else
//            return false;
//    };
    void showHintCurrentEnergy();
    void removeAllHints();
    
    void beginCountdown();
    
    void setLap(int lap, int all);
    
    void touchDownGauge(CCObject *sender);
    
    void respondToGoButton(GaugeRESULT result);
    
    void onGiveUp(CCObject *sender);
    void onInstantWin(CCObject *sender);
    
    bool initGetHelp();
    void showGetHelp();
    void hideGetHelp(bool alsoHidePetSpine);
    void onGetHelp(CCObject *sender);
    
    static void beginCountdown(void *inst, void* sender);
    static void startRunning(void *inst, void* sender);
    static void exitThisScene(void *inst, void* sender);
    
    void animateParticleStarWithDelay(CCNode *onNode, float delay);
    void animateParticleStar(CCNode *onNode);
    void animateParticleDecreaseCostWithDelay(CCNode *onNode, float delay);
    void animateParticleDecreaseCost(CCNode *onNode);
    
    void animateBallEffect(BallEffectType type, CCNode *toNode, float duration);
    
    int getCurrentLap() { return m_currentLap; }
    
    void forceVeryEasyGauge(bool force_);
    void forceHideGiveUpButton(bool force_);
    void forceHideGetHelpButton(bool force_);
    void disableTouchDownGaugeFunction(bool disable_);
    
    CCPoint getBtnGoShowPos();
    CCPoint getBtnGoHidePos();
    CCPoint getGaugeShowPos();
    CCPoint getGaugeHidePos();
    
#pragma mark - Callback functions
    
    void* onUseSkillSender;
    void (*onUseSkill)(void* sender_, SKILL skillUsed);
    
};

#endif /* defined(__Cheetah__RacePanel__) */
