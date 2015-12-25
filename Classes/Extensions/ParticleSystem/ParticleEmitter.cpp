//
//  ParticleEmitter.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 3/21/2557 BE.
//
//

#include "ParticleHeaders.h"

ParticleEmitter* ParticleEmitter::create(EmitterType type, ParticleCache* cache, float emissionRate, float emissionRateRandomRange, int particleAmount)
{
    ParticleEmitter* emitter = new ParticleEmitter();
    emitter->cache_ = cache;
    emitter->particleType_ = cache->particleType_;
    emitter->emissionRate_ = emissionRate;
    emitter->emissionRateRandomRange_ = emissionRateRandomRange;
    emitter->emissionAge_ = FIRST_EMISSION;
    emitter->particleAmount_ = particleAmount;
    emitter->emitterType_ = type;
    emitter->paused_ = false;
    emitter->scheduleUpdate();
    emitter->autorelease();
    return emitter;
}

void ParticleEmitter::start()
{
    if (emitterType_ == EmitterTypeSelfEmitting) {
        
        paused_ = false;
        
    } else {
        this->emitsOneParticle();
    }
    //CCLog_(@"Emittor: start /emitterRate:%f",emitterRate_);
}

void ParticleEmitter::stop()
{
    paused_ = true;
}

void ParticleEmitter::update(float delta_)
{
    if (paused_) return;
    
    float emissionRate = emissionRate_;
    if (emissionRateRandomRange_ != 0.0f)
        emissionRate += (emissionRateRandomRange_ * CCRANDOM_MINUS1_1());
    
    if (emissionAge_ == FIRST_EMISSION)
        emissionAge_ = emissionRate + delta_ +0.01f; //simulate first emission
    
    emissionAge_ -= delta_;
    
    if (emissionRate >= emissionAge_) {
        emissionAge_ = emissionAge_ + emissionRate;
        
        this->emitsOneParticle();
    }
}

void ParticleEmitter::emitOnce()
{
    for (int i = 1; i <= particleAmount_; i++) {
        this->emitsOneParticle();
    }
    this->removeFromParentAndCleanup(true);
}

void ParticleEmitter::emitsOneParticle()
{
    Particle *particle = cache_->getParticle();
    particle->setPosition(this->getPosition());
    particle->setInitialValues(particle->particleType_);
    particle->setParticleType(particle->particleType_);
    particle->resetValues();
    
    //restart selectors in all cases
    particle->stop();
    particle->start();
}


