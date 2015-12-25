//
//  ShockwaveStraight.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/17/2557 BE.
//
//

#ifndef __Cheetah__ShockwaveStraight__
#define __Cheetah__ShockwaveStraight__

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>

#define SHOCKWAVE_SPEED (2800*retinaMultiplier)
#define SHOCKWAVE_SPEED_DURATION 0.5f
#define SHOCKWAVE_LIFE_DURATION 5.0f
#define SHOCKWAVE_TILE_PERCENT_FOR_BANANA 0.75f
#define SHOCKWAVE_TILE_PERCENT_FOR_PLAYER 1.2f

typedef enum {
    ShockWaveTypeNormalSide,
    ShockWaveTypeLeftSide,
    ShockWaveTypeRightSide,
}ShockWaveType;

class Pet;

class ShockwaveStraight: public cocos2d::CCSprite {
    
    ShockWaveType m_type;
    
    float m_usingSkillPetRunSpeed;
    float m_targetPetRunSpeed;
    
    float m_runningTime;
    float m_velocity;
    float m_angle;
    
    bool m_isStarted;
    bool m_isPaused;
    
    bool hitTargetPet;
    
    cocos2d::extension::CCSkeletonAnimation *m_spine;
    
public:
    
    Pet *m_usingSkillPet;
    Pet *m_targetPet;
    
    static ShockwaveStraight* create(Pet *usingSkillPet, Pet *targetPet, ShockWaveType type);
    
    bool init(Pet *usingSkillPet, Pet *targetPet, ShockWaveType type);
    
    void pauseShockwave();
    void resumeShockwave();
    
    virtual void update(float delta);
    
    void startShockwaveStraight();
    void startUpdate();
    void stopShockwaveStraight();
    
};

#endif /* defined(__Cheetah__ShockwaveStraight__) */
