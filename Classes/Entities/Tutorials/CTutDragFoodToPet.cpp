//
//  CTutDragFoodToPet.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutDragFoodToPet.h"
#include "CTutUnit.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

extern float retinaMultiplier;

void CTutDragFoodToPet::perform(void* tutUnit_) {
    
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
        entityAction->addChild(pointer);
        
        if (entityAction) {
            if (entityAction->habitatAction) {
                CCPoint pos = entityAction->habitatAction->getDefaultIconFoodPosition();
                pointer->setPosition(pos);
                
                pointer->runAction(CCRepeatForever::create(CCSequence::create(CCMoveTo::create(1.0f, ccp(-pointer->getContentSize().width*0.5f, 0.0f)),
                                                                              CCMoveTo::create(0.5f, ccp(0.0f, pointer->getContentSize().height*0.5f)),
                                                                              CCMoveTo::create(0.5f, ccp( pointer->getContentSize().width*0.5f, 0.0f)),
                                                                              CCMoveTo::create(0.5f, ccp(0.0f,-pointer->getContentSize().height*0.5f)),
                                                                              CCMoveTo::create(0.5f, ccp(-pointer->getContentSize().width*0.5f, 0.0f)),
                                                                              CCMoveTo::create(0.5f, ccp(0.0f, pointer->getContentSize().height*0.5f)),
                                                                              CCMoveTo::create(0.5f, ccp( pointer->getContentSize().width*0.5f, 0.0f)),
                                                                              CCMoveTo::create(0.5f, ccp(0.0f,-pointer->getContentSize().height*0.5f)),
                                                                              CCFadeTo::create(0.5f, 0),
                                                                              CCMoveTo::create(2.0f, entityAction->habitatAction->getDefaultIconFoodPosition()),
                                                                              CCFadeTo::create(0.5f, 255),
                                                                              NULL)));
            }
        }
    }
    
    if (message != "" && !text) {
        text = CCLabelHelper::createBMFont(message.c_str(), kFontSizeExtraLarge, true, kTextColorWhite);
        text->setPosition(_screenSize.width*0.5f, _screenSize.height*0.25f);
        _tutUnit->addChild(text);
    }
    
}

CTutDragFoodToPet::~CTutDragFoodToPet() {
    
    clearGraphics();
    
}

void CTutDragFoodToPet::update(float delta_) {
    
    if (active) {
        
        if (entityAction) {
            if (entityAction->habitatAction) {
                switch (entityAction->habitatAction->getState()) {
                    case EHAS_STANDBY: {
                        clearGraphics();
                        if (callback) {
                            callback(sender, CTutDragFoodToPetFailed);
                        }
                        active = false;
                    }break;
                    case EHAS_GRASP_FOOD: {
                        clearGraphics();
                    }break;
                        //                    case EHAS_REVERT: {
                        //                        perform(tutUnit);
                        //                    }break;
                    case EHAS_FED: {
                        if (callback) {
                            callback(sender, "");
                        }
                        active = false;
                    }break;
                    default:
                        break;
                }
            }
        }
        
    }
    
}

void CTutDragFoodToPet::clearGraphics() {
    
    if (text) {
        text->removeFromParent();
        text = NULL;
    }
    
    if (pointer) {
        pointer->removeFromParent();
        pointer = NULL;
    }
    
}