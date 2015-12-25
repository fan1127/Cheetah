//
//  TutIntroMerchant.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutIntroMerchant__
#define __Cheetah__TutIntroMerchant__

#include "CTutManager.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../UIs/Panels/HUDPanel.h"
#include "../../UIs/ShopPanel.h"
#include "../../Entities/IsoMapBuilding.h"
#include "../../Entities/Merchant/Merchant.h"

class TutIntoMerchant: public virtual CTutUnit {
    
public:
    
    /** required instance */
    CCNikScrollView* scrollView;
    HUDPanel* hudPanel;
    ShopPanel* shopPanel;
    IsoMapBuilding* isoMapBuilding;
    Merchant* merchant;
    
    static std::string key();
    static bool check();
    static TutIntoMerchant* create(CCNikScrollView* scrollView,
                                   HUDPanel* hudPanel,
                                   ShopPanel* shopPanel,
                                   IsoMapBuilding* isoMapBuilding,
                                   Merchant* merchant);
    
    virtual bool init();
    virtual bool trig();
    virtual void start();
    virtual void finish();
    
    static void onTapTheBG(void* sender, std::string result);
    static void onNodeFocuzed(void *sender, std::string result);
    
};

#endif /* defined(__Cheetah__TutIntroMerchant__) */
