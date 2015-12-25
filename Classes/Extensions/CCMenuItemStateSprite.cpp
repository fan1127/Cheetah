//
//  CCMenuItemBeganTouchSprite.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/12/2557 BE.
//
//

#include "CCMenuItemStateSprite.h"

CCMenuItemStateSprite* CCMenuItemStateSprite::create(CCNode* sprite)
{
    CCMenuItemStateSprite *pRet = new CCMenuItemStateSprite();
    pRet->initWithSprite(sprite);
    pRet->autorelease();
    return pRet;
}

bool CCMenuItemStateSprite::initWithSprite(CCNode* sprite)
{
    if (!CCMenuItemSprite::initWithNormalSprite(sprite, NULL, NULL, NULL, NULL)) {
        return false;
    }
    return true;
}

void CCMenuItemStateSprite::setTarget(cocos2d::CCObject *rec, SEL_MenuHandler selector, CCMenuItemStateSpriteState state)
{
    m_pListeners[state] = rec;
    m_pfnSelectors[state] = selector;
}


void CCMenuItemStateSprite::selected()
{
    cocos2d::CCMenuItemSprite::selected();
    
    if (m_pListeners[CCMenuItemStateSpriteStateTouchDown] && m_pfnSelectors[CCMenuItemStateSpriteStateTouchDown])
    {
        (m_pListeners[CCMenuItemStateSpriteStateTouchDown]->*m_pfnSelectors[CCMenuItemStateSpriteStateTouchDown])(this);
    }
}

void CCMenuItemStateSprite::unselected()
{
    cocos2d::CCMenuItemSprite::unselected();
    if (m_pListeners[CCMenuItemStateSpriteStateTouchUp] && m_pfnSelectors[CCMenuItemStateSpriteStateTouchUp])
    {
        (m_pListeners[CCMenuItemStateSpriteStateTouchUp]->*m_pfnSelectors[CCMenuItemStateSpriteStateTouchUp])(this);
    }
}

void CCMenuItemStateSprite::activate()
{
    
}