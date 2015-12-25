//
//  GachaButton.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/28/2557 BE.
//
//

#include "GachaButton.h"

#include "../Extensions/CCLabelHelper.h"

extern float sizeMultiplier;

GachaButton* GachaButton::create() {
    
    GachaButton *obj = new GachaButton();
    if (!obj->init()) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool GachaButton::init()
{
    if (!CCNikButton::init(CCSprite::createWithSpriteFrameName("_bg_bubble_1.png"), NULL)) {
        return false;
    }
    
    m_rotationSprite = CCSprite::createWithSpriteFrameName("_bg_bubble_1.png");
    m_rotationSprite->setPosition(this->getContentSize()*0.5f+ccp(0.0f,3.0f*sizeMultiplier));
    m_rotationSprite->setScale(0.93f);
    this->addChild(m_rotationSprite);
    
    CCSprite *overlaySprite = CCSprite::createWithSpriteFrameName("_bg_bubble_1.png");
    overlaySprite->setPosition(this->getContentSize()*0.5f+ccp(0.0f,3.0f*sizeMultiplier));
    overlaySprite->setScale(0.93f);
    this->addChild(overlaySprite);
    
    m_badgeSprite = CCSprite::createWithSpriteFrameName("_bg_bubble_1.png");
    m_badgeSprite->setPosition(this->getContentSize() - m_badgeSprite->getContentSize()*0.25f);
    this->addChild(m_badgeSprite);
    
    m_badgeLabel = CCLabelHelper::createBMFont("", kFontSizeMedium, false);
    m_badgeLabel->setPosition(ccp(m_badgeSprite->getContentSize().width*0.45f,
                                  m_badgeSprite->getContentSize().height*0.55f));
    m_badgeSprite->addChild(m_badgeLabel);
    
    setBadge(0);
    
    return true;
}

void GachaButton::setBadge(int badge)
{
    m_badge = badge;
    m_badgeSprite->setVisible(m_badge > 0);
    m_badgeLabel->setString(CCString::createWithFormat("%d",badge)->getCString());
    
    if (m_badge > 0) {
        runRorateAction();
    } else {
        stopRotateAction();
    }
}

void GachaButton::runRorateAction()
{
    m_rotationSprite->runAction(CCRepeatForever::create(CCRotateBy::create(120.0f, 360)));
}

void GachaButton::stopRotateAction()
{
    m_rotationSprite->stopAllActions();
}