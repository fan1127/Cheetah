//
//  EXPProgress.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/18/2557 BE.
//
//

#include "EXPProgress.h"
#include "../Extensions/CCNikButton.h"
#include "../Settings.h"
#include "../Services/GameAudioService.h"
#include "../Extensions/CCPopOverDialog.h"
#include "../AppMacros.h"

#include "../SceneManager.h"
#include "../Scenes/SceneVille.h"
#include "../Scenes/SceneWorldMap.h"

extern CCScene* g_currentScene;
extern TargetDevice targetDevice;
extern float retinaMultiplier;
extern float sizeMultiplier;
extern GameAudioService* g_gameAudio;
USING_NS_CC;

#include "../Services/GameContentService.h"
#include "../Helpers/StringHelper.h"
extern GameContentService* g_gameContent;

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;

EXPProgress* EXPProgress::create() {
    
    EXPProgress *obj = new EXPProgress();
    if (!obj->init()) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool EXPProgress::init()
{
    if (!CCNikButton::init(UIHelper::getSprite(UIHelper::HUDExpirienceBar), NULL))
    {
        return false;
    }
    
    m_inst = this;
    m_sender = NULL;

    this->setTarget(this, menu_selector(EXPProgress::onClickLevel), CCNikButtonEventTouchUpInside);
    this->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);
    
    m_timer = CCProgressTimer::create(UIHelper::getSprite(UIHelper::HUDExpirienceBarColor));
    m_timer->setType(kCCProgressTimerTypeRadial);
    m_timer->setPercentage(0);
    
    m_timer->setPosition(this->getContentSize()*0.5f);
    addChild(m_timer);
    
    m_levelLabel = CCLabelHelper::createBMFont("", kFontSizeVeryLarge, true);
    m_levelLabel->setPosition(this->getContentSize()*0.5f);
    addChild(m_levelLabel);
    return true;
    
}

void EXPProgress::setLevel(int level_) {
    
    m_levelLabel->setString(CCString::createWithFormat("%d", level_)->getCString());

}

int EXPProgress::getLevel() const {
   return StringHelper::toInt(m_levelLabel->getString());
}

const char* EXPProgress::getLevelAsText() const {
    return m_levelLabel->getString();
}

void EXPProgress::setPercentage(float percentage_) {
    
    if (m_timer) {
        m_timer->setPercentage(percentage_);
    }
    
}

void EXPProgress::setPercentage(float fPercentage_, float duration_) {
    
    if (m_timer) {
        m_timer->stopAllActions();
        if (fPercentage_ >= m_timer->getPercentage()) {
            m_timer->runAction(CCSequence::create(CCProgressTo::create(duration_, fPercentage_),
                                                  CCCallFunc::create(this, callfunc_selector(EXPProgress::_onFinished)), NULL));
        }else {
            m_timer->runAction(CCSequence::create(CCProgressTo::create(duration_*0.5f, 100.0f),
                                                  CCCallFunc::create(this, callfunc_selector(EXPProgress::_shouldReset)),
                                                  CCProgressTo::create(duration_*0.5f, fPercentage_),
                                                  CCCallFunc::create(this, callfunc_selector(EXPProgress::_onFinished)), NULL));
        }
    }
    
}

void EXPProgress::setInst(void *inst_) {

    m_inst = inst_;
    
}

void EXPProgress::setSender(void *sender_) {
    
    m_sender = sender_;

}

void EXPProgress::setClearUI(bool status){
    m_clean_status=status;
}

void EXPProgress::_shouldReset(CCObject *sender_) {
    
    m_timer->setPercentage(0.0f);
    
}

void EXPProgress::_onFinished(CCObject* sender_) {
    
    if (onFinished) {
        onFinished(m_inst, m_sender);
    }
    
}
//method clearUI
void EXPProgress::clearUIDialog() {
    
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

void EXPProgress::onClickLevel(cocos2d::CCNode *sender) {
    
    if(m_clean_status) {
        clearUIDialog();
    }
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    snprintf(_buffer, _buffer_size, g_gameContent->Word["hud_popup_level_title"].c_str(), g_playerState->player.racing.level);
    CCLabelBMFont *titleLabel = CCLabelHelper::createBMFont(retinaMultiplier, _buffer, kFontSizeLarge, kTextColorDark);
    
    int _required_exp = 0;
    GDExperienceLevelVO _nxt_exp_vo = g_gameContent->getNextExperienceLevelVO(g_playerState->player.racing.level);
    if (_nxt_exp_vo.level == 0) {
        _required_exp = g_playerState->player.racing.exp;
    }else {
        _required_exp = _nxt_exp_vo.required_exp;
    }
    snprintf(_buffer, _buffer_size, g_gameContent->Word["hud_popup_level_point"].c_str(), g_playerState->player.racing.exp, _required_exp);
    CCLabelBMFont *detailLabel = CCLabelHelper::createBMFont(retinaMultiplier, _buffer, kFontSizeMedium, kTextColorDark);
    
    CCLabelBMFont *detailLabel2 = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["hud_popup_level_description"].c_str(), kFontSizeMedium, kTextColorGray);
    
    
//    cocos2d::CCLabelTTF *detailLabel22 =CCLabelTTF::create( g_gameContent->Word["hud_popup_level_description"].c_str(), 0.1f,kFontSizeMedium,  , 0.1f, kCCVerticalTextAlignmentCenter);
//    CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["hud_popup_level_description"].c_str(), kFontSizeMedium, kTextColorGray);
    
    detailLabel2->setWidth(titleLabel->getContentSize().width*1.5f);
    detailLabel2->setAnchorPoint(ccp(0.0f,1.0f));
    
    switch (targetDevice) {
        case TD_IPHONE: {
            CCSize popoverSize = CCSize(titleLabel->getContentSize().width*3.2, titleLabel->getContentSize().height*8)*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            
            titleLabel->setPosition(ccp(20+ titleLabel->getContentSize().width*0.5f, popover->getContentSize().height - titleLabel->getContentSize().height*0.8f));            popover->addChild(titleLabel);
            
            detailLabel->setPosition(ccp( titleLabel->getPositionX()+titleLabel->getContentSize().width/6.4,
                                         titleLabel->getPositionY() - detailLabel->getContentSize().height*1.2f ));
            
            detailLabel2->setPosition(ccp( detailLabel->getPositionX()+titleLabel->getContentSize().width/6.4,
                                          detailLabel->getPositionY() - detailLabel2->getContentSize().height/1.3f ));
            
            popover->addChild(detailLabel);
            popover->addChild(detailLabel2);
            popover->presentPopover(sender, true);
        }break;
        case TD_IPHONE_RETINA:{
            
            CCSize popoverSize = CCSize(titleLabel->getContentSize().width*3.5, titleLabel->getContentSize().height*2+titleLabel->getContentSize().height*2+detailLabel2->getContentSize().height*
                                        (titleLabel->getContentSize().width/titleLabel->getContentSize().width*1.5f)+detailLabel2->getContentSize().height)*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            
            titleLabel->setPosition(ccp(20+ titleLabel->getContentSize().width*0.5f, popover->getContentSize().height - titleLabel->getContentSize().height*0.8f));            popover->addChild(titleLabel);
            
            detailLabel->setPosition(ccp(titleLabel->getPositionX()+titleLabel->getContentSize().width/15,
                                         titleLabel->getPositionY() - detailLabel->getContentSize().height*1.2f ));
            
            detailLabel2->setPosition(ccp( titleLabel->getContentSize().width/4,
                                          detailLabel->getPositionY() - detailLabel2->getContentSize().height/2.3f ));
            
            popover->addChild(detailLabel);
            popover->addChild(detailLabel2);
            popover->presentPopover(sender, true);
        }break;
        case TD_IPAD:       {
            CCSize popoverSize = CCSize(titleLabel->getContentSize().width*4, titleLabel->getContentSize().height*2+titleLabel->getContentSize().height*2+detailLabel2->getContentSize().height*
                                        (titleLabel->getContentSize().width/titleLabel->getContentSize().width*1.5f)+detailLabel2->getContentSize().height*2)*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            
            titleLabel->setPosition(ccp(20+ titleLabel->getContentSize().width*0.5f, popover->getContentSize().height - titleLabel->getContentSize().height*0.8f));
            popover->addChild(titleLabel);
            
            detailLabel->setPosition(ccp(2+ titleLabel->getPositionX()+titleLabel->getContentSize().width/15,
                                         titleLabel->getPositionY() - detailLabel->getContentSize().height*1.2f));
            
            detailLabel2->setPosition(ccp( titleLabel->getContentSize().width/4,
                                          detailLabel->getPositionY() - detailLabel2->getContentSize().height/2.3f));
            
            
            popover->addChild(detailLabel);
            popover->addChild(detailLabel2);
            popover->presentPopover(sender, true);
        }  break;
        case TD_IPAD_RETINA:    {
            detailLabel2->setWidth(titleLabel->getContentSize().width*2.5f);
            CCSize popoverSize = CCSize(titleLabel->getContentSize().width*3.3,titleLabel->getContentSize().height*2+detailLabel2->getContentSize().height*
                                        (titleLabel->getContentSize().width/titleLabel->getContentSize().width*2.3f))*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            
            titleLabel->setPosition(ccp(titleLabel->getContentSize().width, popover->getContentSize().height - titleLabel->getContentSize().height*1.2));
            popover->addChild(titleLabel);
            
            detailLabel->setPosition(ccp( titleLabel->getPositionX()+titleLabel->getContentSize().width/8,
                                         titleLabel->getPositionY() - detailLabel->getContentSize().height*1.2f ));
            
            detailLabel2->setPosition(ccp( 90,
                                          detailLabel->getPositionY() - detailLabel->getContentSize().height*0.9f ));
            
            popover->addChild(detailLabel);
            popover->addChild(detailLabel2);
            popover->presentPopover(sender, true);
        }  break;
    }
}
