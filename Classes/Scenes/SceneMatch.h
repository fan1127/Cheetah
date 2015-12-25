//
//  SceneMatch.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/9/56 BE.
//
//

#ifndef __Cheetah__SceneMatch__
#define __Cheetah__SceneMatch__

#include "cocos2d.h"

#include "CScene.h"
#include "../Entities/IsoMapMatch.h"
#include "../UIs/Panels/MatchPanel.h"

#include "../Extensions/Modal/CCModalController.h"
#include "../Extensions/CCNikScrollView.h"
#include "../Extensions/CCPopOverDialog.h"

#include "../Entities/Tutorials/TutSecondRacePart3.h"
#include "../Entities/Tutorials/TutSwitchPet.h"
#include "../Entities/Tutorials/TutChooseHelper.h"

#define MATCH_MINIMUM_SCALE_FOR_ZOOMING (scrollView->getMinimumScale()*1.2f)
#define MATCH_MAXIMUM_SCALE_FOR_ZOOMING 1.3f
#define MATCH_ZOOM_DURATION (1.25f)//(1.0f)
#define MATCH_INSTANT_ZOOM_DURATION (0.0f) //less than 0.001
#define MATCH_WAIT_DURATION_BEFORE_ZOOMING_IN (0.25f)

class SceneMatch: public CScene {
    
    bool                m_didFireOut;
    bool                m_waitForResponse;
    
public:
    
    MatchPanel          *matchPanel;
    CTutManager         *tutMgr;
    
    virtual bool init();
    CREATE_FUNC(SceneMatch);
    ~SceneMatch();
    
    void fadeOutButWaitForOpponent(int sceneIndex);
    bool fadeOutThisSceneAndChangeTo(int sceneIndex);
    
    void zoomIn();
    void popupOpponent();
    
};

#endif /* defined(__Cheetah__SceneMatch__) */
