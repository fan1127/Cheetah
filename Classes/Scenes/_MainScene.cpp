//
//  MainScene.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//
/*///QQQ not used in iOS
#include "MainScene.h"
USING_NS_CC;

#include "SceneStartupCommand.h"
#include "SceneMapCommand.h"
#include "SceneRaceCommand.h"

CCScene*            currScene   = NULL;

#include "../Services/GameContentService.h"
GameContentService* gameContent = NULL;

#include "../Services/BackendService.h"
BackendService*     gameBackend = NULL;

#include "../Entities/PlayerState.h"
PlayerState*        playerState = NULL;

CCScene* MainScene::scene() {
    
    CCScene* scene = MainScene::create();
    return scene;
    
}

bool MainScene::init() {
    
    if (!CCScene::init()) {
        return false;
    }
    
    currScene = NULL;
    
    sceneSelector = SCENE_STARTUP;
    sceneSelected = -1;
    
    scheduleUpdate();
    
    return true;
    
}

void MainScene::update(float delta) {
    
    if (sceneSelector != sceneSelected) {
        
        if (currScene) {
            
            //CCDirector::sharedDirector()->popSceneWithTransition(0.5f);
            currScene->removeAllChildrenWithCleanup(true);
            currScene = NULL;
            
        } 
            
            switch (sceneSelector) {
                default:
                case SCENE_STARTUP:
                    currScene = SceneStartupCommand::create();
                    break;
                case SCENE_MAP:
                    currScene = SceneMapCommand::create();
                    break;
                case SCENE_RACE:
                    currScene = SceneRaceCommand::create();
                    break;
            }
            
            printf("pushing scene %d!\n", sceneSelector);
            CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, currScene));
            
            sceneSelected = sceneSelector;

        
                
    }
    
}
*/ ///QQQ
