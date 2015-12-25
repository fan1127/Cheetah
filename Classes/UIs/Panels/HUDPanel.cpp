//
//  HUDPanel.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#include "HUDPanel.h"
USING_NS_CC;
using namespace std;

#include "../../AppMacros.h"
extern TargetDevice targetDevice;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/BackendService.h"
extern BackendService *g_gameBackend;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;
#include "../../Scenes/SceneVille.h"
#include "../../Scenes/SceneWorldMap.h"
extern CCScene* g_currentScene;

#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCPopOverDialog.h"
#include "../../UIs/GachaButton.h"
#include "../../UIs/GachaPanel.h"
#include "../../UIs/Dialogs/SettingDialog.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../UIs/Dialogs/IAPDialog.h"
#include "../../UIs/Dialogs/LeaderboardDialog.h"
#include "../../UIs/Dialogs/TutorialDialog.h"
#include "../../Helpers/FlurryCpp.h"
#include "../../UIs/Dialogs/FBLoginDialog.h"
#include "../../UIs/Dialogs/ScrollDialog.h"
#include "../../UIs/Dialogs/InboxDialog.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/Versioning.h"
#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/CustomEventsHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/PointerPool.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

#if !(IS_RELEASE_BUILD)
#include "../../version.h"

#endif

extern float margin;
extern float retinaMultiplier;
extern float sizeMultiplier;
extern float isIPhoneMultiplier;
extern float isIPadMultiplier;
extern float isIOSMultiplier;
extern float isAndroidMultiplier;
extern unsigned int currentTime;

#define kHeightFromTop (202.0f*sizeMultiplier) //used to be exp ProgressBar height

#define kIconScale1 ((0.85f * isIPhoneMultiplier + 1.0f * isIPadMultiplier) * isIOSMultiplier)
#define kIconScale2 ((0.8f * isIPhoneMultiplier + 1.0f * isIPadMultiplier) * isIOSMultiplier)

CCMenu* HUDPanel::createButtonHelper(const char *text, CCObject* target, SEL_MenuHandler selector)
{
    CCSprite *buttonBg = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);
    buttonBg->setAnchorPoint(ccp(0.5f, 0.5f));
    CCLabelBMFont *buttonLabel = CCLabelHelper::createBMFont(text, kFontSizeLarge, true);
    buttonLabel->setAnchorPoint(ccp(0.5f, 0.5f));
    buttonLabel->setAlignment(kCCTextAlignmentCenter);
    buttonLabel->setPosition(buttonBg->getContentSize().width*0.5f, buttonBg->getContentSize().height*0.55f);
    buttonBg->addChild(buttonLabel);

    CCMenu* button = CCMenuHelper::createMenu(buttonBg, target, selector);
    button->setAnchorPoint(ccp(0.f, 0.f));

    return button;
}

CCPhatTouchableSprite* HUDPanel::createBlackBackgroundHelper(GLubyte opacity)
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    CCSprite* solid_black = blankSpriteWithSize(screenSize);
    CCPhatTouchableSprite* bgSprite = new CCPhatTouchableSprite();
    bgSprite->autorelease();
    bgSprite->initWithTexture(solid_black->getTexture());
    bgSprite->setAnchorPoint(CCPointZero);
    bgSprite->setColor(ccBLACK);
    bgSprite->setOpacity(opacity);
    return bgSprite;
}

HUDPanel *HUDPanel::create() {
    
    if (HUDPanel *pRet = new HUDPanel()) {
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

int HUDPanel::getMargin() const
{
    return margin * 2;
}

bool HUDPanel::init() {
    
    if (!CCLayer::init()) {
        return false;
    }

    m_showVersionNumber = true;

    m_dangerButtonsEnabled = true;
    m_didPressRaceBtn = false;

    m_expProgress = NULL;
    m_nameLabel = NULL;
    photoSprite = NULL;

    m_trackStatusSign = NULL;

    m_enableTrackSignBeating = true;
    m_signalSignSprite = NULL;
    
    clearLevelUpEffectDismissCallback();
    
    for (int i=0; i<HUDPanelBarTotal; i++) {
        resourceBars[i] = NULL;
    }
    
    for (int i=0; i<HUDPanelMenuTotal; i++) {
        menuButtons[i] = NULL;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Status Layer
    statusLayer = CCLayer::create();
    this->addChild(statusLayer);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Place Holder Layer
    menuPlaceHolderLayer = CCLayer::create();
    this->addChild(menuPlaceHolderLayer);
    
    checkTrade();
    
    scheduleUpdate();
    
    return true;
}

void HUDPanel::update(float delta) {
    
    if (!CustomEventsHelper::Instance().shouldAnimateIdle(kIdleAnimationMapButton)) return;
    if (!menuButtons[HUDPanelMenuMap]) return;
    if (menuButtons[HUDPanelMenuMap]->numberOfRunningActions() != 0) return;
    menuButtons[HUDPanelMenuMap]->runAction(CustomEventsHelper::Instance().getIdleAnimation(5.0f,kDefaultIdleAnimationScaleDifference));
    
}


void HUDPanel::onExit()
{
    cocos2d::CCLayer::onExit();
    PointerPool::invalidate(this);
}

void HUDPanel::updateHUD() {
    
    bool update_button_ = true;
    updateHUD(update_button_);
    
}

void HUDPanel::updateHUD(bool update_button_) {
    
    if (!g_playerState) return;
    
    if (m_expProgress) {
        CREATE_CHAR_BUFFER(_buffer, 64);
        
        snprintf(_buffer, _buffer_size, "%s", g_playerState->player.info.name.c_str());
        m_nameLabel->setString(_buffer);
        
        GDExperienceLevelVO _exp_vo = g_gameContent->getNextExperienceLevelVO(g_playerState->player.racing.level);
        if (_exp_vo.level == 0) {
            m_expProgress->setPercentage(100, 1.0f);
        }else {
            m_expProgress->setPercentage((g_playerState->player.racing.exp/(float)_exp_vo.required_exp)*100, 1.0f);
        }
        
        m_fanLabel->setString(CCStringHelper::createWithNumberFormat(g_playerState->player.balance.fan).c_str());
    }
    
    if (menuButtons[HUDPanelMenuMap] && getOverrideExploreButtonBadgeNumber() != HUD_OVERRIDE_EXPLORE_BUTTON_BADGE_NUMBER_DISABLE) {
        menuButtons[HUDPanelMenuMap]->setBadge(getOverrideExploreButtonBadgeNumber());
    }
    
    if (menuButtons[HUDPanelMenuGacha] && g_playerState->Checklist[TutFusePet::key()]) {
        menuButtons[HUDPanelMenuGacha]->setBadge(g_playerState->player.info.free_gacha);
    }
    
}

void HUDPanel::updateHUDTrack(bool track_complete_) {
    
    updateHUDTrack(track_complete_, true);
    
}

void HUDPanel::updateHUDTrack(bool track_complete_, bool update_sign_) {
    
    if (m_trackLabel) {
        CREATE_CHAR_BUFFER(_buffer, 64);
        g_playerState->calculateTrackpointsUsed();
        int _track_in_placeholder_points = 0;
        if (SceneVille* _scene = (SceneVille*)g_currentScene) {
            if (IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap) {
                if (_isoMapBuilding->placeHolder) {
                    if (_isoMapBuilding->placeHolder->track) {
                        _track_in_placeholder_points = _isoMapBuilding->placeHolder->track->tVo.point_used;
                    }
                }
            }
        }
        snprintf(_buffer, _buffer_size, "%d/%d",
                (g_playerState->player.racing._track_points+_track_in_placeholder_points),
                g_playerState->player.racing._track_points_limit);
        m_trackLabel->setString(_buffer);
    }
    
    if (update_sign_) {
        if (track_complete_) {
            changeTrackSignToComplete();
        }else {
            showIncompleteTrackSign();
        }
    }
    
}

void HUDPanel::updateHUDLevel(void *inst, void *sender) {
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille)) return;
    
    HUDPanel* _this = (HUDPanel*)sender;
    if (!_this || _this->m_expProgress->getLevel() >= PlayerState::getInstance()->player.racing.level) return;
 
    _this->m_expProgress->setLevel(g_playerState->player.racing.level);
    if(!canShowLevelUpEffect()) return;
    
    _this->displayLevelUpEffect();
    _scene->addMerchant();
    
}

bool HUDPanel::canShowLevelUpEffect()
{
    return true; //g_playerState->Checklist[TutGacha::key()];
}

bool HUDPanel::isLevelUpEffectShowed()
{
    return (m_levelUpEffect != NULL) && m_levelUpEffect->isVisible();
}

void HUDPanel::displayLevelUpEffect() {

    m_levelUpEffectBackground = createBlackBackgroundHelper(128);
    m_levelUpEffectBackground->setSender(this);
    m_levelUpEffectBackground->onHit = doNothing;
    this->addChild(m_levelUpEffectBackground);

    if(m_levelUpEffect == NULL)
    {
        g_gameAudio->playEffect(AUDIO_RANCH_LEVEL_UP);
        
        m_levelUpEffect = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "ranch_level_up");
        this->addChild(m_levelUpEffect, 0);
        
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        
        m_levelUpEffect->setPosition(ccp(screenSize.width * 0.5f, screenSize.height * 0.5f));
        m_levelUpEffect->setScale(sizeMultiplier);
        m_levelUpEffectDoneButton = createButtonHelper("Done", this, menu_selector(HUDPanel::onLevelUpEffectDoneButton));
        m_levelUpEffectDoneButton->setPosition(screenSize.width/2.f, (screenSize.height/2.f) - (400.f * sizeMultiplier));
        this->addChild(m_levelUpEffectDoneButton);
        
        m_levelUpEffectFont = CCLabelBMFont::create("99", "fonts/dimbo_levelup.fnt" , kCCLabelAutomaticWidth, kCCTextAlignmentCenter);
        m_levelUpEffectFont->setScale(1.0f);
        m_levelUpEffectFont->setColor(ccc3(255,255,255));
        m_levelUpEffectFont->setPosition(0,0);
        m_levelUpEffectFont->setAnchorPoint(ccp(0.5f, 0.5f));
        m_levelUpEffect->addChild(m_levelUpEffectFont);
        
        m_levelUpEffectBannerFont = CCLabelHelper::createBMFont(1.1f, "AWESOME!", kFontSizeExtraLarge, true);
        m_levelUpEffectBannerFont->setAnchorPoint(ccp(0.5f, 0.5f));
        m_levelUpEffectBannerFont->setAlignment(kCCTextAlignmentCenter);
        m_levelUpEffectBannerFont->setPosition(0,-230.f);
        m_levelUpEffect->addChild(m_levelUpEffectBannerFont);

        for(int i=0; i<MAX_FIREWORKS; ++i)
            m_levelUpEffectFireworks[i].init(this);
    }

    m_levelUpEffect->setZOrder(getMaxHUDZOrder() + 2);
    m_levelUpEffectDoneButton->setZOrder(m_levelUpEffect->getZOrder() + 1);
    m_levelUpEffectBackground->setZOrder(m_levelUpEffect->getZOrder() - 1);
    m_levelUpEffectBackground->setVisible(true);
    m_levelUpEffect->setVisible(true);
    m_levelUpEffect->clearAnimation();
    m_levelUpEffect->setAnimation("opening", false);
    float duration = m_levelUpEffect->states[0]->animation->duration * 0.05f;
    m_levelUpEffect->runAction(CCSequence::create(CCDelayTime::create(duration), CCCallFunc::create(this, callfunc_selector(HUDPanel::showButtonInLevelUpEffect)), NULL));

    for(int i=0; i<MAX_FIREWORKS; ++i)
        this->runAction(CCSequence::create(CCDelayTime::create(i * 0.2f), CCCallFunc::create(this, callfunc_selector(HUDPanel::shootFirework)), NULL));

    m_levelUpEffectDoneButton->setVisible(false);
    m_levelUpEffectFont->setVisible(false);
    m_levelUpEffectBannerFont->setVisible(false);

    showAllMainMenuButtons(false);

}

void HUDPanel::showButtonInLevelUpEffect()
{
    if(!m_levelUpEffect->isVisible())
        return;
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_gameAudio->playEffect(AUDIO_RACE_WIN1);

    m_levelUpEffectFont->setString(m_expProgress->getLevelAsText());
    m_levelUpEffectDoneButton->setVisible(true);
    m_levelUpEffectFont->setVisible(true);
    m_levelUpEffectBannerFont->setVisible(true);
    float duration = m_levelUpEffect->states[0]->animation->duration * 0.95f;
    m_levelUpEffect->runAction(CCSequence::create(CCDelayTime::create(duration), CCCallFunc::create(this, callfunc_selector(HUDPanel::setLevelUpEffectToIdle)), NULL));
}

void HUDPanel::setLevelUpEffectToIdle()
{
    m_levelUpEffect->setAnimation("idle", true);
}

void HUDPanel::onLevelUpEffectDoneButton() {
    
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    
    hideLevelUpEffect();
}

void HUDPanel::hideLevelUpEffect() {

    m_levelUpEffectBackground->removeFromParent();
    m_levelUpEffectBackground = NULL;

    m_levelUpEffect->setVisible(false);
    m_levelUpEffectDoneButton->setVisible(false);

    for(int i=0; i<MAX_FIREWORKS; ++i)
        m_levelUpEffectFireworks[i].hide();

    showAllMainMenuButtons(true);
    
    if (m_close_levelup_effect_callback)
        m_close_levelup_effect_callback(m_close_levelup_effect_sender);
}

void HUDPanel::shootFirework()
{
    for(int i=0; i<MAX_FIREWORKS; ++i)
    {
        if(m_levelUpEffectFireworks[i].m_particle->isVisible())
            continue;
        m_levelUpEffectFireworks[i].shoot();
        break;
    }
}

void HUDPanel::Firework::init(HUDPanel* hudPanel)
{
    m_particle = CCParticleSystemQuad::create("common/vfx/fire_work_ranch_level_up.plist");
    m_particle->setPosition(randomPosOnScreen());
    m_particle->setVisible(false);

    //m_spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "firework");
    //m_spine->setPosition(randomPosOnScreen());
    //m_spine->setScale(0.25f * sizeMultiplier);
    //m_spine->setVisible(false);

    m_hudPanel = hudPanel;
    //m_hudPanel->addChild(m_spine);
    m_particle->setZOrder(m_hudPanel->getMaxHUDZOrder() + 3);
    m_hudPanel->addChild(m_particle);
}

void HUDPanel::Firework::shoot()
{
    m_particle->setVisible(true);
    //m_spine->setVisible(true);
    animate();
}

void HUDPanel::Firework::move()
{
    if(!m_hudPanel->isLevelUpEffectShowed())
    {
        hide();
        return;
    }
    m_particle->setPosition(randomPosOnScreen());
    float s = static_cast<float>((double)arc4random() / ARC4RANDOM_MAX);
    m_particle->setScale(0.5f + (s * 2.f));
    //m_spine->setPosition(randomPosOnScreen());
    animate();
}

void HUDPanel::Firework::hide()
{
    m_particle->setVisible(false);
    //m_spine->setVisible(false);
}

void HUDPanel::Firework::animate()
{
    m_particle->resetSystem();
    float fireworkDuration = m_particle->getDuration() * 10.f;
    m_particle->runAction(CCSequence::create(CCDelayTime::create(fireworkDuration), CCCallFunc::create(this, callfunc_selector(HUDPanel::Firework::move)), NULL));

    //m_spine->clearAnimation();
    //m_spine->setAnimation("explode", false);
    //float fireworkDuration = m_spine->states[0]->animation->duration + 0.1f;
    //m_spine->runAction(CCSequence::create(CCDelayTime::create(fireworkDuration), CCCallFunc::create(this, callfunc_selector(HUDPanel::Firework::move)), NULL));
}


CCPoint HUDPanel::Firework::randomPosOnScreen() {
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    float x = static_cast<float>((double)arc4random() / ARC4RANDOM_MAX);
    float y = static_cast<float>((double)arc4random() / ARC4RANDOM_MAX);
    return ccp(screenSize.width*x, screenSize.height*y);
}

void HUDPanel::setLevelUpEffectDismissCallback(void *sender_, void (*callback_)(void *)) {
    
    m_close_levelup_effect_sender = sender_;
    m_close_levelup_effect_callback = callback_;
    
}

void HUDPanel::clearLevelUpEffectDismissCallback() {
    
    m_close_levelup_effect_sender = NULL;
    m_close_levelup_effect_callback = NULL;
    
}

int HUDPanel::getMaxHUDZOrder() const
{
    int maxZOrder = 0;

    for(int i=0; i<HUDPanelMenuTotal; ++i)
    {
        if(!menuButtons[i])
            continue;
        int zOrder = menuButtons[i]->getZOrder();
        if(zOrder > maxZOrder)
            maxZOrder = zOrder;
    }

    for(int i=0; i<HUDPanelBarTotal; ++i)
    {
        int zOrder = resourceBars[i]->getZOrder();
        if(zOrder > maxZOrder)
            maxZOrder = zOrder;
    }
    return maxZOrder;
}

void HUDPanel::showIncompleteTrackSign() {
    
    if (!m_enableTrackSignBeating)
        return;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (!m_trackStatusSign)
    {
        m_trackStatusSign = UIHelper::getSprite(UIHelper::HUDIncompleteTrack);
        m_trackStatusSign->setPosition(ccp(_screenSize.width + m_trackStatusSign->getContentSize().width + getMargin(), 370.0f*sizeMultiplier));
        m_trackStatusSignIcon = UIHelper::getSprite(UIHelper::HUDIncompleteIcon);
        m_trackStatusSign->addChild(m_trackStatusSignIcon);
        m_trackStatusSignIcon->setPositionY(m_trackStatusSign->getContentSize().height*0.7f);
        addChild(m_trackStatusSign);
    }
    else
    {
        m_trackStatusSign->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::HUDIncompleteTrack));
    }

    m_trackStatusSign->setTag(0);
    m_trackStatusSign->stopAllActions();
    m_trackStatusSign->runAction(CCEaseElasticOut::create(CCMoveTo::create(0.5f, ccp(_screenSize.width-getMargin()-m_trackStatusSign->getContentSize().width*0.5f, m_trackStatusSign->getPositionY())), 0.5f));

    m_trackStatusSign->runAction(CCSequence::create(CCScaleTo::create(0.2f, 1.21f),
                                                    CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                                    NULL));
}

void HUDPanel::trackSignBeating()
{
    if (!m_trackStatusSign)
        return;

    if (!m_enableTrackSignBeating)
        return;

    m_trackStatusSign->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.1f),
                                                    CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                                    NULL));
}

void HUDPanel::changeTrackSignToComplete()
{
    if (!m_trackStatusSign)
        return;

    if (!m_enableTrackSignBeating)
        return;

    CCSize _screenSize = getContentSize();
    m_trackStatusSign->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::HUDCompleteTrack));
    m_trackStatusSign->setTag(1);
    m_trackStatusSign->stopAllActions();
    m_trackStatusSign->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.1f),
                                                    CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                                    CCDelayTime::create(1.5f),
                                                    CCEaseElasticIn::create(CCMoveTo::create(0.5f, ccp(_screenSize.width+m_trackStatusSign->getContentSize().width+getMargin(), m_trackStatusSign->getPositionY())), 0.5f),
                                                    CCCallFunc::create(this, callfunc_selector(HUDPanel::hideAndReleaseTrackSign)),
                                                    NULL));

    m_trackStatusSignIcon->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::HUDCompleteIcon));
}

void HUDPanel::hideAndReleaseTrackSign(CCObject* sender_)
{
    if (!m_trackStatusSign)
        return;
    if (!m_enableTrackSignBeating)
        return;
    
    m_trackStatusSign->removeFromParentAndCleanup(true);
    m_trackStatusSign = NULL;
    
}

//TODO: Figure out, what the hell is this doing?
void HUDPanel::checkTrade() {
    
    Building* _building_market = g_playerState->getBuilding(kBuildingFusion, 0);
    
    if (_building_market) {
        bool canTrade = false;
        _building_market->hideBubbleTrade();
        
        for (vector<Pet*>::iterator it=g_playerState->Pets.begin(); it!=g_playerState->Pets.end(); ++it) {
            if (Pet *pet = (Pet*)(*it)) {
                if ( pet->pVo.type != "" &&  pet->ppVo.located_in != "" &&  pet->pVo.next_evolution.size() > 0) {
                    
                    FloatKeyValue _key =  pet->pVo.next_evolution[rand()% pet->pVo.next_evolution.size()];
                    GDPetVO vo = g_gameContent->getPetVO(_key.key);
                    
                    canTrade = true;
                    
                    if (pet) { //evolve
                        
                        //not enough items
                        for (vector<IntKeyValue>::iterator it = pet->pVo.items_needed_to_evolve.begin(); it != pet->pVo.items_needed_to_evolve.end(); ++it) {
                            IntKeyValue _item_needed = (IntKeyValue)(*it);
                            if (_item_needed.key != "") {
                                GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                                if (_itvo.type != "") {
                                    int _item_amount = g_playerState->getNItem(_item_needed.key);
                                    if (_item_amount < _item_needed.value) {
                                        canTrade = false;
                                        break;
                                    }
                                }
                            }
                        }
                        //                            //not enough coins
                        //                            if (g_playerState->player.balance.gold < pet->pVo.coins_needed_to_evolve) {
                        //
                        //                                canTrade = false;
                        //                            }
                        //pet level not at max
                        if (pet->ppVo.level < pet->pVo.max_level) {
                            
                            canTrade = false;
                        }
                        //market level not high enough
                        int unlocks_at_level = 0;
                        if (pet->pVo.next_evolution.size() > 1) {
                            //Find highest unlocks_at_level
                            for (int i=0; i<pet->pVo.next_evolution.size(); i++) {
                                FloatKeyValue _tmp_fkv = pet->pVo.next_evolution[i];
                                if (_tmp_fkv.key != "") {
                                    GDPetVO _tmp_pet_vo = g_gameContent->getPetVO(_tmp_fkv.key);
                                    if (unlocks_at_level < _tmp_pet_vo.unlocks_at_level) {
                                        unlocks_at_level = _tmp_pet_vo.unlocks_at_level;
                                    }
                                }
                            }
                        }
                        if (_building_market->pbVo.level < unlocks_at_level) {
                            
                            canTrade = false;
                        }
                        
                    }
                }
            }
            
            if(canTrade){
                break;
            }
        }
        if (canTrade) {
            if (_building_market) {
                _building_market->showBubbleTrade();
                return;
            }
        }
        
        for (vector<GDPetVO>::iterator it=g_gameContent->Pet.begin(); it!=g_gameContent->Pet.end(); ++it) {
            GDPetVO  vo = (GDPetVO)(*it);
            if ( vo.type != "") {
                if ( vo.items_needed_for_fusion.size() > 0) {
                    
                    canTrade = true;
                    //not enough items
                    for (vector<IntKeyValue>::iterator it = vo.items_needed_for_fusion.begin(); it != vo.items_needed_for_fusion.end(); ++it) {
                        IntKeyValue _item_needed = (IntKeyValue)(*it);
                        if (_item_needed.key != "") {
                            GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                            if (_itvo.type != "") {
                                int _item_amount = g_playerState->getNItem(_item_needed.key);
                                if (_item_amount < _item_needed.value) {
                                    canTrade = false;
                                    break;
                                }
                                
                            }
                        }
                    }
                    //                        //not enough coins
                    //                        if (g_playerState->player.balance.gold < vo.coins_needed_for_fusion) {
                    //
                    //                            canTrade = false;
                    //                        }
                    //market level not high enough
                    if (_building_market->pbVo.level < vo.unlocks_at_level) {
                        canTrade = false;
                    }
                    
                }
            }
            if(canTrade){
                break;
            }
            
        }
        
        if (canTrade) {
            if (_building_market) {
                _building_market->showBubbleTrade();
            }
        }
    }
    
}

void HUDPanel::updateBadgeAchievement() {
    
    if (menuButtons[HUDPanelMenuAchievement]) {
        menuButtons[HUDPanelMenuAchievement]->setBadge(AchievementDialog::checkAchievementPass());
    }
    
}

int HUDPanel::getOverrideExploreButtonBadgeNumber() {
    
    CCLog_("get-override_explore_button_badge_number:%d",CCUserDefault::sharedUserDefault()->getIntegerForKey("override_explore_button_badge_number"));
    
    return CCUserDefault::sharedUserDefault()->getIntegerForKey("override_explore_button_badge_number");
    
}

void HUDPanel::setOverrideExploreButtonBadgeNumber(int value_) {
    
    //This is also called at SceneBoost to init the values to DISABLED
    CCUserDefault::sharedUserDefault()->setIntegerForKey("override_explore_button_badge_number", value_);
    
    CCLog_("set-override_explore_button_badge_number:%d",CCUserDefault::sharedUserDefault()->getIntegerForKey("override_explore_button_badge_number"));
    
    updateHUD();
}

void HUDPanel::updateBadgeInbox() {

    if (menuButtons[HUDPanelMenuInbox]) {
        menuButtons[HUDPanelMenuInbox]->setBadge(g_playerState->getNewHistoryBadgeNumber());
    }

}

void HUDPanel::openIAPStore(RESOURCE_TYPE resourceType) {
    
    CScene *_someScene = (CScene*)g_currentScene;
    CCModalController *_modalController = _someScene->modalController;
    
    if (!_modalController || !_someScene) return;
    if (_modalController->getNStackView() != 0) return;
            
    CCDictionary *params = CCDictionary::create();
    std::string source = "";
    
    switch (resourceType) {
        case RESOURCE_TYPE_GEM:{
            source = "GemsTouch at HUDPanel";
        }break;
        case RESOURCE_TYPE_COIN:{
            source = "CoinTouch at HUDPanel";
        }break;
        case RESOURCE_TYPE_FOOD:{
            source = "FoodTouch at HUDPanel";
        }break;
        default:
            return;
            break;
    }
    
    params->setObject(CCString::create(source), "Source");
    
    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
    
    CSJson::Value eventParameters;
    eventParameters["source"] = source;
    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
    
    FBAppEventsWrapper::logEvent(EVENT_USER_OPEN_IAP_STORE);
    
    AppsFlyerWrapper::trackEvent(EVENT_USER_OPEN_IAP_STORE);
    
    CCSize _screenSize = getContentSize();
    IAPDialog *_dialog = IAPDialog::create(_modalController, resourceType);
    _dialog->setPosition(_screenSize*0.5f + kDialogIAPPosition);
    _someScene->addChild(_modalController, 100);
    _modalController->pushView(_dialog);
    
}

void HUDPanel::clearUIDialog() {
    
    //close Dialog
    if (SceneVille *_scene = (SceneVille*)g_currentScene) {
        if (typeid(*_scene) == typeid(SceneVille)) {
            if (_scene->mapToScreenUI) {
                _scene->mapToScreenUI->closeAllUIs();
            }
            
            if (IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap) {
                if (_isoMapBuilding->entityAction) {
                    _isoMapBuilding->entityAction->finish();
                }
            }
            return;
        }
    }
    
    if (SceneWorldMap *_scene = (SceneWorldMap*)g_currentScene) {
        if (typeid(*_scene) == typeid(SceneWorldMap)) {
            
            return;
        }
    }
}

void HUDPanel::createVersionLabel(cocos2d::CCLayer* parent)
{
    if(!m_showVersionNumber)
        return;
#if !(IS_RELEASE_BUILD)
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    m_versionNumber = CCLabelHelper::createBMFont(Versioning::GetBundleVersionWithGit().c_str(), kFontSizeLarge, true);
    m_versionNumber->setAnchorPoint(ccp(0.5f, 0.0f));
    m_versionNumber->setPosition(ccp(screenSize.width*0.5f, 0.f));
    parent->addChild(m_versionNumber);
#endif
}

#pragma mark - Main Menu Buttons Functions

bool HUDPanel::addExpBar() {
    
    if (m_expProgress) return false;
    
    CCSize _screenSize = getContentSize();
    CREATE_CHAR_BUFFER(_buffer, 64);

    createVersionLabel(statusLayer);

    // Name, Level, Exp
    m_expProgress = EXPProgress::create();
    m_expProgress->setPosition(ccp(m_expProgress->getContentSize().width*0.5f + getMargin(),
                                   _screenSize.height - m_expProgress->getContentSize().height*0.5f - getMargin()));
    m_expProgress->setSender(this);
    m_expProgress->setClearUI(true);
    m_expProgress->onFinished = updateHUDLevel;
    m_expProgress->setLevel(g_playerState->player.racing.level);
    statusLayer->addChild(m_expProgress);
    
    snprintf(_buffer, _buffer_size, "%s", g_playerState->player.info.name.c_str());
    m_nameLabel = CCLabelHelper::createBMFont(retinaMultiplier, _buffer, kFontSizeVeryLarge, true);
    m_nameLabel->setAnchorPoint(ccp(0, 0.5f));
    m_nameLabel->setPosition(ccp(m_expProgress->getPositionX() + m_expProgress->getContentSize().width*0.5f + getMargin(), _screenSize.height - m_nameLabel->getContentSize().height*0.5f - getMargin()));
    statusLayer->addChild(m_nameLabel);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Fan
    CCSprite* t_iconhRank = UIHelper::getSprite(UIHelper::HUDCounterMedalsNTrace);
    m_fanSprite = UIHelper::getSprite(UIHelper::NSMedal);
    m_fanSprite->setPosition(ccp(m_fanSprite->getContentSize().width*0.4f,
                                 t_iconhRank->getContentSize().height*0.55f));
    t_iconhRank->addChild(m_fanSprite);
    
    snprintf(_buffer, _buffer_size, "%s", CCStringHelper::createWithNumberFormat(g_playerState->player.balance.fan).c_str());
    m_fanLabel = CCLabelHelper::createBMFont(retinaMultiplier, _buffer, kFontSizeMedium, true);
    m_fanLabel->setPosition(ccp(t_iconhRank->getContentSize().width*0.65f,
                                t_iconhRank->getContentSize().height*0.5f));
    t_iconhRank->addChild(m_fanLabel);
    
    CCMenuItemScaleSprite* _fanMenuSprite = CCMenuItemScaleSprite::create(t_iconhRank, this, menu_selector(HUDPanel::onExpBarFanPressed));
    m_fanMenu = CCMenu::create(_fanMenuSprite, NULL);
    m_fanMenu->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);
    m_fanMenu->setContentSize(t_iconhRank->getContentSize());
    m_fanMenu->setPosition(ccp(m_expProgress->getPositionX() + m_expProgress->getContentSize().width*0.5f + m_fanMenu->getContentSize().width*0.5f + getMargin(), m_nameLabel->getPositionY() - m_nameLabel->getContentSize().height*0.5f - m_fanMenu->getContentSize().height*0.5f - getMargin()));
    statusLayer->addChild(m_fanMenu);

    return true;
    
}

bool HUDPanel::addHomeUserInfo() {
    
    if (photoSprite) true;
    
    CCSize _screenSize = getContentSize();
    
    float avatarScale = 0.6f;
    CCSprite *_frameSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::AvatarBorder));
    _frameSprite->setScale(avatarScale+kExtraAvatarFrameScale);
    _frameSprite->setPosition(CCPoint(getMargin() + _frameSprite->getContentSize().width*_frameSprite->getScale()*0.5f,
                                      _screenSize.height - getMargin() - _frameSprite->getContentSize().height*_frameSprite->getScale()*0.5f));
    addChild(_frameSprite, 2);
    _frameSprite->setVisible(false);
    
    photoSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::AvatarDefault));
    photoSprite->setScale(0.5f);
    photoSprite->setPosition(_frameSprite->getPosition());
    addChild(photoSprite, 1);
    
    CCLabelBMFont *_homeUserNameLabel = CCLabelHelper::createBMFont(g_opponentState->player.info.name.c_str(), kFontSizeVeryLarge+5, true);
    _homeUserNameLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    _homeUserNameLabel->setAlignment(kCCTextAlignmentLeft);
    _homeUserNameLabel->setPosition(_frameSprite->getPosition()
                                    + ccp(_frameSprite->getContentSize().width*_frameSprite->getScale()*0.5f
                                          +getMargin(),
                                          0.0f));
    addChild(_homeUserNameLabel);
    
    return true;
}

bool HUDPanel::addStreakInfo()
{
    if (streakButton ||
        g_playerState->player.info.win_streak <= 0) return false;
  
    streakButton = WinStreak::create(true, g_playerState->player.info.win_streak);
    if (m_expProgress) {
        streakButton->setPosition(ccp(getMargin() + streakButton->getContentSize().width*0.5,
                                      m_expProgress->getPositionY() -
                                      m_expProgress->getContentSize().height*0.5f -
                                      getMargin() -
                                      streakButton->getContentSize().height*0.5f));
    }else {
        CCSize _screenSize = getContentSize();
        streakButton->setPosition(ccp(getMargin() + streakButton->getContentSize().width*0.5,
                                      _screenSize.height -
                                      getMargin() -
                                      streakButton->getContentSize().height*0.5f));
    }
    addChild(streakButton);
    
    return true;
}

bool HUDPanel::addResourceBar(HUDPanelBar resourceBar_, ResourceProgressButton *refResourceBar)
{
    if (resourceBars[resourceBar_]) return false;
    
    CCSize _screenSize = getContentSize();
    
    CCNikButton *_hudAddGem = CCNikButton::create(UIHelper::getSprite(UIHelper::HUDAdd), NULL);
    
    switch (resourceBar_) {
        case HUDPanelBarGems: {
            resourceBars[resourceBar_] = ResourceProgressButton::create(UIHelper::getSprite(UIHelper::HUDGem));
            resourceBars[resourceBar_]->setTarget(this, menu_selector(HUDPanel::onGemsBarPressed), CCNikButtonEventTouchDown);
            _hudAddGem->setTarget(this, menu_selector(HUDPanel::onGemsBarPressed), CCNikButtonEventTouchDown);
            resourceBars[resourceBar_]->setAmount(g_playerState->player.balance.gem);
            resourceBars[resourceBar_]->setProgressBarEnable(false);
        }break;
        case HUDPanelBarFoods: {
            resourceBars[resourceBar_] = ResourceProgressButton::create(UIHelper::getSprite(UIHelper::HUDFood));
            resourceBars[resourceBar_]->setTarget(this, menu_selector(HUDPanel::onFoodsBarPressed), CCNikButtonEventTouchDown);
            _hudAddGem->setTarget(this, menu_selector(HUDPanel::onFoodsBarPressed), CCNikButtonEventTouchDown);
            resourceBars[resourceBar_]->setAmount(g_playerState->player.balance.food);
            resourceBars[resourceBar_]->setMaximumAmount(g_playerState->getTotalFoodStorageCapacity());
        }break;
        case HUDPanelBarCoins: {
            resourceBars[resourceBar_] = ResourceProgressButton::create(UIHelper::getSprite(UIHelper::HUDCoin));
            resourceBars[resourceBar_]->setTarget(this, menu_selector(HUDPanel::onCoinsBarPressed), CCNikButtonEventTouchDown);
            _hudAddGem->setTarget(this, menu_selector(HUDPanel::onCoinsBarPressed), CCNikButtonEventTouchDown);
            resourceBars[resourceBar_]->setAmount(g_playerState->player.balance.gold);
            resourceBars[resourceBar_]->setMaximumAmount(g_playerState->getTotalCoinStorageCapacity());
        }break;
        default:
            return false;
            break;
    }
    
    float additionalXPosition = 0.0f;

    additionalXPosition = _hudAddGem->getContentSize().width*-0.5f;
    _hudAddGem->setPosition(ccp(resourceBars[resourceBar_]->getContentSize().width,
                                resourceBars[resourceBar_]->getContentSize().height*0.5f));
    resourceBars[resourceBar_]->setLabelOffsetX(_hudAddGem->getContentSize().width*0.5f);

    if (refResourceBar) {
        resourceBars[resourceBar_]->setPosition(ccp(refResourceBar->getPositionX()
                                                    - refResourceBar->getContentSize().width*0.5f
                                                    - getMargin()*2.0f
                                                    - resourceBars[resourceBar_]->getContentSize().width*0.5f
                                                    + additionalXPosition,
                                                    _screenSize.height
                                                    - resourceBars[resourceBar_]->getContentSize().height*0.5
                                                    - getMargin()*1.5f));
    }else {
        resourceBars[resourceBar_]->setPosition(ccp(_screenSize.width - resourceBars[resourceBar_]->getContentSize().width*0.5f
                                                    - getMargin()*2.0f
                                                    + additionalXPosition,
                                                    _screenSize.height
                                                    - resourceBars[resourceBar_]->getContentSize().height*0.5f
                                                    - getMargin()*1.5f));
    }
    
    resourceBars[resourceBar_]->addChild(_hudAddGem);
    _hudAddGem->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);
    resourceBars[resourceBar_]->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);
    statusLayer->addChild(resourceBars[resourceBar_]);
    
    return true;
}

bool HUDPanel::addTrackEditBar() {
    
    return addTrackEditBar(resourceBars[HUDPanelBarGems]);
    
}

bool HUDPanel::addTrackEditBar(ResourceProgressButton *refResourceBar) {
    
    if (m_trackMenu) return false;
    
    CCSize _screenSize = getContentSize();
    
    CCSprite* _trackBgSprite = UIHelper::getSprite(UIHelper::HUDCounterMedalsNTrace);
    CCSprite* _trackIconSprite = UIHelper::getSprite(UIHelper::HUDTrace);
    _trackIconSprite->setPosition(ccp(_trackIconSprite->getContentSize().width*0.0f, _trackBgSprite->getContentSize().height*0.5f));
    _trackBgSprite->addChild(_trackIconSprite);

    m_trackLabel = CCLabelHelper::createBMFont(retinaMultiplier, "", kFontSizeMedium-5.0f, true);
    m_trackLabel->setAnchorPoint(ccp(1, 0.5f));
    m_trackLabel->setPosition(ccp(_trackBgSprite->getContentSize().width*0.95f, _trackBgSprite->getContentSize().height*0.5f));
    _trackBgSprite->addChild(m_trackLabel);

    CCMenuItemScaleSprite* _trackButtonSprite = CCMenuItemScaleSprite::create(_trackBgSprite, this, menu_selector(HUDPanel::onTrackEditPressed));
    m_trackMenu = CCMenu::create(_trackButtonSprite, NULL);
    m_trackMenu->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);
    m_trackMenu->setContentSize(_trackBgSprite->getContentSize());
    
    if (refResourceBar) {
        m_trackMenu->setPosition(ccp(_screenSize.width - (m_trackMenu->getContentSize().width*0.5) - getMargin(),
                                     refResourceBar->getPositionY()
                                     - _trackIconSprite->getContentSize().height * 0.75f
                                     - getMargin()));
    }else {
        m_trackMenu->setPosition(ccp(_screenSize.width - (m_trackMenu->getContentSize().width*0.5) - getMargin(),
                                     _screenSize.height
                                     - _trackIconSprite->getContentSize().height * 0.75f
                                     - getMargin()));
    }
    
    statusLayer->addChild(m_trackMenu);
    
    return true;
}

void HUDPanel::setSignalSignDisplayed(bool displayed) {
    
    CCLog("show: %d", displayed);
    //show
    if (displayed) {
        
        if (m_signalSignSprite) {
            m_signalSignSprite->setVisible(true);
            return;
        }
        
        CCSize s = CCDirector::sharedDirector()->getWinSize();
        m_signalSignSprite = UIHelper::getSprite(UIHelper::HUDSignalSign);
        CCPoint pos = ccp(s.width - getMargin() - m_signalSignSprite->getContentSize().width*0.5f,
                          getMargin() + m_signalSignSprite->getContentSize().height*0.5f);
        if (menuButtons[HUDPanelMenuMap]) {
            pos.x = (menuButtons[HUDPanelMenuMap]->getPositionX()>=0)?
            (s.width*0.5f+menuButtons[HUDPanelMenuMap]->getPositionX()):
            (-s.width*0.5f-menuButtons[HUDPanelMenuMap]->getPositionX());
            pos.y += menuButtons[HUDPanelMenuMap]->getContentSize().height;
        }
        m_signalSignSprite->setPosition(pos);
        statusLayer->addChild(m_signalSignSprite);
        
        CCAnimation *animation = CCAnimation::create();
        auto frames = UIHelper::getAnimationFrames(UIHelper::AnimationHUDSignalSign);
        for (auto &frame : frames) animation->addSpriteFrame(frame);
        animation->setLoops(1);
        animation->setDelayPerUnit(0.5f);
        CCAnimate *animate = CCAnimate::create(animation);
        m_signalSignSprite->runAction(CCRepeatForever::create(animate));
        
        return;
        
    }
    
    //hide
    if (!m_signalSignSprite) return;
    m_signalSignSprite->setVisible(false);
    
}

void HUDPanel::setEnableTrackSignBeating(bool enable_) {
    
    m_enableTrackSignBeating = enable_;
    if (m_trackStatusSign)
        m_trackStatusSign->setVisible(enable_);
}

void HUDPanel::setVisibilityAll(bool visible_)
{
    if (visible_ == false)
        setEnableTrackSignBeating(false);
    
    setVisibilityAllMainMenuButtons(visible_);
    setVisibilityExpBarAndName(visible_);
    setVisibilityFans(visible_);
    setVisibilityResourceBar(visible_);
    setVisibilitySettingButton(visible_);
    setVisibilityTrackEditBar(visible_);
}

void HUDPanel::setVisibilityResourceBar(bool visible_)
{
    if (resourceBars[HUDPanelBarGems])
        resourceBars[HUDPanelBarGems]->setVisible(visible_);
    if (resourceBars[HUDPanelBarFoods])
        resourceBars[HUDPanelBarFoods]->setVisible(visible_);
    if (resourceBars[HUDPanelBarCoins])
        resourceBars[HUDPanelBarCoins]->setVisible(visible_);
}

void HUDPanel::setVisibilityExpBarAndName(bool visible_)
{
    if (m_expProgress)
        m_expProgress->setVisible(visible_);
    if (m_nameLabel)
        m_nameLabel->setVisible(visible_);
}

void HUDPanel::setVisibilityFans(bool visible_)
{
    if (m_fanMenu)
        m_fanMenu->setVisible(visible_);
}

void HUDPanel::setVisibilityTrackEditBar(bool visible_)
{
    if (m_trackMenu)
        m_trackMenu->setVisible(visible_);
}

void HUDPanel::setVisibilitySettingButton(bool visible_)
{
    if (m_settingMenu)
        m_settingMenu->setVisible(visible_);
}

void HUDPanel::setVisibilityMainMenuButton(HUDPanelMenu menuButton_, bool visible_)
{
    if (menuButtons[menuButton_])
        menuButtons[menuButton_]->setVisible(visible_);
}

void HUDPanel::setVisibilityAllMainMenuButtons(bool visible_)
{
    setVisibilityMainMenuButton(HUDPanelMenuShop, visible_);
    setVisibilityMainMenuButton(HUDPanelMenuGacha, visible_);
    setVisibilityMainMenuButton(HUDPanelMenuAchievement, visible_);
    setVisibilityMainMenuButton(HUDPanelMenuFriends, visible_);
    setVisibilityMainMenuButton(HUDPanelMenuInbox, visible_);
    setVisibilityMainMenuButton(HUDPanelMenuMap, visible_);
    setVisibilityMainMenuButton(HUDPanelMenuHome, visible_);
}

bool HUDPanel::addSettingButton() {
    
    if (m_settingMenu)
        return false;
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCSprite* _settingButtonBGSprite = UIHelper::getSprite(UIHelper::HUDSettings);
    CCMenuItemScaleSprite* _settingButtonSprite = CCMenuItemScaleSprite::create(_settingButtonBGSprite, this, menu_selector(HUDPanel::onSettingPressed));
    m_settingMenu = CCMenu::create(_settingButtonSprite, NULL);
    
    //hack, this is to mirror the state of other buttons
    CCSprite *icon = UIHelper::getSprite(UIHelper::HUDAchievements);
    icon->setScale(kIconScale1);
    m_settingMenu->setPosition(ccp(s.width
                                   - getMargin()
                                   - _settingButtonBGSprite->getContentSize().width*0.5f,
                                   s.height
                                   - kHeightFromTop
                                   - getMargin()*2.0f
                                   - icon->getContentSize().height*icon->getScale()*1.5f));
    statusLayer->addChild(m_settingMenu);
    m_settingMenu->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);
    
    return true;
    
}

bool HUDPanel::addMainMenuButton(HUDPanelMenu menuButton_, HUDMenuButton *refMenuButton_, HUDButtonDefaultPosition defaultPosition_) {
    
    if (menuButtons[menuButton_])
        return false;
    
    
    switch (menuButton_) {
        case HUDPanelMenuShop: {
            menuButtons[menuButton_] = HUDMenuButton::create(NULL, UIHelper::getSpritePath(UIHelper::HUDBuildButton), "", 1);
            menuButtons[menuButton_]->setTarget(this, menu_selector(HUDPanel::onShopButtonPressed), CCMenuItemStateTouchDown);
        }break;
        case HUDPanelMenuGacha: {
            menuButtons[menuButton_] = HUDMenuButton::create(NULL, UIHelper::getSpritePath(UIHelper::HUDGatchaButton), "", 1);
            menuButtons[menuButton_]->setAnimation(UIHelper::AnimationHUDGatcha, 3.0f);
            menuButtons[menuButton_]->setTarget(this, menu_selector(HUDPanel::onGachaButtonPressed), CCMenuItemStateTouchDown);
        }break;
        case HUDPanelMenuAchievement: {
            menuButtons[menuButton_] = HUDMenuButton::create(NULL, UIHelper::getSpritePath(UIHelper::HUDAchievements), "", 1);
            menuButtons[menuButton_]->setTarget(this, menu_selector(HUDPanel::onAchievementPressed), CCMenuItemStateTouchDown);
        }break;
        case HUDPanelMenuFriends: {
            menuButtons[menuButton_] = HUDMenuButton::create(NULL, UIHelper::getSpritePath(UIHelper::HUDFriends), "", 1);
            menuButtons[menuButton_]->setTarget(this, menu_selector(HUDPanel::onFriendsButtonPressed), CCMenuItemStateTouchDown);
        }break;
        case HUDPanelMenuInbox: {
            menuButtons[menuButton_] = HUDMenuButton::create(NULL, UIHelper::getSpritePath(UIHelper::HUDInbox), "", 1);
            menuButtons[menuButton_]->setTarget(this, menu_selector(HUDPanel::onInboxButtonPressed), CCMenuItemStateTouchDown);
        }break;
        case HUDPanelMenuMap:
            menuButtons[menuButton_] = HUDMenuButton::create(NULL, UIHelper::getSpritePath(UIHelper::HUDMapButton), "", 1);
            menuButtons[menuButton_]->setTarget(this, menu_selector(HUDPanel::onMapButtonPressed), CCMenuItemStateTouchDown);
            break;
        case HUDPanelMenuHome:
            menuButtons[menuButton_] = HUDMenuButton::create(NULL, UIHelper::getSpritePath(UIHelper::HUDHomeButton), "", 1);
            menuButtons[menuButton_]->setTarget(this, menu_selector(HUDPanel::onHomeButtonPressed), CCMenuItemStateTouchDown);
            break;
        default:
            return false;
            break;
    }
    
    float defaultScale = 1.0f;
    
    switch (menuButton_) {
        case HUDPanelMenuAchievement:
        case HUDPanelMenuFriends:
        case HUDPanelMenuInbox: {
            defaultScale = kIconScale1;
        }break;
        case HUDPanelMenuMap:
        case HUDPanelMenuShop:
        case HUDPanelMenuHome: {
            defaultScale = kIconScale1;
        }break;
        case HUDPanelMenuGacha: {
            defaultScale = kIconScale2;
        }break;
        default:
            return false;
            break;
    }
    
    menuButtons[menuButton_]->setScale(defaultScale);
    menuButtons[menuButton_]->defaultScale = defaultScale;
    menuButtons[menuButton_]->activeScale = defaultScale * 0.9f;
    
    CCSize _screenSize = getContentSize();
    
    if (defaultPosition_ == POS_FIXED_DEFAULT) {
        
        switch (menuButton_) {
            case HUDPanelMenuAchievement: {
                menuButtons[menuButton_]->setPrePositionShowOnly(ccp(getMargin()
                                                                     + menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                     _screenSize.height
                                                                     - kHeightFromTop
                                                                     - getMargin()*2.0f
                                                                     - menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()*0.5f)
                                                                 -_screenSize*0.5f);
                menuButtons[menuButton_]->setBadgePosition(menuButtons[menuButton_]->getContentSize()*0.85f);
            }break;
            case HUDPanelMenuFriends: {
                menuButtons[menuButton_]->setPrePositionShowOnly(ccp(getMargin()
                                                                     + menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                     _screenSize.height
                                                                     - kHeightFromTop
                                                                     - getMargin()*3.0f
                                                                     - menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()*1.5f)
                                                                 -_screenSize*0.5f);
                menuButtons[menuButton_]->setBadgePosition(menuButtons[menuButton_]->getContentSize()*0.85f);
            }break;
            case HUDPanelMenuGacha: {
                
                CCSprite *icon = UIHelper::getSprite(UIHelper::HUDMapButton);
                icon->setScale(kIconScale1);
                
                menuButtons[menuButton_]->setPrePositionShowOnly(ccp(getMargin()
                                                                     + menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                     icon->getContentSize().height*icon->getScale()
                                                                     + getMargin()
                                                                     + menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()*0.5f)
                                                                 -_screenSize*0.5f);
                menuButtons[menuButton_]->setBadgePosition(menuButtons[menuButton_]->getContentSize()*0.85f);
            }break;
            case HUDPanelMenuInbox: {
                menuButtons[menuButton_]->setPrePositionShowOnly(ccp(_screenSize.width
                                                                     - getMargin()*0.5f
                                                                     - menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                     _screenSize.height
                                                                     - kHeightFromTop
                                                                     - getMargin()*2.0f
                                                                     - menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()*0.5f)
                                                                 -_screenSize*0.5f);
                menuButtons[menuButton_]->setBadgePosition(ccp(menuButtons[menuButton_]->getContentSize().width*0.15f,
                                                               menuButtons[menuButton_]->getContentSize().height*0.85f));
            }break;
            default:
                return false;
                break;
        }
        
    } else {
        if (refMenuButton_) {
            if (defaultPosition_ == POS_BOTTOM_ARRANGE_FROM_RIGHT) {
                menuButtons[menuButton_]->setPrePosition(
                                                         CCPoint( refMenuButton_->getPositionX()
                                                                 - refMenuButton_->getContentSize().width*0.5f
                                                                 - getMargin()
                                                                 - menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                 -_screenSize.height*0.5f
                                                                 + getMargin()
                                                                 + menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()*0.5f),
                                                         CCPoint( refMenuButton_->getPositionX()
                                                                 - refMenuButton_->getContentSize().width*0.5f
                                                                 - getMargin()
                                                                 - menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                 -_screenSize.height*0.5f
                                                                 - getMargin()
                                                                 - menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()));
            }else {
                menuButtons[menuButton_]->setPrePosition(
                                                         CCPoint( refMenuButton_->getPositionX()
                                                                 + refMenuButton_->getContentSize().width*0.5f
                                                                 + getMargin()
                                                                 + menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                 -_screenSize.height*0.5f
                                                                 + getMargin()
                                                                 + menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()*0.5f),
                                                         CCPoint( refMenuButton_->getPositionX()
                                                                 + refMenuButton_->getContentSize().width*0.5f
                                                                 + getMargin()
                                                                 + menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                 -_screenSize.height*0.5f
                                                                 - getMargin()
                                                                 - menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()));
            }
        }else {
            if (defaultPosition_ == POS_BOTTOM_ARRANGE_FROM_RIGHT) {
                menuButtons[menuButton_]->setPrePosition(
                                                         CCPoint( _screenSize.width*0.5f
                                                                 - getMargin()
                                                                 - menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                 -_screenSize.height*0.5f
                                                                 + getMargin()
                                                                 + menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()*0.5f),
                                                         CCPoint( _screenSize.width*0.5f
                                                                 - getMargin()
                                                                 - menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                 -_screenSize.height*0.5f
                                                                 - getMargin()
                                                                 - menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()));
            }else {
                menuButtons[menuButton_]->setPrePosition(
                                                         CCPoint(-_screenSize.width*0.5f
                                                                 + getMargin()
                                                                 + menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                 -_screenSize.height*0.5f
                                                                 + getMargin()
                                                                 + menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()*0.5f),
                                                         CCPoint(-_screenSize.width*0.5f
                                                                 + getMargin()
                                                                 + menuButtons[menuButton_]->getContentSize().width*menuButtons[menuButton_]->getScale()*0.5f,
                                                                 -_screenSize.height*0.5f
                                                                 - getMargin()
                                                                 - menuButtons[menuButton_]->getContentSize().height*menuButtons[menuButton_]->getScale()));
            }
        }
        
        if (menuButton_ == HUDPanelMenuShop ||
            menuButton_ == HUDPanelMenuMap)
        {
            menuButtons[menuButton_]->setBadgePosition(ccp(menuButtons[menuButton_]->getContentSize().width*0.85f,
                                                           menuButtons[menuButton_]->getContentSize().height*0.65f));
        }
    }
    
    bool _show_up = true;
    bool _animated = false;
    menuButtons[menuButton_]->setBadge(0);
    menuButtons[menuButton_]->show(_show_up, _animated);
    menuButtons[menuButton_]->setTag(menuButton_);
    
    if (!m_menuItemArray) {
        m_menuItemArray = CCArray::create();
    }
    m_menuItemArray->addObject(menuButtons[menuButton_]);
    
    return true;
    
}

void HUDPanel::buildMainMenuButtons() {
    
    CCAssert(m_menuItemArray != NULL, "m_menuItemArray must not be NULL!");
    if (!m_menuItemArray) return;
    
    if (mainMenu) {
        mainMenu->removeFromParent();
    }
    
    if (!m_mainMenuLayer) {
        m_mainMenuLayer = CCLayer::create();
        addChild(m_mainMenuLayer);
    }
    
    mainMenu = CCMenu::createWithArray(m_menuItemArray);
    mainMenu->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);
    m_mainMenuLayer->addChild(mainMenu);

}

int HUDPanel::getIndexOfMainMenuItemByTag(int tag_) {
    
    CCArray *_arr = mainMenu->getChildren();
    int n = _arr->count();
    for (int i=0; i<n; i++) {
        CCNode *_item = (CCNode*)_arr->objectAtIndex(i);
        if (_item->getTag() == tag_) {
            return i;
        }
    }
    return -1;
    
}

void HUDPanel::showAllMainMenuButtons(bool show_) {
    
    float _delay = 0.0f;
//    if (menuButtons[HUDPanelMenuFriends]) {
//        menuButtons[HUDPanelMenuFriends]->show(show_, true, _delay); _delay += 0.1f;
//    }
//    if (menuButtons[HUDPanelMenuInbox]) {
//        menuButtons[HUDPanelMenuInbox]->show(show_, true, _delay); _delay += 0.1f;
//    }
    if (menuButtons[HUDPanelMenuMap]) {
        menuButtons[HUDPanelMenuMap]->show(show_, true, _delay); _delay += 0.1f;
    }
    
    _delay = 0.0f;
//    if (menuButtons[HUDPanelMenuAchievement]) {
//        menuButtons[HUDPanelMenuAchievement]->show(show_, true, _delay); _delay += 0.1f;
//    }
//    if (menuButtons[HUDPanelMenuGacha]) {
//        menuButtons[HUDPanelMenuGacha]->show(show_, true, _delay); _delay += 0.1f;
//    }
    if (menuButtons[HUDPanelMenuShop]) {
        menuButtons[HUDPanelMenuShop]->show(show_, true, _delay); _delay += 0.1f;
    }
    
}

bool HUDPanel::areAllMainMenuButtonsShowed() {
    
    int number_of_shows = 0;
    int number_of_noaction = 0;
    
    for (int i=0; i<HUDPanelMenuTotal; i++) {
        if (!menuButtons[i]) continue;
        if (!menuButtons[i]->isShowed()) continue;
        number_of_shows ++;
        if (menuButtons[i]->numberOfRunningActions() > 0) continue;
        number_of_noaction ++;
    }
    
    if (number_of_shows > 0 &&
        number_of_shows == number_of_noaction) {
        return true;
    }
    
    return false;
    
}

void HUDPanel::setDangerButtonsEnabled(bool enabled_) {
    
    m_dangerButtonsEnabled = enabled_;
    
    CCLog_("setDangerButtonsEnabled:m_dangerButtonsEnabled:%d",m_dangerButtonsEnabled);
    
    if (resourceBars[HUDPanelBarGems])
        resourceBars[HUDPanelBarGems]->setEnabled(m_dangerButtonsEnabled);
    if (resourceBars[HUDPanelBarFoods])
        resourceBars[HUDPanelBarFoods]->setEnabled(m_dangerButtonsEnabled);
    if (resourceBars[HUDPanelBarCoins])
        resourceBars[HUDPanelBarCoins]->setEnabled(m_dangerButtonsEnabled);
    if (m_trackMenu)
        m_trackMenu->setEnabled(m_dangerButtonsEnabled);
    if (m_settingMenu)
        m_settingMenu->setEnabled(m_dangerButtonsEnabled);
    
    if (menuButtons[HUDPanelMenuShop])
        menuButtons[HUDPanelMenuShop]->setEnabled(m_dangerButtonsEnabled);
    if (menuButtons[HUDPanelMenuGacha])
        menuButtons[HUDPanelMenuGacha]->setEnabled(m_dangerButtonsEnabled);
    if (menuButtons[HUDPanelMenuAchievement])
        menuButtons[HUDPanelMenuAchievement]->setEnabled(m_dangerButtonsEnabled);
    if (menuButtons[HUDPanelMenuFriends])
        menuButtons[HUDPanelMenuFriends]->setEnabled(m_dangerButtonsEnabled);
    if (menuButtons[HUDPanelMenuInbox])
        menuButtons[HUDPanelMenuInbox]->setEnabled(m_dangerButtonsEnabled);
    if (menuButtons[HUDPanelMenuMap])
        menuButtons[HUDPanelMenuMap]->setEnabled(m_dangerButtonsEnabled);
    
}

void HUDPanel::updateResourceBar(HUDPanelBar bar_) {
    
    switch (bar_) {
        case HUDPanelBarGems: {
            int currentBalanceOfGems = g_playerState->player.balance.gem;
            if (resourceBars[HUDPanelBarGems] &&
                resourceBars[HUDPanelBarGems]->addTargetAmount(currentBalanceOfGems)) {
                resourceBars[HUDPanelBarGems]->processTargetAmount();
            }
        }break;
        case HUDPanelBarFoods: {
            int currentBalanceOfFoods = g_playerState->player.balance.food;
            int totalFoodStorageCapacity = g_playerState->getTotalFoodStorageCapacity();
            if (resourceBars[HUDPanelBarFoods] &&
                resourceBars[HUDPanelBarFoods]->addTargetAmount(currentBalanceOfFoods,
                                                                totalFoodStorageCapacity, AUDIO_NONE)) {
                resourceBars[HUDPanelBarFoods]->processTargetAmount();
            }else if (resourceBars[HUDPanelBarFoods]) {
                resourceBars[HUDPanelBarFoods]->setMaximumAmountAnimated(totalFoodStorageCapacity);
            }
        }break;
        case HUDPanelBarCoins: {
            int currentBalanceOfCoins = g_playerState->player.balance.gold;
            int totalCoinStorageCapacity = PlayerState::getInstance()->getTotalCoinStorageCapacity();
            if (resourceBars[HUDPanelBarCoins] &&
                resourceBars[HUDPanelBarCoins]->addTargetAmount(currentBalanceOfCoins,
                                                                totalCoinStorageCapacity, AUDIO_NONE)) {
                resourceBars[HUDPanelBarCoins]->processTargetAmount();
            }else if (resourceBars[HUDPanelBarCoins]) {
                resourceBars[HUDPanelBarCoins]->setMaximumAmountAnimated(totalCoinStorageCapacity);
            }
        }break;
        case HUDPanelBarTotal:
        default:
            break;
    }
    
}

void HUDPanel::updateAllResourceBars() {
    
    for (int i=0; i<HUDPanelBarTotal; i++) {
        updateResourceBar((HUDPanelBar)i);
    }
    
}

#pragma mark - Button callback functions

void HUDPanel::onExpBarFanPressed(cocos2d::CCNode *sender) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    CCSize popoverSize;// = CCSize(titleLabel->getContentSize().width*10.5 , titleLabel->getContentSize().height*7.5)*sizeMultiplier;
    CCLabelBMFont *titleLabel = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["hud_popup_fans_title"].c_str(), kFontSizeLarge, kTextColorDark);
    CCLabelBMFont *detailLabel = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["hud_popup_fans_description"].c_str(), kFontSizeMedium, kTextColorDark);
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    int _visitors_per_hour = g_playerState->getVisitorsPerHour();
    snprintf(_buffer, _buffer_size, g_gameContent->Word["hud_popup_coins_visitors_per_hour"].c_str(), _visitors_per_hour);
    CCLabelBMFont *visitorsPerHourLabel = CCLabelHelper::createBMFont(retinaMultiplier, _buffer, kFontSizeMedium, kTextColorGray);
    
    detailLabel->setWidth(titleLabel->getContentSize().width*4.5f);
    detailLabel->setAnchorPoint(ccp(0.0f,1.0f));
    switch (targetDevice) {
        case TD_IPHONE: {
            popoverSize = CCSize(titleLabel->getContentSize().width*10.5 , titleLabel->getContentSize().height*6.5)*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            
            titleLabel->setPosition(ccp(30+ titleLabel->getContentSize().width*0.5f, popover->getContentSize().height - titleLabel->getContentSize().height*0.8f ));
            popover->addChild(titleLabel);
            
            detailLabel->setPosition(ccp( titleLabel->getContentSize().width*2.75f, titleLabel->getPositionY()-titleLabel->getContentSize().height*1.4));
            popover->addChild(detailLabel);
            
            popover->presentPopover(m_fanMenu, true);
        }break;
        case TD_IPHONE_RETINA:{
            popoverSize = CCSize(titleLabel->getContentSize().width*9.5 , titleLabel->getContentSize().height*4.5+detailLabel->getContentSize().height*(detailLabel->getContentSize().width/(titleLabel->getContentSize().width*4.5f)))*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            
            titleLabel->setPosition(ccp(30+ titleLabel->getContentSize().width*0.5f, popover->getContentSize().height - titleLabel->getContentSize().height*0.6f ));
            popover->addChild(titleLabel);
            
            detailLabel->setPosition(ccp( titleLabel->getContentSize().width*0.6f   ,titleLabel->getPositionY()-titleLabel->getContentSize().height*0.3f));
            popover->addChild(detailLabel);
            
            visitorsPerHourLabel->setPosition(ccp(titleLabel->getContentSize().width*2.4f  ,detailLabel->getPositionY()-titleLabel->getContentSize().height*1.6f));
            popover->addChild(visitorsPerHourLabel);
            
            popover->presentPopover(m_fanMenu, true);
        }break;
        case TD_IPAD:       {
            popoverSize = CCSize(ccp(titleLabel->getContentSize().width*5.5, titleLabel->getContentSize().height*4));
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            
            titleLabel->setPosition(ccp(30+ titleLabel->getContentSize().width*0.5f, popover->getContentSize().height - titleLabel->getContentSize().height*0.8f ));
            popover->addChild(titleLabel);
            
            detailLabel->setPosition(ccp(titleLabel->getContentSize().width*0.6f   ,titleLabel->getPositionY()-titleLabel->getContentSize().height*0.5f));
            popover->addChild(detailLabel);
            
            visitorsPerHourLabel->setPosition(ccp(titleLabel->getContentSize().width*2.4f  ,detailLabel->getPositionY()-titleLabel->getContentSize().height*1.6f));
            popover->addChild(visitorsPerHourLabel);
            
            popover->presentPopover(m_fanMenu, true);
        }  break;
        case TD_IPAD_RETINA:    {
            popoverSize = CCSize(titleLabel->getContentSize().width*8.5 , titleLabel->getContentSize().height*4.5+detailLabel->getContentSize().height*(detailLabel->getContentSize().width/(titleLabel->getContentSize().width*4.5f)))*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            
            titleLabel->setPosition(ccp(150, popover->getContentSize().height - titleLabel->getContentSize().height*1.5f ));
            popover->addChild(titleLabel);
            detailLabel->setWidth(titleLabel->getContentSize().width*6);
            detailLabel->setPosition(ccp(110   ,titleLabel->getPositionY()-titleLabel->getContentSize().height));
            popover->addChild(detailLabel);
            
            visitorsPerHourLabel->setPosition(ccp(titleLabel->getContentSize().width*4.4f  ,detailLabel->getPositionY()-titleLabel->getContentSize().height*2.8f));
            popover->addChild(visitorsPerHourLabel);
            
            
            popover->presentPopover(m_fanMenu, true);
        }  break;
    }
    
    SceneVille *_scene = dynamic_cast<SceneVille*>(g_currentScene);
    if (!_scene) return;
    ((IsoMapBuilding*)_scene->isoMap)->placingStuffFromPlaceHolder(true, (int)BuildingBuildTypeNormal);
    
}

void HUDPanel::onGemsBarPressed(cocos2d::CCNode *sender_) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    clearUIDialog();
    openIAPStore(RESOURCE_TYPE_GEM);
    
}

void HUDPanel::onFoodsBarPressed(cocos2d::CCNode *sender_) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    clearUIDialog();
    openIAPStore(RESOURCE_TYPE_FOOD);
    
}

void HUDPanel::onCoinsBarPressed(cocos2d::CCNode *sender_) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    clearUIDialog();
    openIAPStore(RESOURCE_TYPE_COIN);
    
}

void HUDPanel::onTrackEditPressed(CCObject *sender_) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    if (_isoMapBuilding->placeHolder) {
        if (_isoMapBuilding->placeHolder->track) {
            return;
        }
    }
    
    if (Track* track = _scene->isoMap->getTrackAt(0)) {
        
        _scene->scrollView->zoomOnPosition(track->getPosition()
                                             -(_scene->isoMap->getContentSize()/4.f)
                                             , 1.4f
                                             , 1.0f);
        
        _isoMapBuilding->addTrackWithPlaceHolder(track);
        
    }else {
        GDTrackVO _vo = g_gameContent->getTrackVO("track_dirt");
        if (_vo.type != "") {
            _isoMapBuilding->addTrackWithPlaceHolder(_vo);
        }
    }
    
    _isoMapBuilding->closeAllMapUIs();
    
}

void HUDPanel::onSettingPressed(cocos2d::CCNode *sender) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    SettingDialog *_dialog = SettingDialog::create(_scene->modalController);
    _dialog->setPosition(_screenSize*0.5f);
    _scene->addChild(_scene->modalController, kModalControllerZOrder);
    _scene->modalController->pushView(_dialog);
}

void HUDPanel::onShopButtonPressed(cocos2d::CCNode *sender_) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    if (!g_sceneManager->transition->isFinished())
        return;

    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (!_scene->shopPanel) return;
    
    _scene->shopPanel->openPanel();
    showAllMainMenuButtons(false);
    
}

void HUDPanel::onGachaButtonPressed(cocos2d::CCNode *sender_) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (_scene->gachaPanel) return;
        
    g_gameAudio->clearEffectFadeOut(3.0f);
    g_gameAudio->playMusicFadeIn(MUSIC_GACHA,3.0f);
    
    _scene->presentGachaPanel();
    
    ((IsoMapBuilding*)_scene->isoMap)->placingStuffFromPlaceHolder(true, (int)BuildingBuildTypeNormal);
    
}

void HUDPanel::onAchievementPressed(cocos2d::CCNode *sender) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    clearUIDialog();
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (_scene->modalController->getNStackView() != 0) return;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    achievementDialog = AchievementDialog::create(_scene->modalController);
    achievementDialog->setPosition(_screenSize*0.5+kDialogAchievementPosition);
    _scene->addChild(_scene->modalController, kModalControllerZOrder);
    _scene->modalController->pushView(achievementDialog);
    
}

void HUDPanel::onMapButtonPressed(CCNode *sender_) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    bool _isWaiting = BackendService::getInstance()->isWaitingForQueue() || BackendService::getInstance()->isWaitingForResponse();
    if (_isWaiting) {
        setSignalSignDisplayed(true);
        return;
    }
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene) return;
    
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    if (!_isoMapBuilding) return;
    _isoMapBuilding->isTrackComplete();
    
    if (m_trackStatusSign && m_enableTrackSignBeating) {
        if (m_trackStatusSign->getTag() == 0) { //tag = 0 indicates incomplete
            if (g_playerState->player.racing.host == "") {
                g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, g_gameContent->Word["pick_host_to_complete_track"]);
            }else {
                
                clearUIDialog();
                
                if (_scene->modalController->getNStackView() == 0) {
                    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                    TutorialDialog* tutorialDialog = TutorialDialog::create(_scene->modalController);
                    tutorialDialog->setPosition(_screenSize*0.5f);
                    _scene->addChild(_scene->modalController, kModalControllerZOrder);
                    _scene->modalController->pushView(tutorialDialog);
                }
                
            }
            trackSignBeating();
            return;
        }
    }
    
    if (m_didPressRaceBtn) {
        return;
    }
    
    m_didPressRaceBtn = true;
    
    _isoMapBuilding->placingStuffFromPlaceHolder(true, (int)BuildingBuildTypeNormal);
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_gameAudio->clearEffectFadeOut(3.0f);
    
    if (getOverrideExploreButtonBadgeNumber() != HUD_OVERRIDE_EXPLORE_BUTTON_BADGE_NUMBER_DISABLE) {
        setOverrideExploreButtonBadgeNumber(HUD_OVERRIDE_EXPLORE_BUTTON_BADGE_NUMBER_DISABLE);
    }
    
    CustomEventsHelper::Instance().checkAndIncreaseTimesUsedFor(kIdleAnimationMapButton);
    
    if (g_playerState->WorldMapVillages.size() == 0) {
        _scene->fadeOutButWaitForWorldMap(SCENE_WORLDMAP);
    }else {
        _scene->fadeOutThisSceneAndChangeTo(SCENE_WORLDMAP);
    }
    
}

void HUDPanel::onFriendsButtonPressed(cocos2d::CCNode *sender_) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_sceneManager->dismissPlaceHolder();
    
    clearUIDialog();
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (_scene->modalController->getNStackView() != 0) return;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    LeaderboardDialog* leaderboardDialog = LeaderboardDialog::create(_scene->modalController);
    leaderboardDialog->setPosition(_screenSize*0.5f+kDialogLeaderBoardPosition);
    _scene->addChild(_scene->modalController, kModalControllerZOrder);
    _scene->modalController->pushView(leaderboardDialog);
    
}

void HUDPanel::onInboxButtonPressed(cocos2d::CCNode *sender_) {

    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_sceneManager->dismissPlaceHolder();

    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene) return;
    if (_scene->modalController->getParent()) return;

    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    InboxDialog *_inboxDialog = InboxDialog::create(_scene->modalController, kDialogInboxSize);
    _inboxDialog->setPosition(_screenSize*0.5f + kDialogInboxPosition);
    _scene->addChild(_scene->modalController, kModalControllerZOrder);
    _scene->modalController->pushView(_inboxDialog);
    
}

void HUDPanel::onHomeButtonPressed(cocos2d::CCNode *sender_) {
    
    //TODO: onHomeButtonPressed
    CCLog_("onHomeButtonPressed!");
    g_sceneManager->dismissPlaceHolder();
    
}

#pragma mark - callback functions
////////////////////////////////////////////////////////
// callback method

void HUDPanel::gachaPanelDidClosing(GachaPanel *gachaPanel) {
    
    menuButtons[HUDPanelMenuGacha]->setBadge(g_playerState->player.info.free_gacha);
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    _scene->gachaPanel = NULL;
    
}

void HUDPanel::onMapDataRetrieved(void *sender, std::string resp_str) {

    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parseWorldMapVillages(_root);
    _playerState->parseChestGemsData(_root);
    _playerState->parseHistoryItemData(_root);
    Json_dispose(_root);
    
    HUDPanel* _this = (HUDPanel*)sender;
    if (!_this) return;

    int number_of_new_villages = 0;
    CREATE_CHAR_BUFFER(_buffer, 16);
    CREATE_CHAR_BUFFER(_buffer2, 64);

    for (map<int, PlayerWorldMapVillageVO>::iterator it = g_playerState->WorldMapVillages.begin();
         it != g_playerState->WorldMapVillages.end(); ++it) {

        snprintf(_buffer, _buffer_size, "village%d", (*it).first);
        snprintf(_buffer2, _buffer2_size, "timestamp%d", (*it).first);

        if ((*it).second.status == kPWMVStatus_Other &&
                CCUserDefault::sharedUserDefault()->getIntegerForKey(_buffer) == 1 &&
                g_playerState->Checklist[TutSecondRacePart1::key()]) {
            number_of_new_villages ++;
        }
    }

    if (_this->getOverrideExploreButtonBadgeNumber() == HUD_OVERRIDE_EXPLORE_BUTTON_BADGE_NUMBER_DISABLE) {
        if (_this->menuButtons && _this->menuButtons[HUDPanelMenuMap])
        {
            _this->menuButtons[HUDPanelMenuMap]->setBadge(number_of_new_villages);
        }
    } else {
        if (_this->menuButtons && _this->menuButtons[HUDPanelMenuMap])
        {
            _this->menuButtons[HUDPanelMenuMap]->setBadge(_this->getOverrideExploreButtonBadgeNumber());
        }
    }

    _this->updateBadgeInbox();

}

#pragma mark - BackndService callback

void HUDPanel::onBackendServiceActive(void *sender) {
    
    HUDPanel* _this = (HUDPanel*)sender;
    if (!_this || typeid(*_this) != typeid(HUDPanel)) return;
    
    bool _isWaiting = BackendService::getInstance()->isWaitingForQueue() || BackendService::getInstance()->isWaitingForResponse();
    if (_isWaiting) return;
    _this->setSignalSignDisplayed(false);
    
}
