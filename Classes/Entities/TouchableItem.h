//
//  TouchableItem.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/8/2556 BE.
//
//

#ifndef __Cheetah__TouchableItem__
#define __Cheetah__TouchableItem__

#include "cocos2d.h"

class TouchableItem: public cocos2d::CCLayer {
    
    bool                    m_bIsTouched    = false;
    bool                    m_bSensitive    = false;
    bool                    m_bPressed      = false;
    bool                    m_bTabbed       = false;
    bool                    m_bMoved        = false;
    
    ////////////////////////////////////////////////////////
    // Touch properties
    void                    *m_instance;
    void                    *m_sender;
    
public:
    
    cocos2d::CCSprite       *m_itemSprite   = NULL;
    CCNode                  *parent = NULL;
    
    static TouchableItem *createWithSpriteFrameName(const char *filename_, void *preferenceRootParent_);

    bool initWithSpriteFileName(const char *filename_, void *preferenceRootParent_);
    
    void setInstance(void *inst_);
    void setSender(void *sender_);
    
    ////////////////////////////////////////////////////////
    // callback functions
    void (*onPressed)(void *inst, void* sender);
    void (*onReleased)(void *inst, void* sender);
    void (*onPressing)(void *inst, void* sender);
    void (*onTabbed)(void *inst, void* sender);
    
    ////////////////////////////////////////////////////////
    // touch delegate functions
    void ccTouchesBegan(cocos2d::CCSet *touches_, cocos2d::CCEvent *event_);
    void ccTouchesMoved(cocos2d::CCSet *touches_, cocos2d::CCEvent *event_);
    void ccTouchesEnded(cocos2d::CCSet *touches_, cocos2d::CCEvent *event_);
    void ccTouchesCancelled(cocos2d::CCSet *touches_, cocos2d::CCEvent *event_);
};

#endif /* defined(__Cheetah__TouchableItem__) */
