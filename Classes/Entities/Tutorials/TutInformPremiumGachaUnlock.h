//
//  TutInformPremiumGachaUnlock.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutInformPremiumGachaUnlock__
#define __Cheetah__TutInformPremiumGachaUnlock__

#include "CTutManager.h"
#include "../../UIs/Panels/HUDPanel.h"
#include "../../UIs/ShopPanel.h"
#include "../../Entities/IsoMapBuilding.h"

class TutInformPremiumGachaUnlock: public virtual CTutUnit {
    
    int m_point_at_gacha_btn_index;
    
public:
    
    /** required instance */
    HUDPanel* hudPanel;
    ShopPanel* shopPanel;
    IsoMapBuilding* isoMapBuilding;
    
    static std::string key();
    static bool check();
    static TutInformPremiumGachaUnlock* create(HUDPanel* hudPanel,
                                               ShopPanel* shopPanel,
                                               IsoMapBuilding* isoMapBuilding);
    
    virtual bool init();
    virtual bool trig();
    virtual void start();
    virtual void finish();
    virtual void update(float delta);
    
    static void onTapTheBG(void* sender, std::string result);
    static void onGachaButtonTapped(void* sender, std::string result);
    
};

#endif /* defined(__Cheetah__TutInformPremiumGachaUnlock__) */
