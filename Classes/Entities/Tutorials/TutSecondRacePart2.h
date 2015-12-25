//
//  TutSecondRacePart2.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutSecondRacePart2__
#define __Cheetah__TutSecondRacePart2__

#include "CTutManager.h"
#include "../../Entities/WorldMap/WorldMap.h"
#include "../../UIs/Panels/HUDPanel.h"

class TutSecondRacePart2: public virtual CTutUnit {
    
    int m_visit_button_tut_index;
    
public:
    
    /** required instance */
    WorldMap*       worldMap;
    HUDPanel*       hudPanel;
    
    static std::string key();
    static bool check();
    static TutSecondRacePart2* create(WorldMap* worldMap_, HUDPanel* worldPanel_);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    
    virtual void update(float delta_);
    
    static void onTapTheBG(void* sender_, std::string result_);
    static void onVillageTapped(void* sender_, std::string result_);
    static void onButtonTapped(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutSecondRacePart2__) */
