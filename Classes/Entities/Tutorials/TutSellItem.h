//
//  TutSellItem.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutSellItem__
#define __Cheetah__TutSellItem__

#include "CTutManager.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../UIs/Panels/HUDPanel.h"
#include "../../UIs/ShopPanel.h"
#include "../../Entities/IsoMapBuilding.h"

class TutSellItem: public virtual CTutUnit {
    
public:
    
    /** required instance */
    CCNikScrollView* scrollView;
    HUDPanel* hudPanel;
    ShopPanel* shopPanel;
    IsoMapBuilding* isoMapBuilding;
    
    static std::string key();
    static bool check();
    static TutSellItem* create(CCNikScrollView* scrollView_, HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    static void onBuildingFocuzed(void *sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutSellItem__) */
