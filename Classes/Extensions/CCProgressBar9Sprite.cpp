//
//  CCProgressBar9Sprite.cpp
//  Cheetah
//
//  Created by Kaweerut on 2/25/57 BE.
//
//

#include "CCProgressBar9Sprite.h"

#include "../Settings.h"

CCProgressBar9Sprite *CCProgressBar9Sprite::create(CCScale9Sprite *backgroundSprite, CCScale9Sprite *innerSprite, CCSize size, CCPoint offset, CCPoint shift)
{
    CCProgressBar9Sprite *pRet = new CCProgressBar9Sprite();
    pRet->init(backgroundSprite, NULL, innerSprite, size, offset, shift);
    pRet->autorelease();
    return pRet;
    
}

CCProgressBar9Sprite *CCProgressBar9Sprite::create(CCScale9Sprite *backgroundSprite, CCScale9Sprite *backgroundInnerSprite, CCScale9Sprite *innerSprite, CCSize size, CCPoint offset, CCPoint shift)
{
    CCProgressBar9Sprite *pRet = new CCProgressBar9Sprite();
    pRet->init(backgroundSprite, backgroundInnerSprite, innerSprite, size, offset, shift);
    pRet->autorelease();
    return pRet;
}

bool CCProgressBar9Sprite::init(CCScale9Sprite *backgroundSprite, CCScale9Sprite *backgroundInnerSprite, CCScale9Sprite *innerSprite, CCSize size, CCPoint offset, CCPoint shift)
{
    if (!CCNode::init())
    {
        return false;
    }
    
    _size = size;
    _offset = offset;
    _innerMaxSize = CCSize(_size.width-(_offset.x*2.0f), _size.height-(_offset.y*2.0f));
    _innerMinWidth = 30.0f; // Hack for min 9Sprite
    
    _percentage = 0.0f;
    _BgPercentage = 0.0f;
    
//    _tmpTime = 0;
//    _duration = 0;
//    _increaseRate = 0;
    
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    this->setContentSize(_size);
    
    
    backgroundSprite->setContentSize(_size);
    backgroundSprite->setPosition(this->getContentSize()*0.5f);
    this->addChild(backgroundSprite);
    
    if (backgroundInnerSprite) {
        backgroundTimer = backgroundInnerSprite;
        backgroundTimer->retain();
        backgroundTimer->setContentSize(_innerMaxSize);
        this->setBackgroundPercentage(_BgPercentage);
        backgroundTimer->setAnchorPoint(ccp(0.0f, 0.0f));
        backgroundTimer->setPosition(_offset + shift);
        this->addChild(backgroundTimer);
    }

    timer = innerSprite;
    timer->retain();
    timer->setContentSize(_innerMaxSize);
    this->setPercentage(_percentage);
    timer->setAnchorPoint(ccp(0.0f, 0.5f));
    timer->setPosition(ccp((_size.width-_innerMaxSize.width)*0.5f, (this->getContentSize().height*0.5f)+(_offset.y*0.0f)) + shift);
    this->addChild(timer);

    CCLog_("timer width : %f",timer->getContentSize().width);
    
    return  true;
}

void CCProgressBar9Sprite::update(float delta)
{
    
}

void CCProgressBar9Sprite::setPercentage(float fPercentage)
{
    if (fPercentage > 100.0f)
    {
        fPercentage = 100.0;
    }

    if (timer) {
        _percentage = fPercentage;
        this->setPercentage(fPercentage, 0);
    }
}

void CCProgressBar9Sprite::setPercentage(float fPercentage, float duration)
{
    if (fPercentage > 100.0f)
    {
        fPercentage = 100.0;
    }

    if (timer) {
        _percentage = fPercentage;
        float timeWidth = ((_percentage/100.0f)*(_innerMaxSize.width-_innerMinWidth))+_innerMinWidth;
//        CCScaleTo *scaleTo = CCScaleTo::create(duration, timeWidth/_innerMaxSize.width, 1.0f);
//        timer->runAction(scaleTo);
        timer->setContentSize(CCSize(timeWidth, _innerMaxSize.height));
    }
}

void CCProgressBar9Sprite::setBackgroundPercentage(float fPercentage)
{
    if (backgroundTimer) {
        this->setBackgroundPercentage(fPercentage, 0);
    }
}

void CCProgressBar9Sprite::setBackgroundPercentage(float fPercentage, float duration)
{
    if (fPercentage > 100.0f)
    {
        fPercentage = 100.0;
    }

    if (backgroundTimer) {
        _BgPercentage = fPercentage;
        float timeWidth = ((_BgPercentage/100.0f)*(_innerMaxSize.width-_innerMinWidth))+_innerMinWidth;
//        CCScaleTo *scaleTo = CCScaleTo::create(duration, timeWidth/_innerMaxSize.width, 1.0f);
//        backgroundTimer->runAction(scaleTo);
        backgroundTimer->setContentSize(CCSize(timeWidth, _innerMaxSize.height));
    }
}



