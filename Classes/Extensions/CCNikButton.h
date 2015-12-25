//
//  CCNikButton.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/28/2557 BE.
//
//

#ifndef __Cheetah__CCNikButton__
#define __Cheetah__CCNikButton__

#include "cocos2d.h"

USING_NS_CC;

#define SCALE_ON_TOUCH 0.94f

typedef enum {
    CCNikButtonEventTouchDown,
    CCNikButtonEventTouchUpInside,
    CCNikButtonEventTouchUpOutside
}CCNikButtonEvent;

class CCNikButton: public cocos2d::CCLayerRGBA {
    
    CCNode *contentNode;
    
    bool isMoved;
    bool isDragToOutOfBoundTarget;
    
    bool m_activeScaleEnabled;
    float m_activeScale;
    bool m_enabled;
    CCNikButtonEvent m_event;
    
    void *m_sender;
    
protected:
    
    CCObject*       m_pDragListener;
    SEL_MenuHandler    m_pfnDragSelector;
    
    CCObject *m_pListeners[3];
    SEL_MenuHandler m_pfnSelectors[3];
    
public:
    
    CCSprite *m_normalSprite;
    CCSprite *m_selectedSprite;
    CCSprite *m_disabledSprite;
    
    void setSender(void *sender_) { m_sender = sender_; };
    void (*onHit)(void *inst_, void *sender_);
    
    static CCNikButton *create(CCSprite *normalSprite, CCSprite *selectedSprite);
    bool init(CCSprite *normalSprite, CCSprite *selectedSprite);
    virtual ~CCNikButton();
    
    void setEnableActiveScale(bool enable) { m_activeScaleEnabled = enable; };
    bool isActiveScaleEnabled() { return m_activeScaleEnabled; };
    void setActiveScale(float scale) { m_activeScale = scale; }
    
    bool isEnabled() { return m_enabled; };
    void setEnabled(bool enabled);
    void setDisableSprite(CCSprite *disabledSprite);
    
    void setState();
    
    virtual void addChild(CCNode * child);
    virtual void addChild(CCNode * child, int zOrder);
    virtual void addChild(CCNode *child, int zOrder, int tag);
    
    virtual void setContentSize(CCSize size);
    
    void (*onWillActive)(void* sender);
    void* onWillActiveSender;
    void (*onWillReleased)(void* sender);
    void* onWillReleasedSender;
    
    void setTarget(CCObject *rec, SEL_MenuHandler selector, CCNikButtonEvent state);
    
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(CCTouch *touch, CCEvent *event);
    
    virtual void touchDown();
    virtual void touchUpInside();
    virtual void touchUpOutSide();

    CCNikButtonEvent getButtonEvent() { return m_event; }
    void setButtonEvent(CCNikButtonEvent event_) { m_event = event_; }
    
};

#endif /* defined(__Cheetah__CCNikButton__) */
