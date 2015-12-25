//
//  ProjectileParticle.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 5/8/2557 BE.
//
//

#ifndef __Cheetah__ProjectileParticle__
#define __Cheetah__ProjectileParticle__

#include "cocos2d.h"

USING_NS_CC;

class ProjectileParticle : CCNode {

    CCSprite *m_particleSprite;
    
    float m_currentScale;
    float m_defaultScale;
    float m_velocityScaleUp;
    float m_velocityScaleDown;

    
    float m_duration;
    float m_time;
    
    float m_velocityX;
    float m_velocityY;
    
    float m_accelerateY;
    
    CCPoint m_currentPoint;
    CCPoint m_startPoint;
    CCPoint m_endPoint;
    
public:
    
    static ProjectileParticle *create(CCSprite *particleSprite);
    
    virtual bool init(CCSprite *particleSprite);
    virtual void update(float delta);
    
    ~ProjectileParticle();
    
    void animate(CCNode *onNode, CCPoint startPoint, CCPoint endPoint, float duration);
    void animateCollectItem(CCNode *onNode, CCPoint startPoint, CCPoint endPoint, float duration);
};

#endif /* defined(__Cheetah__ProjectileParticle__) */
