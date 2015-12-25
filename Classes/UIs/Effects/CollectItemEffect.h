//
//  CollectGemsEffect.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/11/2557 BE.
//
//

#ifndef __Cheetah__CollectItemEffect__
#define __Cheetah__CollectItemEffect__

#include "cocos2d.h"

#define kCollectItemEffectZOrder 1000
#define kCollectItemEffectDefaultSprite "common/vfx/blue_particle_fx.png"

class CollectItemEffect : public cocos2d::CCLayer {
    
    cocos2d::CCPoint m_initialPosition;
    cocos2d::CCPoint m_destinationPosition;
    
    std::string effectFilename;
    
    void *m_onFinished_sender;
    void (*m_onFinished_callback)(void*);
    
public:
    
    static bool addCollectItemEffect(CollectItemEffect* effect,
                                     cocos2d::CCNode* node);
    static void remCollectItemEffect(CollectItemEffect* effect);
    static int countCollectItemEffect();
                                     
    static CollectItemEffect* create(const char* spriteFrameName,
                                     const char* effectFileName,
                                     cocos2d::CCPoint initialPosition,
                                     cocos2d::CCPoint destinationPosition,
                                     void* sender,
                                     void (*callback)(void*));
    
    bool init(const char* spriteFrameName,
              const char* effectFilename,
              cocos2d::CCPoint initialPosition,
              cocos2d::CCPoint destinationPosition,
              void* sender,
              void (*callback)(void*));
    
private:
    
    void volatileAnimation(cocos2d::CCSprite* gem);
    void finish();
    
};

#endif /* defined(__Cheetah__CollectItemEffect__) */
