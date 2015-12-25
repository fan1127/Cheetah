//
//  TutFusePet.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutFusePet__
#define __Cheetah__TutFusePet__

#include "CTutManager.h"

#include "../../Extensions/CCNikScrollView.h"
#include "../../UIs/Panels/HUDPanel.h"
#include "../../UIs/ShopPanel.h"
#include "../../Entities/IsoMapBuilding.h"
#include "../../Extensions/Modal/CCModalController.h"

class TutFusePetValidate : public CTut {
    
public:
    
    TutFusePetValidate(void* sender,
                       void (*callback)(void*, std::string)) {
        this->sender = sender;
        this->callback = callback;
    }
    
    virtual void update(float delta);
    
};

class TutFusePet : public CTutUnit {
    
    int m_menu_btn_tut_index;
    int m_trade_btn_tut_index;
    bool m_didSeeAnimalTradeDialog;
    
    cocos2d::CCLabelBMFont *text;
    
public:
    
    /** required instance pointer */
    CCNikScrollView* scrollView;
    HUDPanel* hudPanel;
    ShopPanel* shopPanel;
    IsoMapBuilding* isoMapBuilding;
    CCModalController* modalController;
    
    static std::string key();
    static bool check();
    static TutFusePet* create(CCNikScrollView* scrollView,
                              HUDPanel* hudPanel,
                              ShopPanel* shopPanel,
                              IsoMapBuilding* isoMapBuilding,
                              CCModalController* modalController); //,
    
    virtual bool init();
    virtual bool trig();
    virtual void start();
    virtual void finish();
    virtual void update(float delta);
    
    static void onTapTheBG(void* sender, std::string result);
    static void onBuildingFocuzed(void* sender, std::string result);
    static void onBuildingTapped(void* sender, std::string result);
    static void onMenuButtonTapped(void* sender, std::string result);
    static void onTradeButtonTapped(void* sender, std::string result);
    static void onAllModalClosed(void* sender, std::string result);
    
};

#endif /* defined(__Cheetah__TutFusePet__) */
