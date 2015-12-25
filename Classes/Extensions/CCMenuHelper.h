//
//  CCMenuHelper.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 7/28/2557 BE.
//
//

#ifndef __Cheetah__CCMenuHelper__
#define __Cheetah__CCMenuHelper__

#include <iostream>
#include <string>
#include "cocos2d.h"

#define CCMENUHELPER_TAG_ITEMSCALESPRITE 123

#include "CCMenuItemScaleSprite.h"

class CCMenuHelper: public cocos2d::CCNode {
    
public:
    
    static cocos2d::CCMenu *createMenu(const char* frameName, CCObject* target, cocos2d::SEL_MenuHandler selector);
    static cocos2d::CCMenu *createMenu(cocos2d::CCSprite *sprite, CCObject* target, cocos2d::SEL_MenuHandler selector);
    
    static CCMenuItem *getMenuItemScaleSprite(cocos2d::CCMenu *menu);
    static CCMenuItem *getMenuItemScaleSprite(CCMenu *menu, int index);
};

#endif /* defined(__Cheetah__CCMenuHelper__) */
