//
//  CCNikScrollViewButton.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/20/2557 BE.
//
//

#ifndef __Cheetah__CCNikScrollViewButton__
#define __Cheetah__CCNikScrollViewButton__

#include "cocos2d.h"

USING_NS_CC;

#define NIKBUTTON_SCALE_WHEN_TOUCH 0.95f

class CC_DLL CCNikScrollViewButtonDelegate
{
public:

    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) { return false; };
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {};
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {};
    virtual void ccTouchCancelled(CCTouch *touch, CCEvent *event) {};

};

typedef enum {
    CCNikScrollViewButtonStateTouchUp,
    CCNikScrollViewButtonStateTouchDown,
}CCNikScrollViewButtonState;

class CCNikScrollViewButton: public cocos2d::CCLayer {
    
    bool isMoved;
    bool isDragToOutOfBoundTarget;
    bool m_enabled;
    
protected:
    
    CCNikScrollViewButtonDelegate *m_delegate;
    
    CCNode *m_normalSprite;
    CCNode *m_selectedSprite;
    CCNode *m_disabledSprite;
    
//    CCObject*       m_pListener;
//    SEL_MenuHandler    m_pfnSelector;
    
    CCObject*           m_pDragListener;
    SEL_MenuHandler     m_pfnDragSelector;
    
    CCObject *m_pListeners[2];
    SEL_MenuHandler m_pfnSelectors[2];
    
public:
    
    CCNode *contentNode;
    
    static CCNikScrollViewButton *create();
    static CCNikScrollViewButton *create(CCNode *normalSprite, CCNode *selectedSprite);
    
    bool init();
    bool init(CCNode *normalSprite, CCNode *selectedSprite);
    
    virtual void addChild(CCNode * child);
    virtual void addChild(CCNode * child, int zOrder);
    virtual void addChild(CCNode *child, int zOrder, int tag);
    
    virtual void setContentSize(CCSize size);
    
    bool isEnabled() { return m_enabled; };
    void setEnabled(bool enabled);
    void setDisableSprite(CCNode *disabledSprite);
    
    void setState();
    
    void setTarget(CCObject *rec, SEL_MenuHandler selector, CCNikScrollViewButtonState state);
    void setDragToOutOfBoundTarget(CCObject *rec, SEL_MenuHandler selector);

    
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(CCTouch *touch, CCEvent *event);

    CCNikScrollViewButtonDelegate* getDelegate() { return m_delegate; }
    void setDelegate(CCNikScrollViewButtonDelegate* delegate) { m_delegate = delegate; }
};

#endif /* defined(__Cheetah__CCNikScrollViewButton__) */
