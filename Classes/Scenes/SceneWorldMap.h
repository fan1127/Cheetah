//
//  SceneMap.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 11/27/2556 BE.
//
//

#ifndef Cheetah_SceneWorldMap_h
#define Cheetah_SceneWorldMap_h

#include "cocos2d.h"

#include "../Extensions/Modal/CCModalController.h"
#include "../Scenes/CScene.h"

#include "../Entities/WorldMap/WorldMap.h"
#include "../UIs/Panels/HUDPanel.h"

#include "../Entities/Tutorials/TutSecondRacePart2.h"
#include "../Entities/Tutorials/TutBuildGarage.h"

class SceneWorldMap: public CScene {
    
public:
    
    cocos2d::CCScene        *scene();
    WorldMap                *worldMap;
    CTutManager             *tutMgr;
    
    std::string             oppo_uuid;
   
    virtual bool init(); 
    CREATE_FUNC(SceneWorldMap);
    ~SceneWorldMap();
    
    void addMoreTutorial();
    
    void fadeOutButWaitForCurrentTime(int sceneIndex);
    void fadeOutButWaitForOpponent(int sceneIndex, std::string oppo_uuid);
    void fadeOutThisSceneAndChangeTo(int sceneIndex);
    
    static void fire_currenttime_get(void* sender);
    static void onCurrentTimeReceived(void* sender, std::string respn_str);
    
    static void fire_opponent_get(void* sender);
    static void onOpponentRetrieved(void* sender, std::string respn_str);
    
#pragma mark - HUD Button Callback Functions
    
    void onHomeButtonPressed(cocos2d::CCObject *object);
    
};


#endif
