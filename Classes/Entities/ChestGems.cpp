//
//  ChestGems.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 5/7/2557 BE.
//
//

#include "ChestGems.h"
USING_NS_CC;
using namespace std;
using namespace extension;

extern float sizeMultiplier;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIGemCollect.h"

#include "../Scenes/SceneVille.h"
#include "../Scenes/SceneWorldMap.h"
extern CCScene* g_currentScene;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

bool ChestGems::init() {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_spineChest = NULL;
    m_touchableItem = NULL;
    
    reload();

    m_didOpen = false;
    
    return true;
    
}

void ChestGems::reload() {
    
    if (m_spineChest) {
        m_spineChest->removeFromParent();
    }
    m_spineChest = g_sceneManager->sharedSkeletonData->getSkeleton("common/environment/", "chest_gem");
    m_spineChest->setScale(sizeMultiplier*0.4f);
    addChild(m_spineChest);
    
    if (m_touchableItem) {
        m_touchableItem->removeFromParent();
    }
    m_touchableItem = TouchableItem::createWithSpriteFrameName("_chest_touch_mark.png", this);
    m_touchableItem->onTabbed = onTouched;
    m_touchableItem->setSender(this);
    addChild(m_touchableItem);
    
    setContentSize(m_touchableItem->getContentSize());
    
}

void ChestGems::showup(bool reset) {
    
    if (m_spineChest) {
        m_spineChest->setOpacity(0);
        m_spineChest->setAnimation("idle", true);
        m_spineChest->runAction(CCFadeTo::create(1.0f, 255));
    }
    
    if (!reset) return;
    m_didOpen = false;
    
}

void ChestGems::prepare() {
    
    if (m_didOpen) return;
    
    m_spineChest->setAnimation("prepare_open", true);
    BackendService::getInstance()->send(APIGemCollect::create(cgVo.guid,
                                                              NULL,
                                                              IsoMapBuilding::onGemsCollected));
    m_didOpen = true;
    
}

void ChestGems::openup() {
    
    g_gameAudio->playEffect(AUDIO_CHEST_OPEN);
    m_spineChest->setAnimation("open", true);
    
    runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                 CCCallFunc::create(this, callfunc_selector(ChestGems::collect)),
                                 CCDelayTime::create(1.5f),
                                 CCFadeTo::create(1.0f, 0),
                                 CCRemoveSelf::create(),
                                 NULL));
    
}

void ChestGems::collect(CCObject *obj_) {
    
    switch (g_sceneManager->sceneSelector) {
        case SCENE_VILLE: {
            if (!g_currentScene || !((CScene*)g_currentScene)->isoMap) return;
            ((IsoMapBuilding*)((SceneVille*)g_currentScene)->isoMap)->collectGems(this);
        }break;
        case SCENE_WORLDMAP: {
            if (!g_currentScene || !((SceneWorldMap*)g_currentScene)->worldMap) return;
            ((SceneWorldMap*)g_currentScene)->worldMap->collectGems(this);
        }break;
        default: {
            
        }break;
    }
    
}

void ChestGems::onTouched(void *inst_, void *sender_) {
    
    if (ChestGems* _this = (ChestGems*)sender_) {
        _this->prepare();
    }
    
}