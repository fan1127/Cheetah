//
//  CCMenuItemBeganTouchSprite.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/12/2557 BE.
//
//

#ifndef __Cheetah__CCMenuItemBeganTouchSprite__
#define __Cheetah__CCMenuItemBeganTouchSprite__

#include "cocos2d.h"

USING_NS_CC;

typedef enum {
    CCMenuItemStateSpriteStateTouchDown,
    CCMenuItemStateSpriteStateTouchUp
}CCMenuItemStateSpriteState;

class CCMenuItemStateSprite: public cocos2d::CCMenuItemSprite {
    
    CCObject *m_pListeners[2];
    SEL_MenuHandler m_pfnSelectors[2];
    
public:

    virtual void selected();
    virtual void unselected();
    virtual void activate();
    
    static CCMenuItemStateSprite * create(CCNode* sprite);
    
    bool initWithSprite(CCNode* sprite);
    
    void setTarget(CCObject *rec, SEL_MenuHandler selector, CCMenuItemStateSpriteState state);
};

#endif /* defined(__Cheetah__CCMenuItemBeganTouchSprite__) */
