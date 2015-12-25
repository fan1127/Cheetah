//
//  CloudShadow.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 7/24/2557 BE.
//
//

#ifndef __Cheetah__CloudShadow__
#define __Cheetah__CloudShadow__

#include "cocos2d.h"

#define kCloudShadowScale               28.78f
#define kCloudShadowNum                 3
#define kCloudShadowMovingSpeedMin      3
#define kCloudShadowMovingSpeedMax      13
#define kCloudShadowMovingSpeedRate     0.1f

class CloudShadow : public cocos2d::CCLayer {
    
    cocos2d::CCSprite* m_cloudSprites[kCloudShadowNum];
    
public:
    
    static CloudShadow* createWithContentSize(cocos2d::CCSize contentSize_);
    bool initWithContentSize(cocos2d::CCSize contentSize_);
    
    virtual void update(float delta_);
    
    void resetCloud(cocos2d::CCSprite* cloudSprite_, bool first_time_);
    
};

#endif /* defined(__Cheetah__CloudShadow__) */
