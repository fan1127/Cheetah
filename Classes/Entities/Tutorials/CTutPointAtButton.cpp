//
//  CTutPointAtButton.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutPointAtButton.h"
#include "CTutUnit.h"

#include "../../SceneManager.h"
#include "../../Extensions/CCMenuHelper.h"

extern SceneManager* g_sceneManager;

extern float sizeMultiplier;

void CTutPointAtButton::perform(void* tutUnit_) {
    
    CTutUnit* _tutUnit = (CTutUnit*)tutUnit_;
    if (!_tutUnit) return;
    
    active = true;
    
    tutUnit = tutUnit_;
    
    _tutUnit->removeAllUIs();
    _tutUnit->removeSpotlightBG();
    
    sender = m_sender;
    callback = m_callback;
    
}

CTutPointAtButton::~CTutPointAtButton() {
    
    m_sender = NULL;
    m_callback = NULL;
    clearGraphics();
    
}

void CTutPointAtButton::update(float delta_) {
    
    if (!active || !menuButton) return;
    
    if (!pointer) {
        
        //menuButton->unselect();
        
        CCMenuItem *pointerGroup = CCMenuItem::create();
        if (menuItemButton) {
            pointerGroup->setPosition(menuItemButton->getPosition());
        }
        
        menuButton->addChild(pointerGroup);
        pointer = CCSprite::createWithSpriteFrameName("oldversion/_tutorial_cursor.png");
        pointer->retain();
        pointerGroup->addChild(pointer);
        pointer->setAnchorPoint(ccp(0.5f, 0.0f));
        pointerGroup->setRotation(angle);
        pointer->setPosition(ccp(0.0f, menuButton->getContentSize().height*0.6f));
        
        pointer->
        runAction(CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCMoveTo::create(0.4f, shift + ccp(0.0f, pointer->getContentSize().height * 0.3f)), 3.5f),
                                                             CCEaseOut::create(CCMoveTo::create(0.4f, shift + ccp(0.0f, pointer->getContentSize().height * 0.75f)), 3.5f),
                                                             NULL)));
        
        menuButton->onWillReleasedSender = this;
        menuButton->onWillReleased = CTutPointAtButton::onButtonWillReleased;
        
        CCMenuItem* _menuItem = menuItemButton;
        if (!_menuItem) {
            _menuItem = (CCMenuItem*)menuButton->getChildren()->objectAtIndex(0);
        }else {
            _menuItem->onWillReleaseSender = this;
            _menuItem->onWillRelease = CTutPointAtButton::onButtonWillReleased;
        }
        
        if (m_sensitiveEnabled) {
            _menuItem->onSelectedSender = this;
            _menuItem->onSelectedCallback = CTutPointAtButton::onButtonSelected;
        }else {
            _menuItem->onWillActive = CTutPointAtButton::onButtonWillActive;
            _menuItem->onWillActiveSender = this;
        }
        
    }
    
    if (!effect && m_effectEnabled) {
        if (effectGroup == NULL) {
            effectGroup = CCMenuItem::create();
            effectGroup->retain();
            if (menuItemButton) {
                effectGroup->setPosition(menuItemButton->getPosition());
            }
            menuButton->addChild(effectGroup, -1);
        }
        effect = g_sceneManager->sharedSkeletonData->getSkeleton(kTutorialEffectSrcDir, "Button_effect");
        effect->setAnimation("idle", true);
        effect->setScale(sizeMultiplier*2.0f*1.5f);
        effectGroup->addChild(effect);
    }
    
    if (message != "" && !text) {
        if (CTutUnit* _tutUnit = (CTutUnit*)tutUnit) {
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            text = CCLabelHelper::createBMFont(message.c_str(), kFontSizeExtraLarge, true, kTextColorWhite);
            text->retain();
            text->setPosition(_screenSize.width*0.5f, _screenSize.height*0.25f);
            _tutUnit->addChild(text);
        }
    }
    
}

void CTutPointAtButton::clearGraphics() {
    
    if (text) {
        text->removeFromParent();
        text = NULL;
    }
    
    if (effect) {
        effect->removeFromParent();
        effect = NULL;
    }
    
    if (effectGroup) {
        effectGroup->removeFromParent();
        effectGroup = NULL;
    }
    
    if (pointer) {
        pointer->removeFromParent();
        pointer = NULL;
    }
    
    if (pointerGroup) {
        pointerGroup->removeFromParent();
        pointerGroup = NULL;
    }
    
    if (menuButton) {
        if (menuItemButton) {
            menuItemButton->onSelectedSender = NULL;
            menuItemButton->onSelectedCallback = NULL;
            menuItemButton->onWillActive = NULL;
            menuItemButton->onWillActiveSender = NULL;
            menuItemButton->onWillRelease = NULL;
            menuItemButton->onWillReleaseSender = NULL;
        } else if (CCMenuItem* _menuItem = (CCMenuItem*)CCMenuHelper::getMenuItemScaleSprite(menuButton)) {
            _menuItem->onSelectedSender = NULL;
            _menuItem->onSelectedCallback= NULL;
            _menuItem->onWillActive = NULL;
            _menuItem->onWillActiveSender = NULL;
            _menuItem->onWillRelease = NULL;
            _menuItem->onWillReleaseSender = NULL;
        }
        menuButton->onWillReleasedSender = NULL;
        menuButton->onWillReleased = NULL;
        menuButton = NULL;
    }
    
}

void CTutPointAtButton::onButtonSelected(void *sender, bool select) {
    
    if (!select) return;
    CTutPointAtButton::onButtonWillActive(sender);
    
}

void CTutPointAtButton::onButtonWillActive(void *sender) {
    
    CTutPointAtButton* _this = (CTutPointAtButton*)sender;
    if (!_this) return;
    
    if (_this->callback) {
        _this->clearGraphics();
        _this->active = false;
        _this->callback(_this->sender, "");
    }
    
    _this->callback = NULL;
    _this->sender = NULL;
    
}

void CTutPointAtButton::onButtonWillReleased(void *sender) {
    
    CTutPointAtButton* _this = (CTutPointAtButton*)sender;
    if (!_this) return;
    
    if (_this->callback) {
        _this->clearGraphics();
        _this->active = false;
        _this->callback(_this->sender, CTutPointAtButtonFailed);
    }
    
    _this->callback = NULL;
    _this->sender = NULL;
    
}
