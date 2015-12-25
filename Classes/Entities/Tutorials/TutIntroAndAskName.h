//
//  TutIntroAndAskName.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutIntroAndAskName__
#define __Cheetah__TutIntroAndAskName__

#include "CTutManager.h"

#include "../../Entities/IsoMapBuilding.h"

class TutIntroAndAskName: public CTutUnit {
    
public:
    
    IsoMapBuilding* isoMapBuilding;
    HUDPanel* hudPanel;
    
    static std::string key();
    static bool check();
    static TutIntroAndAskName* create(IsoMapBuilding* isoMapBuilding_, HUDPanel* hudPanel_);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    static void onNameFilled(void* sender_, std::string result_);
    
    static void onUserNameChanged(void* sender, std::string json_str);
    
};

#endif /* defined(__Cheetah__TutIntroAndAskName__) */
