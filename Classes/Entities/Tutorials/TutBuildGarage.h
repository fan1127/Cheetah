//
//  TutBuildGarage.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutBuildGarage__
#define __Cheetah__TutBuildGarage__

#include "CTutManager.h"
#include "../../Entities/WorldMap/WorldMap.h"
#include "../../UIs/Panels/HUDPanel.h"

class TutBuildGarage : public virtual CTutUnit {
    
public:
    
    /** required instance */
    WorldMap*       worldMap;
    HUDPanel*       hudPanel;
    
    static std::string key();
    static bool check();
    static TutBuildGarage* create(WorldMap* worldMap_, HUDPanel* hudPanel_);
    
    virtual bool init();
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutBuildGarage__) */
