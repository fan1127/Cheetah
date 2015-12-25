//
//  TutSwitchPet.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutSwitchPet__
#define __Cheetah__TutSwitchPet__

#include "CTutManager.h"
#include "../../UIs/Panels/MatchPanel.h"

class TutSwitchPet : public virtual CTutUnit {
    
    int m_switch_pet_btn_tut_index;
    
public:
    
    /** required instance */
    MatchPanel*     matchPanel;
    
    static std::string key();
    static bool check();
    static TutSwitchPet* create(MatchPanel* matchPanel_);
    
    virtual bool init();
    virtual void update(float delta_);
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    static void onPetSwitched(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutSwitchPet__) */