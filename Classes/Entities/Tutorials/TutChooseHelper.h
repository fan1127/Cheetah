//
//  TutChooseHelper.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutChooseHelper__
#define __Cheetah__TutChooseHelper__

#include "CTutManager.h"
#include "../../UIs/Panels/MatchPanel.h"

class TutChooseHelper : public virtual CTutUnit {
    
    bool m_didTryToLaunch;
    
    
public:
    
    /** required instance */
    MatchPanel*     matchPanel;
    
    static std::string key();
    static bool check();
    static TutChooseHelper* create(MatchPanel* matchPanel_);
    
    virtual bool init();
    virtual bool trig();
    virtual void start();
    virtual void finish();
    
    static void onWillLaunchChooseHelper(void* sender);
    
    static void onTapTheBG(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutChooseHelper__) */
