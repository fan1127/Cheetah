//
//  TutBuildFeedSilo.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutBuildFeedSilo__
#define __Cheetah__TutBuildFeedSilo__

#include "CTutManager.h"
#include "../../Entities/WorldMap/WorldMap.h"
#include "../../UIs/Panels/HUDPanel.h"

class TutBuildFeedSilo : public virtual CTutUnit {
    
public:
    
    /** required instance */
    WorldMap*       worldMap;
    HUDPanel*       hudPanel;
    
    bool* isIntro;
    static std::string key();
    static std::string keyIntro();
    static bool check();
    static bool checkIntro();
    static TutBuildFeedSilo* create();
    static TutBuildFeedSilo* createIntro();
    
    virtual bool init(bool isIntro =false);
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutBuildFeedSilo__) */
