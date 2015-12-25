//
//  CTutDragFoodToPet.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutDragFoodToPet__
#define __Cheetah__CTutDragFoodToPet__

#include "CTut.h"
#include "../Building.h"
#include "../EntityAction.h"

#define CTutDragFoodToPetFailed "failed"
#define CTutDragFoodToPetFeeded "feeded"
class CTutDragFoodToPet: public CTut {
    
public:
    
    /** Required instances, in order to perform the tutorial, these instances are required */
    EntityAction*       entityAction;
    
    cocos2d::CCSprite*  pointer;
    cocos2d::extension::CCSkeletonAnimation *effect;
    cocos2d::CCLabelBMFont* text;
    std::string         message;
    
    CTutDragFoodToPet() {
        
        CTut();
        id = CT_ID_POINT_BUILDING;
        
        entityAction = NULL;
        
        pointer = NULL;
        effect = NULL;
        text = NULL;
        message = "";
        
    }
    
    CTutDragFoodToPet(EntityAction* entityAction_) { //Building* building_,
        
        CTut();
        id = CT_ID_POINT_BUILDING;
        
        entityAction = entityAction_;
        
        pointer = NULL;
        effect = NULL;
        text = NULL;
        message = "";
        
    }
    
    virtual ~CTutDragFoodToPet();
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
    void clearGraphics();
    
};

#endif /* defined(__Cheetah__CTutDragFoodToPet__) */