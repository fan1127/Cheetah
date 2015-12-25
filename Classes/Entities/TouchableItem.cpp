//
//  TouchableItem.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/8/2556 BE.
//
//

#include "TouchableItem.h"
#include "Settings.h"
USING_NS_CC;

TouchableItem *TouchableItem::createWithSpriteFrameName(const char *filename_, void *preferenceRootParent_) {
    
    TouchableItem *pRet = new TouchableItem();
    pRet->initWithSpriteFileName(filename_, preferenceRootParent_);
    pRet->autorelease();
    return pRet;
    
}

bool TouchableItem::initWithSpriteFileName(const char *filename_, void *preferenceRootParent_) {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    m_instance = this;
    
    m_itemSprite = CCSprite::createWithSpriteFrameName(filename_);
    //m_itemSprite->preferenceRootParent = preferenceRootParent_;
    this->addChild(m_itemSprite);
    
    this->setContentSize(m_itemSprite->getContentSize());
    
    this->setTouchEnabled(true);
    
    return true;
    
}

void TouchableItem::setInstance(void *inst_) {
    
    m_instance = inst_;
    
}

void TouchableItem::setSender(void *sender_) {
    
    m_sender = sender_;
    
}

void TouchableItem::ccTouchesBegan(CCSet *touches, CCEvent *event) {
    
    CCTouch *touch = (CCTouch*)touches->anyObject();
    CCPoint location = touch->getLocation();

    m_bMoved = false;

    CCPoint _scale = ccp(1, 1);
    if (parent) {
        _scale = ccp(parent->getScaleX(), parent->getScaleY());
    }
    
    if (m_itemSprite) {
        
        CCPoint t_itemPosition = convertToWorldSpace(m_itemSprite->getPosition());
        
        CCRect buttonRect_ = CCRect(t_itemPosition.x - (m_itemSprite->getContentSize().width/2)*_scale.x,
                                    t_itemPosition.y - (m_itemSprite->getContentSize().height/2)*_scale.y,
                                    m_itemSprite->getContentSize().width*_scale.x,
                                    m_itemSprite->getContentSize().height*_scale.y);
        
        if (buttonRect_.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
            
            if (!m_bIsTouched && isVisible()) {
                m_bIsTouched = true;
                
                if (onPressed) {
                    onPressed(m_instance, m_sender);
                }
                
                m_bPressed = true;
                m_bTabbed = true;
            }
            
        }
        
    }
    setTouchPriority(-1000);
    
}

void TouchableItem::ccTouchesMoved(CCSet *touches, CCEvent *event)
{
    m_bTabbed = false;
    m_bMoved = true;
}

void TouchableItem::ccTouchesEnded(CCSet *touches, CCEvent *event)
{
    CCTouch *touch = (CCTouch*)touches->anyObject();
    CCPoint location = touch->getLocation();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    if (m_bMoved && !m_bTabbed)
    {
        float distance2 = touch->getStartLocationInView().getDistanceSq(touch->getLocationInView());
        CCLog_("TouchableItem::ccTouchesEnded %f", sqrtf(distance2));

        if (distance2 < ANDROID_TOUCH_MOVE_THRESHOLD*ANDROID_TOUCH_MOVE_THRESHOLD)
        {
            m_bTabbed = true;
        }
    }

#endif

    m_bMoved = false;


    CCPoint _scale = ccp(1, 1);
    if (parent) {
        _scale = ccp(parent->getScaleX(), parent->getScaleY());
    }
    
    if (m_itemSprite) {
        
        CCPoint t_itemPosition = convertToWorldSpace(m_itemSprite->getPosition());
        
        CCRect buttonRect_ = CCRect(t_itemPosition.x - (m_itemSprite->getContentSize().width/2)*_scale.x,
                                    t_itemPosition.y - (m_itemSprite->getContentSize().height/2)*_scale.y,
                                    m_itemSprite->getContentSize().width*_scale.x,
                                    m_itemSprite->getContentSize().height*_scale.y);
        
        
        
        if (!m_bSensitive && m_bPressed) {
            CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
            buttonRect_ = CCRect(0, 0, screenSize.width, screenSize.height);
        }
        
        if (m_bIsTouched) {
            m_bIsTouched = false;
            if (onReleased) {
                onReleased(m_instance, m_sender);
            }
        }
        
        if (buttonRect_.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
            
            if (m_bTabbed) {
                if (onTabbed) {
                    onTabbed(m_instance, m_sender);
                    
                }
            }
            
            m_bPressed = false;
            m_bTabbed = false;
            
        }
    }

    setTouchPriority(0);
}

void TouchableItem::ccTouchesCancelled(CCSet *touches, CCEvent *event)
{
    CCTouch *touch = (CCTouch*)touches->anyObject();
    CCPoint location = touch->getLocation();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    if (m_bMoved && !m_bTabbed)
    {
        float distance2 = touch->getStartLocationInView().getDistanceSq(touch->getLocationInView());
        CCLog_("TouchableItem::ccTouchesEnded %f", sqrtf(distance2));

        if (distance2 < ANDROID_TOUCH_MOVE_THRESHOLD*ANDROID_TOUCH_MOVE_THRESHOLD)
        {
            m_bTabbed = true;
        }
    }

#endif

    m_bMoved = false;

    CCPoint _scale = ccp(1, 1);
    if (parent) {
        _scale = ccp(parent->getScaleX(), parent->getScaleY());
    }
    
    if (m_itemSprite) {
        
        CCPoint t_itemPosition = convertToWorldSpace(m_itemSprite->getPosition());
        
        CCRect buttonRect_ = CCRect(t_itemPosition.x - (m_itemSprite->getContentSize().width/2)*_scale.x,
                                    t_itemPosition.y - (m_itemSprite->getContentSize().height/2)*_scale.y,
                                    m_itemSprite->getContentSize().width*_scale.x,
                                    m_itemSprite->getContentSize().height*_scale.y);
        
        
        
        if (!m_bSensitive && m_bPressed) {
            CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
            buttonRect_ = CCRect(0, 0, screenSize.width, screenSize.height);
        }
        
        if (m_bIsTouched) {
            m_bIsTouched = false;
            if (onReleased) {
                onReleased(m_instance, m_sender);
            }
        }
        
        if (buttonRect_.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
            
            if (m_bTabbed) {
                if (onTabbed) {
                    onTabbed(m_instance, m_sender);
                }
            }
            
            m_bPressed = false;
            m_bTabbed = false;
            
        }
    }
}
