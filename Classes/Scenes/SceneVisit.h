//
//  SceneVisit.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 5/9/2557 BE.
//
//

#ifndef __Cheetah__SceneVisit__
#define __Cheetah__SceneVisit__

#include "CScene.h"
#include "../UIs/Panels/HUDPanel.h"
#include "../Extensions/CCPopOverDialog.h"

class SceneVisit: public CScene {
    
    bool                m_didFireOut = false;
    bool                m_waitForResponse = false;
    
    std::string         m_visit_uuid;
    
public:

    virtual bool init();
    CREATE_FUNC(SceneVisit);
    ~SceneVisit();

    void fadeOutButWaitForOpponent(int sceneIndex, std::string oppo_uuid);
    void fadeOutButWaitForCurrentTime(int sceneIndex);
    
    void zoomIn();
    void popupOpponent();
    
    static void fire_currenttime_get(void* sender);
    static void onCurrentTimeReceived(void* sender, std::string json_str);
    
    static void fire_opponent_get(void* sender);
    static void onOpponentRetrieved(void* sender, std::string json_str);
    
#pragma mark - HUD Button Callback Functions
    void onHomeButtonPressed(cocos2d::CCObject *object);
    
};

#endif /* defined(__Cheetah__SceneVisit__) */
