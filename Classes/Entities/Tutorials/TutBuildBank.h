//
//  TutBuildBank.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutBuildBank__
#define __Cheetah__TutBuildBank__

#include "CTutManager.h"
#include "../../Entities/WorldMap/WorldMap.h"
#include "../../UIs/Panels/HUDPanel.h"

class TutBuildBank : public virtual CTutUnit {
    
public:
    
    /** required instance */
    WorldMap*       worldMap;
    HUDPanel*       hudPanel;
    bool* isIntro;
    static std::string key();
    static std::string keyIntro();
    static bool check();
    static bool checkIntro();
    static TutBuildBank* create();
    static TutBuildBank* createIntro();
    
    virtual bool init(bool isIntro =false);
    virtual void finish();
    
    static void onTapTheBG(void* sender_, std::string result_);
    
};

#endif /* defined(__Cheetah__TutBuildBank__) */
