//
//  CTutDragBuildingToBuild.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutDragBuildingToBuild.h"
#include "CTutUnit.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

extern float retinaMultiplier;

void CTutDragBuildingToBuild::perform(void* tutUnit_) {
    
    CTutUnit* _tutUnit = (CTutUnit*)tutUnit_;
    if (!_tutUnit) return;
    
    active = true;
    
    tutUnit = tutUnit_;
    
    _tutUnit->removeAllUIs();
    _tutUnit->removeSpotlightBG();
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (!pointer) {
        pointer = CCSprite::createWithSpriteFrameName("tutorial_finger0.png");
        pointer->setAnchorPoint(ccp(0.1f, 0.5f));
        buttonItem->addChild(pointer);
        pointer->setPosition(buttonItem->getPreviewBuildingPosition());
    }
    
    if (message != "" && !text) {
        text = CCLabelHelper::createBMFont(message.c_str(), kFontSizeExtraLarge, true, kTextColorWhite);
        text->setPosition(_screenSize.width*0.5f, _screenSize.height*0.25f);
        _tutUnit->addChild(text);
    }
    
}

CTutDragBuildingToBuild::~CTutDragBuildingToBuild() {
    
    clearGraphics();
    
}

void CTutDragBuildingToBuild::update(float delta_) {
    
    if (active) {
        
        // This need to be performed here rather in perform() function above
        // otherwise it's gonna be released by other node before it does perform action
        if (pointer) {
            if (pointer->numberOfRunningActions() == 0) {
                
                CCPoint pos = buttonItem->getPreviewBuildingPosition();
                pointer->stopAllActions();
                pointer->runAction(CCRepeatForever::create(CCSequence::create(CCMoveTo::create(1.0f, pos + ccp(buttonItem->getContentSize().width*0.5f, 0.0f)),
                                                                              CCFadeTo::create(0.5f, 0),
                                                                              CCMoveTo::create(1.0f, pos),
                                                                              CCFadeTo::create(0.5f, 255),
                                                                              NULL)));
                
                if (shopPanel) {
                    shopPanel->senderForTut = this;
                    shopPanel->onCloseForTut = CTutDragBuildingToBuild::onShopPanelClosed;
                    shopPanel->onBuildingAddedForTut = CTutDragBuildingToBuild::onBuildingAdded;
                }
            }
        }
        
    }
    
}

void CTutDragBuildingToBuild::clearGraphics() {
    
    if (text) {
        text->removeFromParent();
        text = NULL;
    }
    
    if (pointer) {
        pointer->removeFromParent();
        pointer = NULL;
    }
    
    if (shopPanel) {
        shopPanel->senderForTut = NULL;
        shopPanel->onCloseForTut = NULL;
        shopPanel->onBuildingAddedForTut = NULL;
    }
    
}

void CTutDragBuildingToBuild::onShopPanelClosed(void* sender_) {
    
    if (CTutDragBuildingToBuild* _this = (CTutDragBuildingToBuild*)sender_) {
        
        _this->clearGraphics();
        _this->active = false;
        
        if (_this->callback) {
            _this->callback(_this->sender, CTutDragBuildingToBuildFailed);
        }
        
        
    }
    
}

void CTutDragBuildingToBuild::onBuildingAdded(void *sender_, void* item_) {
    
    if (CTutDragBuildingToBuild* _this = (CTutDragBuildingToBuild*)sender_) {
        
        _this->clearGraphics();
        _this->active = false;
        if (_this->callback) {
            std::string _type = "";
            if (ShopItemBuilding* _item = (ShopItemBuilding*)item_) {
                _type = _item->vo.type;
            }
            _this->callback(_this->sender, _type);
        }
        
        
    }
    
}
