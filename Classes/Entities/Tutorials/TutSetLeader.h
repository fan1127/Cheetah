//
//  TutSetLeader.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutSetLeader__
#define __Cheetah__TutSetLeader__

#include "CTutManager.h"

#include "../../Extensions/CCNikScrollView.h"
#include "../../Entities/IsoMapBuilding.h"
#include "../../Extensions/Modal/CCModalController.h"

class TutSetLeader : public CTutUnit {
    
    int m_focus_tut_index;
    int m_pointatbuilding_tut_index;
    int m_menu_btn_tut_index;
    int m_speech_in_dialog_tut_index;
    int m_setleader_btn_tut_index;
    
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
    static TutSetLeader* create(CCNikScrollView* scrollView_, HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_, CCModalController* modalController_);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    virtual void update(float delta_);
    
    virtual bool trig();
    
    static void onTapTheBG(void* sender_, std::string result_);
    static void onBuildingFocuzed(void* sender_, std::string result_);
    static void onBuildingTapped(void* sender_, std::string result_);
    static void onMenuButtonTapped(void* sender_, std::string result_);
    static void onSetLeaderButtonTapped(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutSetLeader__) */
