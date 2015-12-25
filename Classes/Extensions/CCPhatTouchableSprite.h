//
//  CCPhatTouchableSprite.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/18/2557 BE.
//
//

#ifndef __Cheetah__CCPhatTouchableSprite__
#define __Cheetah__CCPhatTouchableSprite__

#include "cocos2d.h"

//#define _DEBUG_CCPHATTOUCHABLESPRITE

class CCPhatTouchableSprite : public cocos2d::CCSprite, public cocos2d::CCTargetedTouchDelegate {
    
    void *m_sender;
    bool m_state;
#ifdef _DEBUG_CCPHATTOUCHABLESPRITE
    cocos2d::CCSprite* m_debug_bg;
#endif
    cocos2d::CCPoint m_lastTouch;
    int m_touchPriority;
    
public:
    
    cocos2d::CCNode* rootNode;
    
    void setSender(void *sender_);
    void (*onHit)(void *inst_, void *sender_);
    cocos2d::CCPoint& getLastTouchPos() { return m_lastTouch; }
    
    static CCPhatTouchableSprite* create(const char *pszFileName);
    static CCPhatTouchableSprite* create(const char *pszFileName, int touchPriority);
    static CCPhatTouchableSprite* createWithSpriteFrameName(const char *spriteFrameName);
    static CCPhatTouchableSprite* createWithSpriteFrameName(const char *spriteFrameName, int touchPriority);
#ifdef _DEBUG_CCPHATTOUCHABLESPRITE
    virtual void update(float delta);
#endif
    virtual bool initWithFile(const char *pszFilename);
    virtual bool initWithSpriteFrameName(const char *spriteFrameName);
    
    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    
};

#endif /* defined(__Cheetah__CCPhatTouchableSprite__) */
