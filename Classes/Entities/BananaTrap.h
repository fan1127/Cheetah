//
//  BananaTrap.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/22/2557 BE.
//
//

#ifndef __Cheetah__BananaTrap__
#define __Cheetah__BananaTrap__

#include <iostream>

#include "cocos2d.h"

#define DEFAULT_TRAP_ACTIVATION_TIME 0.1f

USING_NS_CC;

class Pet;
class Track;

class BananaTrap : public CCSprite {
    
    Pet *m_usingSkillPet;
    Track *m_trap;
    
    bool m_addTrapDidFinish;
    float m_sizeMultiplier;
    float m_trapActivationTime;

    float m_startY, m_endY;
    float m_inAirDelayTime;
    float m_timeInAir;

public:
    
    static BananaTrap* create(Pet *usingSkillPet);
    bool init(Pet *usingSkillPet);
    
    void update(float delta);
    
    void addTrapFinished();
    void removeTrap();
};

#endif /* defined(__Cheetah__BananaTrap__) */
