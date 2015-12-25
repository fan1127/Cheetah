//
//  SceneRace.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/13/56 BE.
//
//

#ifndef __Cheetah__SceneRace__
#define __Cheetah__SceneRace__

#include "cocos2d.h"

#include "../Scenes/CScene.h"
#include "../Extensions/CCNikScrollView.h"

#include "../Entities/IsoMapRacing.h"

#include "../Entities/Tutorials/TutFirstRace.h"
#include "../Entities/Tutorials/TutSecondRacePart4.h"
#include "../Entities/Tutorials/TutRaceResult.h"

class SceneRace: public CScene {
  
protected:

public:
    
    RacePanel*          racePanel;
    RaceResultPanel*    raceResultPanel;
    CTutManager*        tutMgr;

    virtual bool init();
    CREATE_FUNC(SceneRace);
    
    void zoom();
    void fadeOutThisSceneAndChangeTo(int sceneIndex) ;
    
};

#endif /* defined(__Cheetah__SceneRace__) */
