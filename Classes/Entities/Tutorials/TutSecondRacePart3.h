//
//  TutSecondRacePart3.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutSecondRacePart3__
#define __Cheetah__TutSecondRacePart3__

#include "CTutManager.h"
#include "../../Entities/IsoMapMatch.h"
#include "../../UIs/Panels/MatchPanel.h"

#include "TutSecondRacePart1.h"

class TutSecondRacePart3: public virtual CTutUnit {
    
public:
    
    /** required instance */
    IsoMapMatch*    isoMapMatch;
    MatchPanel*     matchPanel;
    
    static std::string key();
    static bool check();
    static TutSecondRacePart3* create(IsoMapMatch* isoMapMatch_, MatchPanel* matchPanel_);
    
    virtual bool init();
    virtual void start();
    virtual void finish();
    
    //    static void onTapTheBG(void* sender_, std::string result_);
    static void onButtonTapped(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutSecondRacePart3__) */
