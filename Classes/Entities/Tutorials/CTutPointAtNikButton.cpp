//
//  CTutPointAtNikButton.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutPointAtNikButton.h"
#include "CTutUnit.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

extern float sizeMultiplier;

void CTutPointAtNikButton::perform(void* tutUnit_) {
    
    CTutUnit* _tutUnit = (CTutUnit*)tutUnit_;
    if (!_tutUnit) return;
    
    active = true;
    
    tutUnit = tutUnit_;
    
    _tutUnit->removeAllUIs();
    _tutUnit->removeSpotlightBG();
    
}

CTutPointAtNikButton::~CTutPointAtNikButton() {
    
    sender = NULL;
    callback = NULL;
    clearGraphics();
    
}

void CTutPointAtNikButton::update(float delta_) {
    
    if (active) {
        
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        
        if (!pointer && button) {
            
            pointerGroup = CCNode::create();
            button->addChild(pointerGroup);
            pointer = CCSprite::createWithSpriteFrameName("oldversion/_tutorial_cursor.png");
            pointer->setAnchorPoint(ccp(0.5f, 0.0f));
            pointerGroup->setRotation(angle);
            pointerGroup->addChild(pointer);
            pointerGroup->setPosition(shift); //+ ccp(0.0f, button->getContentSize().height*0.6f)
            
            pointer->setPosition(ccp(0.0f, pointer->getContentSize().height * 0.75f));
            pointer->
            runAction(CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCMoveTo::create(0.4f, ccp(0.0f, pointer->getContentSize().height * 0.3f)), 3.5f),
                                                                 CCEaseOut::create(CCMoveTo::create(0.4f, ccp(0.0f, pointer->getContentSize().height * 0.75f)), 3.5f),
                                                                 NULL)));
            
            button->onWillReleasedSender = this;
            button->onWillReleased = CTutPointAtNikButton::onButtonWillReleased;
            
            button->onWillActiveSender = this;
            button->onWillActive = CTutPointAtNikButton::onButtonWillActive;
            
        }
        
        if (!effect && button && enableEffect) {
            effect = g_sceneManager->sharedSkeletonData->getSkeleton(kTutorialEffectSrcDir, "Button_effect");
            effect->setAnimation("idle", true);
            effect->setPosition(shift);
            effect->setScale(sizeMultiplier*2.0f*1.5f);
            button->addChild(effect, -100);
        }
        
        if (message != "" && !text) {
            if (CTutUnit* _tutUnit = (CTutUnit*)tutUnit) {
                text = CCLabelHelper::createBMFont(message.c_str(), kFontSizeExtraLarge, true, kTextColorWhite);
                text->setPosition(_screenSize.width*0.5f, _screenSize.height*0.25f);
                _tutUnit->addChild(text);
            }
        }
        
    }
    
}

void CTutPointAtNikButton::clearGraphics() {
    
    if (text) {
        text->removeFromParent();
        text = NULL;
    }
    
    if (effect) {
        effect->removeFromParent();
        effect = NULL;
    }
    
    if (pointer) {
        pointer->removeFromParent();
        pointer = NULL;
    }
    
    if (pointerGroup) {
        pointerGroup->removeFromParent();
        pointerGroup = NULL;
    }
    
    if (button) {
        button->onWillActive = NULL;
        button->onWillActiveSender = NULL;
        button->onWillReleased = NULL;
        button->onWillReleasedSender = NULL;
        button = NULL;
    }
    
}

void CTutPointAtNikButton::onButtonWillActive(void *sender_) {
    
    if (CTutPointAtNikButton* _this = (CTutPointAtNikButton*)sender_) {
        if (_this->autoInactive) {
            _this->clearGraphics();
            _this->active = false;
        }
        if (_this->callback) {
            _this->callback(_this->sender, "");
        }
    }
    
}

void CTutPointAtNikButton::onButtonWillReleased(void *sender_) {
    
    if (CTutPointAtNikButton* _this = (CTutPointAtNikButton*)sender_) {
        if (_this->autoInactive) {
            _this->clearGraphics();
            _this->active = false;
        }
        if (_this->callback) {
            _this->callback(_this->sender, CTutPointAtNikButtonFailed);
        }
    }
    
}
