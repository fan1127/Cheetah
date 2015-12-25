//
//  CTutPointAtVillage.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutPointAtVillage__
#define __Cheetah__CTutPointAtVillage__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "CTut.h"
#include "../../Entities/WorldMap/WorldMap.h"

class CTutPointAtVillage: public CTut {
    
public:
    
    /** Required instances, in order to perform the tutorial, these instances are required */
    WorldMap*           worldMap;
    cocos2d::CCSprite*  village;
    
    cocos2d::CCSprite*  pointer;
    cocos2d::extension::CCSkeletonAnimation *effect;
    cocos2d::CCLabelBMFont* text;
    std::string         message;
    
    CTutPointAtVillage(WorldMap* worldMap_, cocos2d::CCSprite* village_, void* sender_, void (*callback_)(void* sender, std::string result_)) {
        
        CTut();
        id = CT_ID_POINT_VILLAGE;
        
        worldMap = worldMap_;
        village = village_;
        
        pointer = NULL;
        effect = NULL;
        text = NULL;
        message = "";
        
        sender = sender_;
        callback = callback_;
        
    }
    
    virtual ~CTutPointAtVillage();
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
    void clearGraphics();
    
};

#endif /* defined(__Cheetah__CTutPointAtVillage__) */
