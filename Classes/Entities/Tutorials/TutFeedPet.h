//
//  TutFeedPet.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutFeedPet__
#define __Cheetah__TutFeedPet__

#include "CTutManager.h"

#include "../../Extensions/CCNikScrollView.h"
#include "../../UIs/Panels/HUDPanel.h"
#include "../../UIs/ShopPanel.h"
#include "../IsoMapBuilding.h"

class TutFeedPet: public virtual CTutUnit {
    
public:
    
    /** required instance */
    CCNikScrollView*    scrollView;
    HUDPanel*           hudPanel;
    ShopPanel*          shopPanel;
    IsoMapBuilding*     isoMapBuilding;
    
    static std::string key();
    static bool check();
    static TutFeedPet* create(CCNikScrollView* scrollView_,
                              HUDPanel* hudPanel_,
                              ShopPanel* shopPanel_,
                              IsoMapBuilding* isoMapBuilding_);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    static void onBuildingDidFocuzed(void* sender_, std::string result_);
    static void onBuildingDidTouched(void* sender_, std::string result_);
    static void onPetFeeded(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutFeedPet__) */
