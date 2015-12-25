//
//  GachaWheel.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/24/2557 BE.
//
//

#ifndef __Cheetah__GachaWheel__
#define __Cheetah__GachaWheel__

#include "cocos2d.h"

#include "../UIs/GachaItem.h"
#include "../VOs/GDPetVO.h"

class GachaWheel;

class CC_DLL GachaWheelDelegate
{
public:
    
    virtual void gachaWheelDidEndFreeSpining(GachaWheel *gachaWheel, GDItemVO itemVO) {};
    virtual void gachaWheelDidEndPremiumSpining(GachaWheel *gachaWheel, GDPetVO petVO) {};
    
};

typedef enum {
    GachaWheelStateShowCase,
    GachaWheelStateStart,
    GachaWheelStateBeforeFinish,
    GachaWheelStateFinish,
}GachaWheelState;

class GachaWheel: public cocos2d::CCNode {
    
    CCNode *contentNode;
    
    float m_rotation;
    
    cocos2d::CCArray *m_items;
    
    GachaType m_gachaType;
    GachaWheelState m_gachaWheelState;
    
    bool isCalurateDecreased;
    bool isCalurateBeforeFinish;
    
    float m_showcaseVelocity;
    float m_lockVelocity;
    float m_velocity;

    float m_increaseAcceleration;
    float m_decreaseAcceleration;
    float m_beforeFinishAcceleration;
    
    float m_extendRotation;
    
    float m_increaseSpeedDuration;
    float m_decreaseSpeedDuration;
    
    float m_beforeFinshDuration;

    float m_duration;
    float m_currentTime;
    
    int m_selectedIndex;
    
    GDPetVO m_selectedPetVO;
    GDItemVO m_selectedItemVO;
    
    float m_estimateRemainingRotation;
    float m_remainingRotation;
    
protected:
    
    GachaWheelDelegate *m_delegate;
    
    
public:
    
    static GachaWheel* create(GachaType type, cocos2d::CCSize size);
    virtual bool init(GachaType type, cocos2d::CCSize size);
    ~GachaWheel();
    
    virtual void update(float delta);
    virtual void visit();
    
    void setDefaultValues();
    void start(GDPetVO selectedPetVO);
    void start(GDItemVO selectedItemVO);
    void beforeFinish();
    void finish();
    void showCase();
    
    void setDelegate(GachaWheelDelegate *delegate) { m_delegate = delegate; };

};

#endif /* defined(__Cheetah__GachaWheel__) */
