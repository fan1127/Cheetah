//
//  CTutPointAtVillage.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutPointAtVillage.h"
#include "CTutUnit.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

extern float sizeMultiplier;

void CTutPointAtVillage::perform(void* tutUnit_) {
    
    CTutUnit* _tutUnit = (CTutUnit*)tutUnit_;
    if (!_tutUnit) return;
    
    active = true;
    
    tutUnit = tutUnit_;
    
    _tutUnit->removeAllUIs();
    _tutUnit->removeSpotlightBG();
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (!pointer && village && worldMap->newLayer) {
        
        pointer = CCSprite::createWithSpriteFrameName("oldversion/_tutorial_cursor.png");
        pointer->setAnchorPoint(ccp(0.5f, 0.0f));
        worldMap->newLayer->addChild(pointer, 200);
        pointer->setPosition(village->getPosition() + village->getContentSize()*0.5f + ccp(0.0f, village->getContentSize().height*0.8f));
        
        pointer->
        runAction(CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCMoveTo::create(0.4f, village->getPosition() + village->getContentSize()*0.5f + ccp(0.0f, pointer->getContentSize().height * 0.3f)), 3.5f),
                                                             CCEaseOut::create(CCMoveTo::create(0.4f, village->getPosition() + village->getContentSize()*0.5f + ccp(0.0f, pointer->getContentSize().height * 0.75f)), 3.5f),
                                                             NULL)));
    }
    
    if (!effect && village && worldMap->newLayer) {
        effect = g_sceneManager->sharedSkeletonData->getSkeleton(kTutorialEffectSrcDir, "Button_effect");
        effect->setAnimation("idle", true);
        effect->setScale(sizeMultiplier*1.5f);
        effect->setPosition(village->getPosition() + ccp(village->getContentSize().width*0.5f, village->getContentSize().height*0.4f));
        worldMap->_scrollView->addChild(effect);
        village->removeFromParentAndCleanup(false);
        worldMap->_scrollView->addChild(village);
    }
    
    if (message != "" && !text) {
        text = CCLabelHelper::createBMFont(message.c_str(), kFontSizeExtraLarge, true, kTextColorWhite);
        text->setPosition(_screenSize.width*0.5f, _screenSize.height*0.25f);
        _tutUnit->addChild(text);
    }
    
}

CTutPointAtVillage::~CTutPointAtVillage() {
    
    sender = NULL;
    callback = NULL;
    clearGraphics();
    
}

void CTutPointAtVillage::update(float delta_) {
    
    if (active) {
        
        if (worldMap->getSelectedVillage() == village) {
            if (callback) {
                callback(sender, "");
            }
            clearGraphics();
            active = false;
        }
        
    }
    
}

void CTutPointAtVillage::clearGraphics() {
    
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
