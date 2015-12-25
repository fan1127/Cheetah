//
//  ExplodsionPaticle.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 5/6/2557 BE.
//
//

#include "ExplosionParticle.h"


ExplosionParticle* ExplosionParticle::create(cocos2d::CCSprite *particleSprite, float velocityX, float velocotyY)
{
    ExplosionParticle *obj = new ExplosionParticle();
    if (!obj->init(particleSprite, velocityX, velocotyY)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool ExplosionParticle::init(cocos2d::CCSprite *particleSprite, float velocityX, float velocotyY)
{
    if (!CCNode::init()) {
        return false;
    }
    
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    
    m_duration = 0.6f;
    m_time = 0;
    
    m_velocityX = velocityX;
    m_velocityY = velocotyY;
    
    m_accelerateY = -1500;
    
    m_scale = particleSprite->getScale();
    m_destinationScale = m_scale*0.1f;
    
    m_velocityScale = (m_destinationScale - m_scale)/m_duration;
    
    m_particleSprite = particleSprite;
    
    this->addChild(m_particleSprite);
    

    return true;
}

void ExplosionParticle::updateValue()
{
    m_velocityScale = (m_destinationScale - m_scale)/m_duration;
}

void ExplosionParticle::start()
{
    scheduleUpdate();
}

void ExplosionParticle::start(float delay)
{
    this->runAction(CCSequence::create(CCDelayTime::create(delay),
                                       CCCallFunc::create(this, callfunc_selector(ExplosionParticle::start)),
                                       NULL));
}

void ExplosionParticle::end()
{
    unscheduleUpdate();
    removeFromParent();
}

void ExplosionParticle::update(float delta)
{
    
    if (m_velocityY < 0) {
        m_accelerateY /= 2;
    }
    
    float sX = m_velocityX*delta;
    float sY = m_velocityY*delta + 0.5*m_accelerateY*pow(delta, 2);
    m_velocityY = m_velocityY + m_accelerateY*delta;
    
    m_particleSprite->setPosition(m_particleSprite->getPosition() + ccp(sX, sY));
    
    m_particleSprite->setScale(m_particleSprite->getScale() + m_velocityScale*delta);
    
    m_time += delta;
    if (m_time > m_duration) {
        end();
    }
}

#pragma mark - Group

ExplosionParticleGroup* ExplosionParticleGroup::create(const char *filename, float scale)
{
    ExplosionParticleGroup *obj = new ExplosionParticleGroup();
    if (!obj->init(filename, scale)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}


ExplosionParticleGroup* ExplosionParticleGroup::animate(const char *filename, float scale, cocos2d::CCNode *onNode)
{
    ExplosionParticleGroup *obj = ExplosionParticleGroup::create(filename, scale);
    obj->setPosition(onNode->getContentSize()*0.5f);
    onNode->addChild(obj, 1000);
    obj->animate(true);
    return obj;
}

bool ExplosionParticleGroup::init(const char *filename, float scale)
{
    if (!CCNode::init()) {
        return false;
    }
    
    m_fileName = filename;
    m_scale = scale;
    
    m_maxVelocityX = 500;
    m_maxVelocityY = 1400;
    m_accelerateY = -7000;
    
    m_amount = 9;
    m_groupAmount = 1;
    m_duration = 0.3f;
    
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    
    return true;
}

void ExplosionParticleGroup::animate(bool removeSelf)
{
    for (int k = 0; k < m_groupAmount; k++) {
        for (int i = 0; i <= 360 ; i+= (360/m_amount)) {
            CCSprite *sprite = CCSprite::create(m_fileName);
            sprite->setScale(m_scale*(0.4 + arc4random()%61/100.f));
            
            float velocityX = m_maxVelocityX*sin(i * 1.0f)*(0.7f + (arc4random()%31)/100.f);
            
            float velocityY = m_maxVelocityY*cos(i * 1.0f)*(0.7f + (arc4random()%31)/100.f);
            if (velocityY < 0) {
                velocityY = velocityY*0.25f;
            }
            
            ExplosionParticle *explosionParticle = ExplosionParticle::create(sprite, velocityX, velocityY);
            explosionParticle->setDuration(m_duration*(0.7f + (arc4random()%31)/100.f));
            explosionParticle->setAccelerateY(m_accelerateY);
            this->addChild(explosionParticle);
            explosionParticle->start(arc4random()%11/100.f);
        }
    }
    
    if (removeSelf) {
        this->runAction(CCSequence::create(CCDelayTime::create(m_duration + 0.1f),
                                           CCRemoveSelf::create(),
                                           NULL));
    }
}
