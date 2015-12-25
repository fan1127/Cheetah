//
//  ParticleCache.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 3/21/2557 BE.
//
//

#include "ParticleHeaders.h"

ParticleCache* ParticleCache::create(ParticleType particleType, int amount)
{
    ParticleCache* cache = new ParticleCache();
    cache->initWithCapacity(amount);
    cache->nextInactiveParticle_ = 0;
    cache->addParticles(particleType, amount);
    cache->autorelease();
    return cache;
}

void ParticleCache::addParticles(ParticleType particleType, int amount)
{
    particleType_ = particleType;
    
    for (int i = 1; i <= amount; i++) {
        Particle *particle = Particle::create(particleType);
        particle->setVisible(false);
        ParticleSystem::sharedSystem()->batch_->addChild(particle);
        this->addObject(particle);
    }
}

void ParticleCache::removeParticles(int amount)
{
    for (int i = 1; i <= amount; i++) {
        this->removeLastObject();
    }
}

Particle* ParticleCache::getParticle()
{
    Particle* particle = (Particle*)this->objectAtIndex(nextInactiveParticle_);
    particle->setVisible(true);
    
    nextInactiveParticle_++;
    if (nextInactiveParticle_ >= this->count()) {
        nextInactiveParticle_ = 0;
    }
    
    return particle;
}


