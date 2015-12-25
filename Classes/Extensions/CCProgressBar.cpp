//
//  CCProgressBar.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 1/17/2557 BE.
//
//

#include "CCProgressBar.h"

CCProgressBar *CCProgressBar::create(cocos2d::CCSprite *backgroundSprite, cocos2d::CCSprite *innerSprite) {
    CCProgressBar *pRet = new CCProgressBar();
    pRet->init(backgroundSprite, NULL, innerSprite);
    pRet->autorelease();
    return pRet;
    
}

CCProgressBar *CCProgressBar::create(cocos2d::CCSprite *backgroundSprite, cocos2d::CCSprite *backgroundInnerSprite, cocos2d::CCSprite *innerSprite) {
    CCProgressBar *pRet = new CCProgressBar();
    pRet->init(backgroundSprite, backgroundInnerSprite, innerSprite);
    pRet->autorelease();
    return pRet;
    
}

bool CCProgressBar::init(cocos2d::CCSprite *backgroundSprite, cocos2d::CCSprite *backgroundInnerSprite, cocos2d::CCSprite *innerSprite)
{
    if (!CCNode::init()) {
        return false;
    }
    
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    this->setContentSize(backgroundSprite->getContentSize());
    
    backgroundSprite->setPosition(this->getContentSize()*0.5f);
    this->addChild(backgroundSprite);
    
    if (backgroundInnerSprite) {
        backgroundTimer = CCProgressTimer::create(backgroundInnerSprite);
        backgroundTimer->setMidpoint(ccp(0, 0));
        backgroundTimer->setBarChangeRate(ccp(1, 0));
        backgroundTimer->setType(kCCProgressTimerTypeBar);
        backgroundTimer->setPercentage(0);
        backgroundTimer->setPosition(this->getContentSize()*0.5f);
        this->addChild(backgroundTimer);
    }
    
    timer = CCProgressTimer::create(innerSprite);
    timer->setMidpoint(ccp(0, 0));
    timer->setBarChangeRate(ccp(1, 0));
    timer->setType(kCCProgressTimerTypeBar);
    timer->setPercentage(0);
    timer->setPosition(this->getContentSize()*0.5f);
    this->addChild(timer);
    
    return  true;
}

void CCProgressBar::setPercentage(float fPercentage)
{
    if (timer) {
        timer->setPercentage(fPercentage);
    }
}

void CCProgressBar::setPercentage(float fPercentage, float duration)
{
    if (timer) {
        timer->stopAllActions();
        timer->runAction(CCProgressTo::create(duration, fPercentage));
    }
}

void CCProgressBar::setBackgroundPercentage(float fPercentage)
{
    if (backgroundTimer) {
        backgroundTimer->setPercentage(fPercentage);
    }
}

void CCProgressBar::setBackgroundPercentage(float fPercentage, float duration)
{
    if (backgroundTimer) {
        backgroundTimer->stopAllActions();
        backgroundTimer->runAction(CCProgressTo::create(duration, fPercentage));
    }
}

