//
//  Vehicle.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/14/2556 BE.
//
//

#ifndef __Cheetah__Vehicle__
#define __Cheetah__Vehicle__

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>

#define kVehicleMovingSpeedDefault      2.5f
#define kVehicleSrcDir                  "common/environment/"

enum VehicleState {
    
    VS_STOP,
    VS_MOVING_LEFT_DOWN,
    VS_MOVING_LEFT_UP,
    VS_MOVING_RIGHT_DOWN,
    VS_MOVING_RIGHT_UP
    
};


class Vehicle: public cocos2d::CCNode {
    
    cocos2d::extension::CCSkeletonAnimation *m_vehicleFront = NULL;
    cocos2d::extension::CCSkeletonAnimation *m_vehicleBack = NULL;
    
    VehicleState        m_state;
    
//    cocos2d::CCPoint    m_distPosition;
    std::vector<cocos2d::CCPoint> m_distPositions;
    int                 m_stopAt;
    float               m_movingSpeed       = kVehicleMovingSpeedDefault;
    
    void _flip(bool flip_);
    
public:
    
    bool                autoRemove;
    bool                dropped;
    
    /* if filename is bus_front.json then spinename will be "bus"
     * In order to create vehicle, it needs two spine files {spinename}_front and {spinename}_back
     * in the directory
     */
    static Vehicle *createWithSpineName(const char *spine_name_, bool auto_remove_, void *preferenceRootParent_);
    bool initWithSpineName(const char *spine_name_, bool auto_remove_, void *preferenceRootParent_);
  
    bool updateMoving(float delta_);
    
//    void setDeliverVistors(bool deliver);
    
    void setVehicleState(VehicleState state_);
    void setVehicleSize(cocos2d::CCSize size1_, cocos2d::CCSize size2_);
    void setVehicleSpeed(float moving_speed_);
    void setVehicleStopAt(int stop_at_);
    
    void addDistinationPosition(cocos2d::CCPoint pos_);
//    void setDriveTo(cocos2d::CCPoint to_pos_);
//    void setDriveTo(cocos2d::CCPoint from_pos_, cocos2d::CCPoint to_pos_);
//    void setDriveTo(cocos2d::CCPoint from_pos_, cocos2d::CCPoint to_pos_, float moving_speed_);
    
    void willContinue();
    
};

#endif /* defined(__Cheetah__Vehicle__) */
