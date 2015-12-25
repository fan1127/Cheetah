//
//  CTutDragBuildingToBuild.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutDragBuildingToBuild__
#define __Cheetah__CTutDragBuildingToBuild__

#include "CTut.h"
#include "../../UIs/ShopPanel.h"
#include "../../UIs/ShopItemBuilding.h"

#define CTutDragBuildingToBuildFailed "failed"

class CTutDragBuildingToBuild: public CTut {
    
public:
    
    /** Required instances, in order to perform the tutorial, these instances are required */
    ShopPanel*          shopPanel;
    ShopItemBuilding*   buttonItem;
    
    cocos2d::CCSprite*  pointer;
    cocos2d::extension::CCSkeletonAnimation *effect;
    cocos2d::CCLabelBMFont* text;
    std::string         message;
    
    CTutDragBuildingToBuild() {
        
        CTut();
        id = CT_ID_DRAG_BUILDING;
        
        shopPanel = NULL;
        buttonItem = NULL;
        
        pointer = NULL;
        effect = NULL;
        text = NULL;
        message = "";
        
    }
    
    CTutDragBuildingToBuild(ShopPanel* shopPanel_, ShopItemBuilding* buttonItem_, std::string message_, void* sender_, void (*callback_)(void*, std::string)) {
        
        CTut();
        id = CT_ID_POINT_BUILDING;
        
        shopPanel = shopPanel_;
        buttonItem = buttonItem_;
        
        pointer = NULL;
        effect = NULL;
        text = NULL;
        message = message_;
        
        sender = sender_;
        callback = callback_;
        
    }
    
    virtual ~CTutDragBuildingToBuild();
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
    void clearGraphics();
    
    static void onShopPanelClosed(void* sender_);
    static void onBuildingAdded(void* sender_, void* item_);
    
};


#endif /* defined(__Cheetah__CTutDragBuildingToBuild__) */
