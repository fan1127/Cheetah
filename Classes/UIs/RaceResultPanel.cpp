//
//  RaceResultPanel.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/2/2557 BE.
//
//

#include "RaceResultPanel.h"
using namespace std;

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Scenes/SceneRace.h"
extern CCScene* g_currentScene;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Entities/PlayerState.h"
extern PlayerState* g_opponentState;

extern float sizeMultiplier;

#include "../Extensions/CCLabelHelper.h"
#include "../Helpers/CCSpriteHelper.h"
#include "../Helpers/CCStringHelper.h"
#include "../Helpers/CCNodeHelper.h"

#include "../Helpers/AdsManagerWrapper.h"

RaceResultPanel* RaceResultPanel::create(Pet *pet) {
    
    RaceResultPanel *obj = new RaceResultPanel();
    if (!obj->init(pet)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool RaceResultPanel::init(Pet *pet)
{
    if (!CCLayer::init()) {
        return false;
    }
    
    this->setTouchEnabled(true);
    this->setTouchMode(kCCTouchesOneByOne);
    
    m_pet = pet;
    coins_reward = 0;
    fans_won = 0;
    
    m_buttonEnabled = true;
    m_startEnabled = true;
    
    raceResultState = NONE;
    
    this->setVisible(false);
    
    canTouch=true;
    
    return true;
}

void RaceResultPanel::start() {

    if (m_pet->petResult == WIN_RESULT) {
        raceResultState = WIN;
    } else {
        raceResultState = LOSE;
    }
    
    if (!m_startEnabled) return;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    CCSprite *bgSpotlightSprite = CCSprite::createWithSpriteFrameName("oldversion/_bg_spotlight.png");
    bgSpotlightSprite->setScaleX((_screenSize.width+20.0f*sizeMultiplier)/bgSpotlightSprite->getContentSize().width);
    bgSpotlightSprite->setScaleY((_screenSize.height+20.0f*sizeMultiplier)/bgSpotlightSprite->getContentSize().height);
    bgSpotlightSprite->setPosition(this->getContentSize()*0.5f);
    addChild(bgSpotlightSprite);
    setVisible(true);
    
    PlayerState::getInstance()->racedAtLeastOnce = true;
    
    if (AdsManagerWrapper::meetAdsDisplayingRequirements())
    {
        AdsManagerWrapper::cacheInterstitial();
    }
    
    if (m_pet->petResult == WIN_RESULT) {
        createWinResult();
    } else {
        createLoseResult();
    }
    
}

void RaceResultPanel::createWinResult() {
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    if (Building* _garage = _playerState->getBuilding(kBuildingGarage, 0)) {
        coins_reward = g_gameContent->cal_coins_reward(_garage->pbVo.level,
                                                       g_opponentState->coinsReward,
                                                       _playerState->player.info.win_streak);
    } else {
        coins_reward = g_gameContent->cal_coins_reward(0,
                                                       g_opponentState->coinsReward,
                                                       _playerState->player.info.win_streak);
    }
    fans_won = g_gameContent->cal_fans_won(g_opponentState->player.info.bot);
    
    _playerState->addGold(coins_reward);
    
    m_spinningGlowSprite = CCSprite::create("common/vfx/spinning_glow.png");
    m_spinningGlowSprite->setVisible(false);
    this->addChild(m_spinningGlowSprite);
    
    m_placeCircleSprite = CCSprite::createWithSpriteFrameName("oldversion/_place_1st_circle.png");
    m_placeCircleSprite->setVisible(false);
    this->addChild(m_placeCircleSprite);
    
    m_ribbonSprite = UIHelper::getScalableSprite(UIHelper::Ribbon);
    m_ribbonSprite->setContentSize(m_ribbonSprite->getContentSize()*1.2f);
    m_ribbonSprite->setVisible(false);
    this->addChild(m_ribbonSprite);
    
    m_pet_spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/pet/", m_pet->pVo.src.c_str());
    m_pet_spine->setVisible(false);
    m_pet_spine->setAnchorPoint(ccp(0.5f, 0.5f));
    this->addChild(m_pet_spine, 0);
    
    m_placeLabel = CCLabelHelper::createBMFont(g_gameContent->Word["raceresult_1st_place"].c_str(), 80, true);
    m_placeLabel->setVisible(false);
    this->addChild(m_placeLabel);
    
    m_fanSprite = CCNikButton::create(UIHelper::getSprite(UIHelper::NSMedal), NULL);
    m_fanSprite->setTarget(this, menu_selector(RaceResultPanel::onFan), CCNikButtonEventTouchDown);
    m_fanSprite->setVisible(false);
    this->addChild(m_fanSprite);
    
    GDItemVO itemVO = g_gameContent->getItemVO(g_opponentState->itemDrops);
    _playerState->itemsToAddToBarn.push_back(itemVO);
    
    m_itemSprite = CCNikButton::create(CCSprite::createWithSpriteFrameName(itemVO.src.c_str()), NULL);
    m_itemSprite->setTarget(this, menu_selector(RaceResultPanel::onItem), CCNikButtonEventTouchDown);
    m_itemSprite->setVisible(false);
    this->addChild(m_itemSprite);
        
    m_coinSprite = CCNikButton::create(UIHelper::getSprite(UIHelper::HUDCoin), NULL);
    m_coinSprite->setTarget(this, menu_selector(RaceResultPanel::onCoin), CCNikButtonEventTouchDown);
    m_coinSprite->setVisible(false);
    this->addChild(m_coinSprite);
    
    m_fanLabel = CCLabelHelper::createBMFont(CCString::createWithFormat("%d",fans_won)->getCString(), 80, true);
    m_fanLabel->setVisible(false);
    this->addChild(m_fanLabel);
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    Building *_itemStorage = _playerState->getBuilding(kBuildingItem, 0);
    
    int nslot = 0;
    if (_itemStorage) nslot = _itemStorage->bVo.param_1;
    
    ccColor3B itemLabelColor = ccc3(255, 255, 255);
    
    if (_playerState->Items.size() >= nslot) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, g_gameContent->Word["item_barn_is_full_msg"].c_str());
        snprintf(_buffer, _buffer_size, "%d", 0);
        itemLabelColor = MSGN_ERROR_COLOR;
    } else {
        snprintf(_buffer, _buffer_size, "%d", 1);
    }
    
    m_itemLabel = CCLabelHelper::createBMFont(_buffer, 80, true, itemLabelColor);
    m_itemLabel->setVisible(false);
    this->addChild(m_itemLabel);
    
    snprintf(_buffer, _buffer_size, "%s", CCStringHelper::createWithNumberFormat(coins_reward).c_str());
    
    m_coinLabel = CCLabelHelper::createBMFont(_buffer, 80, true);
    m_coinLabel->setVisible(false);
    this->addChild(m_coinLabel);
    
    m_continueButton = CCNikButton::create(UIHelper::getSprite(UIHelper::NotificationYes), NULL);
    m_continueButton->setVisible(false);
    this->addChild(m_continueButton);
    m_continueButton->setTarget(this, menu_selector(RaceResultPanel::continueButton), CCNikButtonEventTouchUpInside);
    
    CCLabelBMFont *continueLabel = CCLabelHelper::createBMFont(g_gameContent->Word["raceresult_button_continue"].c_str(), kFontSizeLarge, true);
    continueLabel->setPosition(m_continueButton->getContentSize()*0.5f
                               +ccp(0, 6.0f * sizeMultiplier));
    m_continueButton->addChild(continueLabel);
    
    //position
    m_spinningGlowSprite->setPosition(this->getContentSize()*0.5f + ccp(0, m_placeCircleSprite->getContentSize().height*2.0f*0.25f));
    m_placeCircleSprite->setAnchorPoint(ccp(0.5, 0));
    m_placeCircleSprite->setPosition(this->getContentSize()*0.5f);
    m_ribbonSprite->setPosition(this->getContentSize()*0.5f);
    m_fanSprite->setPosition(m_ribbonSprite->getPosition()
                             + ccp(-m_ribbonSprite->getContentSize().width*2.0f*0.1f,
                                   -m_ribbonSprite->getContentSize().height*2.0f*0.28f
                                   - m_fanSprite->getContentSize().height*0.5f));
    m_itemSprite->setPosition(m_ribbonSprite->getPosition()
                              + ccp(0, -m_ribbonSprite->getContentSize().height*2.0f*0.28f
                                    - m_fanSprite->getContentSize().height*0.5f));
    m_coinSprite->setPosition(m_ribbonSprite->getPosition()
                              + ccp(m_ribbonSprite->getContentSize().width*2.0f*0.1f,
                                -m_ribbonSprite->getContentSize().height*2.0f*0.28f
                                    - m_fanSprite->getContentSize().height*0.5f));
    m_fanLabel->setPosition(m_fanSprite->getPosition()
                            + ccp(0,- m_fanLabel->getContentSize().height*0.5f - m_fanSprite->getContentSize().height*0.5f));
    m_itemLabel->setPosition(m_itemSprite->getPosition()
                             + ccp(0,- m_fanLabel->getContentSize().height*0.5f - m_fanSprite->getContentSize().height*0.5f));
    m_coinLabel->setPosition(m_coinSprite->getPosition()
                             + ccp(0,- m_fanLabel->getContentSize().height*0.5f - m_fanSprite->getContentSize().height*0.5f));
    m_placeLabel->setPosition(m_ribbonSprite->getPosition()
                              + ccp(0, m_ribbonSprite->getContentSize().height*2.0f*0.055f));
    m_continueButton->setPosition(m_itemLabel->getPosition()
                                  + ccp(0, - m_itemLabel->getContentSize().height*0.5f - m_continueButton->getContentSize().height*0.5f));
    m_pet_spine->setPosition(m_ribbonSprite->getPosition()
                             + ccp(0, m_ribbonSprite->getContentSize().height*2.0f*0.3f));
    
    //beginning state
    m_spinningGlowSprite->setOpacity(0);
    m_spinningGlowSprite->setScale(4*sizeMultiplier);
    m_placeCircleSprite->setScale(0);
    m_pet_spine->setScale(0);
    m_placeLabel->setScale(0);
    m_ribbonSprite->setScale(0);
    m_fanSprite->setScale(0);
    m_fanLabel->setScale(0);
    m_itemSprite->setScale(0);
    m_itemLabel->setScale(0);
    m_coinSprite->setScale(0);
    m_coinLabel->setScale(0);
    m_continueButton->setScale(0);
        
    animateWinResult();
}

void RaceResultPanel::animateWinResult()
{
    
    m_spinningGlowSprite->setVisible(true);
    m_placeCircleSprite->setVisible(true);
    m_pet_spine->setVisible(true);
    m_placeLabel->setVisible(true);
    m_ribbonSprite->setVisible(true);
    m_fanSprite->setVisible(true);
    m_fanLabel->setVisible(true);
    m_itemSprite->setVisible(true);
    m_itemLabel->setVisible(true);
    m_coinSprite->setVisible(true);
    m_coinLabel->setVisible(true);
    m_continueButton->setVisible(true);
    
    
    m_spinningGlowSprite->runAction(CCRepeatForever::create(CCRotateBy::create(10.0f, 360)));
    m_spinningGlowSprite->runAction(CCSequence::create(CCDelayTime::create(1.2f),
                                                       CCFadeTo::create(1.0f, 0.6f*255.f),
                                                       NULL));
    m_ribbonSprite->runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                                 CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                                 NULL));
    m_placeCircleSprite->runAction(CCSequence::create(CCDelayTime::create(1.1f),
                                                      CCEaseElasticOut::create(CCScaleTo::create(1.0f, 2.0f)),
                                                      NULL));
    m_placeLabel->runAction(CCSequence::create(CCDelayTime::create(1.2f),
                                               CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                               NULL));
    petActionWin();
    
    m_pet_spine->runAction(CCSequence::create(CCDelayTime::create(1.3f),
                                              CCEaseElasticOut::create(CCScaleTo::create(1.0f, PET_SCALE_RACERESULT)),
                                              NULL));
    m_fanSprite->runAction(CCSequence::create(CCDelayTime::create(1.4f),
                                              CCEaseElasticOut::create(CCScaleTo::create(0.8f, 1.0f)),
                                              NULL));
    m_fanLabel->runAction(CCSequence::create(CCDelayTime::create(1.45f),
                                             CCEaseElasticOut::create(CCScaleTo::create(0.8f, 1.0f)),
                                             NULL));
    m_itemSprite->runAction(CCSequence::create(CCDelayTime::create(1.50f),
                                               CCEaseElasticOut::create(CCScaleTo::create(0.8f, 1.0f)),
                                               NULL));
    m_itemLabel->runAction(CCSequence::create(CCDelayTime::create(1.55f),
                                              CCEaseElasticOut::create(CCScaleTo::create(0.8f, 1.0f)),
                                              NULL));
    m_coinSprite->runAction(CCSequence::create(CCDelayTime::create(1.60f),
                                               CCEaseElasticOut::create(CCScaleTo::create(0.8f, 1.0f)),
                                               NULL));
    m_coinLabel->runAction(CCSequence::create(CCDelayTime::create(1.65f),
                                              CCEaseElasticOut::create(CCScaleTo::create(0.8f, 1.0f)),
                                              NULL));
    m_continueButton->runAction(CCSequence::create(CCDelayTime::create(1.7f),
                                                  CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                                   NULL));
    
    scheduleOnce(schedule_selector(RaceResultPanel::playSoundEffectWin), 1.0f);
    
    this->runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                       CCCallFunc::create(this, callfunc_selector(RaceResultPanel::playStarPaticle)),
                                       NULL));

}

void RaceResultPanel::playStarPaticle()
{
    for (int i = 0; i < arc4random()%12; i++) {
        CCSprite *star = CCSprite::create("common/vfx/star_2.png");
        star->setPosition(this->getContentSize()*0.5f);
        star->setScale(sizeMultiplier*(0.4 + (arc4random()%7)/10.f));
        this->addChild(star, -1);
        
        int angle = arc4random()%360;
        float fangle = angle*M_PI/180;
        
        float distance = MAX(this->getContentSize().width, this->getContentSize().height)/2.f + MAX(star->getContentSize().width, star->getContentSize().height);
        
        CCPoint destinationPoint = star->getPosition() + ccp(distance*cosf(fangle), distance*sinf(fangle));
        
        star->runAction(CCScaleTo::create(0.7f, star->getScale()*(1.5 + arc4random()%3)));
        star->runAction(CCSequence::create(CCMoveTo::create(0.7f, destinationPoint),
                                           CCRemoveSelf::create(),
                                           NULL));
    }
    
    this->runAction(CCSequence::create(CCDelayTime::create(0.1f),
                                       CCCallFunc::create(this, callfunc_selector(RaceResultPanel::playStarPaticle)),
                                       NULL));
}

void RaceResultPanel::playSoundEffectWin()
{
    g_gameAudio->stopAllEffect();
    g_gameAudio->playEffect(AUDIO_RACE_WIN2);
}

void RaceResultPanel::petActionWin()
{
    m_pet_spine->setAnimation("habitat_eat", false);
    m_pet_spine->addAnimation("habitat_idle_stand", true);
}

void RaceResultPanel::skipWinResultAnimation()
{
    m_spinningGlowSprite->setScale(4*sizeMultiplier);
    m_placeCircleSprite->setScale(2);
    m_placeLabel->setScale(1);
    m_ribbonSprite->setScale(1);
    m_coinSprite->setScale(1);
    m_coinLabel->setScale(1);
    m_itemSprite->setScale(1);
    m_itemLabel->setScale(1);
    m_fanSprite->setScale(1);
    m_fanLabel->setScale(1);
    m_continueButton->setScale(1);
    m_pet_spine->setScale(PET_SCALE_RACERESULT);
}

void RaceResultPanel::createLoseResult() {
    
    m_spinningGlowSprite = CCSprite::createWithSpriteFrameName("oldversion/_spotlight.png");
    this->addChild(m_spinningGlowSprite);
    
    m_ribbonSprite = UIHelper::getScalableSprite(UIHelper::RibbonGrey);
    m_ribbonSprite->setContentSize(m_ribbonSprite->getContentSize()*1.2f);
    this->addChild(m_ribbonSprite);
    
    m_pet_spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/pet/", m_pet->pVo.src.c_str());
    m_pet_spine->setVisible(false);
    m_pet_spine->setAnchorPoint(ccp(0.5f, 0.5f));
    this->addChild(m_pet_spine, 0);
    
    m_placeLabel = CCLabelHelper::createBMFont(g_gameContent->Word["raceresult_2nd_place"].c_str(), 80, true);
    this->addChild(m_placeLabel);
    
    m_coinSprite = CCNikButton::create(UIHelper::getSprite(UIHelper::HUDCoin), NULL);
    this->addChild(m_coinSprite);
    
    m_coinLabel = CCLabelHelper::createBMFont(CCString::createWithFormat("%d", coins_reward)->getCString(), 80, true);
    this->addChild(m_coinLabel);
    
    m_continueButton = CCNikButton::create(UIHelper::getSprite(UIHelper::NotificationYes), NULL);
    m_continueButton->setTarget(this, menu_selector(RaceResultPanel::continueButton), CCNikButtonEventTouchUpInside);
    this->addChild(m_continueButton);
    
    CCLabelBMFont *continueLabel = CCLabelHelper::createBMFont(g_gameContent->Word["raceresult_button_continue"].c_str(), kFontSizeLarge, true);
    m_continueButton->addChild(continueLabel);
    
    
    m_spinningGlowSprite->setPosition(ccp(this->getContentSize().width*0.5f,
                                          this->getContentSize().height
                                          - m_spinningGlowSprite->getContentSize().height*0.5f*m_spinningGlowSprite->getScale()));
    m_ribbonSprite->setPosition(this->getContentSize()*0.5f);
    m_pet_spine->setPosition(m_ribbonSprite->getPosition()
                             + ccp(0, m_ribbonSprite->getContentSize().height*2.0f*0.3f));
    m_placeLabel->setPosition(m_ribbonSprite->getPosition()
                              + ccp(0, m_ribbonSprite->getContentSize().height*2.0f*0.055f));
    m_coinSprite->setPosition(m_ribbonSprite->getPosition()
                              + ccp(0, -m_ribbonSprite->getContentSize().height*2.0f*0.25f - m_coinSprite->getContentSize().height*0.5f));
    m_coinLabel->setPosition(m_coinSprite->getPosition()
                             + ccp(0,- m_coinLabel->getContentSize().height*0.5f - m_coinSprite->getContentSize().height*0.5f));
    m_continueButton->setPosition(m_coinLabel->getPosition()
                                  + ccp(0, - m_coinLabel->getContentSize().height*0.5f - m_continueButton->getContentSize().height*0.5f));
    continueLabel->setPosition(m_continueButton->getContentSize()*0.5f
                               + ccp(0, 6.0f * sizeMultiplier));
    
    //beginning state
    m_spinningGlowSprite->setOpacity(0);
    m_spinningGlowSprite->setScale(4*sizeMultiplier);
    m_pet_spine->setScale(0);
    m_placeLabel->setScale(0);
    m_ribbonSprite->setScale(0);
    m_coinSprite->setScale(0);
    m_coinLabel->setScale(0);
    m_continueButton->setScale(0);
    
    
    animateLoseResult();
}

void RaceResultPanel::animateLoseResult()
{
    g_gameAudio->stopAllEffect();
    g_gameAudio->playEffect(AUDIO_RACE_LOSE);
    
    m_spinningGlowSprite->setVisible(true);
    m_pet_spine->setVisible(true);
    m_placeLabel->setVisible(true);
    m_ribbonSprite->setVisible(true);
    m_coinSprite->setVisible(true);
    m_coinLabel->setVisible(true);
    m_continueButton->setVisible(true);
    
    m_spinningGlowSprite->runAction(CCSequence::create(CCDelayTime::create(1.2f),
                                                       CCFadeIn::create(1.0f),
                                                       NULL));
    m_ribbonSprite->runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                                 CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                                 NULL));
    m_placeLabel->runAction(CCSequence::create(CCDelayTime::create(1.2f),
                                               CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                               NULL));
    
    petActionLose();
    
    m_pet_spine->runAction(CCSequence::create(CCDelayTime::create(1.3f),
                                              CCEaseElasticOut::create(CCScaleTo::create(1.0f, PET_SCALE_RACERESULT)),
                                              NULL));
    m_coinSprite->runAction(CCSequence::create(CCDelayTime::create(1.60f),
                                               CCEaseElasticOut::create(CCScaleTo::create(0.8f, 1.0f)),
                                               NULL));
    m_coinLabel->runAction(CCSequence::create(CCDelayTime::create(1.65f),
                                              CCEaseElasticOut::create(CCScaleTo::create(0.8f, 1.0f)),
                                              NULL));
    m_continueButton->runAction(CCSequence::create(CCDelayTime::create(1.7f),
                                                   CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                                   NULL));
}

void RaceResultPanel::skipLoseResultAnimation()
{
    m_spinningGlowSprite->setScale(4);
    m_placeLabel->setScale(1);
    m_ribbonSprite->setScale(1);
    m_coinSprite->setScale(1);
    m_coinLabel->setScale(1);
    m_continueButton->setScale(1);
    m_pet_spine->setScale(PET_SCALE_RACERESULT);
}

void RaceResultPanel::petActionLose()
{
    m_pet_spine->setAnimation("habitat_hungry", true);
}

void RaceResultPanel::continueButton(cocos2d::CCObject *sender)
{
    if (!m_buttonEnabled) return;
    
    if(lockTutorial) {
        m_spinningGlowSprite->removeFromParent();
        m_placeCircleSprite->removeFromParent();
        m_placeLabel->removeFromParent();
        m_ribbonSprite->removeFromParent();
        m_coinSprite->removeFromParent();
        m_coinLabel->removeFromParent();
        
        m_itemSprite->removeFromParent();
        m_itemLabel->removeFromParent();
        m_fanSprite->removeFromParent();
        m_fanLabel->removeFromParent();
        m_continueButton->removeFromParent();
        m_pet_spine->removeFromParent();
        
        lockTutorial = false;
        canTouch = false;
        
    }
    else if(canTouch) {
        
        SceneRace* _scene = (SceneRace*)g_currentScene;
      
        canTouch=false;
        g_gameAudio->stopMusic();
        g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
        g_gameAudio->clearEffectFadeOut(3.0f);
        
        if (m_tutSecondRace) {
            _scene->fadeOutThisSceneAndChangeTo(SCENE_VILLE);
        } else {
            _scene->fadeOutThisSceneAndChangeTo(SCENE_WORLDMAP);
        }
        
    }
}


bool RaceResultPanel::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    if(canTouch) {
        CCPoint point = this->convertTouchToNodeSpace(touch);
        CCRect rect = this->boundingBox();
        if (rect.containsPoint(point) && isVisible()) {
            if (m_pet->petResult == WIN_RESULT) {
                skipWinResultAnimation();
            } else {
                skipLoseResultAnimation();
            }
            return true;
        }
    }
    
    return false;
}

void RaceResultPanel::onFan(cocos2d::CCObject *object_) {
    
    if (CCNode* _node = (CCNode*)object_) {
        
        CCLabelBMFont* _note_label = CCLabelHelper::createBMFont(g_gameContent->Word["note_fan"].c_str(),
                                                                 kFontSizeMedium-5.0f,
                                                                 false,
                                                                 kTextColorDark,
                                                                 _node->getContentSize().width*4,
                                                                 kCCTextAlignmentCenter);
        CCSize _contentSize = CCNodeHelper::getContentSize(_note_label);
        CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
        
        CCPopOverDialog* _hintPopOver = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionAny);
        _note_label->setPosition(_hintPopOver->getContentSize()*0.5f);
        _hintPopOver->addChild(_note_label);
        _hintPopOver->presentPopover(_node, true);
        
        g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
        
    }

}

void RaceResultPanel::onItem(cocos2d::CCObject *object) {
    
    CCNode* _node = (CCNode*)object;
    if (!_node) return;
    
    GDItemVO _item_vo = g_gameContent->getItemVO(g_opponentState->itemDrops);
    if (_item_vo.type == "") return;
        
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    CCSize _popoverSize = CCSizeZero;
    
    // 1. create all elements and sum the content size
    CCLabelBMFont *_titleLabel = CCLabelHelper::createBMFont(_item_vo.title.c_str(), kFontSizeMedium, true);
    CCSize _contentSize = CCNodeHelper::getContentSize(_titleLabel);
    _popoverSize.height += _contentSize.height + kPopOverDialogSpacing*sizeMultiplier;
    if (_popoverSize.width < _contentSize.width) {
        _popoverSize.width = _contentSize.width;
    }
    
    CCLabelBMFont *_descriptionLabel = CCLabelHelper::createBMFont(_item_vo.description2.c_str(),
                                                                   kFontSizeMedium-5.0f,
                                                                   false,
                                                                   kTextColorDark,
                                                                   kPopOverDialogNarmalWidth*sizeMultiplier,
                                                                   kCCTextAlignmentCenter);
    _contentSize = CCNodeHelper::getContentSize(_descriptionLabel);
    _popoverSize.height += _contentSize.height + kPopOverDialogSpacing*sizeMultiplier;
    if (_popoverSize.width < _contentSize.width) {
        _popoverSize.width = _contentSize.width;
    }
    
    _popoverSize.height -= kPopOverDialogSpacing*sizeMultiplier;
    _popoverSize = _popoverSize + kPopOverDialogPadding*sizeMultiplier;
    
    // 2. create a popup dialog with contentSize calculated from all elements
    CCPopOverDialog *popover = CCPopOverDialog::create(_popoverSize, CCPopoverArrowDirectionAny);
    
    // 3. set position of all elements regard contentSize
    _titleLabel->setPosition(ccp(popover->getContentSize().width*0.5f,
                                 popover->getContentSize().height*1.0f
                                 - kPopOverDialogPadding.height*sizeMultiplier*0.5f
                                 - CCNodeHelper::getContentSize(_titleLabel).height*0.5f));
    popover->addChild(_titleLabel);
    
    _descriptionLabel->setPosition(_titleLabel->getPosition() + ccp(0.0f,
                                                                    - CCNodeHelper::getContentSize(_titleLabel).height*0.5f
                                                                    - kPopOverDialogSpacing*sizeMultiplier
                                                                    - CCNodeHelper::getContentSize(_descriptionLabel).height*0.5f));
    popover->addChild(_descriptionLabel);
    
    popover->presentPopover(_node, true);

}

void RaceResultPanel::onCoin(cocos2d::CCObject *object) {
    
    CCNode* _node = (CCNode*)object;
    if (!_node) return;
        
    CCLabelBMFont* _note_label = CCLabelHelper::createBMFont(g_gameContent->Word["note_coin"].c_str(),
                                                             kFontSizeMedium-5.0f,
                                                             false, kTextColorDark,
                                                             _node->getContentSize().width*4,
                                                             kCCTextAlignmentCenter);
    CCSize _contentSize = CCNodeHelper::getContentSize(_note_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog* _hintPopOver = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionAny);
    _note_label->setPosition(_hintPopOver->getContentSize()*0.5f);
    _hintPopOver->addChild(_note_label);
    _hintPopOver->presentPopover(_node, true);
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
}
