//
//  Ability.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 4/7/2557 BE.
//
//

#include "Ability.h"
USING_NS_CC;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIAbilityUpdate.h"

#include "../Services/GameAudioService.h"

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;
#include "../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

extern unsigned int currentTime;

bool Ability::init() {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_seconds_left_to_finish = 0;
    
    aVo = GDAbilityVO();
    paVo = PlayerAbilityVO();
    m_active = true;
    
    return true;
    
}

void Ability::update(float delta_) {
    
    if (!m_active) {
        return;
    }
    
    m_seconds_left_to_finish --;
    if (m_seconds_left_to_finish <= 0) {
        if (paVo.status == kAbilityIsUpgrading) {
            CCLog_("Fire ability_update here!");
            if (Building* _library = g_playerState->getBuilding(kBuildingLibrary, 0)) {
                BackendService::getInstance()->send(APIAbilityUpdate::create(_library,
                                                                             this,
                                                                             false,
                                                                             this,
                                                                             Ability::onUpdated));
            }
        }
        m_seconds_left_to_finish = 0;
    }
    
}

void Ability::setInitial() {
    
    m_seconds_left_to_finish = aVo.time - (currentTime - paVo.timestamp);
    update(0);
    
}

void Ability::setActive(bool active_) {

    m_active = active_;
}

int Ability::getSecondsLeftToFinish() {
    
    return m_seconds_left_to_finish;
    
}

float Ability::getCurrentProgressRatio() {
    
    return ((aVo.time - m_seconds_left_to_finish)/(float)aVo.time)*100.0f;
    
}

void Ability::onUpdated(void *sender, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parseUserRaceData(_root);
    _playerState->parseAbilityData(_root);
    Json_dispose(_root);
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    SceneVille *_sceneVille = (SceneVille*)g_currentScene;
    
    if (_sceneVille->hudPanel) {
        _sceneVille->hudPanel->resourceBars[HUDPanelBarCoins]->addTargetAmount(_playerState->player.balance.gold,
                                                                               _playerState->getTotalCoinStorageCapacity(),
                                                                               AUDIO_NONE);
        _sceneVille->hudPanel->resourceBars[HUDPanelBarCoins]->processTargetAmount();
    }
    
}
