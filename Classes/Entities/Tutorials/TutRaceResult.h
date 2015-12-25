//
//  TutRaceResult.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutRaceResult__
#define __Cheetah__TutRaceResult__

#include "CTutManager.h"

#include "../../Entities/IsoMapRacing.h"

class TutRaceResult: public virtual CTutUnit {
    
public:
    
    /** required instance */
    IsoMapRacing*   isoMapRacing;
    
    static std::string key(int k);
    static bool check();
    static TutRaceResult* create(IsoMapRacing* isoMapRacing_);
    
    virtual bool init();
    virtual bool trig();
    virtual void start();
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutRaceResult__) */
