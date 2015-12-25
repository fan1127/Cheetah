//
//  CCProgressBar9Sprite.h
//  Cheetah
//
//  Created by Kaweerut on 2/25/57 BE.
//
//

#ifndef __Cheetah__CCProgressBar9Sprite__
#define __Cheetah__CCProgressBar9Sprite__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CCProgressBar9Sprite : public cocos2d::CCNode
{
    CCSize _size;
    CCPoint _offset;
    
    float _innerMinWidth;
    CCSize _innerMaxSize;
    float _percentage;
    float _BgPercentage;
    
    CCScale9Sprite *timer = NULL;
    CCScale9Sprite *backgroundTimer = NULL;
    
//    float _tmpTime;
//    float _duration;
//    float _increaseRate;
    
public:
    
    static CCProgressBar9Sprite *create(CCScale9Sprite *backgroundSprite, CCScale9Sprite *innerSprite, CCSize size, CCPoint offset, CCPoint shift);
    static CCProgressBar9Sprite *create(CCScale9Sprite *backgroundSprite, CCScale9Sprite *backgroundInnerSprite,CCScale9Sprite *innerSprite, CCSize size, CCPoint offset, CCPoint shift);
    bool init(CCScale9Sprite *backgroundSprite, CCScale9Sprite *backgroundInnerSprite,CCScale9Sprite *innerSprite, CCSize size, CCPoint offset, CCPoint shift);
    virtual void update(float delta);
    
    void setPercentage(float fPercentage);
    void setPercentage(float fPercentage, float duration);
    void setBackgroundPercentage(float fPercentage);
    void setBackgroundPercentage(float fPercentage, float duration);
    
    
    
};

#endif /* defined(__Cheetah__CCProgressBar9Sprite__) */
