//
//  Building.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#include "Building.h"
USING_NS_CC;
using namespace cocos2d::extension;
using namespace std;

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../AppMacros.h"
//extern CCPoint screenFactor;
extern TargetDevice targetDevice;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIBuildingNew.h"
#include "../Services/APIs/APIBuildingUpdate.h"
#include "../Services/APIs/APIBuildingOpen.h"
#include "../Services/APIs/APIBuildingUpgrade.h"
#include "../Services/APIs/APIBuildingCollect.h"
#include "../Services/APIs/APIBuildingFinishNow.h"
#include "../Services/APIs/APIPetFuse.h"
#include "../Services/APIs/APIPetRetrieve.h"

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "PlayerState.h"


#include "../Helpers/CCSpriteHelper.h"
#include "../Helpers/UIDisplayHelper.h"

#include "../Helpers/AnalyticsHelper.h"
#include "../Helpers/AnalyticsClientWrapper.h"
#include "../Helpers/FlurryCpp.h"

#include "../UIs/Dialogs/AnimalBarnDialog.h"

#include "../Services/GameAudioService.h"
#include "../Helpers/PushNotifications/PushNotifications.h"

#include "../Helpers/FBAppEventsWrapper.h"
#include "../Helpers/AppsFlyerWrapper.h"

extern GameAudioService* g_gameAudio;

extern unsigned int currentTime;
extern double currentTimeClient;
extern float sizeMultiplier;
extern float retinaMultiplier;

Building* Building::createWithBuildingVO(GDBuildingVO bvo_) {
    
    Building* pRet = new Building();
    if (pRet->initWithBuildingVO(bvo_, PlayerBuildingVO())) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return NULL;
    
}

Building* Building::createWithBuildingVO(GDBuildingVO bvo_, PlayerBuildingVO pbvo_) {

    Building* pRet = new Building();
    if (pRet->initWithBuildingVO(bvo_, pbvo_)) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return NULL;
        
}

bool Building::initWithBuildingVO(GDBuildingVO bvo_, PlayerBuildingVO pbvo_) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    bVo = GDBuildingVO(bvo_);
    pbVo = PlayerBuildingVO(pbvo_);
    pbVo.type = bVo.type;
    
    m_progress_bar = NULL;
    m_energy_bar = NULL;
    m_ability_bar = NULL;
    m_delta = 0.0f;
    m_active = true;
    bubbleStatus = false;
    m_countDown = 0;
    m_countUp = 0;
    m_prev_level = 0;
    didMove = false;
    addPetAnimationRunningWithSmallDelay = false;
    
    spineBuilding = NULL;
    spineUpgrade = NULL;
    spineBuildingFront = NULL;
    spineConstruction = NULL;
    
    for (int i=0; i<3; i++) {
        m_params[i] = 0.0f;
    }
    
    unmarkForFinishNow();
    unmarkForPetFuse();
    unmarkForPetRetrieve();
    
    m_collectingBubble = NULL;
    m_collectingAttention = NULL;
    
    m_retrivingItemLayer = NULL;
   
    preferenceRootParent = NULL;
    
    parent = NULL;
    this->pet = NULL;
    product = NULL;
    didTryToCollect = false;
    
    spine_pet_running = NULL;
    
    m_spineLayer = CCLayer::create();
    m_spineLayer->setAnchorPoint(CCPointZero);
    addChild(m_spineLayer);
//    m_actionTouch = NULL;
    m_finishAnimation = NULL;
    
    onBuildingWillOpen = NULL;
    onBuildingWillOpenSender = NULL;
    
    m_upgradableSignSprite = NULL;
    
    createBuilding(false);
    
    return true;
    
}


#pragma mark Core functions

void Building::updateTimer(float delta) {
    
    if (pbVo.status == BUILDING_STATUS_CONSTRUCT) {
        
        if (m_countDown > 0) {
            
            m_countDown --;
            
        } else if (m_countDown == 0) {
            
            CREATE_CHAR_BUFFER(_buf, 64);
            snprintf(_buf, _buf_size, g_gameContent->Word["construction_completed"].c_str(),bVo.title.c_str());
            switch (g_sceneManager->sceneSelector) {
                case SCENE_VILLE:
                    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO,_buf,false);
                    break;
                case SCENE_WORLDMAP:
                    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO,_buf,false);
                    break;
                case SCENE_MATCH:
                case SCENE_MATCH2:
                    g_sceneManager->pendingMsg->addObject(CCString::create(_buf));
                    break;
                case SCENE_RACE:
                case SCENE_RACE2:
                    g_sceneManager->pendingMsg->addObject(CCString::create(_buf));
                    break;
                default:
                    break;
            }

            m_countDown = -1;
            
        }
        
    }else if (pbVo.status == BUILDING_STATUS_UPGRADE) {
        
        if (m_countDown > 0) {
            
            m_countDown --;
            
        } else if (m_countDown == 0) {
            
            CREATE_CHAR_BUFFER(_buf, 64);
            snprintf(_buf, _buf_size, g_gameContent->Word["upgrade_completed"].c_str(),bVo.title.c_str(),bVo.level+1);
            switch (g_sceneManager->sceneSelector) {
                case SCENE_VILLE:
                    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO,_buf,false);
                    break;
                case SCENE_WORLDMAP:
                    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO,_buf,false);
                    break;
                case SCENE_MATCH:
                case SCENE_MATCH2:
                    g_sceneManager->pendingMsg->addObject(CCString::create(_buf));
                    break;
                case SCENE_RACE:
                case SCENE_RACE2:
                    g_sceneManager->pendingMsg->addObject(CCString::create(_buf));
                    break;
                default:
                    break;
                    
            }
            
            m_countDown = -1;
            
        }
        
    }
    
    
}

void Building::update(float delta_)
{
    PlayerState *_playerState = PlayerState::getInstance();
    checkPBVOType(delta_);
    
    if (m_collectingBubble)
    {
        int _amount = (bVo.building_type==BUILDING_TYPE_PRODUCER)?_playerState->player.balance.food:_playerState->player.balance.gold;
        int _capacity = (bVo.building_type==BUILDING_TYPE_PRODUCER)?_playerState->getTotalFoodStorageCapacity():_playerState->getTotalCoinStorageCapacity();
        CCSpriteFrame* _spriteFrame = (_amount>=_capacity)?CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("_effect_glow_orange.png"):CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("_effect_glow.png");
        if (_spriteFrame!=NULL)
        {
            m_collectingBubble->setDisplayFrame(_spriteFrame);
        }
    }
    
    if (pbVo.status == BUILDING_STATUS_CONSTRUCT || pbVo.status == BUILDING_STATUS_UPGRADE)
    {
        if (m_countDown > 0)
        {
            m_countDown --;
        }
        else if (m_countDown == 0)
        {
            m_countDown = -1;
            finishState();
        }
    }
    else if (pbVo.status == BUILDING_STATUS_DELIVERED)
    {
        if (bVo.building_type == BUILDING_TYPE_PRODUCER)
        {
            updateFoodProduction();
            #if FEATURE_COINS_FROM_PET
        }
        else if (bVo.building_type == BUILDING_TYPE_TICKETBOOTH || bVo.building_type == BUILDING_TYPE_HABITAT)
        {
            #else
        }
        else if (bVo.building_type == BUILDING_TYPE_TICKETBOOTH)
        {
            #endif
            updateCoinEarning();
        }
        else if (bVo.building_type == BUILDING_TYPE_SKILLS)
        {
            if (Ability* _ability = _playerState->getUpgradingAbility())
            {
                _ability->update(delta_);
                _showAbilityBar(_ability);
            }
            else
            {
                _hideAbilityBar();
            }
        }
    }
    
    _updateUpgradableSign();
   
}

bool Building::isStateEqual(PlayerBuildingVO vo) {
    
    if (pbVo.buid == vo.buid) return true;
    
    int pattern_count = 0;
    int match_count = 0;
    
    for (int i=0; i<BUILDING_SPACE_GRID_NUM; i++) {
        
        if (pbVo.map[i] == -1)
            continue;
        pattern_count ++;
        
        if (pbVo.map[i] == vo.map[i])
            match_count ++;
    }
    
    return (pattern_count == match_count);
    
}

bool Building::newState(BuildingBuildType buildType, bool unpdate_in_backend) {
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    PlayerState* _playerState = PlayerState::getInstance();
    
    if (pbVo.status == BUILDING_STATUS_PREVIEW) {
        pbVo.level = 1;
        if (BUILD_HABITAT_WITH_GEMS && bVo.building_type == BUILDING_TYPE_HABITAT) {
            pbVo.status = BUILDING_STATUS_DELIVERED;
            int cost_gems = int(bVo.param_2);
            if (_playerState->deductGems(cost_gems)) {
                if (_scene) _scene->hudPanel->updateResourceBar(HUDPanelBarGems);
            }
        }else {
            pbVo.status = BUILDING_STATUS_CONSTRUCT;
        }
        
        currentTime = (int)round(currentTimeClient);
        pbVo.timestamp = currentTime;
        updateCountdown();
        
    }
    
    createBuilding(false);
    updateProgressBarVisibility();
    updateFlip();
    
    if (this->pet) {
        this->pet->reload();
        this->pet->resumeAction();
        this->pet->scheduleUpdate();
    }
    
    // not a new building
    if (pbVo.buid != "") {
        if (!unpdate_in_backend || !m_active) return true;
        _playerState->moveBuilding(this);
        if (bVo.time >= kIsoMapSaveInterval ||
            pbVo.status != BUILDING_STATUS_PREVIEW ||
            pbVo.status != BUILDING_STATUS_CONSTRUCT ) return true;
        BackendService::getInstance()->send(APIBuildingUpdate::create(this,
                                                                      _scene->isoMap,
                                                                      IsoMap::onBuildingUpdated));
        return true;
    }
    
    if (!_scene || typeid(*_scene) != typeid(SceneVille)) return false;
    
    // new building
    if (buildType == BuildingBuildTypeNormal &&
        (!BUILD_HABITAT_WITH_GEMS || bVo.building_type != BUILDING_TYPE_HABITAT)) {
        if (_playerState->deductCoins(bVo.cost_coins)) {
            _scene->hudPanel->updateResourceBar(HUDPanelBarCoins);
        }
    }else if (buildType == BuildingBuildTypeBuyMissingResources) {
        int missing_coins = bVo.cost_coins - _playerState->player.balance.gold;
        if (_playerState->deductCoins(_playerState->player.balance.gold)) {
            _scene->hudPanel->updateResourceBar(HUDPanelBarCoins);
        }
        int cost_gems = GameContentService::getInstance()->get_gemcal_for_coins(missing_coins);
        if (_playerState->deductGems(cost_gems)) {
            _scene->hudPanel->updateResourceBar(HUDPanelBarGems);
        }
    }
    
    CCDictionary *params = CCDictionary::create();
    CSJson::Value eventParameters;
    eventParameters["buildingType"] = pbVo.type;
    eventParameters["toBuildingLevel"] = 1;
    eventParameters["costCoins"] = bVo.cost_coins;
    params->setObject(ccs(AnalyticsClient::getInstance()->write(eventParameters)), "buildBuildingInfo");
    FlurryCpp::logEvent(EVENT_BUILD_BUILDING, params);
    
    AnalyticsClient::getInstance()->logEvent(EVENT_BUILD_BUILDING, eventParameters);
    
    FBAppEventsWrapper::logEvent(EVENT_BUILD_BUILDING);
    
    BackendService::getInstance()->send(APIBuildingNew::create(this,
                                                               (int)buildType,
                                                               _scene->isoMap,
                                                               IsoMap::onBuildingUpdated));
    
    return true;
    
}

bool Building::syncState(PlayerBuildingVO vo) {
    
    pbVo.buid = vo.buid;
    pbVo.flip = vo.flip;
    pbVo.level = vo.level;
    pbVo.status = vo.status;
    pbVo.timestamp = vo.timestamp;

    bVo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level);
    updateCountdown();
    updateFlip();
    
    fireForFinishNow();
    fireForPetFuse();
    fireForPetRetrieve();
    
    updateTrackPointsIfThisIsRaceAsso();

    return true;
    
}

void Building::finishState()
{
    _hideProgressBar();
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille)) return;
    
    if (pbVo.status == BUILDING_STATUS_CONSTRUCT)
    {
        pbVo.status = BUILDING_STATUS_COMPLETE;
    }
    else if (pbVo.status == BUILDING_STATUS_UPGRADE)
    {
        pbVo.status = BUILDING_STATUS_DELIVERED;
        pbVo.level ++;
        bVo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level);
        PlayerState::getInstance()->gainExp(bVo);
        runUpgradeFinishEffect();
        _scene->hudPanel->updateHUD();
        _scene->hudPanel->updateHUDTrack(_scene->isoMap->isTrackComplete());
    }
    
    CREATE_CHAR_BUFFER(_buf, 64);
    snprintf(_buf, _buf_size, g_gameContent->Word["construction_completed"].c_str(), bVo.title.c_str());
    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buf, false);
    
    updateProgressBarVisibility();
    
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    if (!_isoMapBuilding || typeid(*_isoMapBuilding) != typeid(IsoMapBuilding)) return;
    
    if (_isoMapBuilding->entityAction) {
        _isoMapBuilding->entityAction->finish();
    }
    
    if (_isoMapBuilding->placeHolder) {
        if (_isoMapBuilding->placeHolder->building == this) {
            _isoMapBuilding->placingStuffFromPlaceHolder(true, BuildingBuildTypeNormal);
        }
    }
    
    BackendService::getInstance()->send(APIBuildingUpdate::create(this,
                                                                  _scene->isoMap,
                                                                  IsoMap::onBuildingUpdated));
    
}

bool Building::finishNowState(void *sender, void (*callback)(void*,string)) {
    
    if (pbVo.status != BUILDING_STATUS_CONSTRUCT &&
        pbVo.status != BUILDING_STATUS_UPGRADE) return false;
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille)) return false;
    
    PlayerState* _playerState = PlayerState::getInstance();
    int gems_cost = GameContentService::getInstance()->get_gemcal_for_building_finishnow(getSecondsLeftToFinish());
    if (_playerState->deductGems(gems_cost)) {
        _scene->hudPanel->updateResourceBar(HUDPanelBarGems);
    }
    pbVo.level ++;
    bVo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level);
    _playerState->gainExp(bVo);
    runUpgradeFinishEffect();
    _scene->hudPanel->updateHUD();
    _scene->hudPanel->updateHUDTrack(_scene->isoMap->isTrackComplete());
    _scene->hudPanel->updateResourceBar(HUDPanelBarCoins);
    
    pbVo.status = BUILDING_STATUS_DELIVERED;
    updateProgressBarVisibility();
    
    if (pbVo.buid != "") {
        BackendService::getInstance()->send(APIBuildingFinishNow::create(this, sender, callback));
        unmarkForFinishNow();
        return true;
    }

    markForFinishNow(sender, callback);
    
    return false;
    
}

void Building::openState() {

    if (pbVo.status != BUILDING_STATUS_COMPLETE) return;
    
    if (onBuildingWillOpen) {
        onBuildingWillOpen(onBuildingWillOpenSender);
    }
    
    pbVo.status = BUILDING_STATUS_OPEN;
    updateProgressBarVisibility();
    pbVo.status = BUILDING_STATUS_DELIVERED;
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille)) return;

    PlayerState* _playerState = PlayerState::getInstance();
    _playerState->gainExp(bVo);
    runUpgradeFinishEffect();
    _scene->hudPanel->updateHUD();
    _scene->hudPanel->updateHUDTrack(_scene->isoMap->isTrackComplete());
    
    BackendService::getInstance()->send(APIBuildingOpen::create(this, _scene->isoMap, IsoMap::onBuildingUpdated));
    
}

Building::ActionResult Building::upgradeState() {
    
    return upgradeState(BuildingUpgradeTypeNormal, 0);
    
}

Building::ActionResult Building::upgradeState(BuildingUpgradeType upgradeType, int gems_cost) {
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    if (pbVo.status == BUILDING_STATUS_UPGRADE) {
        return OK;
    }
    
    GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level+1);
    if (_nxt_bvo.type == "") {
        return MAXIMUM_LEVEL_REACHED;
    }
    
    Building *_builder = _playerState->getBuilder();
    if (_builder
        && upgradeType == BuildingUpgradeTypeNormal) {
        return BUILDER_IS_BUSY;
    }
    
    if (_playerState->player.balance.gold < _nxt_bvo.cost_coins
        && upgradeType == BuildingUpgradeTypeNormal) {
        return NOT_ENOUGH_COINS;
    }
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    CCAssert(_scene!=NULL, "Should upgrade building only when in SceneVille!");
    
    int _collect_amount = 0;
    if (bVo.building_type == BUILDING_TYPE_PRODUCER) {
        _collect_amount = _playerState->getMaxFoodCanCollect(getResourceProduced());
    }else if (bVo.building_type == BUILDING_TYPE_TICKETBOOTH) {
        _collect_amount = _playerState->getMaxCoinCanCollect(getResourceProduced());
    }
    collectingEffect(_collect_amount);
    
    switch (upgradeType) {
        default:
        case BuildingUpgradeTypeNormal: {
            pbVo.status = BUILDING_STATUS_UPGRADE;
            currentTime = (int)round(currentTimeClient);
            pbVo.timestamp = currentTime;
            updateCountdown();
            updateProgressBarVisibility();
            if (_playerState->deductCoins(_nxt_bvo.cost_coins)) {
                _scene->hudPanel->updateResourceBar(HUDPanelBarCoins);
            }
        }break;
        case BuildingUpgradeTypeInstant: {
            pbVo.status = BUILDING_STATUS_DELIVERED;
            currentTime = (int)round(currentTimeClient);
            pbVo.timestamp = currentTime;
            updateCountdown();
            updateProgressBarVisibility();
            if (_playerState->deductGems(gems_cost)) {
                _scene->hudPanel->updateResourceBar(HUDPanelBarGems);
            }
            pbVo.level ++;
            bVo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level);
            _playerState->gainExp(bVo);
            runUpgradeFinishEffect();
            _scene->hudPanel->updateHUD();
        }break;
        case BuildingUpgradeTypeBuyMissingResources: {
            pbVo.status = BUILDING_STATUS_UPGRADE;
            currentTime = (int)round(currentTimeClient);
            pbVo.timestamp = currentTime;
            updateCountdown();
            updateProgressBarVisibility();
            if (_playerState->deductGems(gems_cost)) {
                _scene->hudPanel->updateResourceBar(HUDPanelBarGems);
            }
            if (_playerState->deductCoins(_playerState->player.balance.gold)) {
                _scene->hudPanel->updateResourceBar(HUDPanelBarCoins);
            }
        }break;
    }
    
    CCDictionary *params = CCDictionary::create();
    CSJson::Value eventParameters;
    eventParameters["buildingType"] = pbVo.type; //Current Building Type
    eventParameters["toBuildingLevel"] = pbVo.level+1; //Building Level to be upgraded to
    eventParameters["costCoins"] = bVo.cost_coins;
    params->setObject(ccs(AnalyticsClient::getInstance()->write(eventParameters)), "upgradeBuildingInfo");
    FlurryCpp::logEvent(EVENT_UPGRADE_BUILDING, params);
    
    AnalyticsClient::getInstance()->logEvent(EVENT_UPGRADE_BUILDING, eventParameters);
    
    FBAppEventsWrapper::logEvent(EVENT_UPGRADE_BUILDING);
    
    BackendService::getInstance()->send(APIBuildingUpgrade::create(this,
                                                                   _collect_amount,
                                                                   getResourceProduced(),
                                                                   (int)upgradeType,
                                                                   getParent(),
                                                                   IsoMap::onBuildingUpdated));
    
    return OK;
    
}


bool Building::createBuilding() {
    
    return createBuilding(true);
    
}

bool Building::createBuilding(bool bumping) {

    return createBuilding(bumping, false);
    
}

bool Building::createBuilding(bool bumping, bool force) {
    
    cleanEffect();
    
    //Load spine for main building
    if (!spineBuilding || m_prev_level != pbVo.level || force) {
        
        m_prev_level = pbVo.level;
        if (BUILD_HABITAT_WITH_GEMS && bVo.building_type == BUILDING_TYPE_HABITAT) {
//            bVo = g_gameContent->getBuildingVO(pbVo.type, PlayerState::getInstance())
        }else {
            bVo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level);
        }
        
        
        if (spineBuilding) {
            spineBuilding->removeFromParentAndCleanup(true);
        }
        
        spineBuilding = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, bVo.src.c_str());
        //spineBuilding->preferenceRootParent = preferenceRootParent;
        spineBuilding->setContentSize(CCSize(bVo.src_width, bVo.src_height));
        if (targetDevice != TD_IPAD_RETINA) {
            spineBuilding->setScale(0.5f);
        }
        spineBuilding->setAnimation(kBuildingCommonAniIdle, true);
        m_spineLayer->addChild(spineBuilding, 1);
        m_spineLayer->setContentSize(spineBuilding->getContentSize());
    
        if (spineBuildingFront) {
            spineBuildingFront->removeFromParentAndCleanup(true);
            spineBuildingFront = NULL;
        }
        
        //Load spine for front part of building
        if (bVo.type == kBuildingHabitat ||
            bVo.type == kBuildingHabitatPremium) {
            spineBuildingFront = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, bVo.src.c_str());
            //spineBuildingFront->preferenceRootParent = preferenceRootParent;
            spineBuildingFront->setContentSize(CCSize(bVo.src_width, bVo.src_height));
            if (targetDevice != TD_IPAD_RETINA) {
                spineBuildingFront->setScale(0.5f);
            }
            //spineBuildingFront->setSpecifiedBoneToRender(kBuildingSpecificFront);
            spineBuildingFront->setAnimation(kBuildingCommonAniIdle, true);
            m_spineLayer->addChild(spineBuildingFront, 3);
        }
        
    }else {
        
        //Continue updating animation for spines
        spineBuilding->unscheduleUpdate();
        spineBuilding->scheduleUpdate();
        if (spineBuildingFront) {
            spineBuildingFront->unscheduleUpdate();
            spineBuildingFront->scheduleUpdate();
        }
        
    }
    
    //Update pet it building if there's any
    if (this->pet) {
        this->pet->reload(true);
    }
    
    initSpineConstruction(force);
    initSpineUpgrading(force);

    setPosition(IsoMap::getPositionByMapIndexes(pbVo.map, g_gameContent->tileSize, g_gameContent->tileShift));
   
    //Make it bump (animation)
    if (bumping) {
        stopAllActions();
        runAction(CCSequence::create(CCScaleTo::create(0.1f, 0.8f),
                                     CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f), 0.5f),
                                     NULL));
    }
    
    
    
    return true;
    
}

bool Building::initSpineConstruction(bool force) {
    
    if (!spineConstruction || force) {
        if (spineConstruction) spineConstruction->removeFromParent();
        spineConstruction = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, kBuildingConstructionSrc);
        //spineConstruction->preferenceRootParent = preferenceRootParent;
        spineConstruction->setContentSize(CCSize(bVo.src_width, bVo.src_height));
        if (targetDevice != TD_IPAD_RETINA) {
            spineConstruction->setScale(0.5f);
        }
        spineConstruction->setVisible(false);
        addChild(spineConstruction);
    }else {
        spineConstruction->unscheduleUpdate();
        spineConstruction->scheduleUpdate();
    }
    
    return true;
    
}

bool Building::initSpineUpgrading(bool force) {
    
    if (!spineUpgrade || force) {
        if (spineUpgrade) spineUpgrade->removeFromParent();
        spineUpgrade = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, kBuildingUpgradeSrc);
        //spineUpgrade->preferenceRootParent = preferenceRootParent;
        spineUpgrade->setContentSize(CCSize(bVo.src_width, bVo.src_height));
        if (targetDevice != TD_IPAD_RETINA) {
            spineUpgrade->setScale(0.5f);
        }
        spineUpgrade->setVisible(false);
        addChild(spineUpgrade);
    }else {
        spineUpgrade->unscheduleUpdate();
        spineUpgrade->scheduleUpdate();
    }
    
    return true;
    
}

void Building::willRemoveFromIsoMap() {
    
    _checkAndDeleteRetrievingItemLayer(true);
    hideCollectingBubble();
    
}

#pragma mark - Mark for actions

void Building::markForFinishNow(void *sender, void (*callback)(void *, std::string)) {
    
    m_markForFinishNow = true;
    m_holdFinishNowSender = sender;
    m_holdFinishNowCallback = callback;
    
}

void Building::unmarkForFinishNow() {
    
    m_markForFinishNow = false;
    m_holdFinishNowSender = NULL;
    m_holdFinishNowCallback = NULL;
    
}

void Building::fireForFinishNow() {
    
    if (!m_markForFinishNow) return;
    
    BackendService::getInstance()->send(APIBuildingFinishNow::create(this,
                                                                    m_holdFinishNowSender,
                                                                    m_holdFinishNowCallback));
    unmarkForFinishNow();
    
}

void Building::markForPetRetrieve(void *sender, void (*callback)(void *, std::string)) {
    
    m_markForPetRetrieve = true;
    m_holdPetRetrieveSender = sender;
    m_holdPetRetrieveCallback = callback;
    
}

void Building::unmarkForPetRetrieve() {

    m_markForPetRetrieve = false;
    m_holdPetRetrieveSender = NULL;
    m_holdPetRetrieveCallback = NULL;
    
}

void Building::fireForPetRetrieve() {
    
    if (!m_markForPetRetrieve) return;
    
    BackendService::getInstance()->send(APIPetRetrieve::create(this->pet,
                                                               this,
                                                               m_holdPetRetrieveSender,
                                                               m_holdPetRetrieveCallback));
    unmarkForPetRetrieve();
    
}

void Building::markForPetFuse(GDPetVO pet_vo,
                              bool instant,
                              void *sender,
                              void (*callback)(void *, std::string)) {
    
    m_markForPetFuse = true;
    m_holdPetFuseVo = pet_vo;
    m_holdPetFuseInstant = instant;
    m_holdPetFuseSender = sender;
    m_holdPetFuseCallback = callback;
    
}

void Building::unmarkForPetFuse() {
    
    m_markForPetFuse = false;
    m_holdPetFuseVo = GDPetVO();
    m_holdPetFuseInstant = false;
    m_holdPetFuseSender = NULL;
    m_holdPetFuseCallback = NULL;
    
}

void Building::fireForPetFuse() {
    
    if (!m_markForPetFuse) return;
    
    BackendService::getInstance()->send(APIPetFuse::create(m_holdPetFuseVo,
                                                           this,
                                                           m_holdPetFuseInstant,
                                                           m_holdPetFuseSender,
                                                           m_holdPetFuseCallback));
    
    unmarkForPetFuse();
    
}

void Building::updateTrackPointsIfThisIsRaceAsso() {
    
    if (pbVo.type != kBuildingAssociation)
        return;
    
    PlayerState::getInstance()->calculateTrackpointsUsed();
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (typeid(*_scene) != typeid(SceneVille))
        return;
    
    _scene->hudPanel->updateHUDTrack(_scene->isoMap->isTrackComplete());
    
}

#pragma mark - building actions

void Building::collect() {
    
    collect(true);
    
}

void Building::collect(bool tell_backend_) {
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*g_currentScene) != typeid(SceneVille)) return;
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    bool _didCollect = false;
    int _collect_amount = 0;
    int _collect_mode = -1;
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    
    // collect food
    if (bVo.building_type == BUILDING_TYPE_PRODUCER) {
        float _total_food_storage_capacity = _playerState->getTotalFoodStorageCapacity();
        float _resource_produced = getResourceProduced();
        if (_playerState->player.balance.food >= _total_food_storage_capacity) {
            _collect_mode = 0; //food storage full - cannot collect
            _didCollect = true;
            _scene->addFeedSiloTutorials();
            snprintf(_buffer, _buffer_size, "Upgrade or Build more Food Silo to increase storage");
        }else if ((_playerState->player.balance.food + _resource_produced) > _total_food_storage_capacity) {
            _collect_mode = 1; //collect some part from building
            _collect_amount = _total_food_storage_capacity - _playerState->player.balance.food;
            g_gameAudio->playEffect(AUDIO_COLLECT);
        }else {
            _collect_mode = 2; //collect all from building
            _collect_amount = _resource_produced;
            g_gameAudio->playEffect(AUDIO_COLLECT);
        }
    }
    
    // collect coins - do the same step with collect food above
#if FEATURE_COINS_FROM_PET
    if ((bVo.building_type == BUILDING_TYPE_TICKETBOOTH) ||
        (bVo.building_type == BUILDING_TYPE_HABITAT && this->pet)) {
#else
    if (bVo.building_type == BUILDING_TYPE_TICKETBOOTH) {
#endif
        float _total_coin_storage_capacity = _playerState->getTotalCoinStorageCapacity();
        float _coins_produced = getResourceProduced();
        if (_playerState->player.balance.gold >= _total_coin_storage_capacity) {
            _collect_mode = 0;
            _didCollect = true;
            _scene->addBankTutorials();
            snprintf(_buffer, _buffer_size, "Upgrade or Build more Banks to increase storage");
        }else if ((_playerState->player.balance.gold + _coins_produced) > _total_coin_storage_capacity) {
            _collect_mode = 1;
            _collect_amount = _total_coin_storage_capacity - _playerState->player.balance.gold;
            g_gameAudio->playEffect(AUDIO_COLLECT);
        }else {
            _collect_mode = 2;
            _collect_amount = _coins_produced;
            CCLog_("%f", _coins_produced);
            g_gameAudio->playEffect(AUDIO_COLLECT);
        }
    }

    _didCollect = collectingEffect(_collect_amount);
        
    hideCollectingBubble();
    if (_collect_mode < 0 || !_didCollect) {
        didTryToCollect = true;
        return;
    }

    int full_amount = (int)getResourceProduced();
    deductResourceProduced(_collect_amount);
    
    if (_collect_mode == 0) { // tried to collect, but counld not
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
        didTryToCollect = true;
        return;
    }
        
    if (_collect_mode == 1) {
        didTryToCollect = true;
    }
    
    if (tell_backend_) {
        BackendService::getInstance()->send(APIBuildingCollect::create(this,
                                                                       _collect_amount,
                                                                       full_amount,
                                                                       this,
                                                                       Building::onResourceCollected));
    }
        
}
    
void Building::onResourceTouched(void *inst, void *sender) {
    
    Building *_this = (Building*)sender;
    _this->collect();
    
}
    
void Building::onResourceCollected(void *sender, std::string json_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json* _root = Json_create(json_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parsePetData(_root);
    _playerState->parseBuildingData(_root);
    Json_dispose(_root);
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    SceneVille *_scene              = (SceneVille*)g_currentScene;
    IsoMapBuilding *_isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    Building *_building             = (Building*)sender;
    
    if ((_building->bVo.building_type == BUILDING_TYPE_TICKETBOOTH
#if FEATURE_COINS_FROM_PET
         || _building->bVo.building_type == BUILDING_TYPE_HABITAT
#endif
         ) && _isoMapBuilding->collectableCoins->getNItems() == 0) {
        _scene->hudPanel->updateResourceBar(HUDPanelBarCoins);
    }
    
    if ((_building->bVo.building_type == BUILDING_TYPE_PRODUCER
         ) && _isoMapBuilding->collectableFoods->getNItems() == 0) {
        _scene->hudPanel->updateResourceBar(HUDPanelBarFoods);
    }

}

bool Building::collectingEffect(int collect_amount_) {
    
    if (collect_amount_ <= 0) return false;
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return false;

    SceneVille *_scene = (SceneVille*)g_currentScene;
    IsoMapBuilding *_isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    
    CollectableItems *_collectableResources = NULL;
    if (bVo.building_type == BUILDING_TYPE_PRODUCER) {
        _collectableResources = _isoMapBuilding->collectableFoods;
    }else if (bVo.building_type == BUILDING_TYPE_TICKETBOOTH
#if FEATURE_COINS_FROM_PET
              || (bVo.building_type == BUILDING_TYPE_HABITAT && this->pet)
#endif
              ) {
        _collectableResources = _isoMapBuilding->collectableCoins;
    }
    
    if (!_collectableResources) return false;
    
    if (product) { //if food icon appears on building, then collected icon should start from there before fly to HUD
        _collectableResources->add(convertToWorldSpace(product->getPosition()), _isoMapBuilding->getParent()->getScale(), collect_amount_,bVo);
    }else { // otherwise collected icon will start from the building and fly to HUD
        _collectableResources->add(parent->convertToWorldSpace(getPosition()), _isoMapBuilding->getParent()->getScale(), collect_amount_,bVo);
    }
    
    // Text Effect
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "%d", collect_amount_);
    float totalAnimationTime = kCollectAnimTotalAnimationTime;
    float fadeTime = kCollectAnimFadeTime;
    CCLabelBMFont *_collectedAmountLabel = CCLabelHelper::createBMFont( _buffer, kFontSizeVeryLarge, true);
    _collectedAmountLabel->setColor(kTextColorFood);
    _collectedAmountLabel->setPosition(ccp(0.0f, bVo.src_height*0.7f*retinaMultiplier));
    addChild(_collectedAmountLabel, 10);
    _collectedAmountLabel->
    runAction(CCSequence::create(CCSpawn::create(CCSequence::create(CCDelayTime::create(totalAnimationTime-fadeTime),
                                                                    CCFadeOut::create(fadeTime),
                                                                    NULL),
                                                 CCMoveBy::create(totalAnimationTime, ccp(0.0f, kCollectAnimDistanceMoved*retinaMultiplier))
                                                 ,
                                                 NULL)
                                 ,
                                 CCRemoveSelf::create(),
                                 NULL));
    
    return true;
    
}
    

    
bool Building::isMovable() {
 
    return (bVo.movable && pbVo.buid != "");
    
}

string Building::getMoveJsonRendered() {
    
    CREATE_CHAR_BUFFER(_buffer, 512);
    string _json_str = "";
    
    _json_str.append("{");
    snprintf(_buffer, _buffer_size, "\"buid\":\"%s\",", pbVo.buid.c_str());
    _json_str.append(_buffer);
    
    CREATE_CHAR_BUFFER(_map, ((BUILDING_SPACE_GRID_NUM*5)+2));
    snprintf(_map, _map_size, "[");
    for (int i=0; i<BUILDING_SPACE_GRID_NUM; i++) {
        if (pbVo.map[i] >= 0) {
            if (i != 0) {
                snprintf(_map, _map_size, "%s,%d", _map, pbVo.map[i]);
            }else {
                snprintf(_map, _map_size, "%s%d", _map, pbVo.map[i]);
            }
        }
    }
    snprintf(_map, _map_size, "%s]", _map);
    snprintf(_buffer, _buffer_size, "\"map\":%s", _map);
    _json_str.append(_buffer);
    _json_str.append("}");
    return _json_str.c_str();
    
}

void Building::updateCountdown() {
    
    if (pbVo.status == BUILDING_STATUS_CONSTRUCT) {
        
        m_countDown = (pbVo.timestamp + bVo.time) - currentTime;
        
        if (m_countDown < 0) {
            m_countDown = 0;
        }

    }else if (pbVo.status == BUILDING_STATUS_DELIVERED) {
        

    
    }else if (pbVo.status == BUILDING_STATUS_UPGRADE) {
        
        GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(bVo.type, bVo.level+1);
        if (_nxt_bvo.type == "") {
            return;
        }
        m_countDown = (pbVo.timestamp + _nxt_bvo.time) - currentTime;
        
        if (m_countDown < 0) {
            m_countDown = 0;
        }

        if (!PushNotifications::Instance().isEnabled() && m_countDown > PushNotifications::upgradeCountdownMin)
        {
            PushNotifications::Instance().notify(PushNotifications::FirstTimeBuildingUpgrade, bVo.title);
        }
    }
}

void Building::updateFlip() {
    
    if (m_spineLayer) {
        m_spineLayer->setContentSize(this->getContentSize());
        float _scaleX = fabsf(m_spineLayer->getScaleX());
        if (pbVo.flip) {
            m_spineLayer->setScaleX(-_scaleX);
        }else {
            m_spineLayer->setScaleX(_scaleX);
        }
    }
    
}

//void Building::constructionGraphic() {
//    
//    m_spineLayer->setVisible(false);
//    spineConstruction->setVisible(true);
//    spineConstruction->setAnimation(kBuildingConstructionAniIdle, true);
//    m_countDown = -2; //must be 2, do not change
//    
//}

//void Building::upgradingGraphic() {
//    
//    spineUpgrade->setVisible(true);
//    spineUpgrade->setAnimation(kBuildingUpgradeAniIdle, true);
//    m_countDown = -2;
//    
//}
 
void Building::updateProgressBarVisibility() {
    
    if (spineBuilding) {
        spineBuilding->setColor(ccWHITE);
    }
    if (spineBuildingFront) {
        spineBuildingFront->setColor(ccWHITE);
    }
    if (spineConstruction) {
        spineConstruction->setColor(ccWHITE);
    }
    if (spineUpgrade) {
        spineUpgrade->setColor(ccWHITE);
    }
    
    if (pbVo.status == BUILDING_STATUS_CONSTRUCT ) {
        
        m_spineLayer->setVisible(false);
        spineConstruction->setVisible(true);
        spineConstruction->setAnimation(kBuildingConstructionAniIdle, true);
        _showProgressBar();
        
    }else if (pbVo.status == BUILDING_STATUS_UPGRADE) {
        
        spineUpgrade->unscheduleUpdate();
        spineUpgrade->scheduleUpdate();
        spineUpgrade->setVisible(true);
        spineUpgrade->setAnimation(kBuildingUpgradeAniIdle, true);
        _showProgressBar();
        
    }else if (pbVo.status == BUILDING_STATUS_COMPLETE) {
        
        m_spineLayer->setVisible(false);
        spineConstruction->setVisible(true);
        spineConstruction->setAnimation(kBuildingConstructionAniFinish, true);
        
    }else if (pbVo.status == BUILDING_STATUS_OPEN) {
        
        m_spineLayer->setVisible(true);
        spineConstruction->setVisible(true);
        spineConstruction->setAnimation(kBuildingConstructionAniOpen, false);
        
    }else if (pbVo.status == BUILDING_STATUS_DELIVERED) {
        
        m_spineLayer->setVisible(true);
        bool _reveal = false;
        if (spineConstruction->states[0]->animation) {
            if ((strcmp(spineConstruction->states[0]->animation->name, kBuildingConstructionAniFinish) == 0)){
                _reveal = true;
            }
        }
        if (_reveal) {
            spineConstruction->setVisible(true);
            spineConstruction->setAnimation(kBuildingConstructionAniOpen, false);
        }else {
            spineConstruction->setVisible(false);
        }
        spineUpgrade->setVisible(false);
        _hideProgressBar();
        
    }
    
}

//void Building::openBuilding() {
//    
//    if (onBuildingWillOpen) {
//        onBuildingWillOpen(onBuildingWillOpenSender);
//    }
//    
//    this->pbVo.status = BUILDING_STATUS_OPEN;
//    this->createBuilding(false);
//    this->updateProgressBarVisibility();
//    scheduleOnce(schedule_selector(Building::openBuildingInBackend), 1.0f);
//    
//}
//
//void Building::openBuildingInBackend() {
//    
//    BackendService::getInstance()->send(APIBuildingOpen::create(this, this->getParent(), IsoMap::onBuildingUpdated));
//
//}

string Building::getBuildingStatus() {
    
    return pbVo.status;
    
}

void Building::setBuildingStatus(string status) {
    
    pbVo.status = status;
    
}

void Building::runUpgradeFinishEffect() {
    
    g_gameAudio->playEffect(AUDIO_MAGIC_STAR);
    
    m_finishAnimation = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, "upgrade_finish");
    //m_finishAnimation->preferenceRootParent = preferenceRootParent;
    m_finishAnimation->setContentSize(CCSize(bVo.src_width, bVo.src_height));
    if (targetDevice != TD_IPAD_RETINA) {
        m_finishAnimation->setScale(0.5f);
    }
    m_finishAnimation->setAnimation("idle", false);
    m_spineLayer->addChild(m_finishAnimation, 5);
    
    m_finishAnimation->stopAllActions();
    m_finishAnimation->runAction(CCSequence::create(CCDelayTime::create(3.0f),
                                                    CCCallFunc::create(this, callfunc_selector(Building::nullUpgradeFinishEffect)),
                                                    CCRemoveSelf::create(),
                                                    NULL));
    
    stopAllActions();
    runAction(CCSequence::create(CCScaleTo::create(0.1f, 0.8f),
                                 CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f), 0.5f),
                                 NULL));
    
}

void Building::nullUpgradeFinishEffect() {
    
    m_finishAnimation = NULL;
    
}

void Building::cleanEffect() {
    
    if (m_finishAnimation != NULL ) {
        m_finishAnimation->removeFromParentAndCleanup(true);
        m_finishAnimation = NULL;
    }
    
}

void Building::hideProgressBarAnimated(float duration_) {
    
    if (m_progress_bar) {
        if (m_progress_bar->getTag() == 1) {
            m_progress_bar->setTag(0);
            CCModalController::hideAllChildrensAnimated(m_progress_bar, duration_);
        }
    }
    
    if (m_energy_bar) {
        if (m_energy_bar->getTag() == 1) {
            m_energy_bar->setTag(0);
            CCModalController::hideAllChildrensAnimated(m_energy_bar, duration_);
        }
    }
    
}

void Building::showProgressBarAnimated(float duration_) {
    
    if (m_progress_bar) {
        if (m_progress_bar->getTag() == 0) {
            m_progress_bar->setTag(1);
            CCModalController::showAllChildrensAnimated(m_progress_bar, duration_);
        }
    }
    
    if (m_energy_bar) {
        if (m_energy_bar->getTag() == 0) {
            m_energy_bar->setTag(1);
            CCModalController::showAllChildrensAnimated(m_energy_bar, duration_);
        }
    }
    
}

#pragma mark Habitat functions

bool Building::addPetToBuilding(Pet *pet) {//Falling to Habitat
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    if (pbVo.type != kBuildingHabitat &&
        pbVo.type != kBuildingHabitatPremium)
        return false;
    
    if (this->pet &&
        this->pet != pet)
        return false;
    
    pet->removeFromParent();
    if (this->pet) {
        this->pet->removeFromParent();
    }
    
    pbVo.puid = pet->ppVo.puid;
    pet->ppVo.located_in = pbVo.buid;
    
    if (this->pet == pet) {
        this->pet = pet;
        this->pet->preferenceRootParent = preferenceRootParent;
        m_spineLayer->addChild(this->pet, 2);

        this->pet->reload(true);
        this->pet->randomIdleAction();
        this->pet->scheduleUpdate();
        return true;
    }
    
    this->pet = pet;
    this->pet->preferenceRootParent = preferenceRootParent;
    m_spineLayer->addChild(this->pet, 2);

    if (SceneVille *_scene = (SceneVille*)g_currentScene) {
        if (this->pet->emotionHungry) {
            this->pet->emotionHungry->setVisible(false);
        }
        
        CCPoint petPosition = CCPointZero;
        CCPoint positionOnScreen = m_spineLayer->convertToWorldSpace(petPosition);
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        float remainingHeight = MAX(screenSize.height * 0.5f, screenSize.height - positionOnScreen.y);
        CCPoint startPosition = ccp(0, remainingHeight/_scene->scrollView->getMinimumScale()*1.1f);
        this->pet->setPosition(startPosition);
        
        CCSprite *lightA = CCSprite::create("common/vfx/light_fx_a.png");
        lightA->setScale(sizeMultiplier);
        lightA->setAnchorPoint(ccp(0.5f, sizeMultiplier*0.1f));
        lightA->setPosition(ccp(0, -lightA->getContentSize().height*lightA->getScale()*0.1f));
        this->pet->addChild(lightA, 0);
        
        CCSprite *lightB = CCSprite::create("common/vfx/light_fx_b.png");
        lightB->setScale(sizeMultiplier);
        lightB->setPosition(ccp(0, 0));
        lightB->setOpacity(0);
        this->pet->addChild(lightB, 0);
        
        float zoomDuration = 1.0f;
        float delayDuration = zoomDuration * 0.15f;
        
        _scene->scrollView->zoomOnPosition(this->getPosition()
                                            -_scene->isoMap->getContentSize()/4.f
                                            +ccp(0, this->bVo.src_height*sizeMultiplier*0.5f), //position
                                            1.0f, //scale
                                            zoomDuration //duration
                                            );
        
        CCSize scalePetSize = CCSizeMake(this->pet->getScaleX(), this->pet->getScaleY());
        
        lightA->runAction(CCSequence::create(CCDelayTime::create(delayDuration),
                                             CCScaleTo::create(0.5f, lightA->getScaleX(), lightA->getScaleY()*1.3f),
                                             CCRemoveSelf::create(),
                                             NULL));
        
        this->pet->runAction(CCSequence::create(CCDelayTime::create(delayDuration),
                                          CCMoveTo::create(0.5f, CCPointZero),
                                          NULL));
        
        lightB->runAction(CCSequence::create(CCDelayTime::create(0.5f +delayDuration),
                                             CCScaleTo::create(0.5f, 1),
                                             CCRemoveSelf::create(),
                                             NULL));
        
        lightB->runAction(CCSequence::create(CCDelayTime::create(0.5f +delayDuration),
                                             CCFadeIn::create(0),
                                             CCFadeOut::create(0.5f),
                                             NULL));
        
        addPetAnimationRunningWithSmallDelay = true;
        
        this->pet->runAction(CCSequence::create(CCDelayTime::create(delayDuration),
                                          CCScaleTo::create(0.5f, scalePetSize.width, scalePetSize.height*1.5f),
                                          CCCallFunc::create(this, callfunc_selector(Building::playDropAnimalSound)),
                                          CCScaleTo::create(0.1f, scalePetSize.width*1.5f, scalePetSize.height*0.5f),
                                          CCScaleTo::create(0.1f, scalePetSize.width, scalePetSize.height),
                                          CCCallFunc::create(this, callfunc_selector(Building::addPetToBuildingAnimationDidFinish)),
                                          CCDelayTime::create(0.75f),
                                          CCCallFunc::create(this, callfunc_selector(Building::addPetToBuildingAnimationDidFinishWithSmallDelay)),
                                          NULL));
        
        g_gameAudio->playEffect(AUDIO_ANIMAL_PUSH);
        
    } else {
        this->pet->reload(true);
        this->pet->randomIdleAction();
        this->pet->scheduleUpdate();
    }
    
    if (_playerState->player.racing.host == "") {
        _playerState->player.racing.host = this->pet->ppVo.puid;
        setPetAsHost(true);
    }
    
    setInitialCoins();
    
    return true;

}


void Building::setPetAsHost(bool host_) {
    
    if (bVo.building_type == BUILDING_TYPE_HABITAT && this->pet) {
        
        m_bIsHost = host_;
        CREATE_CHAR_BUFFER(_buffer, 64);
        
        if (m_bIsHost) {
            
            spineBuilding->removeFromParentAndCleanup(true);
            snprintf(_buffer, _buffer_size, "%s_premium", bVo.src.c_str());
            spineBuilding = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, _buffer);
            //spineBuilding->preferenceRootParent = preferenceRootParent;
            spineBuilding->setContentSize(CCSize(bVo.src_width, bVo.src_height));
            if (targetDevice != TD_IPAD_RETINA) {
                spineBuilding->setScale(0.5f);
            }
            spineBuilding->setAnimation("flag", true);
            m_spineLayer->addChild(spineBuilding, 1);
            m_spineLayer->setContentSize(spineBuilding->getContentSize());
            
            if (spineBuildingFront) {
                spineBuildingFront->removeFromParentAndCleanup(true);
                spineBuildingFront = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, _buffer);
                //spineBuildingFront->preferenceRootParent = preferenceRootParent;
                spineBuildingFront->setContentSize(CCSize(bVo.src_width, bVo.src_height));
                if (targetDevice != TD_IPAD_RETINA) {
                    spineBuildingFront->setScale(0.5f);
                }
                spineBuildingFront->setAnimation("flag", true);
                //spineBuildingFront->setSpecifiedBoneToRender(kBuildingSpecificFront);
                m_spineLayer->addChild(spineBuildingFront, 3);
            }
            
        }else {
            
            spineBuilding->removeFromParentAndCleanup(true);
            spineBuilding = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, bVo.src.c_str());
            //spineBuilding->preferenceRootParent = preferenceRootParent;
            spineBuilding->setContentSize(CCSize(bVo.src_width, bVo.src_height));
            if (targetDevice != TD_IPAD_RETINA) {
                spineBuilding->setScale(0.5f);
            }
            spineBuilding->setAnimation("idle", false);
            m_spineLayer->addChild(spineBuilding, 1);
            m_spineLayer->setContentSize(spineBuilding->getContentSize());
            
            if (spineBuildingFront) {
                spineBuildingFront->removeFromParentAndCleanup(true);
                spineBuildingFront = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, bVo.src.c_str());
                //spineBuildingFront->preferenceRootParent = preferenceRootParent;
                spineBuildingFront->setContentSize(CCSize(bVo.src_width, bVo.src_height));
                if (targetDevice != TD_IPAD_RETINA) {
                    spineBuildingFront->setScale(0.5f);
                }
                spineBuildingFront->setAnimation("idle", false);
                //spineBuildingFront->setSpecifiedBoneToRender(kBuildingSpecificFront);
                m_spineLayer->addChild(spineBuildingFront, 3);
            }
            
        }
        
    }
    
}

//Fly up to top of the screen
void Building::removePetFromBuilding() {
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    pbVo.puid = "";

    if (_playerState->player.racing.host == this->pet->ppVo.puid) {
        _playerState->player.racing.host = "";
        setPetAsHost(false);
    }
    
    if (!this->pet) return;
    
    if (SceneVille *_scene = (SceneVille*)g_currentScene) {
        
        if (this->pet->emotionHungry) {
            this->pet->emotionHungry->setVisible(false);
        }
        
        CCPoint petPosition = CCPointZero;
        CCPoint positionOnScreen = m_spineLayer->convertToWorldSpace(petPosition);
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        float remainingHeight = screenSize.height - positionOnScreen.y;
        CCPoint endPosition = ccp(0, remainingHeight/_scene->scrollView->getMinimumScale()*1.1f);
        this->pet->setPosition(CCPointZero);
        
        CCSprite *lightA = CCSprite::create("common/vfx/light_fx_a.png");
        lightA->setScale(sizeMultiplier);
        lightA->setAnchorPoint(ccp(0.5f, 1.0f));
        lightA->setPosition(ccp(0, lightA->getContentSize().height*lightA->getScale()*0.1f));
        lightA->setFlipY(true);
        lightA->setScaleY(sizeMultiplier*0.1f);
        this->pet->addChild(lightA, 0);
        
        CCSprite *lightB = CCSprite::create("common/vfx/light_fx_b.png");
        lightB->setScale(sizeMultiplier);
        lightB->setPosition(ccp(0, 0));
        this->pet->addChild(lightB, 0);
        
        CCSize scalePetSize = CCSizeMake(this->pet->getScaleX(), this->pet->getScaleY());
        
        lightB->runAction(CCSequence::create(CCScaleTo::create(0.5f, 0),
                                             CCRemoveSelf::create(),
                                             NULL));
        
        this->pet->runAction(CCSequence::create(CCScaleTo::create(0.1f, scalePetSize.width*1.5f, scalePetSize.height*0.5f),
                                          CCScaleTo::create(0.1f, scalePetSize.width, scalePetSize.height*1.5f),
                                          CCScaleTo::create(0.5f, scalePetSize.width, scalePetSize.height),
                                          NULL));
        
        lightA->runAction(CCSequence::create(CCDelayTime::create(0.2f),
                                             CCScaleTo::create(0.2f, sizeMultiplier, sizeMultiplier*1.3f),
                                             CCRemoveSelf::create(),
                                             NULL));
        
        this->pet->runAction(CCSequence::create(CCDelayTime::create(0.2f),
                                          CCMoveTo::create(0.5f, endPosition),
                                          CCDelayTime::create(0.1f),
                                          CCCallFunc::create(this, callfunc_selector(Building::removePetFromBuildingAnimationDidFinish)),
                                          NULL));
        
        g_gameAudio->playEffect(AUDIO_ANIMAL_PUSH);
    
        return;
        
    }
    
    this->pet->ppVo.located_in = "";
    this->pet->removeFromParent();
    this->pet = NULL;
    
}

void Building::playDropAnimalSound()
{
    g_gameAudio->playEffect(AUDIO_DROP_ANIMAL_TO_BUILDING);
}

void Building::addPetToBuildingAnimationDidFinish() {
    
    this->pet->reload(true);
    this->pet->randomIdleAction();
    this->pet->scheduleUpdate();
    if (this->pet->emotionHungry) {
        this->pet->emotionHungry->setVisible(true);
    }
    
}

void Building::addPetToBuildingAnimationDidFinishWithSmallDelay()
{
    addPetAnimationRunningWithSmallDelay = false;
}

void Building::removePetFromBuildingAnimationDidFinish()
{
    
    Building *building = PlayerState::getInstance()->getBuilding(kBuildingAnimal, 0);
    
    if (SceneVille* _scene = (SceneVille*)g_currentScene) {
        _scene->scrollView->zoomOnPosition(building->getPosition()
                                             -_scene->isoMap->getContentSize()/4.f
                                             +ccp(0,
                                                  building->bVo.src_height*sizeMultiplier*0.5f), 1.3f,
                                             1.0f);
    }
    
    std::vector<GDPetVO> pets;
    pets.push_back(this->pet->pVo);
    building->addAnimalToBarn(pets);
    pets.clear();
    
    this->pet->ppVo.located_in = "";
    this->pet->removeFromParent();
    if (this->pet->emotionHungry) {
        this->pet->emotionHungry->setVisible(true);
    }
    this->pet = NULL;
    
    setInitialCoins();
    
}
    
bool Building::isPetContained() {
    
    return (this->pet != NULL &&
            pbVo.puid != "");
    
}

#pragma mark FoodProduction functions

void Building::setInitialProduct() {
    
    if (bVo.building_type == BUILDING_TYPE_PRODUCER) {
        setInitialProducedFood();
#if FEATURE_COINS_FROM_PET
    }else if (bVo.building_type == BUILDING_TYPE_TICKETBOOTH ||
              bVo.building_type == BUILDING_TYPE_HABITAT) {
#else 
    }else if (bVo.building_type == BUILDING_TYPE_TICKETBOOTH) {
#endif
        setInitialCoins();
    }else if (bVo.building_type == BUILDING_TYPE_ASSOCIATION) {
        PlayerState::getInstance()->calculateTrackpointsUsed();
    }
    
}

void Building::setInitialProducedFood() {
    
    m_resourceProduced = (currentTime - pbVo.timestamp) *  bVo.param_2; // param2 = unit produced per second
    
    if (m_resourceProduced > bVo.param_1) { //param1 = max resource produced
        m_resourceProduced = bVo.param_1;
    }
    
}

void Building::updateFoodProduction() {
    
    m_resourceProduced += bVo.param_2;
    
    if (m_resourceProduced >= getAmountToShowFoodIcon()) {
    
        if (m_resourceProduced >= bVo.param_1) {
            m_resourceProduced = bVo.param_1;
            _showCollectingBubble(true);
        }else {
            _showCollectingBubble(false);
        }
        
    } else {
        
        hideCollectingBubble();
        
    }
    
    //CCLog_("%f", m_resourceProduced);
    
}

float Building::getAmountEnoughToCollectFood() {
    
    return kBuldingMinimumProducedAmountToCollect;
    
}

float Building::getAmountToShowFoodIcon() {
    
    return bVo.param_3;
    
}





float Building::getProductRate() {
    
    GDBuildingVO _bvo_max = bVo;
    GDBuildingVO _bvo_tmp = bVo;
    int i = 0;
    while (_bvo_tmp.type != "" && i < 5) {
        _bvo_max = _bvo_tmp;
        _bvo_tmp = g_gameContent->getBuildingVO(_bvo_tmp.type, _bvo_tmp.level+1);
        i++;
    }
    
    return bVo.param_2/_bvo_max.param_2;
    
}

float Building::getNextProductRate() {
    
    GDBuildingVO _svo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level);
    GDBuildingVO _bvo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level+1);
    if (_svo.type == "" || _bvo.type == "") {
        CCLog_("(Building::getNextProductRate) Error: building=%s level=%d doesn't exist!", pbVo.type.c_str(), pbVo.level+1);
        return 0.0f;
    }
    
    GDBuildingVO _bvo_max = _svo;
    GDBuildingVO _bvo_tmp = _svo;
    int i = 0;
    while (_bvo_tmp.type != "" && i < 5) {
        _bvo_max = _bvo_tmp;
        _bvo_tmp = g_gameContent->getBuildingVO(_bvo_tmp.type, _bvo_tmp.level+1);
        i++;
    }
    
    return _bvo.param_2/_bvo_max.param_2;
    
}

float Building::getFoodCapacityRate() {
    
    GDBuildingVO _bvo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level);
    if (_bvo.type == "") {
        CCLog_("(Building::getFoodCapacityRate) Error: building=%s doesn't exist!", pbVo.type.c_str());
        return -1;
    }
    
    GDBuildingVO _bvo_max = _bvo;
    GDBuildingVO _bvo_tmp = _bvo;
    int i = 0;
    while (_bvo_tmp.type != "" && i < 5) {
        _bvo_max = _bvo_tmp;
        _bvo_tmp = g_gameContent->getBuildingVO(_bvo_tmp.type, _bvo_tmp.level+1);
        i++;
    }
    
    return _bvo.param_1/_bvo_max.param_1;
    
}

float Building::getNextFoodCapacityRate() {
    
    GDBuildingVO _svo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level);
    GDBuildingVO _bvo = g_gameContent->getBuildingVO(pbVo.type, pbVo.level+1);
    if (_svo.type == "" || _bvo.type == "") {
        CCLog_("(Building::getFoodCapacityRate) Error: building=%s level=%d doesn't exist!", pbVo.type.c_str(), pbVo.level);
        return -1;
    }
    
    GDBuildingVO _bvo_max = _svo;
    GDBuildingVO _bvo_tmp = _svo;
    int i = 0;
    while (_bvo_tmp.type != "" && i < 5) {
        _bvo_max = _bvo_tmp;
        _bvo_tmp = g_gameContent->getBuildingVO(_bvo_tmp.type, _bvo_tmp.level+1);
        i++;
    }
    
    return _bvo.param_1/_bvo_max.param_1;
    
}

bool Building::_showProgressBar() {
    
    if (m_active) {
        if (!m_progress_bar) {
            m_progress_bar = ProgressBar::create(this);
            addChild(m_progress_bar, 5);
        } else {
            m_progress_bar->startUpdate();
        }
        m_progress_bar->setPosition(ccp(0.0f, bVo.src_height*retinaMultiplier*0.6f));
        if (m_energy_bar) {
            m_energy_bar->setPosition(ccp(0.0f, bVo.src_height*retinaMultiplier*0.4f));
        }
        return true;
    }
    
    _hideProgressBar();
    return false;
    
}

void Building::_hideProgressBar() {

    if (m_progress_bar) {
        m_progress_bar->removeFromParentAndCleanup(true);
        m_progress_bar = NULL;
    }

    if (m_energy_bar) {
        m_energy_bar->setPosition(ccp(0.0f, bVo.src_height*retinaMultiplier*0.6f));
    }
    
}

bool Building::_showEnergyBar() {
    
    if (m_active) {
        
        if (!m_energy_bar) {
            m_energy_bar = ProgressBar::create(this->pet);
            addChild(m_energy_bar, 5);
        }else {
           
        }
        m_energy_bar->update();
    
        if (m_progress_bar) {
            m_energy_bar->setPositionY(bVo.src_height*retinaMultiplier*0.4f);
        }else {
            m_energy_bar->setPositionY(bVo.src_height*retinaMultiplier*0.6f);
        }
        
    }else {
        _hideProgressBar();
    }
    
    if (this->pet) {
        this->pet->hideHungry();
    }
    
    return false;
    
}

void Building::_hideEnergyBar() {
    
    if (m_energy_bar) {
        m_energy_bar->removeFromParentAndCleanup(true);
        m_energy_bar = NULL;
    }
    
}

bool Building::_showAbilityBar(void* ability_) {
    
    if (m_active) {
        
        if (!m_ability_bar) {
            m_ability_bar = ProgressBar::create(ability_);
            addChild(m_ability_bar, 5);
        }
//        else {
//            m_ability_bar->startUpdate();
//        }
        m_ability_bar->update();
        
        if (m_progress_bar) {
            m_ability_bar->setPositionY(bVo.src_height*retinaMultiplier*0.4f);
        }else {
            m_ability_bar->setPositionY(bVo.src_height*retinaMultiplier*0.6f);
        }
        
    }else {
        _hideAbilityBar();
    }
    
    return false;
    
}

void Building::_hideAbilityBar() {
    
    if (m_ability_bar) {
        m_ability_bar->removeFromParentAndCleanup(true);
        m_ability_bar = NULL;
    }
    
}


bool Building::_showCollectingBubble(bool max_) {

    PlayerState *_playerState = PlayerState::getInstance();
    
    if (m_active) {
        
        if (bVo.building_type == BUILDING_TYPE_PRODUCER) {
            if (_playerState->player.balance.food >= _playerState->getTotalFoodStorageCapacity() && didTryToCollect) {
                return false;
            }
        }else {
            if (_playerState->player.balance.gold >= _playerState->getTotalCoinStorageCapacity() && didTryToCollect) {
                return false;
            }
        }
        
        didTryToCollect = false;
        
        if (!m_collectingBubble) {
            
            m_collectingBubble = CCSprite::createWithSpriteFrameName("_effect_glow.png");
            //m_collectingBubble->preferenceRootParent = preferenceRootParent;
            if (bVo.building_type == BUILDING_TYPE_HABITAT) {
                m_collectingBubble->setPositionY(bVo.src_height*sizeMultiplier*0.55f);
            } else {
                m_collectingBubble->setPositionY(bVo.src_height*sizeMultiplier*0.75f);
            }
            m_collectingBubble->stopAllActions();
            m_collectingBubble->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(3.0f, 0.7f),
                                                                                     CCScaleTo::create(3.0f, 0.9f), NULL)));
            addChild(m_collectingBubble, 0);
        }
        
        if (!product && getParent()) {
            
            if (bVo.building_type == BUILDING_TYPE_PRODUCER) {
                product = CCPhatTouchableSprite::createWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::HUDIconFoodNoShadow)), HUD_MENU_TOUCH_PRIORITY+1);
            }else {
                product = CCPhatTouchableSprite::createWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::HUDIconCoinNoShadow)), HUD_MENU_TOUCH_PRIORITY+1);
            }
            addChild(product);
            
            
            product->setPosition(m_collectingBubble->getPosition());
            product->onHit = Building::onResourceTouched;
            product->setSender(this);
            
        }
        
        if (max_) {
            if (product) {
                if (product->numberOfRunningActions() == 0) {
                    m_collectingAttention = CCSequence::create(CCScaleTo::create(0.1f, 1.1f),
                                                               CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                                               NULL);
                    product->runAction(m_collectingAttention);
                }
            }
        }
        
        if (getParent()) {
            if (getParent()->getParent()) {
                product->rootNode = getParent()->getParent();
            }
        }
        
        
        return true;
        
    }
    
    return false;
    
}

int Building::getSecondsLeftToFinish() {
    
    if (pbVo.status != BUILDING_STATUS_UPGRADE &&
        pbVo.status != BUILDING_STATUS_CONSTRUCT) {
        return 0;
    }
    
    return m_countDown;
    
}

float Building::getPercentToFinish(GDBuildingVO nxt_bvo_) {
    
    float _percent = (1.0f - (float(m_countDown)/(float)nxt_bvo_.time))*100.0f;
    if (_percent > 100.0f) _percent = 100.0f;
    if (_percent < 0.0f) _percent = 0.0f;
    return _percent;
    
}

#pragma mark Ticketbooth funtions

void Building::setInitialCoins() {
    
    if (bVo.building_type == BUILDING_TYPE_TICKETBOOTH) {
        //m_params[0] is visitor per hour
        m_params[0] = PlayerState::getInstance()->getVisitorsPerHour();
        
        //bVo.param_1 is ticket price
        //m_params[1] is coin earned per hour
        m_params[1] = (m_params[0]*bVo.param_1);
        
        //m_params[2] is unit produced per second
        m_params[2] = m_params[1]/3600;
        
        m_resourceProduced = (currentTime - pbVo.timestamp) *  m_params[2];
        
        if (m_resourceProduced > bVo.param_2) { //param1 = max resource produced
            m_resourceProduced = bVo.param_2;
        }
        
    }
    
#if FEATURE_COINS_FROM_PET
    else if (bVo.building_type == BUILDING_TYPE_HABITAT) {

        if (this->pet) {

            float _coins_produced_per_second = this->pet->getCoinsProducedPerSecond();;
            float _coins_produced_capacity = this->pet->getCoinsProducedCapacity();
            float _delta_in_second = (currentTime - this->pet->ppVo.coins_timestamp);
            
            m_resourceProduced = _delta_in_second * _coins_produced_per_second;
            if (m_resourceProduced > _coins_produced_capacity) {
                m_resourceProduced = _coins_produced_capacity;
            }
            
        }else {
            m_resourceProduced = 0;
        }
        
    }
#endif
    
}

void Building::updateCoinEarning() {
    
    if (bVo.building_type == BUILDING_TYPE_TICKETBOOTH) {
        
        m_resourceProduced += m_params[2];
        if (m_resourceProduced >= getAmountToShowCoinIcon())
        {
            if (m_resourceProduced >= bVo.param_2)
            {
                 m_resourceProduced = bVo.param_2;
                 _showCollectingBubble(true);
            }
            else
            {
                _showCollectingBubble(false);
            }
        }
        else
        {
            hideCollectingBubble();
        }
    }
    
#if FEATURE_COINS_FROM_PET
    else if (bVo.building_type == BUILDING_TYPE_HABITAT)
    {
        bool _hide_collecting_bubble = true;
        bool pet_arrived_habitat = (!addPetAnimationRunningWithSmallDelay);
        
        if (this->pet && pet_arrived_habitat)
        {
            if (!m_energy_bar && !this->pet->emotionHungry)
            {
                float _coins_produced_per_second = this->pet->getCoinsProducedPerSecond();
                float _coins_produced_capacity = this->pet->getCoinsProducedCapacity();
                m_resourceProduced += _coins_produced_per_second;

                if (m_resourceProduced > 1)
                {
                    float _delta_in_second = m_resourceProduced / _coins_produced_per_second;
                    if (_delta_in_second >= g_gameContent->GameConfig["N_SECOND_TO_SHOW_COIN_ICON"])
                    {
                        if (m_resourceProduced >= _coins_produced_capacity)
                        {
                             m_resourceProduced = _coins_produced_capacity;
                            _showCollectingBubble(true);
                        }
                        else
                        {
                            _showCollectingBubble(false);
                        }
                        _hide_collecting_bubble = false;
                    }
                }
            }
        }
        if(_hide_collecting_bubble)
        {
            hideCollectingBubble();
        }
    }
#endif
    
}

float Building::getAmountEnoughToCollectCoin() {
    
    return kBuldingMinimumProducedAmountToCollect;//bVo.param_1;
    
}

float Building::getAmountToShowCoinIcon() {
    
    float _result = bVo.param_2*g_gameContent->GameConfig["SHOW_COIN_ICON_AT_RATIO"];
    if (_result < bVo.param_1) {
        _result = bVo.param_1;
    }
    return _result;
    
}

#pragma mark Common Functions

void Building::setActive(bool active_) {
    
    m_active = active_;
    
}

void Building::hideCollectingBubble() {
    
    if (product) {
        product->removeFromParentAndCleanup(true);
        product = NULL;
    }
    
    if (m_collectingBubble) {
        m_collectingBubble->removeFromParentAndCleanup(true);
        m_collectingBubble = NULL;
    }
    
}

float Building::getResourceProduced() {
    
    return m_resourceProduced;
    
}
    
void Building::deductResourceProduced(float collect_amount_) {
    
    m_resourceProduced -= collect_amount_;
    if (m_resourceProduced < 0.0f) {
        m_resourceProduced = 0.0f;
    }
    
}

void Building::_checkAndCreateRetrievingItemLayer() {
    
    if (m_retrivingItemLayer) return;
    
    m_retrivingItemLayer = CCLayer::create();
    m_retrivingItemLayer->setContentSize(getContentSize());
    addChild(m_retrivingItemLayer, kBuildingRetrivingItemLayerZOrder);
    
}

void Building::_checkAndDeleteRetrievingItemLayer(bool force_delete_) {
    
    if (!m_retrivingItemLayer) return;
    
    if ((m_gotItems.size() == 0 && m_gotPets.size() == 0) ||
        force_delete_) {
        m_retrivingItemLayer->removeFromParent();
        m_retrivingItemLayer = NULL;
    }
    
}

void Building::addAnimalToBarn(std::vector<GDPetVO> gotPets)
{
    m_gotPets = gotPets;
    animateAddAnimalToBarnIndex = 0;

    animateAnimalToBarn();
}

void Building::animateAnimalToBarn()
{
    if (animateAddAnimalToBarnIndex >= m_gotPets.size()) {
        m_gotPets.clear();
        if (g_sceneManager->sceneSelector == SCENE_VILLE) {
            if (SceneVille* _sceneVille = (SceneVille*)g_currentScene) {
                _sceneVille->addMorePetsTutorial();
            }
        }
        return;
    }
    
    GDPetVO petVO = m_gotPets[animateAddAnimalToBarnIndex];
    CCNode *item = CCNode::create();
    item->setAnchorPoint(ccp(0.5f, 0.5f));
    
    CCString *fileName = CCString::createWithFormat("%s.png",petVO.src.c_str());
    CCSprite *petSprite = CCSprite::createWithSpriteFrameName(fileName->getCString());
    petSprite->setScale(0.5f);
    item->setContentSize(petSprite->getContentSize()*0.5f);
    petSprite->setPosition(item->getContentSize()*0.5f);
    
    CCSprite *fxB1 = CCSprite::create("common/vfx/fx_gacha_to_barn_b.png");
    fxB1->setScale(sizeMultiplier*0.5f);
    fxB1->setPosition(item->getContentSize()*0.5f);
    item->addChild(fxB1);
    
    CCSprite *fxB2 = CCSprite::create("common/vfx/fx_gacha_to_barn_b.png");
    fxB2->setScale(sizeMultiplier*0.5f);
    fxB2->setPosition(item->getContentSize()*0.5f);
    
    item->addChild(fxB2);
    
    CCSprite *fxA = CCSprite::create("common/vfx/fx_gacha_to_barn_a.png");
    fxA->setScale(sizeMultiplier*0.5f);
    fxA->setPosition(item->getContentSize()*0.5f);
    item->addChild(fxA);
    
    item->addChild(petSprite);
    
    item->setPosition(ccp(0, bVo.src_height*0.6f*sizeMultiplier));
    item->setScale(0);
    
    _checkAndCreateRetrievingItemLayer();
    m_retrivingItemLayer->addChild(item, m_gotPets.size() - animateAddAnimalToBarnIndex);
    
    fxB1->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, 360)));
    fxB2->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, -360)));
    fxA->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, 360)));
    
    item->runAction(CCScaleTo::create(0.3f, 1));
    item->runAction(CCSequence::create(CCDelayTime::create(1.25f),
                                       CCCallFunc::create(this, callfunc_selector(Building::playSoundAnimateToBarn)),
                                       CCMoveTo::create(0.1f, ccp(0, bVo.src_height*0.7f*sizeMultiplier)),
                                       CCMoveTo::create(0.2f, ccp(0, bVo.src_height*0.2f*sizeMultiplier)),
                                       CCRemoveSelf::create(),
                                       NULL));
    
    item->runAction(CCSequence::create(CCDelayTime::create(1.35f),
                                       CCScaleTo::create(0.01f, 0.7f, 1.2f),
                                       CCScaleTo::create(0.2f, 0.7f*0.5f, 1.2f*0.5f),
                                       NULL));
    
    animateAddAnimalToBarnIndex++;
    
    float scale = this->getScale();
    
    m_retrivingItemLayer->runAction(CCSequence::create(CCDelayTime::create(1.45),
                                       CCScaleTo::create(0.1f, scale*1.2f),
                                       CCScaleTo::create(0.1f, scale),
                                       NULL));
    
    m_retrivingItemLayer->runAction(CCSequence::create(CCDelayTime::create(1.35f),
                                       CCCallFunc::create(this, callfunc_selector(Building::animateAnimalToBarn)),
                                       NULL));
}

void Building::playSoundAnimateToBarn()
{
    g_gameAudio->playEffect(AUDIO_ANIMAL_PUSH);
}


void Building::addItemToBarn(std::vector<GDItemVO> gotItems)
{
    m_gotItems = gotItems;
    animateAddItemToBarnIndex = 0;
    
    animateItemToBarn();
}

void Building::animateItemToBarn()
{
    if (animateAddItemToBarnIndex >= m_gotItems.size()) {
        m_gotItems.clear();
        SceneVille *_sceneVille = (SceneVille*)g_currentScene;
        if (_sceneVille && typeid(*g_currentScene) == typeid(SceneVille)) {
            _sceneVille->hudPanel->checkTrade();
        }
        return;
    }
    
    GDItemVO itemVO = m_gotItems[animateAddItemToBarnIndex];
    CCNode *item = CCNode::create();
    item->setAnchorPoint(ccp(0.5f, 0.5f));
    
    CCString *fileName = CCString::createWithFormat("%s",itemVO.src.c_str());
    CCSprite *petSprite = CCSprite::createWithSpriteFrameName(fileName->getCString());
    petSprite->setScale(0.5f);
    item->setContentSize(petSprite->getContentSize()*0.5f);
    petSprite->setPosition(item->getContentSize()*0.5f);
    
    CCSprite *fxB1 = CCSprite::create("common/vfx/fx_gacha_to_barn_b.png");
    fxB1->setScale(sizeMultiplier*0.5f);
    fxB1->setPosition(item->getContentSize()*0.5f);
    item->addChild(fxB1);
    
    CCSprite *fxB2 = CCSprite::create("common/vfx/fx_gacha_to_barn_b.png");
    fxB2->setScale(sizeMultiplier*0.5f);
    fxB2->setPosition(item->getContentSize()*0.5f);
    
    item->addChild(fxB2);
    
    CCSprite *fxA = CCSprite::create("common/vfx/fx_gacha_to_barn_a.png");
    fxA->setScale(sizeMultiplier*0.5f);
    fxA->setPosition(item->getContentSize()*0.5f);
    item->addChild(fxA);
    item->addChild(petSprite);
    item->setPosition(ccp(0, bVo.src_height*0.6f*sizeMultiplier));
    item->setScale(0);
    
    _checkAndCreateRetrievingItemLayer();
    m_retrivingItemLayer->addChild(item, m_gotItems.size() - animateAddItemToBarnIndex);
    
    fxB1->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, 360)));
    fxB2->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, -360)));
    fxA->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, 360)));
    
    item->runAction(CCScaleTo::create(0.3f, 1));
    item->runAction(CCSequence::create(CCDelayTime::create(1.25f),
                                       CCCallFunc::create(this, callfunc_selector(Building::playSoundAnimateToBarn)),
                                       CCMoveTo::create(0.1f, ccp(0, bVo.src_height*0.7f*sizeMultiplier)),
                                       CCMoveTo::create(0.2f, ccp(0, bVo.src_height*0.2f*sizeMultiplier)),
                                       CCRemoveSelf::create(),
                                       NULL));
    
    item->runAction(CCSequence::create(CCDelayTime::create(1.35f),
                                       CCScaleTo::create(0.01f, 0.7f, 1.2f),
                                       CCScaleTo::create(0.2f, 0.7f*0.5f, 1.2f*0.5f),
                                       NULL));
    
    
    float scale = this->getScale();
    
    m_retrivingItemLayer->runAction(CCSequence::create(CCDelayTime::create(1.45),
                                       CCScaleTo::create(0.f, scale*1.2f),
                                       CCScaleTo::create(0.1f, scale),
                                       NULL));
    
    animateAddItemToBarnIndex++;
    
    m_retrivingItemLayer->runAction(CCSequence::create(CCDelayTime::create(1.35f),
                                       CCCallFunc::create(this, callfunc_selector(Building::animateItemToBarn)),
                                       NULL));
}

void Building::checkPBVOType(float delta_){

    PlayerState *_playerState = PlayerState::getInstance();
    
      if(pbVo.type==kBuildingAnimal) {
          if(_playerState->getNHomelessPet() >= bVo.param_1){
              //show msg at animal barn
              if (!_bubble) {
                   _bubble = CCSprite::createWithSpriteFrameName("oldversion/_bubble_transparent.png");
                  CCLabelBMFont *_gemValue = CCLabelHelper::createBMFont(retinaMultiplier, "Full", kFontSizeExtraLarge, true);
                  _gemValue->setAnchorPoint(ccp(0.5f, 1));
                  _gemValue->setPosition(ccp(_bubble->getContentSize().width*0.5f,_bubble->getContentSize().height*0.9f));
                  
                  _bubble->addChild(_gemValue);
                  _bubble->setPosition(ccp(0.5f, 340*retinaMultiplier));
                  this->addChild(_bubble);
              }
          }else{
              if (_bubble) {
                  _bubble->removeFromParentAndCleanup(true);
                  _bubble = NULL;
              }
          }
          
      }else if(pbVo.type == kBuildingItem) {
          
          if (_playerState->getNItems() >= bVo.param_1) {
              //show msg at animal barn
              if (!_bubble) {
                  _bubble = CCSprite::createWithSpriteFrameName("oldversion/_bubble_transparent.png");
                  CCLabelBMFont *_gemValue = CCLabelHelper::createBMFont(retinaMultiplier, "Full", kFontSizeExtraLarge, true);
                  _gemValue->setAnchorPoint(ccp(0.5f, 1));
                  _gemValue->setPosition(ccp(_bubble->getContentSize().width*0.5f,_bubble->getContentSize().height*0.9f));
                  
                  _bubble->addChild(_gemValue);
                  _bubble->setPosition(ccp(0.5f, 340*retinaMultiplier));
                  this->addChild(_bubble);
              }
          }else{
              if (_bubble) {
                  _bubble->removeFromParentAndCleanup(true);
                  _bubble = NULL;
              }
          }
          
      }else if(pbVo.type==kBuildingHabitat){
         
          if (this->pet) {
              this->pet->updatePet(delta_);
              if (this->pet->ppVo.energy < 100) {
                  _showEnergyBar();
                  showProgressBarAnimated(kEntityActionTransitionSpeed);
              }else {
                  _hideEnergyBar();
              }
              
          }else {
              _hideEnergyBar();
              //        hideProgressBarAnimated(kEntityActionTransitionSpeed);
          }
          
      }
      else if(pbVo.type==kBuildingFusion){
          if(bubbleStatus){
              if (!_bubble_maket) {
                  _bubble_maket = CCSprite::createWithSpriteFrameName("oldversion/_bubble_transparent.png");
                  CCSprite *sprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::HUDCheckmarkBig));
                  sprite->setScale(0.95f);
                  sprite->setAnchorPoint(ccp(0.5f, 1));
                  sprite->setPosition(ccp(_bubble_maket->getContentSize().width*0.5f,_bubble_maket->getContentSize().height*0.9f));
                  
                  _bubble_maket->addChild(sprite);
                  _bubble_maket->setPosition(ccp(0.5f, 340*retinaMultiplier));
                  this->addChild(_bubble_maket);
              }
          }else{
              //remove bubble
              if (_bubble_maket) {
                  _bubble_maket->removeFromParentAndCleanup(true);
                  _bubble_maket = NULL;
              }
          }
          
      }
 }

void Building::showBubbleTrade() {
    
      bubbleStatus=true;
    
}

void Building::hideBubbleTrade() {
    
     bubbleStatus=false;
    
}
    
void Building::entityActionStart(void *entityAction) {
    
    if (!m_active)
        return;
    
    if (m_energy_bar) {
        m_energy_bar->setVisible(false);
    }
    if (m_ability_bar) {
        m_ability_bar->setVisible(false);
    }
    if (m_progress_bar) {
        m_progress_bar->setVisible(false);
    }
}

void Building::entityActionFinish(void *entityAction) {
    
    if (!m_active)
        return;
    
    float delay = 0.115f +0.04f;
    
    if (m_energy_bar) {
        m_energy_bar->runAction(CCSequence::create(CCDelayTime::create(delay),
                                                   CCShow::create(),
                                                   NULL));
        
    }
    if (m_ability_bar) {
        m_ability_bar->runAction(CCSequence::create(CCDelayTime::create(delay),
                                                    CCShow::create(),
                                                    NULL));
    }
    if (m_progress_bar) {
        m_progress_bar->runAction(CCSequence::create(CCDelayTime::create(delay),
                                                     CCShow::create(),
                                                     NULL));
    }
}

void Building::_updateUpgradableSign() {

    PlayerState *_playerState = PlayerState::getInstance();
    bool _show = true;
    
    // Don't show upgrade sign for Habitat as it is not upgradable building
    if (BUILD_HABITAT_WITH_GEMS && bVo.building_type == BUILDING_TYPE_HABITAT) {
        _show = false;
    }
    
    //some building is under construction
    if (_playerState->getBuilder()) {
        _show = false;
    }
    
    //building is not ready for upgrade
    if (pbVo.status != BUILDING_STATUS_DELIVERED) {
        _show = false;
    }
    
    //while building(library) is upgrading an ability, building itself couldn't be upgraded
    if (pbVo.type == kBuildingLibrary) {
        if (_playerState->getUpgradingAbility()) {
            _show = false;
        }
    }
    
    //building reached the highest level
    if (pbVo.level >= g_gameContent->getBuildingMaxLevel(bVo.type)) {
        _show = false;
    }else {
    
        //upgrade hasn't unlock just yet
        if (_show) {
            
            GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(bVo.type, pbVo.level+1);
            if (_nxt_bvo.type == "") {
                _show = false;
            }else {
            
                if (bVo.type == kBuildingHQ) {
                    //HQ building upgrade condition regards town level
                    if (_playerState->player.racing.level < _nxt_bvo.unlocks_at_level) {
                        _show = false;
                    }
                    
                }else {
                    
                    if (Building* _building_hq = _playerState->getBuilding(kBuildingHQ, 0)) {
                        if (_building_hq->pbVo.level < _nxt_bvo.unlocks_at_level) {
                            _show = false;
                        }
                    }else {
                        _show = false;
                    }
                }
                
            }
            
        }
        
    }
    
    if (_show) {
        if (!m_upgradableSignSprite) {
            m_upgradableSignSprite = CCSprite::createWithSpriteFrameName("_upgradable_mark.png");
            m_upgradableSignSprite->setPosition(ccp(g_gameContent->tileSize.width*bVo.grid_row*0.25f,-g_gameContent->tileSize.height*bVo.grid_col*0.125f));
            m_upgradableSignSprite->setTag(0);
        }
        if (m_upgradableSignSprite->getTag() == 0) {
            m_upgradableSignSprite->setTag(1);
            addChild(m_upgradableSignSprite);
            m_upgradableSignSprite->setOpacity(0);
            m_upgradableSignSprite->runAction(CCFadeTo::create(1.0f, 255));
        }
    }else { //hide
        if (m_upgradableSignSprite) {
            if (m_upgradableSignSprite->getTag() == 1) {
                m_upgradableSignSprite->setTag(0);
                m_upgradableSignSprite->runAction(CCSequence::create(CCFadeTo::create(1.0f, 0), CCRemoveSelf::create(), NULL));
                m_upgradableSignSprite = NULL;
            }
        }
    }
    
}

