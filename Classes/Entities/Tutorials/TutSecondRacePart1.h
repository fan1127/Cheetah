//
//  TutSecondRacePart1.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutSecondRacePart1__
#define __Cheetah__TutSecondRacePart1__

#include "CTutManager.h"
#include "../../UIs/Panels/HUDPanel.h"
#include "../../UIs/ShopPanel.h"
#include "../../Entities/IsoMapBuilding.h"

class TutSecondRacePart1: public virtual CTutUnit {
    
public:
    
    /** required instance */
    HUDPanel* hudPanel;
    ShopPanel* shopPanel;
    IsoMapBuilding* isoMapBuilding;
    
    static std::string key();
    static bool check();
    static TutSecondRacePart1* create(HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    static void onButtonTapped(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutSecondRacePart1__) */