//
//  CTutPointAtPowerupButtons.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutPointAtPowerupButtons.h"
#include "CTutUnit.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

extern float sizeMultiplier;

void CTutPointAtPowerupButtons::perform(void* tutUnit_) {
    
    CTutUnit* _tutUnit = (CTutUnit*)tutUnit_;
    if (!_tutUnit) return;
    
    active = true;
    
    tutUnit = tutUnit_;
    
    _tutUnit->removeAllUIs();
    _tutUnit->removeSpotlightBG();
    
}

CTutPointAtPowerupButtons::~CTutPointAtPowerupButtons() {
    
    sender = NULL;
    callback = NULL;
    clearGraphics();
    
}

void CTutPointAtPowerupButtons::update(float delta_) {
    
    if (active) {
        
        if (racePanel && racePanel->m_groupPowerupButtons) {
            
            CTutUnit* _tutUnit = (CTutUnit*)tutUnit;
            if (!_tutUnit) return;
            
            if (CCNikButton* button = (CCNikButton*)racePanel->m_groupPowerupButtons->getChildByTag(buttonSkill)) {
                
                CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                
                if (!pointer && button) {
                    
                    shift = button->getContentSize()*0.5f;
                    
                    pointerGroup = CCNode::create();
                    button->addChild(pointerGroup);
                    pointerGroup->setRotation(angle);
                    
                    pointer = CCSprite::createWithSpriteFrameName("oldversion/_tutorial_cursor.png");
                    pointer->setAnchorPoint(ccp(0.5f, 0.0f));
                    pointerGroup->addChild(pointer);
                    pointer->setPosition(shift + ccp(0.0f, button->getContentSize().height*0.6f));
                    
                    pointer->
                    runAction(CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCMoveTo::create(0.5f, shift + ccp(0.0f, pointer->getContentSize().height * 0.3f)), 3.5f),
                                                                         CCEaseOut::create(CCMoveTo::create(0.5f, shift + ccp(0.0f, pointer->getContentSize().height * 0.75f)), 3.5f),
                                                                         NULL)));
                }
                
                if (!effect && button && enableEffect) {
                    effect = g_sceneManager->sharedSkeletonData->getSkeleton(kTutorialEffectSrcDir, "Button_effect");
                    effect->setAnimation("idle", true);
                    effect->setPosition(shift);
                    effect->setScale(sizeMultiplier*2.0f*1.5f);
                    button->addChild(effect, -100);
                }
                
                if (message != "" && !text) {
                    text = CCLabelHelper::createBMFont(message.c_str(), kFontSizeExtraLarge, true, kTextColorWhite);
                    text->setPosition(_screenSize.width*0.5f, _screenSize.height*0.25f);
                    _tutUnit->addChild(text);
                }
                
                
                if (button) {
                    if (button->getButtonEvent() == CCNikButtonEventTouchDown) {
                        if (callback) {
                            callback(sender, "");
                        }
                        clearGraphics();
                        active = false;
                    }
                }else {
                    if (callback) {
                        callback(sender, "cancelled");
                    }
                    clearGraphics();
                    active = false;
                }
                
            }
            
        }
        
        
    }
    
}

void CTutPointAtPowerupButtons::clearGraphics() {
    
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
    
    if (pointerGroup && pointerGroup->getParent()) {
        pointerGroup->removeFromParent();
        pointerGroup = NULL;
    }
    
}
