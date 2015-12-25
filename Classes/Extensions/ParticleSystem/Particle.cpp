//
//  Particle.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 3/21/2557 BE.
//
//

#include "ParticleHeaders.h"

Particle* Particle::create(ParticleType particleType)
{
    Particle* particle = new Particle();
    const char* spriteFrameName = "run_smoke.png";
    particle->initWithSpriteFrameName(spriteFrameName);
    particle->setInitialValues(particleType);
    particle->setParticleType(particleType);
    particle->resetValues();
    
    //TODO:.. think about memory management. retain and release when cache is destroy?
    particle->autorelease();
    return particle;
}

void Particle::setInitialValues(ParticleType particleType)
{
    stage_ = 1;
    
    this->setVisible(false);
    
    particleType_ = particleType;
    particleTypeNext_ = ParticleTypeNothing;
    
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    
    //default (normal/nochanges) values
    life_ = psv0(1.0f, 0.0f);
    color__.start.r = psv0(255.0f, 0.0f);
    color__.start.g = psv0(255.0f, 0.0f);
    color__.start.b = psv0(255.0f, 0.0f);
    color__.end.r = psv0(255.0f, 0.0f);
    color__.end.g = psv0(255.0f, 0.0f);
    color__.end.b = psv0(255.0f, 0.0f);
    color__.speed = 1.0f;
    color__.ageSpeed = 1.0f;
    color__.ease = kEaseLinear;
    color__.animate = false;
    position__.start.x = psv0(0.0f, 0.0f);
    position__.start.y = psv0(0.0f, 0.0f);
    position__.end.x = psv0(0.0f, 0.0f);
    position__.end.y = psv0(0.0f, 0.0f);
    position__.speed = 1.0f;
    position__.ageSpeed = 1.0f;
    position__.ease = kEaseLinear;
    position__.autoAngle = false;
    position__.animate = false;
    scale__.start = psv0(1.0f, 0.0f);
    scale__.end = psv0(1.0f, 0.0f);
    scale__.speed = 1.0f;
    scale__.ageSpeed = 1.0f;
    scale__.ease = kEaseLinear;
    scale__.animate = false;
    opacity__.start = psv0(255, 0);
    opacity__.end = psv0(255, 0);
    opacity__.speed = 1.0f;
    opacity__.ageSpeed = 1.0f;
    opacity__.ease = kEaseLinear;
    opacity__.animate = false;
    rotation__.start = psv0(0.0f, 0.0f);
    rotation__.end = psv0(0.0f, 0.0f);
    rotation__.speed = 1.0f;
    rotation__.ageSpeed = 1.0f;
    rotation__.ease = kEaseLinear;
    rotation__.animate = false;
    
    physics_.linearVelocity.x = psv0(0.0f, 0.0f);
    physics_.linearVelocity.y = psv0(0.0f, 0.0f);
    physics_.maxLinearVelocity.x = psv0(0.0f, 0.0f);
    physics_.maxLinearVelocity.y = psv0(0.0f, 0.0f);
    physics_.minLinearVelocity.x = psv0(0.0f, 0.0f);
    physics_.minLinearVelocity.y = psv0(0.0f, 0.0f);
    physics_.linearAcceleration.x = psv0(0.0f, 0.0f);
    physics_.linearAcceleration.y = psv0(0.0f, 0.0f);
    physics_.angularVelocity = psv0(0.0f, 0.0f);
    physics_.angularAcceleration = psv0(0.0f, 0.0f);
}

void Particle::setParticleType(ParticleType particleType)
{
    cocos2d::CCSpriteFrameCache* cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    
    if (particleType == ParticleTypeRunningSmokeUp) {
        
        this->setDisplayFrame(cache->spriteFrameByName("run_smoke_op3.png"));
        //this->setAnchorPoint(ccp(0.5f, 0.0f));
        
        position__.start.x = psv0(0.0f, 5.0f);
        position__.start.y = psv0(0.0f, 5.0f);
        
        //life_ = psv0(0.14f, 0.1f);
        life_ = psv0(0.512f, 0.1f);
        
        scale__.animate = true;
        scale__.start = psv0(kMinimumScale, 0.0f);
        scale__.end = psv0(0.18f, 0.07f);
        
        opacity__.start = psv0(255.0f, 0.0f);
        
        
        //velocity = ค่าเริ่มต้น
        //acceleration = แรงพุ่ง แรงเฉื่อย
        
        physics_.linearVelocity.x = psv0(0.0f, 2.0f);
        physics_.linearVelocity.y = psv0(0.0f, 10.0f);
        physics_.linearAcceleration.x = psv0(0.0f, 30.0f);
        physics_.linearAcceleration.y = psv0(0.0f, 30.0f);
        physics_.angularVelocity = psv0(0.0f, 0.0f);
        physics_.angularAcceleration = psv0(0.0f, 0.0f);
        physics_.enable = true;
        
        
        particleTypeNext_ = ParticleTypeRunningSmokeDown;
        //particleTypeNext_ = ParticleTypeNothing;
        
    } else if (particleType == ParticleTypeRunningSmokeDown) {
        
        //life_ = psv0(1.066666667f, 0.032f);
        life_ = psv0(1.28f, 0.0384f);
        
        //scale__.ease = 0.8;
        scale__.ease = kEaseLinear;
        scale__.animate = true;
        scale__.end = psv0(0.0f, 0.0f);
        
        //rotation__.ageSpeed = 4.0f; //.1953125
        
        opacity__.start = psv0(255.0f, 0.0f);
        
        particleTypeNext_ = ParticleTypeNothing;
        
        
        
    } else if (particleType == ParticleTypeRunSmoke1) {
        
        this->setDisplayFrame(cache->spriteFrameByName("run_smoke_detail_op2.png"));
        //this->setAnchorPoint(ccp(0.5f, 0.0f));
        
        position__.start.x = psv0(0.0f, 5.0f);
        position__.start.y = psv0(0.0f, 5.0f);
        
        //life_ = psv0(0.14f, 0.1f);
        life_ = psv0(0.512f, 0.1f);
        
        scale__.animate = true;
        scale__.start = psv0(kMinimumScale, 1.0f);
        scale__.end = psv0(1.6f, 0.07f);
        
        opacity__.start = psv0(255.0f, 0.0f);
        
        physics_.linearVelocity.x = psv0(0.0f, 2.0f);
        physics_.linearVelocity.y = psv0(0.0f, 10.0f);
        physics_.linearAcceleration.x = psv0(0.0f, 30.0f);
        physics_.linearAcceleration.y = psv0(0.0f, 30.0f);
        physics_.angularVelocity = psv0(120.0f, 60.0f);
        physics_.angularAcceleration = psv0(-10.0f, 0.0f);
        physics_.enable = true;
        
        color__.start.r = psv0(247.0f, 0.0f);
        color__.start.g = psv0(238.0f, 0.0f);
        color__.start.b = psv0(226.0f, 0.0f);
        
        particleTypeNext_ = ParticleTypeRunSmoke2;
        
    } else if (particleType == ParticleTypeRunSmoke2) {
        
        
        //life_ = psv0(1.066666667f, 0.032f);
        life_ = psv0(1.28f, 0.0384f);
        
        //scale__.ease = 0.8;
        scale__.ease = kEaseLinear;
        scale__.animate = true;
        scale__.end = psv0(0.0f, 0.0f);
        
        //rotation__.ageSpeed = 4.0f; //.1953125
        
        opacity__.start = psv0(255.0f, 0.0f);
        
        particleTypeNext_ = ParticleTypeNothing;
        
        
    }
}

void Particle::start()
{
    this->setVisible(true);
    this->unscheduleUpdate();
    this->scheduleUpdate();
    
    
    //TODO:... do this in update instead
    //scheduleOnce(schedule_selector(Particle::remove), life_.rValue-age_);
}

void Particle::stop()
{
    this->unscheduleUpdate();
}

void Particle::resetValues()
{
    
    
    age_ = 0.0f;
    position__.age = 0.0f;
    position__.ageSpeed = 1.0f;
    color__.age = 0.0f;
    color__.ageSpeed = 1.0f;
    scale__.age = 0.0f;
    scale__.ageSpeed = 1.0f;
    opacity__.age = 0.0f;
    opacity__.ageSpeed = 1.0f;
    rotation__.age = 0.0f;
    rotation__.ageSpeed = 1.0f;
    
    life_.rValue = this->getRValueFromPSValue(life_);
    
    //TODO:.. use lifeLeft_ to keep life between stop and start?
    lifeLeft_ = life_.rValue;
    
    //Color
    if (stage_ > 1) {
        color__.start.r.rValue = this->getColor().r;
        color__.start.g.rValue = this->getColor().g;
        color__.start.b.rValue = this->getColor().b;
    } else {
        color__.start.r.rValue = this->getRValueFromPSValue(color__.start.r);
        color__.start.g.rValue = this->getRValueFromPSValue(color__.start.g);
        color__.start.b.rValue = this->getRValueFromPSValue(color__.start.b);
    }
    color__.end.r.rValue = this->getRValueFromPSValue(color__.end.r);
    color__.end.g.rValue = this->getRValueFromPSValue(color__.end.g);
    color__.end.b.rValue = this->getRValueFromPSValue(color__.end.b);
    
    this->setColor(cocos2d::ccc3((int)color__.start.r.rValue,
                                 (int)color__.start.g.rValue,
                                 (int)color__.start.b.rValue));
    
    //Rotation
    if (stage_ > 1) {
        rotation__.start.rValue = this->getRotation();
    } else {
        rotation__.start.rValue = this->getRValueFromPSValue(rotation__.start);
    }
    rotation__.start.rValue = this->getRValueFromPSValue(rotation__.start);
    rotation__.end.rValue = this->getRValueFromPSValue(rotation__.end);
    this->setRotation(rotation__.start.rValue);
    
    //Position
    position__.start.x.rValue = this->getRValueFromPSValue(position__.start.x);
    position__.start.y.rValue = this->getRValueFromPSValue(position__.start.y);
    position__.end.x.rValue = this->getRValueFromPSValue(position__.end.x);
    position__.end.y.rValue = this->getRValueFromPSValue(position__.end.y);
    
    if (position__.autoAngle && position__.animate) {
        float dx = position__.end.x.rValue - position__.start.x.rValue;
        float dy = position__.end.y.rValue - position__.start.y.rValue;
        
        float autoAngle = CC_RADIANS_TO_DEGREES(atan2(dy, dx));
        
        if (autoAngle < 0) {
            autoAngle = (autoAngle * -1) + 90.0f;
        } else if (autoAngle > 0){
            autoAngle = 90.0f - autoAngle;
        }
        
        this->setRotation(autoAngle);
    }
    
    if (stage_ > 1) {
        position__.start.x.rValue = this->getPositionX();
        position__.start.y.rValue = this->getPositionY();
    } else {
        position__.start.x.rValue += this->getPositionX();
        position__.start.y.rValue += this->getPositionY();
    }
    position__.end.x.rValue += this->getPositionX();
    position__.end.y.rValue += this->getPositionY();
    
    this->setPosition(ccp(position__.start.x.rValue,
                          position__.start.y.rValue));
    
    //Scale
    if (stage_ > 1) {
        scale__.start.rValue = this->getScale();
    } else {
        scale__.start.rValue = this->getRValueFromPSValue(scale__.start);
    }
    scale__.end.rValue = this->getRValueFromPSValue(scale__.end);
    this->setScale(scale__.start.rValue);
    
    //Opacity
    if (stage_ > 1) {
        opacity__.start.rValue = this->getOpacity();
    } else {
        opacity__.start.rValue = this->getRValueFromPSValue(opacity__.start);
    }
    opacity__.start.rValue = this->getRValueFromPSValue(opacity__.start);
    opacity__.end.rValue = this->getRValueFromPSValue(opacity__.end);
    this->setOpacity(opacity__.start.rValue);
    
    //Physics ------
    if (physics_.enable) {
        //Position -
        physics_.linearVelocity.x.rValue = this->getRValueFromPSValue(physics_.linearVelocity.x);
        physics_.linearVelocity.y.rValue = this->getRValueFromPSValue(physics_.linearVelocity.y);
        physics_.maxLinearVelocity.x.rValue = this->getRValueFromPSValue(physics_.maxLinearVelocity.x);
        physics_.maxLinearVelocity.y.rValue = this->getRValueFromPSValue(physics_.maxLinearVelocity.y);
        physics_.minLinearVelocity.x.rValue = this->getRValueFromPSValue(physics_.minLinearVelocity.x);
        physics_.minLinearVelocity.y.rValue = this->getRValueFromPSValue(physics_.minLinearVelocity.y);
        physics_.linearAcceleration.x.rValue = this->getRValueFromPSValue(physics_.linearAcceleration.x);
        physics_.linearAcceleration.y.rValue = this->getRValueFromPSValue(physics_.linearAcceleration.y);
        
        //Rotation -
        physics_.angularVelocity.rValue = this->getRValueFromPSValue(physics_.angularVelocity);
        physics_.angularAcceleration.rValue = this->getRValueFromPSValue(physics_.angularAcceleration);
    }
}

void Particle::update(float delta_)
{
    //CCLog_(@"Particle: update:%f scale:%f",delta_, self.scale);
    
    //Aging
    age_ = age_ + delta_;
    
    //TODO:.. temp
    //if (true) { //
    if (age_ <= life_.rValue) {
        
        //Start-to-End Animation
        if (position__.animate) {
            if (position__.ease != kEaseLinear) {
                position__.ageSpeed = position__.ageSpeed * position__.ease;
                position__.age = position__.age + (delta_ * position__.ageSpeed);
            } else {
                position__.age = position__.age + delta_;
            }
            
            if ((position__.age * position__.speed) <= life_.rValue) {
                this->setPosition(this->getNewCCPointFromPSPointAnimation(position__));
            }
        }
        
        if (color__.animate) {
            if (color__.ease != kEaseLinear) {
                color__.ageSpeed = color__.ageSpeed * color__.ease;
                color__.age = color__.age + (delta_ * color__.ageSpeed);
            } else {
                color__.age = color__.age + delta_;
            }
            
            if ((color__.age * color__.speed) <= life_.rValue) {
                this->setColor(this->getNewColorFromPSColorAnimation(color__));
            }
        }
        
        if (scale__.animate) {
            if (scale__.ease != kEaseLinear) {
                scale__.ageSpeed = scale__.ageSpeed * scale__.ease;
                scale__.age = scale__.age + (delta_ * scale__.ageSpeed);
            } else {
                scale__.age = scale__.age + delta_;
            }
            
            if ((scale__.age * scale__.speed) <= life_.rValue) {
                this->setScale(this->getNewValueFromPSValueAnimation(scale__));
                
                //when nearly invisible, remove right away
                if (this->getScale() < kMinimumScale) {
                    this->remove();
                }
            }
        }
        
        if (opacity__.animate) {
            if (opacity__.ease != kEaseLinear) {
                opacity__.ageSpeed = opacity__.ageSpeed * opacity__.ease;
                opacity__.age = opacity__.age + (delta_ * opacity__.ageSpeed);
            } else {
                opacity__.age = opacity__.age + delta_;
            }
            
            if ((opacity__.age * opacity__.speed) <= life_.rValue) {
                this->setOpacity((int)this->getNewValueFromPSValueAnimation(opacity__));
                
                //when nearly invisible, remove right away
                if (this->getOpacity() <= 10) {
                    this->remove();
                }
            }
        }
        
        if (rotation__.animate) {
            if (rotation__.ease != kEaseLinear) {
                rotation__.ageSpeed = rotation__.ageSpeed * rotation__.ease;
                rotation__.age = rotation__.age + (delta_ * rotation__.ageSpeed);
            } else {
                rotation__.age = rotation__.age + delta_;
            }
            
            if ((rotation__.age * rotation__.speed) <= life_.rValue) {
                this->setRotation(this->getNewValueFromPSValueAnimation(rotation__));
            }
        }
        
        //Physics Animation
        if (physics_.enable) {
            //Linear Acceleration
            physics_.linearVelocity.x.rValue =
            physics_.linearVelocity.x.rValue + (physics_.linearAcceleration.x.rValue * delta_);
            physics_.linearVelocity.y.rValue =
            physics_.linearVelocity.y.rValue + (physics_.linearAcceleration.y.rValue * delta_);
            /*
             //Max-Min Linear Velocity
             if (physics_.maxLinearVelocity.x.rValue != 0.0f)
             {
             if (physics_.linearVelocity.x.rValue >= physics_.maxLinearVelocity.x.rValue)
             physics_.linearVelocity.x.rValue = physics_.maxLinearVelocity.x.rValue;
             if (physics_.linearVelocity.x.rValue <= physics_.minLinearVelocity.x.rValue)
             physics_.linearVelocity.x.rValue = physics_.minLinearVelocity.x.rValue;
             }
             if (physics_.maxLinearVelocity.y.rValue != 0.0f)
             {
             if (physics_.linearVelocity.y.rValue >= physics_.minLinearVelocity.y.rValue)
             physics_.linearVelocity.y.rValue = physics_.minLinearVelocity.y.rValue;
             }
             */
            //Linear Transform
            this->setPosition(ccp(this->getPositionX() + (physics_.linearVelocity.x.rValue * delta_),
                                  this->getPositionY() + (physics_.linearVelocity.y.rValue * delta_)));
            
            
            //Angular Acceleration
            physics_.angularVelocity.rValue =
            physics_.angularVelocity.rValue + (physics_.angularAcceleration.rValue * delta_);
            
            //Angular Transform
            this->setRotation(this->getRotation() + (physics_.angularVelocity.rValue * delta_));
        }
    } else {
        
        if (particleTypeNext_ != ParticleTypeNothing)
        {
            stage_++;
            this->setParticleType(particleTypeNext_);
            this->resetValues();
        } else {
            this->remove();
        }
    }
}

float Particle::getRValueFromPSValue(PSValue psValue)
{
    //if (NO) {
    if (psValue.valueVar == 0.0f) {
        return psValue.value;
    } else {
        return psValue.value + (psValue.valueVar * CCRANDOM_MINUS1_1());
    }
}

float Particle::getNewValueFromPSValueAnimation(PSValueAnimation anim)
{
    return anim.start.rValue +
    (((anim.age * anim.speed)/ life_.rValue) * (anim.end.rValue - anim.start.rValue));
}

cocos2d::CCPoint Particle::getNewCCPointFromPSPointAnimation(PSPointAnimation anim)
{
    return ccp(anim.start.x.rValue +
               (((anim.age * anim.speed)/ life_.rValue) *
                (anim.end.x.rValue  - anim.start.x.rValue)),
               anim.start.y.rValue +
               (((anim.age * anim.speed)/ life_.rValue) *
                (anim.end.y.rValue  - anim.start.y.rValue))
               );
    /*
     return ccp(anim.start.x.rValue +
     (((age_ * anim.speed)/ life_.rValue) * (anim.end.x.rValue  - anim.start.x.rValue)),
     anim.start.y.rValue +
     (((age_ * anim.speed)/ life_.rValue) * (anim.end.y.rValue  - anim.start.y.rValue))
     );
     */
}

cocos2d::ccColor3B Particle::getNewColorFromPSColorAnimation(PSColorAnimation anim)
{
    return cocos2d::ccc3(anim.start.r.rValue +
                         (((anim.age * anim.speed)/ life_.rValue) *
                          (anim.end.r.rValue  - anim.start.r.rValue)),
                         anim.start.g.rValue +
                         (((anim.age * anim.speed)/ life_.rValue) *
                          (anim.end.g.rValue  - anim.start.g.rValue)),
                         anim.start.b.rValue +
                         (((anim.age * anim.speed)/ life_.rValue) *
                          (anim.end.b.rValue  - anim.start.b.rValue)));
}


void Particle::remove()
{
    this->stop();
    this->setVisible(false);
}

void Particle::destroy()
{
    this->unscheduleUpdate();
    this->removeFromParentAndCleanup(true);
}
