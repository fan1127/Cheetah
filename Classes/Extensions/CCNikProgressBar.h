//
//  CCNikProgressBar.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/4/2557 BE.
//
//

#ifndef __Cheetah__CCNikProgressBar__
#define __Cheetah__CCNikProgressBar__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CCScale9SpriteMask : public CCNode {
    
public:
    
    static CCScale9SpriteMask* create();
    virtual bool init();
    
    virtual void visit();
};

class CCNikProgressBar : public CCNode {
    
    CCSprite *m_leftSprite;
    CCSprite *m_rightSprite;
    
    CCScale9SpriteMask *m_maskSprite;
    CCScale9Sprite *m_progressBarSprite;
    
    
    float m_percentage;
    float m_curretPercentage;
    
    float m_duration;
    
    float m_velocity;
    
public:
    
    static CCNikProgressBar* create(CCSprite *leftSprite, CCScale9Sprite *progressBarSprite, CCSprite *rightSprite);
    virtual bool init(CCSprite *leftSprite, CCScale9Sprite *progressBarSprite, CCSprite *rightSprite);
    
    virtual void update(float delta);
    virtual void visit();
    virtual void setContentSize(CCSize size);
    
    void setPercentage(float percentage);
    void setPercentage(float percentage, float duration);
    
};

#endif /* defined(__Cheetah__CCNikProgressBar__) */
