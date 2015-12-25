//
//  CCMenuItemCustomLabel.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 11/29/2556 BE.
//
//

#include "CCMenuItemScaleSprite.h"

CCMenuItemScaleSprite* CCMenuItemScaleSprite::create(CCNode* sprite, CCObject* target, SEL_MenuHandler selector) {
    
    CCMenuItemScaleSprite *pRet = new CCMenuItemScaleSprite();
    if (pRet->initWithSprite(sprite, target, selector)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
    
}

bool CCMenuItemScaleSprite::initWithSprite(CCNode* sprite, CCObject* target, SEL_MenuHandler selector) {
    
    if (!CCMenuItemSprite::initWithNormalSprite(sprite, NULL, NULL, target, selector)) {
        return false;
    }
    activeScale = 0.9f;
    defaultScale = 1.0f;
    return true;
    
}


void CCMenuItemScaleSprite::selected()
{
    cocos2d::CCMenuItemSprite::selected();
    this->setScale(activeScale);
    
    if (m_pListeners[CCMenuItemStateTouchDown] && m_pfnSelectors[CCMenuItemStateTouchDown])
    {
        (m_pListeners[CCMenuItemStateTouchDown]->*m_pfnSelectors[CCMenuItemStateTouchDown])(this);
    }
}

void CCMenuItemScaleSprite::unselected()
{
    cocos2d::CCMenuItemSprite::unselected();
    this->setScale(defaultScale);
    
    if (m_pListeners[CCMenuItemStateTouchUp] && m_pfnSelectors[CCMenuItemStateTouchUp])
    {
        (m_pListeners[CCMenuItemStateTouchUp]->*m_pfnSelectors[CCMenuItemStateTouchUp])(this);
    }
}

void CCMenuItemScaleSprite::setTarget(cocos2d::CCObject *rec, SEL_MenuHandler selector, CCMenuItemState state)
{
    m_pListeners[state] = rec;
    m_pfnSelectors[state] = selector;
}

