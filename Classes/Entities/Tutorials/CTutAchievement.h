//
//  CTutAchievement.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutAchievement__
#define __Cheetah__CTutAchievement__

#include <stdio.h>
#include "CTutUnit.h"

#define AVAILABLE_VILLAGES_AFTER_TUTORIAL_ENDS 2

class TutAchievement : public CTutUnit {
    
    int m_first_dialog_tut_index;
    int m_point_at_achievement_button_tut_index;
    int m_last_dialog_tut_index;
    
    bool waitingForAchievementWindowToClose;
    
public:
    
    HUDPanel* hudPanel;
    ShopPanel* shopPanel;
    IsoMapBuilding* isoMapBuilding;
    CCModalController* modalController;
    
    static std::string key();
    static bool check();
    static TutAchievement* create(HUDPanel* hudPanel_,
                                  ShopPanel* shopPanel_,
                                  IsoMapBuilding* isoMapBuilding_,
                                  CCModalController* modalController_);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    virtual void update(float delta_);
    
    static void tapBGAndContinue(void* sender_, std::string result_);
    static void tapBGAndEndTutorial(void* sender_, std::string result_);
    static void onTapAchievementButton(void* sender_, std::string result_);
};

#endif /* defined(__Cheetah__CTutAchievement__) */
