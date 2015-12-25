//
//  SceneVisit.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 5/9/2557 BE.
//
//

#include "SceneVisit.h"
USING_NS_CC;
using namespace std;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;
extern CCScene* g_currentScene;

#include "../Entities/PlayerState.h"

#include "../Services/BackendService.h"
#include "../Services/APIs/APICurrentTimeGet.h"
#include "../Services/APIs/APIOpponentGet.h"

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Entities/IsoMapMatch.h"

#include "../Extensions/WeatherEffects/WeatherEnvironment.h"

extern float sizeMultiplier;

bool SceneVisit::init() {
    
    if (!CCScene::init()) {
        return false;
    }
    
    m_visit_uuid = "";
    
    IsoMapMatch* _isoMapMatch = IsoMapMatch::create();
    _isoMapMatch->setAnchorPoint(ccp(0.5f, 0.5f));
    _isoMapMatch->setPosition(_isoMapMatch->getContentSize()*-1/4.f);
    
    mapToScreenUI = MapToScreenUI::create();
    addChild(mapToScreenUI, 101);
    _isoMapMatch->mapToScreenUI = mapToScreenUI;
    
    modalController = CCModalController::create();
    modalController->setSender(this);
    modalController->retain();
    
    scrollView = CCNikScrollView::create();
    scrollView->setZoomingEnabled(true);
    scrollView->addChild(_isoMapMatch);
    scrollView->setContentView(_isoMapMatch->getContentSize()/2.f);
    scrollView->setMaximumScale(1.5f);
    scrollView->setMinimumScale(scrollView->getContentSize().width/(_isoMapMatch->getContentSize().width/2.f));
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, scrollView->getMinimumScale(), 0.0f);
    this->addChild(scrollView);
    scrollView->setDelegate(_isoMapMatch);
    
    isoMap = _isoMapMatch;
    
#ifdef ENABLE_SNOW
    addChild(WeatherEnvironment::create(scrollView, isoMap, PARTICLE_TYPE_SNOW, SNOW_DEFAULT_AMOUNT), 2);
#endif
    
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    
    ///////////////////////////////////
    // UI Panels
    hudPanel = HUDPanel::create();
    hudPanel->addHomeUserInfo();
    hudPanel->addMainMenuButton(HUDPanelMenuHome, NULL, POS_BOTTOM_ARRANGE_FROM_RIGHT);
    hudPanel->addMainMenuButton(HUDPanelMenuFriends, NULL, POS_FIXED_DEFAULT);
    HUDMenuButton *friendsMenu = hudPanel->menuButtons[HUDPanelMenuFriends];
    friendsMenu->setPosition(ccp(friendsMenu->getPosition().x,
                                 screenSize.height*0.5f
                                 -(220.0f*sizeMultiplier)
                                 ));
    hudPanel->buildMainMenuButtons();
    addChild(hudPanel, 9);

    //override button callback
    hudPanel->menuButtons[HUDPanelMenuHome]->setTarget(this, menu_selector(SceneVisit::onHomeButtonPressed), CCMenuItemStateTouchDown);
    
    g_gameAudio->playEffectFadeOutAndFadeIn(AUDIO_AMBIENCE_FARM, true, 0.25f);
    g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_VISIT, true, 0.25f, 0.25f, 0.25f);
    
    scheduleUpdate();
    
    scheduleOnce(schedule_selector(SceneVisit::zoomIn), 0.75f);
    
    return true;
    
}

SceneVisit::~SceneVisit() {
    
    if (modalController) {
        modalController->release();
        modalController = NULL;
    }
    
}

void SceneVisit::zoomIn() {
    
    IsoMapMatch* _isoMapMatch = (IsoMapMatch*)isoMap;
    if (!_isoMapMatch) return;
    
    if (PlayerState::getOpponent()->HostHabitat) {
        CCPoint _map_pos = PlayerState::getOpponent()->HostHabitat->getPosition();
        CCLog_("[[%f,%f]]", _map_pos.x, _map_pos.y);
        scrollView->zoomOnPosition(_map_pos - _isoMapMatch->getContentSize()/4.f, 1.3f, 1.0f);
        return;
    }
    
    if (Building *_building = PlayerState::getPlayer()->getBuilding(kBuildingHQ, 0)) {
        CCPoint _map_pos = _building->getPosition();
        CCLog_("[[%f,%f]]", _map_pos.x, _map_pos.y);
        scrollView->zoomOnPosition(_map_pos - _isoMapMatch->getContentSize()/4.f, 1.3f, 1.0f);
        return;
    }
    
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, 1.3f, 1.0f);
    
}

void SceneVisit::fadeOutButWaitForOpponent(int sceneIndex, string oppo_uuid) {
    
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = fire_opponent_get;
    }
    
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, scrollView->getMinimumScale(), 0.6f);
    m_visit_uuid = oppo_uuid;
    
    g_sceneManager->sceneSelector = (SceneList)sceneIndex;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    
}

void SceneVisit::fadeOutButWaitForCurrentTime(int sceneIndex) {
    
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = fire_currenttime_get;
    }
    
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, scrollView->getMinimumScale(), 0.6f);
    g_sceneManager->sceneSelector = (SceneList)sceneIndex;
    
    if (g_sceneManager->transition) {
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    
}

void SceneVisit::fire_currenttime_get(void *sender) {
    
    BackendService::getInstance()->send(APICurrentTimeGet::create(sender,
                                                                  SceneVisit::onCurrentTimeReceived));
    
}

void SceneVisit::onCurrentTimeReceived(void *sender, std::string json_str) {
    
    //JsonAllocator allocator;
    Json* root = Json_create(json_str.c_str());
    PlayerState::getInstance()->parseCurrentTime(root);
    Json_dispose(root);
    
    g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
    g_sceneManager->switchScene(g_sceneManager);
    
}

void SceneVisit::fire_opponent_get(void *sender) {
    
    SceneVisit* _this = (SceneVisit*)g_currentScene;
    if (!_this) return;
    
    BackendService::getInstance()->cancelAllRequestsInQueue();
    BackendService::getInstance()->send(APIOpponentGet::create(_this->m_visit_uuid,
                                                               _this,
                                                               SceneVisit::onOpponentRetrieved));
    
}

void SceneVisit::onOpponentRetrieved(void *sender, std::string json_str) {
    
    SceneVisit* _this = (SceneVisit*)g_currentScene;
    if (!_this) return;
    
    if (!PlayerState::getOpponent()->parseOppoData(json_str.c_str())) return;
    
    g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
    g_sceneManager->switchScene(g_sceneManager);
    
}


#pragma mark - HUD Button Callback Functions

void SceneVisit::onHomeButtonPressed(cocos2d::CCObject *object) {
    
    CCMenuItem *_menuItem = (CCMenuItem*)object;
    _menuItem->setEnabled(false);

    //play next music
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    g_gameAudio->clearEffectFadeOut(3.0f);
    
    //clear g_opponentState uuid
    PlayerState::getOpponent()->player.uuid = "";
    fadeOutButWaitForCurrentTime(SCENE_VILLE);
    
}

