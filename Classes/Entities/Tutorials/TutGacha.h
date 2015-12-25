//
//  TutGacha.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutGacha__
#define __Cheetah__TutGacha__

#include "CTutManager.h"

#include "../../UIs/Panels/HUDPanel.h"
#include "../../UIs/ShopPanel.h"
#include "../../Entities/IsoMapBuilding.h"

#include "TutSecondRacePart1.h"

class TutGacha : public CTutUnit {
    
    int m_point_at_gacha_btn_index;
    int m_gacha_spin_btn_tut_index;
    
public:
    
    /** required instance pointer */
    HUDPanel* hudPanel;
    ShopPanel* shopPanel;
    IsoMapBuilding* isoMapBuilding;
    
    static std::string key();
    static bool check();
    static TutGacha* create(HUDPanel* hudPanel, ShopPanel* shopPanel, IsoMapBuilding* isoMapBuilding);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    virtual void update(float delta);
    
    static void onTapTheBG(void* sender, std::string result);
    static void onGachaButtonTapped(void* sender, std::string result);
    static void onGachaSpinned(void* sender, std::string result);
    
};

#endif /* defined(__Cheetah__TutGacha__) */
