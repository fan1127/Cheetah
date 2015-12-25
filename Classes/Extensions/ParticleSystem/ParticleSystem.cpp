//
//  ParticleSystem.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 3/21/2557 BE.
//
//

#include "ParticleHeaders.h"

#include "../../AppMacros.h"

extern TargetDevice targetDevice;


static ParticleSystem *s_SharedSystem = NULL;

ParticleSystem* ParticleSystem::sharedSystem()
{
    if (s_SharedSystem == NULL)
    {
        s_SharedSystem = new ParticleSystem();
        s_SharedSystem->init();
    }
    
    return s_SharedSystem;
}

bool ParticleSystem::init()
{
    if (!CCNode::init()) {
        return false;
    }
    
    switch (targetDevice) {
        case TD_IPHONE:         batch_ = cocos2d::CCSpriteBatchNode::create("common/particles/particles-medium.png", 1);     break;
        case TD_IPHONE_RETINA:  batch_ = cocos2d::CCSpriteBatchNode::create("common/particles/particles-medium.png", 1);     break;
        case TD_IPAD:           batch_ = cocos2d::CCSpriteBatchNode::create("common/particles/particles-large.png", 1);      break;
        case TD_IPAD_RETINA:    batch_ = cocos2d::CCSpriteBatchNode::create("common/particles/particles-large.png", 1);      break;
    }
    s_SharedSystem->addChild(batch_);
    cacheStore_ = new cocos2d::CCDictionary;
    
    addParticleCache(ParticleTypeRunSmoke1, kParticleTypeRunSmoke1Amount);
    
    return true;
}

ParticleSystem::~ParticleSystem() {
    
    if (batch_) {
        batch_->removeFromParentAndCleanup(true);
        batch_ = NULL;
    }
    
}

void ParticleSystem::pauseAllParticles()
{
    CCObject *object = NULL;
    CCARRAY_FOREACH(batch_->getChildren(), object)
    {
        Particle *particle = (Particle*)object;
        
        if (particle->isVisible()) {
            particle->stop();
        }
    }
}

void ParticleSystem::resumeAllParticles()
{
    CCObject *object = NULL;
    CCARRAY_FOREACH(batch_->getChildren(), object)
    {
        Particle *particle = (Particle*)object;
        if (particle->isVisible()) {
            particle->start();
        }
    }
}

void ParticleSystem::removeAllParticles()
{
    CCObject *object = NULL;
    CCARRAY_FOREACH(batch_->getChildren(), object)
    {
        Particle *particle = (Particle*)object;
        particle->remove();
    }
}

void ParticleSystem::addParticleCache(ParticleType particleType, int particleAmount)
{
    if (this->getParticleCache(particleType) != NULL) {
        ParticleCache* cache = this->getParticleCache(particleType);
        cache->addParticles(particleType, particleAmount);
    } else {
        cacheStore_->setObject(ParticleCache::create(particleType, particleAmount),
                               cocos2d::CCString::createWithFormat("%d",particleType)->getCString());
    }
}

ParticleCache* ParticleSystem::getParticleCache(ParticleType particleType)
{
    return (ParticleCache*)cacheStore_->objectForKey(cocos2d::CCString::createWithFormat("%d",particleType)->getCString());
}




