//
//  CCProgressBar.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 1/17/2557 BE.
//
//

#ifndef __Cheetah__CCProgressBar__
#define __Cheetah__CCProgressBar__

#include "cocos2d.h"

USING_NS_CC;

class CCProgressBar : public cocos2d::CCNode
{
    
    const char *_name;
    CCProgressTimer *timer = NULL;
    CCProgressTimer *backgroundTimer = NULL;
public:
    
    static CCProgressBar *create(CCSprite *backgroundSprite, CCSprite *innerSprite);
    static CCProgressBar *create(CCSprite *backgroundSprite, CCSprite *backgroundInnerSprite,CCSprite *innerSprite);
    bool init(CCSprite *backgroundSprite, CCSprite *backgroundInnerSprite,CCSprite *innerSprite);
    
    void setPercentage(float fPercentage);
    void setPercentage(float fPercentage, float duration);
    void setBackgroundPercentage(float fPercentage);
    void setBackgroundPercentage(float fPercentage, float duration);
};

#endif /* defined(__Cheetah__CCProgressBar__) */
