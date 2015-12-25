//
//  TutRetrievePet.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutRetrievePet__
#define __Cheetah__TutRetrievePet__

#include "CTutManager.h"

#include "../../Extensions/CCNikScrollView.h"
#include "../../UIs/Panels/HUDPanel.h"
#include "../../UIs/ShopPanel.h"
#include "../../Entities/IsoMapBuilding.h"
#include "../../Extensions/Modal/CCModalController.h"

class TutRetrievePetValidate : public CTut {
    
public:
    
    TutRetrievePetValidate(void* sender_, void (*callback_)(void* sender, std::string result_)) {
        sender = sender_;
        callback = callback_;
    }
    
    virtual void update(float delta_);
    
};

class TutRetrievePet : public CTutUnit {
    
    int m_menu_btn_tut_index;
    int m_retrieve_btn_tut_index;
    bool m_didSeeAnimalDialog;
    
    cocos2d::CCLabelBMFont *text;
    
public:
    Pet *m_retrievePet;
    
    /** required instance pointer */
    CCNikScrollView* scrollView;
    HUDPanel* hudPanel;
    ShopPanel* shopPanel;
    IsoMapBuilding* isoMapBuilding;
    CCModalController* modalController;
    
    static std::string key();
    static bool check();
    static TutRetrievePet* create(CCNikScrollView* scrollView_, HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_, CCModalController* modalController_); //,
    
    virtual bool init();
    virtual bool trig();
    virtual void start();
    virtual void finish();
    virtual void update(float delta_);
    
    static void onTapTheBG(void* sender_, std::string result_);
    static void onBuildingFocuzed(void* sender_, std::string result_);
    static void onBuildingTapped(void* sender_, std::string result_);
    static void onMenuButtonTapped(void* sender_, std::string result_);
    static void onTradeButtonTapped(void* sender_, std::string result_);
    static void onAllModalClosed(void* sender_, std::string result);
    
};


#endif /* defined(__Cheetah__TutRetrievePet__) */
