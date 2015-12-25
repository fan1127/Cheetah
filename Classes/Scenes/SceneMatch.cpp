//
//  SceneMatch.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/9/56 BE.
//
//

#include "SceneMatch.h"
USING_NS_CC;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;
extern CCScene* g_currentScene;

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;

#include "../Services/BackendService.h"
extern BackendService* g_gameBackend;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Extensions/WeatherEffects/WeatherEnvironment.h"
#include "../Helpers/DataCache.h"
#include "../UIs/BlockingLayer.h"

bool SceneMatch::init() {
  
    if (!CCScene::init()) {
        return false;
    }
    
    m_didFireOut = false;
    m_waitForResponse = false;
    
    IsoMapMatch* _isoMapMatch = IsoMapMatch::create();
    _isoMapMatch->setAnchorPoint(ccp(0.5f, 0.5f));
    _isoMapMatch->setPosition(_isoMapMatch->getContentSize()*-1/4.f);
    
    mapToScreenUI = MapToScreenUI::create();
    addChild(mapToScreenUI, 98);
    _isoMapMatch->mapToScreenUI = mapToScreenUI;
    
    modalController = CCModalController::create();
    modalController->setSender(this);
//    modalController->onWillPopView = willPopView;
//    modalController->onPushView = disableTouch;
//    modalController->onPopView = enableTouch;
//    modalController->onCloseAll = enableTouch;
    modalController->retain();
    
    scrollView = CCNikScrollView::create();
    scrollView->setZoomingEnabled(true);
    scrollView->addChild(_isoMapMatch);
    scrollView->setContentView(_isoMapMatch->getContentSize()/2.f);
    scrollView->setMaximumScale(2.f);
    scrollView->setMinimumScale(scrollView->getContentSize().width/(_isoMapMatch->getContentSize().width/2.f));
    
    if (g_opponentState->HostHabitat) {
        CCPoint _map_pos = g_opponentState->HostHabitat->getPosition();
        scrollView->zoomOnPosition(_map_pos - _isoMapMatch->getContentSize()/4.f, MATCH_MINIMUM_SCALE_FOR_ZOOMING, MATCH_INSTANT_ZOOM_DURATION);
    } else {
        scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, MATCH_MINIMUM_SCALE_FOR_ZOOMING, MATCH_INSTANT_ZOOM_DURATION);
    }
    addChild(scrollView);
    scrollView->setDelegate(_isoMapMatch);
    
    hudPanel = HUDPanel::create();
    hudPanel->addHomeUserInfo();
    hudPanel->addResourceBar(HUDPanelBarGems, NULL);
    hudPanel->addResourceBar(HUDPanelBarFoods, hudPanel->resourceBars[HUDPanelBarGems]);
    hudPanel->addResourceBar(HUDPanelBarCoins, hudPanel->resourceBars[HUDPanelBarFoods]);
    addChild(hudPanel, 9);
    hudPanel->resourceBars[HUDPanelBarGems]->setMaxLevelLabelShowed(false);
    
    matchPanel = MatchPanel::create(hudPanel);
    addChild(matchPanel, 99);
    matchPanel->showOpponentDialog = true;
    matchPanel->isoMap = _isoMapMatch;
    
    tutMgr = CTutManager::create();
#ifndef DEBUG_SKIP_SECOND_RACE
    tutMgr->addTut(TutSecondRacePart3::create(_isoMapMatch, matchPanel));
#endif
    tutMgr->addTut(TutSwitchPet::create(matchPanel));
    tutMgr->addTut(TutChooseHelper::create(matchPanel));
    addChild(tutMgr, 2000);
    
    isoMap = _isoMapMatch;
    
#ifdef ENABLE_SNOW
    addChild(WeatherEnvironment::create(scrollView, isoMap, PARTICLE_TYPE_SNOW, SNOW_DEFAULT_AMOUNT), 2);
#endif
    
    g_gameAudio->playEffectFadeOutAndFadeIn(AUDIO_AMBIENCE_FARM, true, 0.25f);
//    g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_MATCH, true, 0.25f, 0.25f, 0.25);
    
    scheduleOnce(schedule_selector(SceneMatch::zoomIn), MATCH_WAIT_DURATION_BEFORE_ZOOMING_IN);

    DataCache::Instance().invalidateData(DataCache::Helper);
    DataCache::Instance().fetchData(DataCache::Helper);

    return true;
}

SceneMatch::~SceneMatch() {
    
    if (modalController) {
        modalController->release();
        modalController = NULL;
    }
    
}

void SceneMatch::zoomIn() {
    
    IsoMapMatch* _isoMapMatch = (IsoMapMatch*)isoMap;
    if(matchPanel) matchPanel->showOpponentDialog = false;
    
    if (g_opponentState->HostHabitat) {
        CCPoint _map_pos = g_opponentState->HostHabitat->getPosition();
        CCLog_("[[%f,%f]]", _map_pos.x, _map_pos.y);
        scrollView->zoomOnPosition(_map_pos - _isoMapMatch->getContentSize()/4.f, MATCH_MAXIMUM_SCALE_FOR_ZOOMING, MATCH_ZOOM_DURATION);
        
        return;
    }
    
    if (Building *_building = g_playerState->getBuilding(kBuildingHQ, 0)) {
        CCPoint _map_pos = _building->getPosition();
        CCLog_("[[%f,%f]]", _map_pos.x, _map_pos.y);
        scrollView->zoomOnPosition(_map_pos - _isoMapMatch->getContentSize()/4.f, MATCH_MAXIMUM_SCALE_FOR_ZOOMING, MATCH_ZOOM_DURATION);
        return;
    }
    
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, MATCH_MAXIMUM_SCALE_FOR_ZOOMING, MATCH_ZOOM_DURATION);
    
}

void SceneMatch::fadeOutButWaitForOpponent(int sceneIndex) {
    
    g_gameAudio->stopAllEffect();
    g_gameAudio->stopMusic();
    
    m_waitForResponse = true;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = NULL;
    }
    fadeOutThisSceneAndChangeTo(sceneIndex);
    
}

bool SceneMatch::fadeOutThisSceneAndChangeTo(int sceneIndex) {
    
    g_gameAudio->stopAllEffect();
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_gameAudio->stopMusic();
    
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, scrollView->getMinimumScale(), 0.6f);
    g_sceneManager->sceneSelector = (SceneList)sceneIndex;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    return true;
    
}
