//
//  ParticleClone.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 3/28/2557 BE.
//
//

#include "ParticleHeaders.h"


ParticleClone* ParticleClone::create(Particle *particle, cocos2d::CCPoint shadowOffset, float shadowOpacity)
{
    ParticleClone* particleClone = new ParticleClone();
    particleClone->initWithSpriteFrame(particle->displayFrame());
    particleClone->setInitialValues(particle, shadowOffset, shadowOpacity);
    return particleClone;
}
void ParticleClone::setInitialValues(Particle *particle, cocos2d::CCPoint shadowOffset, float shadowOpacity)
{
    shadowOffset_ = shadowOffset;
    shadowOpacityMult_ = shadowOpacity / 255.0f;
    
    cloneTarget_ = particle;
    cloneTarget_->retain();
    cloneTarget_->scheduleUpdate();
    
    this->setColor(cocos2d::ccc3(0,0,0));
}

void ParticleClone::update(float delta_)
{
    if (cloneTarget_->getParent() == NULL) {
        cloneTarget_->release();
        this->removeFromParentAndCleanup(true);
    }
    
    this->setPosition(cloneTarget_->getPosition() + shadowOffset_);
    this->setScale(cloneTarget_->getScale());
    this->setOpacity(cloneTarget_->getOpacity() * shadowOpacityMult_);
    this->setRotation(cloneTarget_->getRotation());
}

