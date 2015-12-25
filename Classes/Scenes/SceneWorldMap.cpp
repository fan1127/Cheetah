//
//  SceneMap.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 11/27/2556 BE.
//
//

#include "SceneWorldMap.h"
USING_NS_CC;
using namespace std;

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;
extern CCScene *g_currentScene;

#include "../Entities/PlayerState.h"
extern PlayerState *g_playerState;
extern PlayerState *g_opponentState;

#include "../Services/BackendService.h"
#include "../Services/APIs/APICurrentTimeGet.h"
#include "../Services/APIs/APIOpponentGet.h"

#include "../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

CCScene* SceneWorldMap::scene() {
    
    CCScene *scene = SceneWorldMap::create();
    scene->autorelease();
    return scene;
    
}

bool SceneWorldMap::init() {
    
    if (!CCScene::init()) {
        return false;
    }
    
    modalController = CCModalController::create();
    modalController->setSender(this);
    modalController->retain();
    
    worldMap = WorldMap::create();
    addChild(worldMap);

    hudPanel = HUDPanel::create();
    hudPanel->addExpBar();
    hudPanel->addStreakInfo();
    hudPanel->addResourceBar(HUDPanelBarGems, NULL);
    hudPanel->addResourceBar(HUDPanelBarCoins, hudPanel->resourceBars[HUDPanelBarGems]);
    hudPanel->addMainMenuButton(HUDPanelMenuHome, NULL, POS_BOTTOM_ARRANGE_FROM_RIGHT);
    hudPanel->buildMainMenuButtons();
    worldMap->hudPanel = hudPanel;
    addChild(hudPanel);
    hudPanel->resourceBars[HUDPanelBarGems]->setMaxLevelLabelShowed(false);
    //override button callback
    hudPanel->menuButtons[HUDPanelMenuHome]->setTarget(this, menu_selector(SceneWorldMap::onHomeButtonPressed), CCMenuItemStateTouchDown);
    
    worldMap->collectableGems = CollectableItems::create(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsGemIconForAnimation)));
    worldMap->collectableGems->onCollected = WorldMap::onGemsIconHitHUD;// worldPanel->gemIconBeating;
    worldMap->collectableGems->setSender(worldMap);
    this->addChild(worldMap->collectableGems, 10);
    
    CCSprite* gemIconSprite = hudPanel->resourceBars[HUDPanelBarGems]->getIconSprite();
    gemIconSprite->convertToWorldSpaceAR(CCPointZero);
    CCPoint pos = gemIconSprite->convertToWorldSpaceAR(CCPointZero);
    float scale = hudPanel->getScale()*gemIconSprite->getScale();
    worldMap->collectableGems->setDest(pos, scale);
    
    CREATE_CHAR_BUFFER(_buf, 64);
    
    if (g_sceneManager) {
        if (g_sceneManager->pendingMsg) {
            CCObject *obj;
            CCARRAY_FOREACH(g_sceneManager->pendingMsg, obj)
            {
                snprintf(_buf, _buf_size, "%s",((CCString *)obj)->getCString());
                if(obj!=NULL){
                    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO,_buf );
                }
            }
            g_sceneManager->pendingMsg->removeAllObjects();
        }
    }
    
    tutMgr = CTutManager::create();
#ifndef DEBUG_SKIP_SECOND_RACE
    tutMgr->addTut(TutSecondRacePart2::create(worldMap, hudPanel));
#endif
    addChild(tutMgr, 2000);
    addMoreTutorial();
    
    g_gameAudio->playEffectFadeOutAndFadeIn(AUDIO_AMBIENCE_FARM, true, 0.25f);
    
    return true;
    
}

SceneWorldMap::~SceneWorldMap() {
    
    if (modalController != NULL) {
        modalController->release();
        modalController = NULL;
    }
    
}

void SceneWorldMap::addMoreTutorial() {
    
    if (tutMgr) {
        tutMgr->addTut(TutBuildGarage::create(worldMap, hudPanel));
    }
    
}

void SceneWorldMap::fadeOutButWaitForCurrentTime(int sceneIndex_) {
    
    g_gameAudio->stopAllEffect();
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    g_sceneManager->sceneSelector = (SceneList)sceneIndex_;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = fire_currenttime_get;
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    
}

void SceneWorldMap::fadeOutButWaitForOpponent(int sceneIndex_, string oppo_uuid_) {
    
    g_gameAudio->stopAllEffect();
    g_gameAudio->playEffect(AUDIO_CLOUD_CLOSE);
    
    oppo_uuid = oppo_uuid_;
    
    g_sceneManager->sceneSelector = (SceneList)sceneIndex_;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = fire_opponent_get;
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    
}

void SceneWorldMap::fadeOutThisSceneAndChangeTo(int sceneIndex_) {

    g_gameAudio->stopAllEffect();
    
    g_sceneManager->sceneSelector = (SceneList)sceneIndex_;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    
}

void SceneWorldMap::fire_currenttime_get(void *sender) {
    
    BackendService::getInstance()->send(APICurrentTimeGet::create(sender,
                                                                  SceneWorldMap::onCurrentTimeReceived));
    
}

void SceneWorldMap::onCurrentTimeReceived(void *sender_, std::string respn_json) {
    
    //JsonAllocator allocator;
    Json* root = Json_create(respn_json.c_str());
    PlayerState::getInstance()->parseCurrentTime(root);
    Json_dispose(root);

    g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
    g_sceneManager->switchScene(g_sceneManager);

    g_playerState->calAllBuildingCountdown();
    
}

void SceneWorldMap::fire_opponent_get(void *sender_) {

    SceneWorldMap* _this = (SceneWorldMap*)g_currentScene;
    if (!_this) return;
    
    BackendService::getInstance()->cancelAllRequestsInQueue();
    BackendService::getInstance()->send(APIOpponentGet::create(_this->oppo_uuid,
                                                               _this,
                                                               SceneWorldMap::onOpponentRetrieved));
    
}

void SceneWorldMap::onOpponentRetrieved(void *sender, std::string respn_str) {
    
    SceneWorldMap* _this = (SceneWorldMap*)g_currentScene;
    if (!_this) return;
    
    if (!PlayerState::getOpponent()->parseOppoData(respn_str.c_str())) return;
    
    g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
    g_sceneManager->switchScene(g_sceneManager);

}

#pragma mark - HUD Button Callback Functions

void SceneWorldMap::onHomeButtonPressed(cocos2d::CCObject *object_) {
    
    CCMenuItem *_menuItem = (CCMenuItem*)object_;
    _menuItem->setEnabled(false);
    
    //play next music
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_gameAudio->clearEffectFadeOut(3.0f);
    
    //clear g_opponentState uuid
    g_opponentState->player.uuid = "";
    fadeOutButWaitForCurrentTime(SCENE_VILLE);
    
}
