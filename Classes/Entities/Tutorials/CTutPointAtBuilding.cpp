//
//  CTutPointAtBuilding.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutPointAtBuilding.h"
#include "CTutUnit.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

extern float retinaMultiplier;

void CTutPointAtBuilding::perform(void* tutUnit_) {
    
    CTutUnit* _tutUnit = (CTutUnit*)tutUnit_;
    if (!_tutUnit) return;
    
    active = true;
    
    tutUnit = tutUnit_;
    
    _tutUnit->removeAllUIs();
    _tutUnit->removeSpotlightBG();
    
}

CTutPointAtBuilding::~CTutPointAtBuilding() {
    
    sender = NULL;
    callback = NULL;
    clearGraphics();
    
}

void CTutPointAtBuilding::update(float delta_) {
    
    if (active) {
        
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        
        if (!pointer && building) {
            
            pointer = CCSprite::createWithSpriteFrameName("oldversion/_tutorial_cursor.png");
            pointer->setAnchorPoint(ccp(0.5f, 0.0f));
            building->addChild(pointer);
            pointer->setPosition(ccp(0.0f, building->bVo.src_height*retinaMultiplier*0.5f));
            
            pointer->
            runAction(CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCMoveTo::create(0.4f, ccp(0.0f, building->bVo.src_height*retinaMultiplier*0.3f + pointer->getContentSize().height * 0.3f)), 3.5f),
                                                                 CCEaseOut::create(CCMoveTo::create(0.4f, ccp(0.0f, building->bVo.src_height*retinaMultiplier*0.3f + pointer->getContentSize().height * 0.75f)), 3.5f),
                                                                 NULL)));
            
            building->onBuildingWillOpen = CTutPointAtBuilding::onBuildingWillOpen;
            building->onBuildingWillOpenSender = this;
            
        }
        
        if (!effect && m_effectEnabled && building) {
            effect = g_sceneManager->sharedSkeletonData->getSkeleton(kTutorialEffectSrcDir, "Button_effect");
            effect->setAnimation("idle", true);
            building->addChild(effect, -1);
        }
        
        if (message != "" && !text && building) {
            if (CTutUnit* _tutUnit = (CTutUnit*)tutUnit) {
                text = CCLabelHelper::createBMFont(message.c_str(), kFontSizeExtraLarge, true, kTextColorWhite);
                text->setPosition(_screenSize.width*0.5f, _screenSize.height*0.25f);
                _tutUnit->addChild(text);
            }
        }
        
        if (entityAction) {
            if (entityAction->getState() == EAS_SHOW_MENU &&
                entityAction->getEntity() == building) {
                clearGraphics();
                active = false;
                if (callback) {
                    callback(sender, "");
                }
            }
        }
        
    }
    
}

void CTutPointAtBuilding::clearGraphics() {
    
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
    
}

#pragma mark - Callback
void CTutPointAtBuilding::onBuildingWillOpen(void* sender_) {
    
    if (CTutPointAtBuilding* _this = (CTutPointAtBuilding*)sender_) {
        _this->clearGraphics();
        _this->active = false;
        if (_this->callback) {
            _this->callback(_this->sender, "");
        }
    }
    
}
