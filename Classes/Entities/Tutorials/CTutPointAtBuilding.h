//
//  CTutPointAtBuilding.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutPointAtBuilding__
#define __Cheetah__CTutPointAtBuilding__

#include "CTut.h"
#include "../Building.h"
#include "../EntityAction.h"

#define CTutPointAtBuildingFailed "failed"

class CTutPointAtBuilding: public CTut {
    
    bool                m_effectEnabled;
    
public:
    
    /** Required instances, in order to perform the tutorial, these instances are required */
    Building*           building;
    EntityAction*       entityAction;
    
    cocos2d::CCSprite*  pointer;
    cocos2d::extension::CCSkeletonAnimation *effect;
    cocos2d::CCLabelBMFont* text;
    std::string         message;
    
    CTutPointAtBuilding() {
        
        CTut();
        id = CT_ID_POINT_BUILDING;
        m_effectEnabled = true;
        
        building = NULL;
        entityAction = NULL;
        
        pointer = NULL;
        effect = NULL;
        text = NULL;
        message = "";
        
    }
    
    CTutPointAtBuilding(Building* building_, EntityAction* entityAction_, std::string message_, void* sender_, void (*callback_)(void*,std::string)) {
        
        CTut();
        id = CT_ID_POINT_BUILDING;
        m_effectEnabled = true;
        
        building = building_;
        entityAction = entityAction_;
        
        pointer = NULL;
        effect = NULL;
        text = NULL;
        message = message_;
        
        sender = sender_;
        callback = callback_;
        
    }
    
    virtual ~CTutPointAtBuilding();
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
    void setEffectEnabled(bool enabled_) { m_effectEnabled = enabled_; }
    void clearGraphics();
    
    static void onBuildingWillOpen(void* sender_);
    
};

#endif /* defined(__Cheetah__CTutPointAtBuilding__) */
