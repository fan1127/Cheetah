//
//  CCPhatTouchableSprite.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/18/2557 BE.
//
//

#include "CCPhatTouchableSprite.h"
USING_NS_CC;

#ifdef _DEBUG_CCPHATTOUCHABLESPRITE
#include "../Helpers/CCSpriteHelper.h"
#endif

void CCPhatTouchableSprite::setSender(void *sender_) {
    
    m_sender = sender_;
    
}

CCPhatTouchableSprite* CCPhatTouchableSprite::create(const char *pszFileName)
{
    CCPhatTouchableSprite *pRet = new CCPhatTouchableSprite();
    if (pRet && pRet->initWithFile(pszFileName))
    {
        pRet->m_touchPriority = 0;
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

CCPhatTouchableSprite* CCPhatTouchableSprite::create(const char *pszFileName, int touchPriority)
{
    CCPhatTouchableSprite *pRet = new CCPhatTouchableSprite();
    if (pRet && pRet->initWithFile(pszFileName))
    {
        pRet->m_touchPriority = touchPriority;
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

CCPhatTouchableSprite* CCPhatTouchableSprite::createWithSpriteFrameName(const char *spriteFrameName) {
    
    CCPhatTouchableSprite *pRet = new CCPhatTouchableSprite();
    if (pRet->initWithSpriteFrameName(spriteFrameName)) {
        pRet->m_touchPriority = 0;
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
    
}

CCPhatTouchableSprite* CCPhatTouchableSprite::createWithSpriteFrameName(const char *spriteFrameName, int touchPriority) {
    
    CCPhatTouchableSprite *pRet = new CCPhatTouchableSprite();
    if (pRet->initWithSpriteFrameName(spriteFrameName)) {
        pRet->m_touchPriority = touchPriority;
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
    
}

bool CCPhatTouchableSprite::initWithFile(const char *pszFilename) {
    
    if (!CCSprite::initWithFile(pszFilename)) {
        return false;
    }

    rootNode = NULL;
    
    return true;
    
}

bool CCPhatTouchableSprite::initWithSpriteFrameName(const char *spriteFrameName) {
    
    if (!CCSprite::initWithSpriteFrameName(spriteFrameName)) {
        return false;
    }
    
    rootNode = NULL;

    return true;
    
}

#ifdef _DEBUG_CCPHATTOUCHABLESPRITE
void CCPhatTouchableSprite::update(float delta) {
 
    if (m_debug_bg) {
        
        CCRect _sprite_rect;
        
        if (rootNode) {
            
            if (!m_debug_bg->getParent()) {
                CCNode* _rootNode = rootNode;
                while (_rootNode->getParent()) {
                    _rootNode = _rootNode->getParent();
                }
                if (_rootNode) {
                    _rootNode->addChild(m_debug_bg);
                }
            }
            
            CCNode *node = (CCNode*)this;
            float scale = node->getScale();
            CCPoint world_point = node->getPosition();//*scale;
            
            while (node->getParent()) {
                node = node->getParent();
                float scale2 = node->getScale();
                CCPoint pos2 = node->getPosition();
                world_point = world_point * scale2;
                world_point = world_point + pos2;
                scale *= scale2;
                if (node == rootNode) {
                    break;
                }
            }
            
            CCSize size = getContentSize();
            
            _sprite_rect = CCRect(world_point.x - (size.width*0.5f)*scale,
                                  world_point.y - (size.height*0.5f)*scale,
                                  size.width*scale,
                                  size.height*scale);
            
        }else {
            
            CCPoint world_point = convertToWorldSpace(getPosition());
            _sprite_rect = CCRect(world_point.x,
                                  world_point.y,
                                  getContentSize().width,
                                  getContentSize().height);
            
        }
        
        m_debug_bg->setPosition(_sprite_rect.origin);
        m_debug_bg->setScaleX(_sprite_rect.size.width/this->getContentSize().width);
        m_debug_bg->setScaleY(_sprite_rect.size.height/this->getContentSize().height);
        
    }
    
}
#endif

void CCPhatTouchableSprite::onEnter(){
#ifdef _DEBUG_CCPHATTOUCHABLESPRITE
    if (!m_debug_bg) {
        m_debug_bg = blankSpriteWithSize(this->getContentSize());
        m_debug_bg->setOpacity(128);
        m_debug_bg->setColor(ccRED);
        m_debug_bg->setAnchorPoint(CCPointZero);
        scheduleUpdate();
    }
#endif
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, m_touchPriority, true);
    CCSprite::onEnter();
}

void CCPhatTouchableSprite::onExit(){
#ifdef _DEBUG_CCPHATTOUCHABLESPRITE
    if (m_debug_bg) {
        m_debug_bg->removeFromParent();
        m_debug_bg = NULL;
    }
#endif
    rootNode = NULL;
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCSprite::onExit();
}

bool CCPhatTouchableSprite::ccTouchBegan(CCTouch* touch, CCEvent* event){

    CCPoint touch_location = touch->getLocation();
    CCRect _sprite_rect;
    m_lastTouch = touch_location;
    
    if (rootNode) {
        
        CCNode *node = (CCNode*)this;
        float scaleX = node->getScaleX();
        float scaleY = node->getScaleY();
        CCPoint world_point = node->getPosition();
        
        while (node->getParent()) {
            node = node->getParent();
            float scale2X = node->getScaleX();
            float scale2Y = node->getScaleY();
            CCPoint pos2 = node->getPosition();
            scaleX *= scale2X;
            scaleY *= scale2Y;
            world_point = ccp(world_point.x * scale2X,
                              world_point.y * scale2Y);
            world_point = world_point + pos2;
            if (node == rootNode) {
                break;
            }
        }
        
        CCSize size = getContentSize();
        
        _sprite_rect = CCRect(world_point.x - (size.width*0.5f)*scaleX,
                              world_point.y - (size.height*0.5f)*scaleY,
                              size.width*scaleX,
                              size.height*scaleY);
        
    }else {
    
        CCPoint world_point = convertToWorldSpace(getPosition());
        _sprite_rect = CCRect(world_point.x,
                              world_point.y,
                              getContentSize().width,
                              getContentSize().height);
        
    }

#ifdef _DEBUG_CCPHATTOUCHABLESPRITE
    CCLog_("[%0.0f<->%0.0f - %0.0f<->%0.0f] <== {%0.0f,%0.0f}",
          _sprite_rect.origin.x,
          _sprite_rect.origin.x+_sprite_rect.size.width,
          _sprite_rect.origin.y,
          _sprite_rect.origin.y+_sprite_rect.size.height,
          touch_location.x,
          touch_location.y);
#endif
    
    if (_sprite_rect.intersectsRect(CCRect(touch_location.x,
                                           touch_location.y,
                                           1,
                                           1))) {
#ifdef _DEBUG_CCPHATTOUCHABLESPRITE
        CCLog_("HIT!");
#endif
        
        if (onHit) {
            onHit(this, m_sender);
        }
        
        m_state = 0;
        return true;
    }

    return false;
}

void CCPhatTouchableSprite::ccTouchMoved(CCTouch* touch, CCEvent* event){
    //do what you want
}

void CCPhatTouchableSprite::ccTouchEnded(CCTouch* touch, CCEvent* event){
    //do your job here
}