//
//  CCMenuItemCustomLabel.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 11/29/2556 BE.
//
//

#ifndef __Cheetah__CCMenuItemScaleSprite__
#define __Cheetah__CCMenuItemScaleSprite__

#include "cocos2d.h"

USING_NS_CC;

typedef enum {
    CCMenuItemStateTouchDown,
    CCMenuItemStateTouchUp
}CCMenuItemState;

class CCMenuItemScaleSprite: public cocos2d::CCMenuItemSprite {
    
    CCObject *m_pListeners[2];
    SEL_MenuHandler m_pfnSelectors[2];
    
public:

    void setTarget(CCObject *rec, SEL_MenuHandler selector, CCMenuItemState state);
    
    float activeScale;
    float defaultScale;
    
    virtual void selected();
    virtual void unselected();

    static CCMenuItemScaleSprite * create(CCNode* sprite, CCObject* target, SEL_MenuHandler selector);
    
    bool initWithSprite(CCNode* sprite, CCObject* target, SEL_MenuHandler selector);
};

#endif
