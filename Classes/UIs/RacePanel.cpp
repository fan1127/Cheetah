//
//  RacePanel.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/23/56 BE.
//
//

#include "RacePanel.h"
USING_NS_CC;
using namespace std;



#include "../UIs/PowerupButton.h"

#include "../Extensions/CCNikButton.h"

#include "../Extensions/ExplosionParticle.h"
#include "../Extensions/ProjectileParticle.h"

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../Services/GameAudioService.h"

extern GameAudioService* g_gameAudio;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

//#include "../Entities/TutorialManager.h"
//extern TutorialManager *g_gameTutorial;

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;
#include "../AppMacros.h"

#include "../Helpers/CCStringHelper.h"

extern TargetDevice targetDevice;

#include "../Scenes/SceneRace.h"
extern CCScene* g_currentScene;

extern float sizeMultiplier;
extern float retinaMultiplier;
extern float isIPhoneMultiplier;
extern float isIPadMultiplier;
extern float isIOSMultiplier;

#include "../Helpers/FlurryCpp.h"
#include "../Extensions/CCMenuHelper.h"
#include "../Helpers/Versioning.h"
#include "../Helpers/CCSpriteHelper.h"

#if !(IS_RELEASE_BUILD)
#include "../version.h"
#endif

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void RacePanel::createVersionLabel(cocos2d::CCLayer* parent)
{
    if(!m_showVersionNumber)
        return;
#if !(IS_RELEASE_BUILD)
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    m_versionNumber = CCLabelHelper::createBMFont(retinaMultiplier, Versioning::GetBundleVersionWithGit().c_str(), kFontSizeLarge, true);
    m_versionNumber->setAnchorPoint(ccp(0.5f, 0.0f));
    m_versionNumber->setPosition(ccp(screenSize.width*0.5f, 0.f));
    parent->addChild(m_versionNumber);
#endif
}

bool RacePanel::init()
{
    if (!CCLayer::init())
    {
        return false;
    }

    m_showVersionNumber = true;
    createVersionLabel(this);

    m_signSprite = NULL;
    
    m_forceVeryEasyGauge = false;
    m_forceHideGiveUpButton = false;
    m_forceHideGetHelpButton = false;
    m_disableTouchDownGaugeFunction = false;
    
    m_olderCost = OLDER_COST_INIT_VALUE;
    
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    
    m_btnGo = CCNikButton::create(UIHelper::getSprite(UIHelper::RaceElementsGoButton), NULL);
    m_btnGo->setTarget(this, menu_selector(RacePanel::touchDownGauge), CCNikButtonEventTouchDown);
    m_btnGo->setAnchorPoint(ccp(0.5f, 0.5f));
    m_btnGo->setPosition(getBtnGoHidePos());
    this->hideBtn(0);
    this->addChild(m_btnGo);
    
    m_tutorialLabel = CCLabelHelper::createBMFont("", kFontSizeLarge, true);
    m_tutorialLabel->setPosition(ccp(screenSize.width * 0.5f, 400.0f * sizeMultiplier));
    m_tutorialLabel->setVisible(false);
    this->addChild(m_tutorialLabel, 1001);
    
    CCLabelBMFont *goLabel = CCLabelHelper::createBMFont("Go!", 100, true);
    goLabel->setPosition(ccp(m_btnGo->getContentSize().width*0.5,m_btnGo->getContentSize().height*0.56f));
    m_btnGo->addChild(goLabel);

    m_btnGiveUp = CCNikButton::create(UIHelper::getSprite(UIHelper::RaceElementsGiveUpButton1), NULL);
    CCLabelBMFont *giveUpLabel = CCLabelHelper::createBMFont("Give Up", kFontSizeSmall+5.0f, true);
    giveUpLabel->setPosition(ccp(m_btnGiveUp->getPositionX() + m_btnGiveUp->getContentSize().width*0.46f, m_btnGiveUp->getContentSize().height*0.5f));
    giveUpLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    giveUpLabel->setAlignment(kCCTextAlignmentRight);
    m_btnGiveUp->addChild(giveUpLabel);
    m_btnGiveUp->setTarget(this, menu_selector(RacePanel::onGiveUp), CCNikButtonEventTouchUpInside);
    m_btnGiveUp->setPosition(ccp(m_btnGiveUp->getContentSize().width * 0.5f
                                 + 32.0f * sizeMultiplier,
                                 screenSize.height-(25.0f * sizeMultiplier)-(m_btnGiveUp->getContentSize().height*0.5f)));
    m_btnGiveUp->setAnchorPoint(ccp(0.5f, 0.5f));
    m_btnGiveUp->setVisible(false);
    this->addChild(m_btnGiveUp);
    
#if !(IS_RELEASE_BUILD)
#if SELECT_SERVER == DEV_SERVER
    CCNikButton *instantWin = CCNikButton::create(UIHelper::getSprite(UIHelper::RaceElementsGiveUpButton1), NULL);
    CCLabelBMFont *instantWinLabel = CCLabelHelper::createBMFont("Instant Win", kFontSizeVerySmall, true);
    instantWinLabel->setPosition(ccp(instantWin->getContentSize().width*0.46f, instantWin->getContentSize().height*0.5f));
    instantWinLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    instantWin->addChild(instantWinLabel);
    instantWin->setTarget(this, menu_selector(RacePanel::onInstantWin), CCNikButtonEventTouchUpInside);
    instantWin->setPosition(ccp(m_btnGiveUp->getContentSize().width * 0.5f
                                + 32.0f * sizeMultiplier + instantWin->getContentSize().width,
                                screenSize.height-(25.0f * sizeMultiplier)-(m_btnGiveUp->getContentSize().height*0.5f)));
    this->addChild(instantWin);
#endif
#endif
    
    //POSITION
    m_lblPosTitle = CCLabelHelper::createBMFont("POS", RACEPANEL_LBL_ROW_TOP_1_FONTSIZE, true);
    m_lblPosTitle->setPosition(ccp(RACEPANEL_LBL_COL_TOP_1,
                                   RACEPANEL_LBL_ROW_TOP_1));
    m_lblPosTitle->setVisible(false);
    this->addChild(m_lblPosTitle);
    
    m_lblPos = CCLabelHelper::createBMFont("1ST", RACEPANEL_LBL_ROW_TOP_2_FONTSIZE, true);
    m_lblPos->setPosition(ccp(RACEPANEL_LBL_COL_TOP_1,
                              RACEPANEL_LBL_ROW_TOP_2));
    m_lblPos->setVisible(false);
    this->addChild(m_lblPos);
    
    //TIMER
    m_lblTimerTitle = CCLabelHelper::createBMFont(" ", RACEPANEL_LBL_ROW_TOP_1_FONTSIZE, true);
    m_lblTimerTitle->setPosition(ccp(RACEPANEL_LBL_COL_CENTER,
                                     RACEPANEL_LBL_COL_CENTER_ROW_1));
    m_lblTimerTitle->setVisible(false);
    this->addChild(m_lblTimerTitle);
    
    m_lblTimer = CCLabelHelper::createBMFont(" ", RACEPANEL_LBL_FONTSIZE_BIG, true);
    m_lblTimer->setPosition(ccp(RACEPANEL_LBL_COL_CENTER,
                                RACEPANEL_LBL_COL_CENTER_ROW_1));
    m_lblTimer->setVisible(false);
    this->addChild(m_lblTimer);
    
    m_lblTimerEffect = CCLabelHelper::createBMFont(" ", RACEPANEL_LBL_FONTSIZE_BIG, true);
    m_lblTimerEffect->setPosition(ccp(RACEPANEL_LBL_COL_CENTER,
                                      RACEPANEL_LBL_ROW_TOP_2));
    m_lblTimerEffect->setVisible(false);
    this->addChild(m_lblTimerEffect);
    
    //LAP
    m_lblLapTitle = CCLabelHelper::createBMFont("LAP", RACEPANEL_LBL_ROW_TOP_1_FONTSIZE, true);
    m_lblLapTitle->setPosition(ccp(RACEPANEL_LBL_COL_TOP_2,
                                   RACEPANEL_LBL_ROW_TOP_1));
    m_lblLapTitle->setVisible(false);
    this->addChild(m_lblLapTitle);
    
    m_lblLap = CCLabelHelper::createBMFont(" ", RACEPANEL_LBL_ROW_TOP_2_FONTSIZE, true);
    m_lblLap->setPosition(ccp(RACEPANEL_LBL_COL_TOP_2,
                              RACEPANEL_LBL_ROW_TOP_2));
    m_lblLap->setVisible(false);
    this->addChild(m_lblLap);
    
    //TIME ADDED
    m_groupTimeAdded = CCNode::create();
    m_groupTimeAdded->setPosition(ccp(RACEPANEL_LBL_COL_CENTER,
                                      RACEPANEL_LBL_COL_CENTER_ROW_2));
    m_groupTimeAdded->setVisible(false);
    this->addChild(m_groupTimeAdded);
    
    m_bgTimeAdded = UIHelper::getScalableSprite(UIHelper::AchievementBG);
    m_bgTimeAdded->setPosition(ccp(110.0f*sizeMultiplier,
                                   0.0f));
    m_bgTimeAdded->setAnchorPoint(ccp(1.0f,0.5f));
    m_bgTimeAdded->setOpacity(153);
    m_groupTimeAdded->addChild(m_bgTimeAdded);
    
    m_lblTimeAdded = CCLabelHelper::createBMFont(" ", RACEPANEL_LBL_ROW_TOP_2_FONTSIZE, true);
    m_lblTimeAdded->setPosition(ccp(98.0f*sizeMultiplier,
                                   0.0f));
    m_lblTimeAdded->setAnchorPoint(ccp(1.0f,0.5f));
    m_groupTimeAdded->addChild(m_lblTimeAdded);
    
    
    //SPEED
    m_groupSpeed = CCNode::create();
    m_groupSpeed->setPosition(ccp(screenSize.width - (123.0f * sizeMultiplier),
                                  screenSize.height - (125.0f * sizeMultiplier)));
    m_groupSpeed->setAnchorPoint(ccp(0.0f, 0.0f));
    this->addChild(m_groupSpeed);
    m_groupSpeed->setVisible(false);
    
    m_speedCircle = UIHelper::getSprite(UIHelper::RaceElementsSpeedometer);
    m_speedCircle->setPosition(CCPointZero);
    m_speedCircle->setAnchorPoint(ccp(0.5f, 0.3f));
    m_groupSpeed->addChild(m_speedCircle);
    
    m_speedHand = UIHelper::getSprite(UIHelper::RaceElementsSpeedometerArrow);
    m_speedHand->setPosition(CCPointZero);
    m_speedHand->setAnchorPoint(ccp(0.5f, 0.5f));
    m_speedHand->setRotation(-90);
    m_groupSpeed->addChild(m_speedHand);
    
    CCLabelBMFont* speedTitle = CCLabelHelper::createBMFont(g_gameContent->Word["speed_hud_title"].c_str(), RACEPANEL_LBL_ROW_TOP_1_FONTSIZE, true);
    speedTitle->setPosition(ccp(-64.0f*sizeMultiplier,
                                -30.0f*sizeMultiplier));
    speedTitle->setAnchorPoint(ccp(0.5f, 0.5f));
    m_groupSpeed->addChild(speedTitle);
    
    m_lblSpeed = CCLabelHelper::createBMFont(" ", RACEPANEL_LBL_ROW_TOP_2_FONTSIZE, true);
    m_lblSpeed->setPosition(ccp(64.0f*sizeMultiplier,
                                -40.0f*sizeMultiplier));
    m_lblSpeed->setAnchorPoint(ccp(0.5f, 0.5f));
    m_groupSpeed->addChild(m_lblSpeed);

    ///////////////////////////////////////////////////////
    // Start running logic properties
    m_racePanelState = RPS_STANDBY;
    m_countDown = 3;
    
    rankPopup = Rank::create(this);
    rankPopup->retain();
    
    
    
    

    m_helperPetSpine = NULL;
    m_helperPhotoSprite = NULL;
    m_btnGetHelp = NULL;
    

    
    int i = 0;
    for (vector<string>::iterator it=g_gameContent->AbilitiesOrder.begin(); it!=g_gameContent->AbilitiesOrder.end(); ++it) {
        
        Ability* _ability = g_playerState->Abilities[*it];
        if (_ability && _ability->paVo.level > 0) {
            
            const char* abilityType = _ability->paVo.type.c_str();
            
            SKILL skill = SKILL_NONE;
            if (strcmp(abilityType, kAbilityDashForward) == 0) {
                skill = SKILL_DASH;
            } else if (strcmp(abilityType, kAbilityBananaTrap) == 0) {
                skill = SKILL_TRAP;
            } else if (strcmp(abilityType, kAbilityStomp) == 0) {
                skill = SKILL_STOMP;
            } else if (strcmp(abilityType, kAbilityShootForward) == 0) {
                skill = SKILL_SHOCKWAVE1;
            } else if (strcmp(abilityType, kAbilityDoubleShoot) == 0) {
                skill = SKILL_SHOCKWAVE2;
            }
            
            m_playerUsableSkills[i] = skill;
            i++;
        }
    }
    if (i<SKILL_COUNT_PLUS_ONE-1) {
        m_playerUsableSkills[i] = SKILL_NONE;
        i++;
    }
    
    createGroupEnergy();
    
    onUseSkillSender = NULL;
    onUseSkill = NULL;
    
    scheduleUpdate();
    
    return true;
    
}

void RacePanel::initReplayUI()
{
    m_blackBackground = HUDPanel::createBlackBackgroundHelper(128);
    m_blackBackground->setVisible(false);
    this->addChild(m_blackBackground, 100);
    
    float goHomeButtonScale = ((0.85f * isIPhoneMultiplier + 1.0f * isIPadMultiplier) * isIOSMultiplier);

    m_btnReplay1x = createReplayButton(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::ReplayHUD1x)), 1.0f, menu_selector(RacePanel::onReplaySetSpeed1XButtonPressed));
    m_btnReplay2x = createReplayButton(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::ReplayHUD2x)), 1.0f, menu_selector(RacePanel::onReplaySetSpeed2XButtonPressed));
    m_btnReplayPlay = createReplayButton(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::ReplayHUDPlaySmall)), 1.0f, menu_selector(RacePanel::onReplayPlayButtonPressed));
    m_btnReplayPause = createReplayButton(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::ReplayHUDPause)), 1.0f, menu_selector(RacePanel::onReplayPauseButtonPressed));
    m_btnGoHome = createReplayButton(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::HUDHomeButton)),
                                     goHomeButtonScale,
                                     menu_selector(RacePanel::onGoHomeButtonPressed));
    
    m_btnGoHome->setZOrder(1001);
    
    float margin = 10.0f*sizeMultiplier;

    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint speedButtonPos = ccp(m_btnReplay1x->getContentSize().width*0.5f
                                 + margin,
                                 m_btnReplay1x->getContentSize().height*0.5f
                                 + margin);
    CCPoint playButtonPos = ccp(m_btnReplay1x->getContentSize().width
                                + margin
                                + m_btnReplayPlay->getContentSize().width*0.5f
                                + margin,
                                m_btnReplayPlay->getContentSize().height*0.5f
                                + margin);
    CCPoint homeButtonPos = ccp(screenSize.width
                                - (m_btnGoHome->getContentSize().width*goHomeButtonScale*0.5f)
                                - margin,
                                (m_btnGoHome->getContentSize().height*goHomeButtonScale*0.5f)
                                + margin);

    m_btnReplay1x->setPosition(speedButtonPos);
    m_btnReplay2x->setPosition(speedButtonPos);
    m_btnReplayPlay->setPosition(playButtonPos);
    m_btnReplayPause->setPosition(playButtonPos);
    m_btnGoHome->setPosition(homeButtonPos);

    m_btnReplay1x->setVisible(false);
    m_btnReplay2x->setVisible(true);
    m_btnReplayPlay->setVisible(false);
    m_btnReplayPause->setVisible(true);
    m_btnGoHome->setVisible(true);


    CCSprite* playAgain = UIHelper::getSprite(UIHelper::UISprites(UIHelper::ReplayHUDPlayBig));
    CCLabelBMFont* label = CCLabelHelper::createBMFont("Play Again", kFontSizeLarge, true);
    CCSize btnSize = playAgain->getContentSize();
    label->setPosition(ccp(btnSize.width/2.f, -20.0f*sizeMultiplier));
    playAgain->addChild(label);

    m_btnReplayAgain = CCMenuHelper::createMenu(playAgain, this, menu_selector(RacePanel::onReplayAgainButtonPressed));
    m_btnReplayAgain->setVisible(false);
    this->addChild(m_btnReplayAgain, 1001);

    m_replayLabel = CCLabelHelper::createBMFont("Replay", kFontSizeLarge, true, kTextColorRed);
    m_replayLabel->setPosition(ccp(150.f * sizeMultiplier, screenSize.height - (120.f * sizeMultiplier)));
    this->addChild(m_replayLabel, 1001);

    m_replayTimerLabel = CCLabelHelper::createBMFont("Replay Ends In", kFontSizeLarge, true, kTextColorWhite);
    m_replayTimerLabel->setPosition(ccp(screenSize.width / 2.f, screenSize.height - (120.f * sizeMultiplier)));
    this->addChild(m_replayTimerLabel);

    m_replayTimer = CCLabelHelper::createBMFont("0m 0s", kFontSizeLarge, true, kTextColorWhite);
    m_replayTimer->setPosition(ccp(screenSize.width / 2.f, screenSize.height - (200.f * sizeMultiplier)));
    this->addChild(m_replayTimer);
}

void RacePanel::showReplayAgainButton()
{
    if(m_btnReplayAgain->isVisible())
        return;
    CCLog_("show replay again button");
    m_blackBackground->setVisible(true);
    m_btnReplayAgain->setVisible(true);
    m_btnReplay1x->setVisible(false);
    m_btnReplay2x->setVisible(false);
    m_btnReplayPlay->setVisible(false);
    m_btnReplayPause->setVisible(false);
}

void RacePanel::setReplayTime(float time)
{
    int totalSeconds = static_cast<int>(time);
    int seconds = totalSeconds % 60;
    int minutes = (totalSeconds / 60) % 60;
    if(seconds < 0)
        seconds = 0;
    if(minutes < 0)
        minutes = 0;
    CREATE_CHAR_BUFFER(timeStr, 256);
    snprintf(timeStr, timeStr_size, "%dm %ds", minutes, seconds);
    m_replayTimer->setString(timeStr);
}

CCMenu* RacePanel::createReplayButton(const char* spriteName, float buttonScale, SEL_MenuHandler selector)
{
    CCSprite* buttonSprite = CCSprite::createWithSpriteFrameName(spriteName);
    CCMenu* button = CCMenuHelper::createMenu(buttonSprite, this, selector);
    buttonSprite->setScale(buttonScale);
    if (buttonScale != 1.0f) {
        CCSize buttonFrameSize = button->getContentSize();
        CCSize spriteSize = buttonSprite->getContentSize()*buttonSprite->getScale();
        CCPoint offset = ccp((buttonFrameSize.width - spriteSize.width) * 0.5f,
                             (buttonFrameSize.height - spriteSize.height) * 0.5f);
        buttonSprite->setPosition(offset);
    }
    this->addChild(button);
    return button;
}

void RacePanel::onReplayPlayButtonPressed(CCObject *sender)
{
    CCLog_("Replay: Play");
    m_btnReplayPlay->setVisible(false);
    m_btnReplayPause->setVisible(true);
    if(m_btnReplay1x->isVisible())
        setReplayPlaybackSpeed(2.f);
    else if(m_btnReplay2x->isVisible())
        setReplayPlaybackSpeed(1.f);
    //else if(m_btnReplay4x->isVisible())
    //    setReplayPlaybackSpeed(4.f);
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
}
void RacePanel::onReplayPauseButtonPressed(CCObject *sender)
{
    CCLog_("Replay: Pause");
    m_btnReplayPause->setVisible(false);
    m_btnReplayPlay->setVisible(true);
    setReplayPlaybackSpeed(0.f);
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
}
void RacePanel::onReplaySetSpeed1XButtonPressed(CCObject *sender)
{
    CCLog_("Replay: 1x");
    m_btnReplay1x->setVisible(false);
    m_btnReplay2x->setVisible(true);
    setReplayPlaybackSpeed(1.f);
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
}
void RacePanel::onReplaySetSpeed2XButtonPressed(CCObject *sender)
{
    CCLog_("Replay: 2x");
    m_btnReplay2x->setVisible(false);
    m_btnReplay1x->setVisible(true);
    setReplayPlaybackSpeed(2.f);
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
}
//void RacePanel::onReplaySetSpeed4XButtonPressed(CCObject *sender)
//{
//    CCLog_("Replay: 4x");
//    m_btnReplay4x->setVisible(false);
//    m_btnReplay1x->setVisible(true);
//    setReplayPlaybackSpeed(4.f);
//}

void RacePanel::onReplayAgainButtonPressed(CCObject *sender)
{
    CCLog_("Replay: Again");
    SceneRace *_scene = (SceneRace*)g_currentScene;
    if(!_scene) return;
    
    g_gameAudio->stopMusic();
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_gameAudio->clearEffectFadeOut(3.0f);

    SceneList nextScene = SCENE_RACE;
    if (g_sceneManager->sceneSelector == SCENE_RACE) {
        nextScene = SCENE_RACE2;
    }
    _scene->fadeOutThisSceneAndChangeTo(nextScene);
}

void RacePanel::onGoHomeButtonPressed(CCObject *sender)
{
    CCLog_("Replay: GoHome");
    SceneRace* _scene = (SceneRace*)g_currentScene;
    if(!_scene) return;
    
    g_gameAudio->stopMusic();
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_gameAudio->clearEffectFadeOut(3.0f);
    _scene->fadeOutThisSceneAndChangeTo(SCENE_VILLE);
}

void RacePanel::setReplayPlaybackSpeed(float speed)
{
    SceneRace *_scene = (SceneRace*)g_currentScene;
    if (!_scene) return;
    
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
    CCAssert(_isoMapRacing, "isoMapRacing is null");
    CCAssert(_isoMapRacing->m_pet_player->isPlaybackReplayDataEnabled(), "not in replay mode");
    _isoMapRacing->setTimeScale(speed);
}

RacePanel::~RacePanel()
{
    if (rankPopup) {
        rankPopup->release();
        rankPopup = NULL;
    }
}

void RacePanel::updateSpeedProgressBar(float percentDecimal)
{
    m_speedHand->setRotation(-90.0f + (180.0f * percentDecimal));
}

void RacePanel::update(float delta) {
    
    static float t_timeDelta = 0;
    t_timeDelta += delta;
    
}

void RacePanel::showBtn(float delay) {
    
    m_btnGo->setPosition(getBtnGoShowPos());
    m_btnGo->setVisible(true);
    m_btnGo->setScale(1.0f);
    
}

void RacePanel::createGroupEnergy()
{
    float marginWidth = this->getContentSize().width*0.01f; //TODO: temp
    
    m_groupEnergy = CCNode::create();
    
    _energyIcon = UIHelper::getSprite(UIHelper::RaceElementsPowerIcon);
    
    //Note: needs to be "000" for setTwoNodesToMiddlePosition to work
    m_skillCostLabel = CCLabelHelper::createBMFont("000", kFontSizeExtraLarge, true,
                                                   ccc3(255, 255, 255),
                                                   kCCLabelAutomaticWidth,
                                                   kCCTextAlignmentRight);
    m_bgEnergy = UIHelper::getScalableSprite(UIHelper::AchievementBG);
    m_bgEnergy->setAnchorPoint(ccp(0.5f,0.5f));
    m_bgEnergy->setContentSize(ccp(m_bgEnergy->getContentSize().width*3.0f, m_bgEnergy->getContentSize().height*1.6f));
    m_bgEnergy->setOpacity(153);
    m_groupEnergy->addChild(m_bgEnergy);
    
    m_groupEnergy->setPosition(DEFAULT_GROUP_ENERGY_POSITION);
    m_groupEnergy->addChild(m_skillCostLabel,3);
    
    UIHelper::setTwoNodesToMiddlePosition(m_skillCostLabel, _energyIcon, 10.0f*sizeMultiplier, ccp(0, 0));
    
    m_groupEnergy->addChild(_energyIcon,2);
    m_groupEnergy->setVisible(false);
    this->addChild(m_groupEnergy);
}

void RacePanel::showGroupEnergy(bool animated_)
{
    if (m_groupPowerupButtons) {
        
        //starts from SKILL_DASH
        for (int i = SKILL_DASH; i < kRacePanelButtonMax; i++) {
            //takes any button
            
            CCNode *node = (CCNode*)m_groupPowerupButtons->getChildByTag(i);
            if (node) {
                
                m_groupEnergy->setPosition(ccp(m_groupEnergy->getPosition().x,
                                               node->getPosition().y
                                               + node->getContentSize().height*0.5f
                                               + m_bgEnergy->getContentSize().height*0.5f
                                               + 40.0f * sizeMultiplier //extra offset
                                               ));
                
                if (animated_) {
                    //TODO: add animations
                    
                    m_groupEnergy->setVisible(true);
                } else {
                    
                    m_groupEnergy->setVisible(true);
                }
                
                return;
            }
        }
    }
}


void RacePanel::hideGroupEnergy(bool animated_)
{
    m_groupEnergy->setVisible(false);
}

void RacePanel::addSpotlightBG() {
    
    addSpotlightBG(ZORDER_BELOW_POWERUPBUTTONS);
}

void RacePanel::addSpotlightBG(int zOrder) {

    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (!m_bg) {
        m_bg = CCSprite::createWithSpriteFrameName("oldversion/_bg_spotlight.png");
        m_bg->setScaleX((_screenSize.width+20.0f*sizeMultiplier)/m_bg->getContentSize().width);
        m_bg->setScaleY((_screenSize.height+20.0f*sizeMultiplier)/m_bg->getContentSize().height);
        m_bg->setContentSize(_screenSize);
        m_bg->setAnchorPoint(CCPointZero);
        m_bg->setPosition(ccp(-10.0f*sizeMultiplier, -10.0f*sizeMultiplier));
        addChild(m_bg, zOrder);
    }
}

void RacePanel::touchSpotlightBG()
{
    CCLog_("touchSpotlightBG");
}

void RacePanel::removeSpotlightBG()
{
    if (m_bg)
    {
        m_bg->removeFromParent();
        m_bg = NULL;
    }
}

void RacePanel::setAllPowerupButtonsCostVisible(bool costVisible)
{
    CCObject *object;
    CCARRAY_FOREACH(m_groupPowerupButtons->getChildren(), object)
    {
        PowerupButton *button = (PowerupButton *)object;
        button->setCostVisible(costVisible);
    }
}

void RacePanel::forceEnabledPowerupButton(SKILL skill, bool forceEnable)
{
    CCObject *object;
    CCARRAY_FOREACH(m_groupPowerupButtons->getChildren(), object)
    {
        PowerupButton *button = (PowerupButton *)object;
        if (button->skill() == skill) {
            button->forceEnabled(forceEnable);
        }
    }
}

void RacePanel::turnOffAllForceEnabledPowerupButtons()
{
    CCObject *object;
    CCARRAY_FOREACH(m_groupPowerupButtons->getChildren(), object)
    {
        PowerupButton *button = (PowerupButton *)object;
        button->setForceEnabledStatus(false);
    }
}

void RacePanel::showPowerUpButtons(SKILL *skills_, bool animated_)
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    
    float marginX = 20.0f * sizeMultiplier;
    
    SceneRace* _scene = (SceneRace*)g_currentScene;
    if (!_scene) return;
    
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;

    if (m_groupPowerupButtons == NULL)
    {
        m_groupPowerupButtons = CCNode::create();
        this->addChild(m_groupPowerupButtons, ZORDER_POWERUPBUTTONS);
    }
    
    m_groupPowerupButtons->removeAllChildrenWithCleanup(true);
    m_groupPowerupButtons->setVisible(true);
    
    int totalSkillsCount = kRacePanelButtonMax;
    int ii = 0;
    
//    float delayBeforeAnimation = 0.0f;
    
    int countAvalableSkills = 0;
    for (int i = 0; i < kRacePanelButtonMax; i++) {
        
        SKILL _skill = skills_[i];
        
        if (_skill == SKILL_DASH ||
            _skill == SKILL_TRAP ||
            _skill == SKILL_STOMP ||
            _skill == SKILL_SHOCKWAVE1 ||
            _skill == SKILL_SHOCKWAVE2)
        {
            countAvalableSkills++;
        }
    }
    
    for (int i = 0; i < kRacePanelButtonMax; i++) {
        
        SKILL _skill = skills_[i];
        
        string abilityType = "";
        SEL_MenuHandler abilitySelector;
        unsigned int cost = 0;
        
        if (_skill == SKILL_DASH)
        {
            abilityType = kAbilityDashForward;
            abilitySelector = menu_selector(RacePanel::useDash);
            cost = _isoMapRacing->m_pet_player->getDashCost();
        }
        else if (_skill == SKILL_TRAP)
        {
            abilityType = kAbilityBananaTrap;
            abilitySelector = menu_selector(RacePanel::useTrap);
            cost = _isoMapRacing->m_pet_player->getTrapCost();
        }
        else if (_skill == SKILL_STOMP)
        {
            abilityType = kAbilityStomp;
            abilitySelector = menu_selector(RacePanel::useStomp);
            cost = _isoMapRacing->m_pet_player->getStompCost();
        }
        else if (_skill == SKILL_SHOCKWAVE1)
        {
            abilityType = kAbilityShootForward;
            abilitySelector = menu_selector(RacePanel::useShockwaveStraight1);
            cost = _isoMapRacing->m_pet_player->getShockwave1Cost();
        }
        else if (_skill == SKILL_SHOCKWAVE2)
        {
            abilityType = kAbilityDoubleShoot;
            abilitySelector = menu_selector(RacePanel::useShockwaveStraight2);
            cost = _isoMapRacing->m_pet_player->getShockwave2Cost();
        }
        else
        {
            continue;
        }
        
        GDAbilityVO aVo = g_gameContent->getAbilityVO(abilityType, 1);

        auto pos = aVo.src.find_first_of("12345");
        CCAssert(pos != string::npos, "Invalid ability icon name!");
        int abilities[] = {3, 4, 2, 0, 1};
        int num = abilities[aVo.src[pos] - '0' - 1];
        auto normalSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::AbilitiesPowerup1 + num));
        auto disabledSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::AbilitiesPowerup1Disabled + num));

        PowerupButton *button = PowerupButton::create(UIHelper::getSprite(UIHelper::AbilitiesAbilitiesButton),
                                                      UIHelper::getSprite(UIHelper::AbilitiesButtonDisabled),
                                                      normalSprite,
                                                      disabledSprite,
                                                      cost,
                                                      _skill);
        
        button->setTarget(this, abilitySelector, CCNikButtonEventTouchDown);

        button->setPosition(ccp(
                                screenSize.width
                                -
                                40.0f * sizeMultiplier
                                -
                                button->getContentSize().width * ((totalSkillsCount-ii) - 0.5f) //first time only needs half a button's width
                                -
                                marginX * (totalSkillsCount-ii)
                                +
                                ((button->getContentSize().width + marginX) * (totalSkillsCount - countAvalableSkills))
                                ,
                                25.0f * sizeMultiplier
                                + button->getContentSize().height * 0.5f
                                ));
        
        CCLog_("%d/%d button.x:%f, y:%f", ii, totalSkillsCount,button->getPosition().x, button->getPosition().y);
        
        button->setTag(_skill);
        m_groupPowerupButtons->addChild(button);
//        }
        
        button->setEnabled(true);
        
        if (animated_) {
            //TODO: add fast animations to each buttons
            
            //set initial values
            button->setVisible(false);
            
            
            //animate to display
            button->setVisible(true);
        }
        
        ii++;
    }
}

void RacePanel::hidePowerUpButtons(bool animated)
{
    if (m_groupPowerupButtons) {
        m_groupPowerupButtons->setVisible(false);
        m_groupPowerupButtons->removeAllChildrenWithCleanup(true);
        m_groupPowerupButtons = NULL;
    }
    
//    m_groupEnergy->setVisible(false);
//    
//    float marginWidth = this->getContentSize().width*0.02f;
//    float marginHeight = this->getContentSize().height*0.02f;
//    int count = m_buttonPowerUps->count();
//    for (int i = 0; i < count; i++) {
//        CCNikButton *button = (CCNikButton *)m_buttonPowerUps->objectAtIndex(i);
//        CCPoint point;
//       
//        
//                if(i>3){
//                    point = ccp(this->getContentSize().width - (button->getContentSize().width*0.5f + marginWidth) - (marginWidth + button->getContentSize().width)*(count - i - 1),
//                                -button->getContentSize().height*2+ marginHeight);
//                }else{
//                    point = ccp(this->getContentSize().width - (button->getContentSize().width*0.5f + marginWidth) - (marginWidth + button->getContentSize().width)*(count - i - 2), -button->getContentSize().height*2 + marginHeight);
//                }
//             
//        
//        
////        CCPoint point = ccp(this->getContentSize().width - (button->getContentSize().width*0.5f + marginWidth) - (marginWidth + button->getContentSize().width)*(count - i - 1), - button->getContentSize().height*0.5f - marginHeight);
//        
//        if (animated) {
//            CCSequence *sequence = CCSequence::create(CCDelayTime::create(0.1f*i),
//                                                      CCEaseElasticOut::create(CCMoveTo::create(0.3f, point), 0.5f),
//                                                      NULL);
//            button->runAction(sequence);
//        } else {
//            button->setPosition(point);
//        }
//    }
//    
//    PowerupButton *lastButton = (PowerupButton *)m_buttonPowerUps->lastObject();
//    if (lastButton) {
//        CCPoint point = ccp(this->getContentSize().width - marginWidth - m_bgEnergy->getContentSize().width*1.3f, -m_skillCostLabel->getContentSize().height*0.5f);
//        if (animated) {
//            CCSequence *sequence = CCSequence::create(CCDelayTime::create(0.6f),
//                                                      CCEaseElasticOut::create(CCMoveTo::create(0.3f, DEFAULT_GROUP_ENERGY_POSITION), 0.5f),
//                                                      NULL);
//            m_groupEnergy->runAction(sequence);
//        } else {
//            m_groupEnergy->setPosition(point);
//        }
//    }
}

void RacePanel::hideBtn(float delay) {
    
    m_btnGo->setVisible(false);
    m_btnGo->setScale(0.0f);
}

void RacePanel::showTrafficLight(float delay) {
    
    if (!m_uiTrafficShow && CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(m_uiTraffic) == 0) {
        m_uiTrafficShow = true;
        m_uiTraffic->setPosition(m_uiTrafficHidePos);
        m_uiTraffic->runAction(CCSequence::create(CCDelayTime::create(delay), CCEaseElasticOut::create(CCMoveTo::create(RACEPANEL_MENU_MOVING_SPEED, m_uiTrafficShowPos), RACEPANEL_MENU_EASING_SPEED), NULL));
    }
}

void RacePanel::hideTrafficLight(float delay)
{
   if (m_uiTrafficShow)
   {
        m_uiTrafficShow = false;
        m_uiTraffic->setPosition(m_uiTrafficShowPos);
        m_uiTraffic->runAction(CCSequence::create(CCDelayTime::create(delay), CCEaseElasticOut::create(CCMoveTo::create(RACEPANEL_MENU_MOVING_SPEED, m_uiTrafficHidePos), RACEPANEL_MENU_EASING_SPEED), NULL));
    }
}

void RacePanel::showGauge()
{
    if (gaugePanel == NULL)
    {
        if (m_forceVeryEasyGauge) {
            gaugePanel = GaugePanel::create("veryeasy1");
        } else {
            gaugePanel = GaugePanel::create("");
        }
        gaugePanel->setPosition(getGaugeHidePos());
        this->addChild(gaugePanel);
    }
    
    showGauge(0.5f);
    showBtn(0.5f);
    gaugePanel->start();
    
    SceneRace *_scene = (SceneRace*)g_currentScene;
    IsoMapRacing *isoMapRacing = (IsoMapRacing*)_scene->isoMap;
    isoMapRacing->changeRacingState(MRS_READY);
}

void RacePanel::showGauge(float delay)
{
    gaugePanel->setPosition(getGaugeShowPos());
    
    gaugePanel->setVisible(true);
}

void RacePanel::hideGauge(float delay)
{
    gaugePanel->setVisible(false);
    gaugePanel->stop();
}

void RacePanel::showHUD()
{
    m_lblPosTitle->setVisible(true);
    m_lblPos->setVisible(true);
    m_lblTimer->setVisible(true);
    m_lblLapTitle->setVisible(true);
    m_lblLap->setVisible(true);
    m_groupSpeed->setVisible(true);
    
    if (this->m_forceHideGiveUpButton) return;
    m_btnGiveUp->setVisible(true);
}

void RacePanel::hideHUD()
{
    m_lblPosTitle->setVisible(false);
    m_lblPos->setVisible(false);
    m_lblTimerTitle->setVisible(false);
    m_lblTimer->setVisible(false);
    m_lblTimerEffect->setVisible(false);
    m_lblLapTitle->setVisible(false);
    m_lblLap->setVisible(false);
    m_groupTimeAdded->setVisible(false);
    m_groupSpeed->setVisible(false);
    
    m_btnGiveUp->setVisible(false);
    if (m_btnGetHelp) m_btnGetHelp->setVisible(false);
}

void RacePanel::onGiveUp(cocos2d::CCObject *sender) {
    
    SceneRace* _scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
    
    _isoMapRacing->m_giveUp = true;
    _isoMapRacing->removeHelperPet();
    _isoMapRacing->m_pet_opponent->m_summaryTrack
    = _isoMapRacing->m_pet_opponent->m_trackCount * _isoMapRacing->m_pet_opponent->max_round;
    
}

bool RacePanel::initGetHelp() {
    
    if (g_playerState->HelperVo.pet_type == "") return false;
    if (m_btnGetHelp || m_helperPetSpine || m_helperPhotoSprite) return false;
    
    CCSprite *buttonGetHelpSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::RaceElementsButtonHelperIsFriend));
    
    m_btnGetHelp = CCNikButton::create(buttonGetHelpSprite, NULL);
    m_btnGetHelp->setVisible(false);
    addChild(m_btnGetHelp, ZORDER_ABOVE_POWERUPBUTTONS);
    
    if(g_opponentState->replayGuestRaw != "" && g_opponentState->replayHelperPet != "") {
        g_playerState->HelperVo.pet_type = g_opponentState->opponentPet.helper_pet;
    }
    
    if (g_playerState->HelperVo.pet_type != "info" &&
        g_playerState->HelperVo.pet_type != "facebook" &&
        g_playerState->HelperVo.pet_type != "invite")
    {
        m_helperPetSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, g_playerState->HelperVo.pet_type.c_str());
        m_helperPetSpine->setScale(1.111111111f * sizeMultiplier);
        m_helperPetSpine->setScaleX(m_helperPetSpine->getScaleX() * -1.0f);
        m_helperPetSpine->setPosition(m_btnGetHelp->getPosition() + kHelperPetSpineOffsetToHelperButton);
        m_helperPetSpine->setAnimation("habitat_idle_stand", true);
        m_helperPetSpine->setVisible(false);
        this->addChild(m_helperPetSpine, ZORDER_ABOVE_POWERUPBUTTONS+1);
        
        if (g_playerState->HelperVo.isFriend) {
            m_helperPhotoSprite = CCLoadSpriteURL::create((char*)UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::AvatarDefault)), kDefaultAvatarSize*sizeMultiplier, (char*)g_playerState->HelperVo.photo.c_str(), (char*)g_playerState->HelperVo.photo.c_str());
        } else {
            m_helperPhotoSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::AvatarDefault));
        }
        
        m_helperPhotoSprite->setScale((float)(0.65*0.97));
        m_helperPhotoSprite->setPosition(m_btnGetHelp->getContentSize()*0.5f
                                         + ccp(110.0f*sizeMultiplier,
                                               8.0f*sizeMultiplier));
        m_btnGetHelp->addChild(m_helperPhotoSprite);
        
        CCSprite* _frame_avatar = UIHelper::getSprite(UIHelper::UISprites(UIHelper::AvatarBorder));
        _frame_avatar->setPosition(m_helperPhotoSprite->getPosition());
        _frame_avatar->setScale(0.65f);
        m_btnGetHelp->addChild(_frame_avatar);
        _frame_avatar->setVisible(g_playerState->HelperVo.isFriend);
    }
    
    m_btnGetHelp->setTarget(this, menu_selector(RacePanel::onGetHelp), CCNikButtonEventTouchUpInside);
    m_btnGetHelp->setPosition(ccp(POS_X_btnGetHelp - m_btnGetHelp->getContentSize().width*2.0f,
                                  POS_Y_btnGetHelp));
    m_btnGetHelp->setAnchorPoint(ccp(0.5f, 0.5f));
    
    return true;
}

void RacePanel::onInstantWin(cocos2d::CCObject *sender) {
    
    SceneRace* _scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
        
    _isoMapRacing->removeHelperPet();
    _isoMapRacing->m_pet_player->m_summaryTrack = _isoMapRacing->m_pet_player->m_trackCount * _isoMapRacing->m_pet_player->max_round;
    
}

void RacePanel::showGetHelp()
{
    //NOTE: This function is called in update
    
    if (!m_helperPetSpine || !m_btnGetHelp) return;
    if (m_btnGetHelp->isVisible()) return;
    if (this->m_forceHideGetHelpButton) return;
    
    m_btnGetHelpIsHiding = false;
    m_btnGetHelp->setEnabled(true);
    m_btnGetHelp->setVisible(true);
    m_btnGetHelp->runAction(CCEaseBackOut::create(CCMoveTo::create(0.2f, ccp(POS_X_btnGetHelp,
                                                                             POS_Y_btnGetHelp))));
    m_helperPetSpine->setVisible(true);
    m_helperPetSpine->setPosition(m_btnGetHelp->getPosition() + kHelperPetSpineOffsetToHelperButton);
    m_helperPetSpine->runAction(CCEaseBackOut::create(CCMoveTo::create(0.2f, ccp(POS_X_btnGetHelp,
                                                                                 POS_Y_btnGetHelp)
                                                                       + kHelperPetSpineOffsetToHelperButton)));
}

void RacePanel::hideGetHelp(bool alsoHidePetSpine)
{
    //NOTE: This function is called in update
    
    if (!m_helperPetSpine || !m_btnGetHelp) return;
    
    if (m_btnGetHelpIsHiding) return;
    m_btnGetHelpIsHiding = true;
    
    m_btnGetHelp->setEnabled(false);
    m_btnGetHelp->runAction(CCEaseBackOut::create(CCMoveTo::create(0.2f, ccp(POS_X_btnGetHelp - m_btnGetHelp->getContentSize().width*2.0f,
                                                                             POS_Y_btnGetHelp))));
    if (alsoHidePetSpine) {
        m_helperPetSpine->runAction(CCEaseBackOut::create(CCMoveTo::create(0.2f, kHelperPetSpineOffsetToHelperButton
                                                                           + ccp(POS_X_btnGetHelp - m_btnGetHelp->getContentSize().width*2.0f,
                                                                                 POS_Y_btnGetHelp))));
    }
}

void RacePanel::onGetHelp(cocos2d::CCObject *sender) {
    
    if (m_btnGetHelpIsHiding) return;

    SceneRace* _scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;

    if(_isoMapRacing->m_pet_player->isPlaybackReplayDataEnabled())
        return;

    if (_isoMapRacing->m_pet_opponent->m_summaryTrack >= 5) { //opponent needs to run at least 5 tracks
        _isoMapRacing->addHelperPet();
        hideGetHelp(false);
    }
    
}

void RacePanel::touchDownGauge(cocos2d::CCObject *sender) {
    
    if (this->m_disableTouchDownGaugeFunction) return;
    respondToGoButton(gaugePanel->getCurrentPinResult());
    
}

void RacePanel::respondToGoButton(GaugeRESULT result) {
    
    g_gameAudio->stopAllEffect();
    g_gameAudio->playEffect(AUDIO_RACE_START);
    g_gameAudio->playEffect(AUDIO_RACE_CHEERS,false);
    
    SceneRace *_scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
    
    _scene->racePanel->hideGauge(0.1f);
    _scene->racePanel->hideBtn(0.3f);
    
    _isoMapRacing->changeRacingState(MRS_GO);
    _isoMapRacing->startRunningWithGaugeResult(result);
    
    if(_isoMapRacing->m_pet_player->isPlaybackReplayDataEnabled())
        initReplayUI();
    
    if (!g_playerState->Checklist[TutFirstRace::key()]) {
        //don't start logging this time event during the TutFirstRace tutorial
    } else {
        FlurryCpp::logEvent(EVENT_RACE_NORMAL_PLAY, true);
    }
    
}

void RacePanel::beginCountdown() {
    
    if (m_racePanelState < RPS_COUNTDOWN) {
        m_racePanelState = RPS_COUNTDOWN;
    }
    
}

void RacePanel::beginCountdown(void *inst, void* sender) {
    
    if(RacePanel* this_ = (RacePanel*)inst) {
        if (this_->m_racePanelState < RPS_COUNTDOWN) {
            this_->m_racePanelState = RPS_COUNTDOWN;
        }
    }
    
}

void RacePanel::startRunning(void *inst, void* sender) {
    
    CCLog_("=====RacePanel::startRunning=====");
    
//    if (RacePanel* this_ = (RacePanel*)inst) {
//        SceneRaceCommand* command = (SceneRaceCommand*)g_currentScene;
//        if (this_->m_racePanelState == RPS_LAUNCH || this_->m_racePanelState == RPS_RUNNING) {
////            command->startRunning(false);
//        } else {
////            command->startRunning(true);
//        }
//    }
}

void RacePanel::setLap(int lap, int all)
{
    if (lap == all) { //Final lap
        
        showFinalLap();
        
        m_lblLap->setString(CCString::createWithFormat("%d/%d",lap  ,all)->getCString());
    } else if (lap < all) {
        m_lblLap->setString(CCString::createWithFormat("%d/%d",lap ,all)->getCString());
    } else if (lap > all) {
//        m_lblLap->setString(CCString::createWithFormat("Lap %d/%d",lap ,all)->getCString());
    }
    
    if (m_currentLap <= lap && lap > 1) {
        
        animateBallEffect(BallEffectTypeFromPlayer, m_lblLap, 0.6f);
        animateParticleStarWithDelay(m_lblLap, 0.6f);
        addGlowEffectWithDelay(m_lblLap, 0.6f);

        CCSequence *sequence = CCSequence::create(CCDelayTime::create(0.6f),
                                                  CCScaleTo::create(0.15, -1, 1),
                                                  CCScaleTo::create(0.15, 1, 1),
                                                  CCScaleTo::create(0.15, -1, 1),
                                                  CCScaleTo::create(0.15, 1, 1),
                                                  NULL);
        
        m_lblLap->runAction(CCEaseOut::create(sequence, 2));
        
    }
    
    
    m_currentLap = lap;
}

void RacePanel::exitThisScene(void *inst, void *sender) {
    
    if (SceneRace* _scene = (SceneRace*)g_currentScene) {
        
        g_gameAudio->stopMusic();
        g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
        g_gameAudio->clearEffectFadeOut(3.0f);
        _scene->fadeOutThisSceneAndChangeTo(SCENE_WORLDMAP);
    }
    
}

void RacePanel::showFinalLap()
{
    showSign("oldversion/_sign_final_lap.png", 0.35f);
}

void RacePanel::showTimeUp()
{
    showSign("oldversion/_sign_time_up.png", 1.0f);
}

void RacePanel::showSign(const char* signPng, float animationTime)
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (m_signBgSprite == NULL) {
        m_signBgSprite = CCSprite::createWithSpriteFrameName("oldversion/_sign_bg.png");
        this->addChild(m_signBgSprite);
    }
    m_signBgSprite->setScale(2.0f);
    m_signBgSprite->setPosition(ccp(m_signBgSprite->getContentSize().width * -1.0f, screenSize.height*0.5f));
    
    if (m_signSprite == NULL) {
        m_signSprite = CCSprite::createWithSpriteFrameName(signPng);
        this->addChild(m_signSprite);
    }
    
    m_signSprite->setPosition(ccp(screenSize.width + m_signSprite->getContentSize().width, screenSize.height*0.5f));
    
    m_signBgSprite->stopAllActions();
    m_signBgSprite->runAction(CCSequence::create(CCEaseExponentialOut::create(CCMoveTo::create(animationTime, screenSize*0.5f)),
                                                 CCEaseExponentialIn::create(CCMoveTo::create(animationTime,
                                                                                               ccp(screenSize.width + m_signBgSprite->getContentSize().width, screenSize.height*0.5f))),
                                                 NULL));
    
    m_signSprite->stopAllActions();
    m_signSprite->runAction(CCSequence::create(CCEaseExponentialOut::create(CCMoveTo::create(animationTime, screenSize*0.5f)),
                                                 CCEaseExponentialIn::create(CCMoveTo::create(animationTime,
                                                                                               ccp(m_signBgSprite->getContentSize().width * -1.0f, screenSize.height*0.5f))),
                                                 NULL));
}

void RacePanel::showHintCurrentEnergy()
{
    if (m_hintGroup == NULL) {
        m_hintGroup = CCNode::create();
    }
    m_hintGroup->setVisible(true);
    this->addChild(m_hintGroup, 100000);
    
    CCScale9Sprite *hintBg = CCScale9Sprite::createWithSpriteFrameName("oldversion/_ground_9sprite_dialog.png");
    hintBg->setContentSize(CCSizeMake(320.0f*sizeMultiplier, 288.0f*sizeMultiplier));
    m_hintGroup->addChild(hintBg);
    hintBg->setPosition(m_groupEnergy->getPosition() + ccp(0.0f, -100.0f*sizeMultiplier));
    
    CCSprite *hintArrow = CCSprite::createWithSpriteFrameName("oldversion/_mini_ground_arrow_white.png");
    hintArrow->setPosition(ccp(hintBg->getContentSize().height*0.5f,
                               -hintArrow->getContentSize().height*0.25f +2.0f*sizeMultiplier));
    hintArrow->setRotation(-90.0f);
    hintBg->addChild(hintArrow);
    
    CCLabelBMFont *hintText = CCLabelHelper::createBMFont("Your current energy", kFontSizeLarge, kTextColorDark);
    hintText->setWidth(hintBg->getContentSize().width-80.0f*sizeMultiplier);
    hintText->setAlignment(kCCTextAlignmentLeft);
    hintText->setPosition(hintBg->getContentSize()*0.5f);
    hintBg->addChild(hintText);
    
    CCAction *action = CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCFadeOut::create(0.6f), 5.0f),
                                                                  CCEaseOut::create(CCFadeIn::create(0.6f), 5.0f),
                                                                  NULL));
    CCAction *action2 = CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCFadeOut::create(0.6f), 5.0f),
                                                                   CCEaseOut::create(CCFadeIn::create(0.6f), 5.0f),
                                                                   NULL));
    CCAction *action3 = CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCFadeOut::create(0.6f), 5.0f),
                                                                   CCEaseOut::create(CCFadeIn::create(0.6f), 5.0f),
                                                                   NULL));
    hintBg->runAction(action);
    hintArrow->runAction(action2);
    hintText->runAction(action3);
}

void RacePanel::removeAllHints()
{
    if (m_hintGroup) {
        m_hintGroup->setVisible(false);
        m_hintGroup->removeAllChildrenWithCleanup(true);
    }
}

void RacePanel::noEnergyUIFeedback()
{
    if (!m_groupEnergy->isVisible()) return;
    
    m_groupEnergy->stopAllActions();
    m_groupEnergy->setScale(0.5);
//    m_bgEnergy->setColor(ENERGY_COLOR_RED);
    m_groupEnergy->runAction(CCSequence::create(CCEaseBackOut::create(CCScaleTo::create(0.15f, 1.0f)),
                                                CCCallFunc::create(this, callfunc_selector(RacePanel::resetEnergyColor)),
                                                NULL));
//    m_skillCostLabel->stopAllActions();
    m_skillCostLabel->setColor(ENERGY_COLOR_RED);
//    m_skillCostLabel->runAction(CCSequence::create(CCDelayTime::create(0.75f),
//                                                   CCScaleTo::create(0.1f, 1.4f),
//                                                   CCFadeOut::create(0),
//                                                   CCDelayTime::create(0.05f),
//                                                   CCFadeIn::create(0),
//                                                   CCScaleTo::create(0.1f,1.0f),
//                                                   NULL));
}

void RacePanel::resetEnergyColor()
{
    m_groupEnergy->setScale(1.0f);
//    m_bgEnergy->setColor(ENERGY_BG_NORMAL);
    
    m_skillCostLabel->setColor(ENERGY_COLOR_NORMAL);
}

void RacePanel::addGlowEffect(cocos2d::CCNode *onNode)
{
    addGlowEffectWithDelay(onNode, 0);
}

void RacePanel::addGlowEffectWithDelay(cocos2d::CCNode *onNode, float delay)
{
    if (!onNode->isVisible()) return;
    
    CCSprite *effect = CCSprite::create("common/vfx/fx_gacha_to_barn_b2.png");
    effect->setPosition(onNode->getPosition());
    effect->setScale(0);
    this->addChild(effect);
    
    effect->runAction(CCSequence::create(CCDelayTime::create(delay),
                                         CCScaleTo::create(0.1f, (onNode->getContentSize().width/effect->getContentSize().width)*1.4f),
                                         CCFadeOut::create(0.1f),
                                         CCRemoveSelf::create(),
                                         NULL));
}

#pragma mark - PowerUp Button

//void RacePanel::resetAllSkillsCost()
//{
//    CCObject *object;
//    CCARRAY_FOREACH(m_buttonPowerUps, object) {
//        PowerupButton *button = (PowerupButton *)object;
//        button->resetTimeUsed();
//    }
//}

void RacePanel::checkSkillCost(int newCost)
{
    if (m_olderCost == OLDER_COST_INIT_VALUE) {
        m_olderCost = newCost;
        
        setCost(CCString::createWithFormat("%d", newCost));
        
    } else if (m_olderCost == newCost) {
        return;
    }
    
    if (m_groupPowerupButtons) {
        CCObject *object;
        CCARRAY_FOREACH(m_groupPowerupButtons->getChildren(), object) {
            PowerupButton *button = (PowerupButton *)object;
            button->checkCost(newCost);
        }
    }
    
    if (m_groupEnergy->isVisible()) {
        CCNode *tempNode = CCNode::create();
        tempNode->setPosition(m_groupEnergy->convertToWorldSpace(m_skillCostLabel->getPosition()));
        
        if (m_olderCost < newCost) {
            //animate when cost is added
            
            if (m_skillCostLabel) {
                
                animateBallEffect(BallEffectTypeFromPlayer, tempNode, 0.75f);
                animateParticleStarWithDelay(tempNode, 0.75f);
                
                if (m_skillCostLabel->isVisible()) {
                    m_skillCostLabel->stopAllActions();
                    m_skillCostLabel->runAction(CCSequence::create(CCDelayTime::create(0.75f),
                                                                   CCScaleTo::create(0.1f, 1.4f),
                                                                   CCFadeOut::create(0),
                                                                   CCCallFuncO::create(this, callfuncO_selector(RacePanel::addGlowEffect), tempNode),
                                                                   CCCallFuncO::create(this, callfuncO_selector(RacePanel::setCost), CCString::createWithFormat("%d", newCost)),
                                                                   CCDelayTime::create(0.05f),
                                                                   CCFadeIn::create(0),
                                                                   CCScaleTo::create(0.1f,1.0f),
                                                                   NULL));
                }
            }
            
        } else if (m_olderCost > newCost) {
            //animate when cost is reduced
            
            if (m_skillCostLabel) {
                
                animateParticleDecreaseCost(tempNode);
                
                if (m_skillCostLabel->isVisible()) {
                    m_skillCostLabel->stopAllActions();
                    m_skillCostLabel->runAction(CCSequence::create(//no delay
                                                                   CCScaleTo::create(0.1f, 1.4f),
                                                                   CCFadeOut::create(0),
                                                                   CCCallFuncO::create(this, callfuncO_selector(RacePanel::addGlowEffect), tempNode),
                                                                   CCCallFuncO::create(this, callfuncO_selector(RacePanel::setCost), CCString::createWithFormat("%d", newCost)),
                                                                   CCDelayTime::create(0.05f),
                                                                   CCFadeIn::create(0),
                                                                   CCScaleTo::create(0.1f, 1.0f),
                                                                   NULL));
                }
            }
        }
    }
    
    m_olderCost = newCost;
}

void RacePanel::setCost(cocos2d::CCString *costString)
{
    if (m_skillCostLabel) {
        
        m_skillCostLabel->setString(costString->getCString());
        m_skillCostLabel->setScale(1.0f);
    }
}

void RacePanel::updateCostOnButton(CCObject *button, int cost)
{
    PowerupButton *aButton = (PowerupButton *)button;
    aButton->updateCost(cost);
}

void RacePanel::useDash(cocos2d::CCObject *sender) {
    
    SceneRace *_scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
 
    if (_isoMapRacing->gameIsPaused() &&
        !_isoMapRacing->isTutorialMode()) return;
    
    int skillCost = _isoMapRacing->m_pet_player->getDashCost();
    int timeUsed = _isoMapRacing->m_pet_player->getDashTimeUsed();
    int sumTrackDifference = g_gameContent->cal_round_to_nearest_5(_isoMapRacing->m_pet_player->m_summaryTrack - _isoMapRacing->m_pet_opponent->m_summaryTrack);
   
    if (_isoMapRacing->useDash(sender)) {
        
        CCDictionary *ccDict = CCDictionary::create();
        ccDict->setObject(CCString::create("Dash"), "Skill");
        ccDict->setObject(CCString::createWithFormat("%d", skillCost), "Skill Cost");
        ccDict->setObject(CCString::createWithFormat("%d", timeUsed+1), "Current Time Use");
        ccDict->setObject(CCString::createWithFormat("%d", sumTrackDifference), "Sum Track Difference");
        FlurryCpp::logEvent(EVENT_USE_SKILL, ccDict);
        
        if (onUseSkill) {
            onUseSkill(onUseSkillSender, SKILL_DASH);
        }
        
    }
    
}

void RacePanel::useTrap(cocos2d::CCObject *sender) {
    
    SceneRace *_scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
    
    if (_isoMapRacing->gameIsPaused() &&
        !_isoMapRacing->isTutorialMode()) return;
        
    int skillCost = _isoMapRacing->m_pet_player->getTrapCost();
    int timeUsed = _isoMapRacing->m_pet_player->getTrapTimeUsed();
    int sumTrackDifference = g_gameContent->cal_round_to_nearest_5(_isoMapRacing->m_pet_player->m_summaryTrack - _isoMapRacing->m_pet_opponent->m_summaryTrack);
        
    if (_isoMapRacing->useTrap(sender)) {
        
        CCDictionary *ccDict = CCDictionary::create();
        ccDict->setObject(CCString::create("Trap"), "Skill");
        ccDict->setObject(CCString::createWithFormat("%d", skillCost), "Skill Cost");
        ccDict->setObject(CCString::createWithFormat("%d", timeUsed+1), "Current Time Use");
        ccDict->setObject(CCString::createWithFormat("%d", sumTrackDifference), "Sum Track Difference");
        FlurryCpp::logEvent(EVENT_USE_SKILL, ccDict);
        
        if (onUseSkill) {
            onUseSkill(onUseSkillSender, SKILL_TRAP);
        }
    }

}

void RacePanel::useStomp(cocos2d::CCObject *sender)
{
    SceneRace *_scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
    
    if (_isoMapRacing->gameIsPaused() &&
        !_isoMapRacing->isTutorialMode()) return;
        
    int skillCost = _isoMapRacing->m_pet_player->getStompCost();
    int timeUsed = _isoMapRacing->m_pet_player->getStompTimeUsed();
    int sumTrackDifference = g_gameContent->cal_round_to_nearest_5(_isoMapRacing->m_pet_player->m_summaryTrack - _isoMapRacing->m_pet_opponent->m_summaryTrack);
    
    if (_isoMapRacing->useStomp(sender)) {
        
        CCDictionary *ccDict = CCDictionary::create();
        ccDict->setObject(CCString::create("Stomp"), "Skill");
        ccDict->setObject(CCString::createWithFormat("%d", skillCost), "Skill Cost");
        ccDict->setObject(CCString::createWithFormat("%d", timeUsed+1), "Current Time Use");
        ccDict->setObject(CCString::createWithFormat("%d", sumTrackDifference), "Sum Track Difference");
        FlurryCpp::logEvent(EVENT_USE_SKILL, ccDict);
        
        if (onUseSkill) {
            onUseSkill(onUseSkillSender, SKILL_STOMP);
        }
    }
}

void RacePanel::useShockwaveStraight1(cocos2d::CCObject *sender)
{
    SceneRace *_scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
    
    if (_isoMapRacing->gameIsPaused() &&
        !_isoMapRacing->isTutorialMode()) return;
    
    int skillCost = _isoMapRacing->m_pet_player->getShockwave1Cost();
    int timeUsed = _isoMapRacing->m_pet_player->getShockwave1TimeUsed();
    int sumTrackDifference = g_gameContent->cal_round_to_nearest_5(_isoMapRacing->m_pet_player->m_summaryTrack - _isoMapRacing->m_pet_opponent->m_summaryTrack);
    
    if (_isoMapRacing->useShockwaveStraight1(sender)) {
        
        CCDictionary *ccDict = CCDictionary::create();
        ccDict->setObject(CCString::create("Shockwave 1"), "Skill");
        ccDict->setObject(CCString::createWithFormat("%d", skillCost), "Skill Cost");
        ccDict->setObject(CCString::createWithFormat("%d", timeUsed+1), "Current Time Use");
        ccDict->setObject(CCString::createWithFormat("%d", sumTrackDifference), "Sum Track Difference");
        FlurryCpp::logEvent(EVENT_USE_SKILL, ccDict);

        if (onUseSkill)
        {
            onUseSkill(onUseSkillSender, SKILL_SHOCKWAVE1);
        }
    }
}

void RacePanel::useShockwaveStraight2(cocos2d::CCObject *sender)
{
    SceneRace *_scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
    
    if (_isoMapRacing->gameIsPaused() &&
        !_isoMapRacing->isTutorialMode()) return;
    
    int skillCost = _isoMapRacing->m_pet_player->getShockwave2Cost();
    int timeUsed = _isoMapRacing->m_pet_player->getShockwave2TimeUsed();
    int sumTrackDifference = g_gameContent->cal_round_to_nearest_5(_isoMapRacing->m_pet_player->m_summaryTrack - _isoMapRacing->m_pet_opponent->m_summaryTrack);
    
    if (_isoMapRacing->useShockwaveStraight2(sender)) {
        
        CCDictionary *ccDict = CCDictionary::create();
        ccDict->setObject(CCString::create("Shockwave 2"), "Skill");
        ccDict->setObject(CCString::createWithFormat("%d", skillCost), "Skill Cost");
        ccDict->setObject(CCString::createWithFormat("%d", timeUsed+1), "Current Time Use");
        ccDict->setObject(CCString::createWithFormat("%d", sumTrackDifference), "Sum Track Difference");
        FlurryCpp::logEvent(EVENT_USE_SKILL, ccDict);

        if (onUseSkill)
        {
            onUseSkill(onUseSkillSender, SKILL_SHOCKWAVE2);
        }
    }
}

void RacePanel::animateParticleStarWithDelay(cocos2d::CCNode *onNode, float delay)
{
    if (!onNode->isVisible()) return;
    
    this->runAction(CCSequence::create(CCDelayTime::create(delay),
                                       CCCallFuncO::create(this, callfuncO_selector(RacePanel::animateParticleStar), onNode),
                                       NULL));
}

void RacePanel::animateParticleStar(cocos2d::CCNode *onNode)
{
    if (!onNode->isVisible()) return;
    
    ExplosionParticleGroup *particle = ExplosionParticleGroup::create("common/vfx/star_2.png", sizeMultiplier);
    particle->setPosition(onNode->getPosition() - ccp(0, onNode->getContentSize().height*0.25f));
    this->addChild(particle, 1000);
    particle->animate(true);
}

void RacePanel::animateParticleDecreaseCostWithDelay(cocos2d::CCNode *onNode, float delay)
{
    if (!onNode->isVisible()) return;
    
    this->runAction(CCSequence::create(CCDelayTime::create(delay),
                                       CCCallFuncO::create(this, callfuncO_selector(RacePanel::animateParticleDecreaseCost), onNode),
                                       NULL));
}

void RacePanel::animateParticleDecreaseCost(cocos2d::CCNode *onNode)
{
    if (!onNode->isVisible()) return;
    
    ExplosionParticleGroup *particle = ExplosionParticleGroup::create("common/vfx/fx_gacha_to_barn_b2.png", sizeMultiplier*0.2f);
    particle->setAmount(18);
    particle->setPosition(onNode->getPosition() - ccp(0, onNode->getContentSize().height*0.25f));
    this->addChild(particle, 1000);
    particle->animate(true);
}

void RacePanel::animateBallEffect(BallEffectType type, cocos2d::CCNode *toNode, float duration)
{
    if (!toNode->isVisible()) return;

    SceneRace* _scene = (SceneRace*)g_currentScene;
    if (!_scene) return;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing*)_scene->isoMap;
    
    Pet *pet;
    if (type == BallEffectTypeFromPlayer) {
        pet = _isoMapRacing->m_pet_player;
    } else {
        pet = _isoMapRacing->m_pet_opponent;
    }
    CCPoint startPoint = _isoMapRacing->convertToWorldSpace(pet->getPosition());
    startPoint = this->convertToNodeSpace(startPoint);
    
    CCSprite *particleSprite = CCSprite::create("common/vfx/blue_particle_fx.png");
    particleSprite->setScale(sizeMultiplier);
    ProjectileParticle *particle = ProjectileParticle::create(particleSprite);
    particle->animate(this, startPoint, toNode->getPosition(), duration);
}

void RacePanel::forceVeryEasyGauge(bool force_) {
    m_forceVeryEasyGauge = force_;
}
void RacePanel::forceHideGiveUpButton(bool force_) {
    m_forceHideGiveUpButton = force_;
    if (m_forceHideGiveUpButton)
        if (m_btnGiveUp)
            m_btnGiveUp->setVisible(false);
}
void RacePanel::forceHideGetHelpButton(bool force_) {
    m_forceHideGetHelpButton = force_;
    if (m_forceHideGetHelpButton)
        if (m_btnGetHelp)
            m_btnGetHelp->setVisible(false);
}
void RacePanel::disableTouchDownGaugeFunction(bool disable_) {
    m_disableTouchDownGaugeFunction = disable_;
}

CCPoint RacePanel::getBtnGoShowPos()
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    return ccp(screenSize.width - m_btnGo->getContentSize().width*0.6f, m_btnGo->getContentSize().height*0.6f);
}

CCPoint RacePanel::getBtnGoHidePos()
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    return ccp(screenSize.width - m_btnGo->getContentSize().width*0.6f, - m_btnGo->getContentSize().height);
}

CCPoint RacePanel::getGaugeShowPos()
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    return ccp(MIN(screenSize.width*0.5f, getBtnGoShowPos().x - (gaugePanel->_gaugeBarBG->getContentSize().width*0.45f)), getBtnGoShowPos().y);
}

CCPoint RacePanel::getGaugeHidePos()
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    return ccp(MIN(screenSize.width*0.5f, getBtnGoShowPos().x - (gaugePanel->_gaugeBarBG->getContentSize().width*0.45f)), -70.0f * sizeMultiplier);
}

