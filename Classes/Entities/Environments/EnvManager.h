//
//  EnvManager.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/14/2556 BE.
//
//

#ifndef __Cheetah__EnvManager__
#define __Cheetah__EnvManager__

#include "Vehicle.h"
#include "Visitor.h"
#include "CloudShadow.h"

class EnvManager: public cocos2d::CCLayer {
    
    std::vector<Vehicle*>       m_vehicleLane1;
    std::vector<Vehicle*>       m_vehicleLane2;
    std::vector<Visitor*>       m_visitors;
    CloudShadow*                m_cloudShadow;
    
    float                       m_timer;
    int                         m_count;
    float                       m_ratio;
    int                         m_limit_visitor;
    
public:
    
    void                        *preferentRootParent     = NULL;

    static EnvManager *createWithParent(void *preferentRootParent_);
    virtual bool init();
//    virtual ~EnvManager();
//    virtual void cleanup();
    virtual void update(float delta_);
    
    bool addVehicleLane1(const char *spine_name_);
    bool addVehicleLane2(const char *spine_name_);

};

#endif /* defined(__Cheetah__EnvManager__) */
