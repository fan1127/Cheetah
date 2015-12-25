//
//  TutSecondRacePart4.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutSecondRacePart4__
#define __Cheetah__TutSecondRacePart4__

#include "CTutManager.h"
#include "../../Entities/IsoMapRacing.h"
#include "../../UIs/RacePanel.h"

#include "../../Settings.h"
#include "TutSecondRacePart1.h"

class TutSecondRacePart4: public virtual CTutUnit {
    
public:
    
    /** required instance */
    IsoMapRacing*   isoMapRacing;
    RacePanel*      racePanel;
    
    static std::string key();
    static bool check();
    static TutSecondRacePart4* create(IsoMapRacing* isoMapRacing_, RacePanel* racePanel_);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    //    static void onContinue(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutSecondRacePart4__) */
