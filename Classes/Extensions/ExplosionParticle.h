//
//  ExplodsionPaticle.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 5/6/2557 BE.
//
//

#ifndef __Cheetah__ExplosionParticle__
#define __Cheetah__ExplosionParticle__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

class ExplosionParticle : public CCNode {
    
    CCSprite *m_particleSprite;
    
    float m_duration;
    float m_time;
    
    float m_scale;
    float m_destinationScale;
    float m_velocityScale;

    float m_velocityX;
    float m_velocityY;
    
    float m_accelerateY;
    
public:
    
    static ExplosionParticle *create(CCSprite *particleSprite, float velocityX, float velocotyY);
    virtual bool init(CCSprite *particleSprite, float velocityX, float velocotyY);
    
    virtual void update(float delta);
    
    void setAccelerateY(float accelerate) { m_accelerateY = accelerate; updateValue();}
    void setDuration(float duration) { m_duration = duration; updateValue();}
    
    void updateValue();
    
    void start();
    void start(float delay);
    void end();

};

class ExplosionParticleGroup : public CCNode {
    
    
    const char *m_fileName;
    
    float m_scale;
    float m_maxVelocityX;
    float m_maxVelocityY;
    float m_accelerateY;
    
    float m_duration;

    int m_groupAmount;
    int m_amount;
    
public:
    
    static ExplosionParticleGroup *create(const char *filename, float scale);
    virtual bool init(const char *filename, float scale);
    
    static ExplosionParticleGroup *animate(const char *filename, float scale, CCNode *onNode);
    
    void setAmount(int amount) { m_amount = amount; }
    void setGroupAmount(int amount) { m_groupAmount = MAX(9, amount); }
    
    void setMaxVelocityX(float velocity) { m_maxVelocityX = velocity; }
    void setMaxVelocityY(float velocity) { m_maxVelocityY = velocity; }
    
    void setAccelerateY(float accelerate) { m_accelerateY = accelerate; }
    
    void setDuration(float duration) { m_duration = duration; }
    
    void animate(bool removeSelf);
    
};


#endif /* defined(__Cheetah__ExplosionParticle__) */
