//
//  IsoMapRacing.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/20/56 BE.
//
//

#include "IsoMapRacing.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include <spine/Json.h>

#include "../Entities/AI.h"

#include "../Services/GameContentService.h"
#include "../Extensions/jsonconverter/CCJSONConverter.h"
extern GameContentService *g_gameContent;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

extern PlayerState* g_playerState;
extern PlayerState *g_playerState_restore;
extern PlayerState* g_opponentState;

#include "../Scenes/SceneRace.h"
extern CCScene* g_currentScene;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIHelperPetUse.h"
#include "../Services/APIs/APIRaceUpdate.h"

#include "../Extensions/ParticleSystem/ParticleHeaders.h"

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Entities/ShockwaveStraight.h"

#include "../Extensions/ExplosionParticle.h"

#include "../Helpers/FlurryCpp.h"
#include "../Helpers/AnalyticsHelper.h"
#include "../Helpers/AnalyticsClientWrapper.h"

#include "../Helpers/CustomEventsHelper.h"

#include "../Helpers/FBAppEventsWrapper.h"
#include "../Helpers/AppsFlyerWrapper.h"
#define PET_HABITAT_WIN PET_HABITAT_EAT
#define PET_HABITAT_LOSE PET_HABITAT_HUNGRY
extern float retinaMultiplier;
extern float sizeMultiplier;

class PlayerState;

IsoMapRacing *IsoMapRacing::create(RacePanel *racePanel_) {
    
    if (IsoMapRacing *pRet = new IsoMapRacing()) {
        pRet->racePanel = racePanel_;
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool IsoMapRacing::init() {
    
    if (!IsoMap::initWithPreferenceRootParent(NULL)) {
        return false;
    }
    
    if (!g_opponentState) {
        CCLog_("(IsoMapRacing::init) Error: opponent is uninitialized!");
        return false;
    }
    
    if (!g_opponentState->didParseData()) {
        CCLog_("(IsoMapRacing::init) Error: opponent is empty!");
        return false;
    }
    CCLog_("%d", g_opponentState->playerPet.top_speed);
    CCLog_("%d", g_opponentState->opponentPet.top_speed);

    composeMap(g_opponentState, IMV_RACING, NULL);
    
    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    if (!mapPlayerState) {
        CCLog_("(IsoMapRacing::init) Error: MapPlayerState is uninititalized!\n");
        return false;
    }
    
    m_giveUp = false;
    m_gameIsPaused = false;
    
    m_helperTime = 0.0f;
    m_delayUntilHelperWillBeRemoved = 0.0f;
    
    _timeUpDisplayed = false;
    
    _raceTimeLeft = g_gameContent->GameConfig["RACE_TIME_LIMIT_SECS"];
    _checkpointBonusTime = g_gameContent->GameConfig["CHECKPOINT_BONUS_TIME"];
    _totalLaps = g_gameContent->GameConfig["TOTAL_LAPS"];

    timeScale = 1.f;

#ifdef DEBUG_RUN_FOREVER
    _raceTimeLeft = 10000.0f;
    _totalLaps = DEBUG_RUN_FOREVER;
#endif
    
    //m_tutorialMode (for searching)
    if (!g_playerState->Checklist[TutFirstRace::key()]) {
        _raceTimeLeft = 1000000.0f;
        _totalLaps = 1000000;
    }
    
    g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "wind_spin");
    g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "vfx_stomp");
    g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "hit_by_shockwave");
    g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "smoke_falling");
    
    m_trackCount = analystTrackDirection();
    
    if (Track* track = getTrackAtIndex(_mapPlayerState->player.racing.start)) {
        
//        GDPetVO _pet_player_vo = g_gameContent->getPetVO(mapPlayerState->playerPet.type);
//        m_pet_player = Pet::createWithPetVO(_pet_player_vo, mapPlayerState->playerPet);
//        m_pet_player->setZOrder(2);
//        m_pet_player->isoMapRacing = this;
//        this->addChild(m_pet_player);
//        
//        GDPetVO _pet_opponent_vo = g_gameContent->getPetVO(mapPlayerState->opponentPet.type);
//        m_pet_opponent = Pet::createWithPetVO(_pet_opponent_vo, mapPlayerState->opponentPet);
//        m_pet_opponent->setZOrder(3);
//        m_pet_opponent->isoMapRacing = this;
//        this->addChild(m_pet_opponent);
//        
//        m_pet_player->opponentPet = m_pet_opponent;
//        m_pet_opponent->opponentPet = m_pet_player;
//        
//        if (mapPlayerState->replayRaw != "") {
//            
//            CCDictionary *replayDictionary = CCJSONConverter::sharedConverter()->dictionaryFrom(mapPlayerState->replayRaw.c_str());
//            m_pet_player->replayRecord = (CCDictionary *)replayDictionary->objectForKey("host");
//            m_pet_player->replayRecord->retain();
//            m_pet_opponent->replayRecord = (CCDictionary *)replayDictionary->objectForKey("guest");
//            m_pet_opponent->replayRecord->retain();
//            m_pet_player->is_replay = true;
//            m_pet_opponent->is_replay = true;
//            is_replay = true;
//            CCLog_("%s",mapPlayerState->replayRaw.c_str());
//            m_pet_player->prepareRunning(this, track, mapPlayerState->player.racing.direction, 1);
//            m_pet_opponent->prepareRunning(this, track, mapPlayerState->player.racing.direction, 0);
//            
//            CCSequence *sequence = CCSequence::create(CCDelayTime::create(2),CCCallFunc::create(this, callfunc_selector(IsoMapRacing::playReplay)), NULL);
//            this->runAction(sequence);
//        } else {
//            m_pet_player->prepareRunning(this, track, mapPlayerState->player.racing.direction, 0);
//            m_pet_opponent->prepareRunning(this, track, mapPlayerState->player.racing.direction, 1);
//        }
        
//        if (g_gameContent->userRecord) {
//            GDPetVO _pet_opponent_vo = g_gameContent->getPetVO(_mapPlayerState->opponentPet.type);
//            m_pet_player = Pet::createWithPetVO(_pet_opponent_vo, _mapPlayerState->opponentPet);
//            m_pet_player->m_trackCount = m_trackCount;
//            m_pet_player->setZOrder(2);
//            m_pet_player->isoMapRacing = this;
//            this->addChild(m_pet_player);
//            
//            GDPetVO _pet_player_vo = g_gameContent->getPetVO(_mapPlayerState->playerPet.type);
//            m_pet_opponent = Pet::createWithPetVO(_pet_player_vo, _mapPlayerState->playerPet);
//            m_pet_opponent->m_trackCount = m_trackCount;
//            m_pet_opponent->setZOrder(3);
//            m_pet_opponent->isoMapRacing = this;
//            this->addChild(m_pet_opponent);
//            
//            userReplayRecord = g_gameContent->userRecord;
//            m_pet_player->replayRecord = g_gameContent->playerRecord;
//            m_pet_opponent->replayRecord = g_gameContent->opponentRecord;
//            m_pet_player->is_replay = true;
//            m_pet_opponent->is_replay = true;
//            is_replay = true;
//            
//            m_pet_player->prepareRunning(this, track, _mapPlayerState->player.racing.direction, 1);
//            m_pet_opponent->prepareRunning(this, track, _mapPlayerState->player.racing.direction, 0);
//            CCSequence *sequence = CCSequence::create(CCDelayTime::create(2),CCCallFunc::create(this, callfunc_selector(IsoMapRacing::playReplay)), NULL);
//            this->runAction(sequence);
//        } else {
        
        GDPetVO _pet_player_vo = g_gameContent->getPetVO(_mapPlayerState->playerPet.type);
        m_pet_player = Pet::createWithPetVO(_pet_player_vo, _mapPlayerState->playerPet);
        m_pet_player->m_trackCount = m_trackCount;
        m_pet_player->playerState = g_playerState;
        m_pet_player->initilizeEnergyInformation();
        m_pet_player->setZOrder(2);
        m_pet_player->isoMapRacing = this;
        this->addChild(m_pet_player);
        
        GDPetVO _pet_opponent_vo = g_gameContent->getPetVO(_mapPlayerState->opponentPet.type);
        m_pet_opponent = Pet::createWithPetVO(_pet_opponent_vo, _mapPlayerState->opponentPet);
        m_pet_opponent->m_trackCount = m_trackCount;
        m_pet_opponent->playerState = g_opponentState;
        m_pet_opponent->initilizeEnergyInformation();
        m_pet_opponent->setZOrder(3);
        m_pet_opponent->isoMapRacing = this;
        this->addChild(m_pet_opponent);

        if(_mapPlayerState->replayGuestRaw != "")
        {
            CCLog_("Load Replay Data");
            m_pet_player->enablePlaybackReplayData(true, _mapPlayerState->replayHostRaw.c_str());
            m_pet_opponent->enablePlaybackReplayData(true, _mapPlayerState->replayGuestRaw.c_str());
        }

        m_rankPopoverDisplayTime = RANKCHANGED_POPOVER_DISPLAY_TIME_DIFF_PET;
        if (m_pet_player->ppVo.type == m_pet_opponent->ppVo.type) {
            m_rankPopoverDisplayTime = RANKCHANGED_POPOVER_DISPLAY_TIME_SAME_PET;
        }
        
        userReplayRecord = CCDictionary::create();
        userReplayRecord->retain();
        
        CCArray *spins = CCArray::create();
        userReplayRecord->setObject(spins, "spins");
        
        m_pet_player->prepareRunning(this, track, _mapPlayerState->player.racing.direction, 0);
        m_pet_opponent->prepareRunning(this, track, _mapPlayerState->player.racing.direction, 1);
        
        m_groupSpineEffectsAboveAnimals = CCNode::create();
        this->addChild(m_groupSpineEffectsAboveAnimals, 50);
        m_groupNodeEffects = CCNode::create();
        this->addChild(m_groupNodeEffects, 51);
        m_effectsArray = CCArray::create();
        m_effectsArray->retain();
        m_shockwaveArray = CCArray::create();
        m_shockwaveArray->retain();
        
        m_pet_helper_cache = NULL;
        
        // USE HELPER PET & REMOVE FROM LIST
        g_playerState->HelperVo = PlayerHelperPetVO();
        if (g_playerState->HelperPetsSelectedIndex >= 0 &&
            g_playerState->HelperPetsSelectedIndex < g_playerState->HelperPets.size()) {
            PlayerHelperPetVO _vo = g_playerState->HelperPets[g_playerState->HelperPetsSelectedIndex];
            if (_vo.uuid != "") {
                CCLog_("%s -> %s (%d", _vo.name.c_str(), _vo.pet_type.c_str(), _vo.pet_horse_power);
                for (map<string, int>::iterator it=_vo.abilities.begin(); it != _vo.abilities.end(); ++it) {
                    CCLog_("%s (%d)", (*it).first.c_str(), (*it).second);
                }
#ifndef DEBUG_USE_HELPER_FOREVER
                BackendService::getInstance()->send(APIHelperPetUse::create(_vo.uuid, NULL, NULL));
#endif
            }
            
            g_playerState->HelperVo.copy(_vo);
            
            g_playerState->HelperPets.erase(g_playerState->HelperPets.begin() + g_playerState->HelperPetsSelectedIndex);
            g_playerState->HelperPetsSelectedIndex = -1;

            m_pet_helper_cache = createHelperPet(_vo);
        }
        
        if(m_pet_player->isPlaybackReplayDataEnabled())
        {
            const string& helperPetType = _mapPlayerState->opponentPet.helper_pet;
            if(helperPetType != "")
            {
                m_pet_helper_cache = createHelperPet(helperPetType);
                m_pet_helper_cache->enablePlaybackReplayData(true, _mapPlayerState->replayHelperPet.c_str());
            }
        }

        racePanel->initGetHelp();

//        }
        
        m_pet_player->opponentPet = m_pet_opponent;
        m_pet_opponent->opponentPet = m_pet_player;
        
        m_pet_player->isPlayerPet = true;
        m_pet_opponent->isPlayerPet = false;
        
        
        int trackPoints = 0;
        for (vector<Track*>::iterator it=_mapPlayerState->Tracks.begin(); it!=_mapPlayerState->Tracks.end(); ++it) {
            if (Track* _track = (Track*)(*it)) {
                trackPoints += _track->tVo.point_used;
            }
        }
        
        CCLog_("trackPoints : %d",trackPoints);
        
        //Calculate
        /*
        if (trackPoints < 100) {
            m_pet_player->max_round = 2;
            m_pet_opponent->max_round = 2;
        } else if (trackPoints < 200) {
            m_pet_player->max_round = 3;
            m_pet_opponent->max_round = 3;
        } else if (trackPoints < 300) {
            m_pet_player->max_round = 4;
            m_pet_opponent->max_round = 4;
        } else if (trackPoints < 400) {
            m_pet_player->max_round = 5;
            m_pet_opponent->max_round = 5;
        }
        */
        m_pet_player->max_round = _totalLaps;
        m_pet_opponent->max_round = _totalLaps;
    }
    m_racingState = MRS_PREPARE;
    
    this->addChild(ParticleSystem::sharedSystem(), 100);
    
    float partcleRate = 0.03f;
    float partcleRateVariation = 0.01f;
    
    ParticleSystem::sharedSystem()->removeAllParticles();
    
    //moved this to ParticleSystemClass so it is called only once
//    ParticleSystem::sharedSystem()->addParticleCache(ParticleTypeRunSmoke1, kParticleTypeRunSmoke1Amount);
    
    _emitter = ParticleEmitter::create(EmitterTypeSelfEmitting,
                                       ParticleSystem::sharedSystem()->getParticleCache(ParticleTypeRunSmoke1),
                                       partcleRate,
                                       partcleRateVariation,
                                       kParticleTypeRunSmoke1Amount);
    ParticleSystem::sharedSystem()->addChild(_emitter);
//    _emitter->start();
    
    _emitter2 = ParticleEmitter::create(EmitterTypeSelfEmitting,
                                       ParticleSystem::sharedSystem()->getParticleCache(ParticleTypeRunSmoke1),
                                       partcleRate,
                                       partcleRateVariation,
                                       kParticleTypeRunSmoke1Amount);
    ParticleSystem::sharedSystem()->addChild(_emitter2);
    
    _emitter3 = ParticleEmitter::create(EmitterTypeSelfEmitting,
                                        ParticleSystem::sharedSystem()->getParticleCache(ParticleTypeRunSmoke1),
                                        partcleRate,
                                        partcleRateVariation,
                                        kParticleTypeRunSmoke1Amount);
    ParticleSystem::sharedSystem()->addChild(_emitter3);
    
//    _emitter2->start();
    
//    _effectLayer = cocos2d::CCLayer::create();
//    _effectLayer->setContentSize(this->getContentSize());
//    _effectLayer->setPosition(ccp(0.0f, 0.0f));
//    this->addChild(_effectLayer,100);
    
    
    int amountOfTracks = (int) _mapPlayerState->Tracks.size();
    
    addVisitor(amountOfTracks * kAmountOfTracksToVisitorsRatio);
    
    this->runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                       CCCallFunc::create(this, callfunc_selector(IsoMapRacing::zoom)),
                                       NULL));
    
    this->runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                       CCCallFunc::create(this, callfunc_selector(IsoMapRacing::startUpdate)),
                                       NULL));
    
    
    m_AI_on_opponent = AI::create(this, m_pet_opponent, m_pet_player);
    m_AI_on_opponent->retain();
    
    
    onRaceResultSender = NULL;
    onShowRaceResult = NULL;
    
    m_isRemovingHelperPet = false;
    
    return true;
}

void IsoMapRacing::cleanup()
{
    auto parent = ParticleSystem::sharedSystem()->getParent();
    CCAssert(parent == this, "wrong parent");
    parent->removeChild(ParticleSystem::sharedSystem());
}

void IsoMapRacing::draw()
{
    
}

void IsoMapRacing::startUpdate()
{
    racePanel->checkSkillCost(m_pet_player->remainingCost());
    
    racePanel->rankPopup->animateRank(m_pet_player, RankTypeYou, 10000.0f);
    
//    m_pet_player->showEnergyInfo();
//    m_pet_opponent->showEnergyInfo();
    
    scheduleUpdate();
}

void IsoMapRacing::addAIToPlayer()
{
    m_AI_on_player = AI::create(this, m_pet_player, m_pet_opponent);
    m_AI_on_player->retain();
}

void IsoMapRacing::compose()
{
//    if (is_replay) {
//        racePanel->slotPanel->setPet(m_pet_opponent);
//    } else {
//        racePanel->slotPanel->setPet(m_pet_player);
//    }
}

IsoMapRacing::~IsoMapRacing()
{
    if (userReplayRecord) {
        userReplayRecord->release();
        userReplayRecord = NULL;
    }
    
    if (m_AI_on_opponent) {
        m_AI_on_opponent->release();
        m_AI_on_opponent = NULL;
    }
    
    if (m_AI_on_player) {
        m_AI_on_player->release();
        m_AI_on_player = NULL;
    }
    
    if (m_pet_helper_cache) {
        m_pet_helper_cache->removeFromParentAndCleanup(true);
        m_pet_helper_cache = NULL;
        m_pet_helper = NULL;
    }
    
    if (m_effectsArray) {
        m_effectsArray->release();
        m_effectsArray = NULL;
    }
    
    if (m_shockwaveArray) {
        m_shockwaveArray->release();
        m_shockwaveArray = NULL;
    }
}

void IsoMapRacing::playReplay()
{
    racePanel->hideBtn(0);
    racePanel->beginCountdown();
}


void IsoMapRacing::updateTime() {
    
    int raceTimeLeft = (int)_raceTimeLeft;
    int minutes = raceTimeLeft / 60;
    int seconds = raceTimeLeft % 60;
    const char* minutesString;
    const char* secondsString;
    if (minutes >= 10) {
        minutesString = CCString::createWithFormat("%d",minutes)->getCString();
    } else {
        minutesString = CCString::createWithFormat("0%d",minutes)->getCString();
    }
    if (seconds >= 10) {
        secondsString = CCString::createWithFormat("%d",seconds)->getCString();
    } else {
        secondsString = CCString::createWithFormat("0%d",seconds)->getCString();
    }
    
    racePanel->m_lblTimer->setString(CCString::createWithFormat("%s:%s",minutesString,secondsString)->getCString());
    
}

void IsoMapRacing::addTime(float timeToAdd, const char* title) {
    
    _raceTimeLeft += timeToAdd;
    
    updateTime();
    
    if (!racePanel->m_lblTimer->isVisible()) {
        return;
    }
    
    racePanel->addGlowEffectWithDelay(racePanel->m_lblTimer, 0.45f);
    racePanel->animateParticleStarWithDelay(racePanel->m_lblTimer, 0.45f);
    CCSequence *sequence = CCSequence::create(CCDelayTime::create(0.45f),
                                              CCScaleTo::create(0.15, -1, 1),
                                              CCScaleTo::create(0.15, 1, 1),
                                              CCScaleTo::create(0.15, -1, 1),
                                              CCScaleTo::create(0.15, 1, 1),
                                              NULL);
    
    racePanel->m_lblTimer->runAction(CCEaseOut::create(sequence, 2));
    
//    float timerTitleScaleX = racePanel->m_lblTimerTitle->getScaleX();
//    float timerTitleScaleY = racePanel->m_lblTimerTitle->getScaleY();
//    CCSequence* timerTitleSequence = CCSequence::create(CCShow::create(),
//                                                        CCScaleTo::create(0.133f, timerTitleScaleX, timerTitleScaleY*1.2f),
//                                                        CCScaleTo::create(0.065f, timerTitleScaleX, timerTitleScaleY),
//                                                        CCDelayTime::create(1.5f),
//                                                        CCScaleTo::create(0.032f, timerTitleScaleX, timerTitleScaleY*1.2f),
//                                                        CCScaleTo::create(0.065f, timerTitleScaleX, timerTitleScaleY),
//                                                        CCHide::create(),
//                                                        NULL);

    CCSequence* groupTimeAddedSequence = CCSequence::create(CCShow::create(),
                                                            CCScaleTo::create(0.133f, 1.0f, 1.2f),
                                                            CCScaleTo::create(0.065f, 1.0f, 1.0f),
                                                            CCDelayTime::create(1.5f),
                                                            CCScaleTo::create(0.032f, 1.0f, 1.2f),
                                                            CCScaleTo::create(0.065f, 1.0f, 0.0f),
                                                            CCHide::create(),
                                                            NULL);
    
    //hide until can find another place to put this
//    racePanel->m_lblTimerTitle->setString(title);
//    racePanel->m_lblTimerTitle->stopAllActions();
//    racePanel->m_lblTimerTitle->setScaleY(0.0f);
//    racePanel->m_lblTimerTitle->setVisible(false);
//    racePanel->m_lblTimerTitle->runAction(timerTitleSequence);
    
    racePanel->m_lblTimeAdded->setString(CCString::createWithFormat("+%d",(int)timeToAdd)->getCString());
    racePanel->m_groupTimeAdded->stopAllActions();
    racePanel->m_groupTimeAdded->setScaleY(0.0f);
    racePanel->m_groupTimeAdded->setVisible(false);
    racePanel->m_groupTimeAdded->runAction(groupTimeAddedSequence);
}

void IsoMapRacing::pauseGame()
{
    if (m_gameIsPaused) return;
    
    SceneRace* _scene = (SceneRace*)g_currentScene;
    if (!_scene) return;
    
    m_gameIsPaused = true;
    
    _emitter->stop();
    _emitter2->stop();
    if (m_pet_helper) _emitter3->stop();
    
    m_pet_player->pauseRacing();
    m_pet_opponent->pauseRacing();
    if (m_pet_helper) m_pet_helper->pauseRacing();
    
    
    CCObject *object = NULL;
    CCARRAY_FOREACH(m_groupSpineEffectsAboveAnimals->getChildren(), object)
    {
        CCSkeletonAnimation *spine = (CCSkeletonAnimation *)object;
        spine->timeScale = 0.0f;
    }
    
    CCObject *object4 = NULL;
    CCARRAY_FOREACH(m_groupNodeEffects->getChildren(), object4)
    {
        CCNode *node = (CCNode *)object4;
        node->pauseSchedulerAndActions();
    }
    
    CCObject *object5 = NULL;
    CCARRAY_FOREACH(m_effectsArray, object5)
    {
        CCNode *node = (CCNode *)object5;
        node->pauseSchedulerAndActions();
    }
    
    CCObject *object6 = NULL;
    CCARRAY_FOREACH(m_shockwaveArray, object6)
    {
        CCNode *node = (CCNode *)object6;
        node->pauseSchedulerAndActions();
    }
    
    _scene->scrollView->pauseSchedulerAndActions();
    
    ParticleSystem::sharedSystem()->pauseAllParticles();
    
    
    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    for (vector<Track*>::iterator it=_mapPlayerState->Tracks.begin(); it!=_mapPlayerState->Tracks.end(); ++it) {
        if (Track* _track = (Track*)(*it)) {
            if (_track->m_visitors) {
                if (_track->m_visitors->count() > 0) {
                    CCObject *object = NULL;
                    CCARRAY_FOREACH(_track->m_visitors, object)
                    {
                        Visitor *visitor = (Visitor*)object;
                        visitor->pauseVisitor();
                    }
                }
            }
        }
    }
}

void IsoMapRacing::resumeGame()
{
    if (!m_gameIsPaused) return;
    
    SceneRace* _scene = (SceneRace*)g_currentScene;
    if (!_scene) return;
    
    m_gameIsPaused = false;
    
    _emitter->start();
    _emitter2->start();
    if (m_pet_helper) _emitter3->start();
    
    m_pet_player->resumeRacing();
    m_pet_opponent->resumeRacing();
    if (m_pet_helper) m_pet_helper->resumeRacing();
    
    CCObject *object = NULL;
    CCARRAY_FOREACH(m_groupSpineEffectsAboveAnimals->getChildren(), object)
    {
        CCSkeletonAnimation *spine = (CCSkeletonAnimation *)object;
        spine->timeScale = 1.0f;
    }
    
    CCObject *object4 = NULL;
    CCARRAY_FOREACH(m_groupNodeEffects->getChildren(), object4)
    {
        CCNode *node = (CCNode *)object4;
        node->resumeSchedulerAndActions();
    }
    
    CCObject *object5 = NULL;
    CCARRAY_FOREACH(m_effectsArray, object5)
    {
        CCNode *node = (CCNode *)object5;
        node->resumeSchedulerAndActions();
    }
    
    CCObject *object6 = NULL;
    CCARRAY_FOREACH(m_shockwaveArray, object6)
    {
        CCNode *node = (CCNode *)object6;
        node->resumeSchedulerAndActions();
    }
    
    _scene->scrollView->resumeSchedulerAndActions();
    
    ParticleSystem::sharedSystem()->resumeAllParticles();
    
    
    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    for (vector<Track*>::iterator it=_mapPlayerState->Tracks.begin(); it!=_mapPlayerState->Tracks.end(); ++it) {
        if (Track* _track = (Track*)(*it)) {
            if (_track->m_visitors) {
                if (_track->m_visitors->count() > 0) {
                    CCObject *object = NULL;
                    CCARRAY_FOREACH(_track->m_visitors, object)
                    {
                        Visitor *visitor = (Visitor*)object;
                        visitor->resumeVisitor();
                    }
                }
            }
        }
    }
}

void IsoMapRacing::update(float delta) {
    
    if (m_gameIsPaused) return;
    
    SceneRace* _scene = (SceneRace*)g_currentScene;
    IsoMapRacing *_isoMapRacing = (IsoMapRacing *)_scene->isoMap;

    delta *= getTimeScale();

    m_pet_player->setZOrder(int(IsoMap::getContentSize().height-m_pet_player->getPosition().y));
    m_pet_opponent->setZOrder(int(IsoMap::getContentSize().height-m_pet_opponent->getPosition().y));
    
    if (m_pet_player->m_currPetAction == PET_RUN) {
        _emitter->start();
        _emitter->setPosition(m_pet_player->getPosition());
    } else {
        _emitter->stop();
    }
    if (m_pet_opponent->m_currPetAction == PET_RUN) {
        _emitter2->start();
        _emitter2->setPosition(m_pet_opponent->getPosition());
    } else {
        _emitter2->stop();
    }
    
    _emitter->emissionRate_ = EMISSION_RATE_A*m_pet_player->m_currSpeed + EMISSION_RATE_B;
    _emitter2->emissionRate_ =  EMISSION_RATE_A*m_pet_opponent->m_currSpeed + EMISSION_RATE_B;
    
    if (m_pet_helper) {
        m_helperTime += delta;
        
//        int timeToDisplay = (int)ceilf(HELPER_TIME_TO_REMOVE - m_helperTime);
//        if (timeToDisplay <= 0) timeToDisplay = 0;
//        racePanel->m_labelGetHelp->setString(CCString::createWithFormat("%d", timeToDisplay)->getCString());
        
        if (m_pet_helper->m_currPetAction == PET_RUN) {
            _emitter3->start();
            _emitter3->setPosition(m_pet_helper->getPosition());
        } else {
            _emitter3->stop();
        }
        _emitter3->emissionRate_ =  EMISSION_RATE_A*m_pet_helper->m_currSpeed + EMISSION_RATE_B;
    } else {
        _emitter3->stop();
    }
    
    if (m_pet_opponent->m_summaryTrack >= 5 &&
        m_pet_helper_cache &&
        m_helperTime == 0.0f &&
        !(m_pet_opponent->m_isFinishedRunning ||
          m_pet_player->m_isFinishedRunning))
    {
        racePanel->showGetHelp();
    }
    
    if (m_pet_helper)
    {
        int speedStatValue = MAX(m_pet_opponent->getStatStraightSpeed(), m_pet_opponent->getStatCornerSpeed());
        int sumTrackDifference = m_pet_opponent->m_summaryTrack - m_pet_helper->m_summaryTrack;
        int speedMult = 2;
        
        int setToSpeed;// = speedStatValue;
        
        if (sumTrackDifference >= 0) { //if opponent is ahead
            setToSpeed = MIN(400, speedStatValue + (speedStatValue*0.6f) + (speedMult * sumTrackDifference));
        } else {
            setToSpeed = MAX(5, speedStatValue - (speedStatValue*0.4f) + (speedMult * sumTrackDifference));
        }
        
        m_pet_helper->setStatStraightSpeed(setToSpeed);
        m_pet_helper->setStatCornerSpeed(setToSpeed);
        
        if (m_pet_helper->m_helperStunAmount <= 0)
//            &&
//            m_helperTime >= HELPER_TIME_TO_REMOVE)
        {
            m_delayUntilHelperWillBeRemoved -= delta;
            
            if (m_delayUntilHelperWillBeRemoved <= 0.0f)
            {
                racePanel->hideGetHelp(true);
                removeHelperPet();
            }
//            else
//            {
//                if (m_pet_opponent->m_summaryTrack >= 5) {
//                    racePanel->showGetHelp();
//                }
//            }
        }
    }
    
//    float a = -47500;
//    float b = 1475;
//    float c = 0;
//    _emitter->emissionRate_ = (-b - sqrt(pow(b, 2) - 4*a*(c - m_pet_player->m_runSpeed)))/(2*a);
    
//    CCLog_("emissionRate_ : %f", _emitter->emissionRate_);
    
    racePanel->checkSkillCost(m_pet_player->remainingCost());
    
    if (m_racingState == MRS_PREPARE && m_racingStatePlayed == MRS_PREPARE) {
        m_pet_player->setAnimation(PET_STANDBY);
        m_pet_opponent->setAnimation(PET_STANDBY);
        
        m_racingStatePlayed = MRS_PREPARE;
        
    } else if (m_racingState == MRS_READY && m_racingStatePlayed < MRS_READY) {
        m_pet_player->setAnimation(PET_READY);
        m_pet_opponent->setAnimation(PET_READY);
        
        m_racingStatePlayed = MRS_READY;
    } else if (m_racingState == MRS_GO) {
        
        time += delta;
        m_pet_player->updateRunning(this ,delta);
        m_pet_player->setZOrder(int(IsoMap::getContentSize().height-m_pet_player->getPosition().y));
        
        m_pet_opponent->updateRunning(this ,delta);
        m_pet_opponent->setZOrder(int(IsoMap::getContentSize().height-m_pet_opponent->getPosition().y));
        
        if (m_pet_helper) {
            m_pet_helper->updateRunning(this ,delta);
            if (m_pet_helper->can_running) m_pet_helper->setZOrder(int(IsoMap::getContentSize().height-m_pet_helper->getPosition().y));
        }
        
        m_racingStatePlayed = MRS_GO;

        if(m_pet_player->isPlaybackReplayDataEnabled())
        {
            racePanel->hidePowerUpButtons(false);
            Pet* pet = (m_pet_player->getReplayDataDuration() >= m_pet_opponent->getReplayDataDuration()) ? m_pet_player : m_pet_opponent;
            racePanel->setReplayTime(pet->getRemainingReplayTime());
        }
        else
        {
            if (m_pet_opponent->can_running) {
                m_AI_on_opponent->updateAI(delta);
            }

            if (m_AI_on_player) {
                if (m_pet_player->can_running) {
                    m_AI_on_player->updateAI(delta);
                }
            }

            if (m_AI_on_helper && m_pet_helper) {
                if (m_pet_helper->can_running) {
                    m_AI_on_helper->updateAI(delta);
                }
            }

            if (!m_tutorialMode) {
                if (!racePanel->m_lblPosTitle->isVisible()
                        && !m_pet_player->m_isFinishedRunning
                        && !m_pet_opponent->m_isFinishedRunning) {
                    racePanel->showHUD();
                }
            }
        }
        
//        racePanel->m_lblSpeed->setString(CCString::createWithFormat("%d",(int)m_pet_player->getSpeedDisplay())->getCString());
        
//        CCString *speedString;
//        float speed = m_pet_player->getSpeedDisplay();
//        if (speed < 1) {
//            speedString = CCString::createWithFormat("%.1f", m_pet_player->getSpeedDisplay());
//        } else {
//            speedString = CCString::createWithFormat("%.0f", m_pet_player->getSpeedDisplay());
//        }
        
        racePanel->m_lblSpeed->setString(CCString::createWithFormat("%.0f", m_pet_player->getSpeedDisplay())->getCString());
        
        float current = m_pet_player->m_currSpeed;
        float max = m_pet_player->getTopSpeed();
        float min = m_pet_player->getMinSpeed();
        float percentDecimal = (current-min)/(max-min);
        if (percentDecimal>1.0f) percentDecimal = 1.0f;
        if (percentDecimal<0.0f) percentDecimal = 0.0f;
        racePanel->updateSpeedProgressBar(percentDecimal);

        if(m_pet_player->isPlaybackReplayDataEnabled() && m_pet_player->isPlaybackFinished() && m_pet_opponent->isPlaybackFinished())
        {
            _timeUpDisplayed = true;
            racePanel->showReplayAgainButton();
        }

        _raceTimeLeft = _raceTimeLeft - delta;
        if (_raceTimeLeft <= 0 && !m_pet_player->m_isFinishedRunning && !m_pet_opponent->m_isFinishedRunning && !_timeUpDisplayed) {
            
            _timeUpDisplayed = true;
            
            _isoMapRacing->m_pet_opponent->m_summaryTrack
            = _isoMapRacing->m_pet_opponent->m_trackCount * _isoMapRacing->m_pet_opponent->max_round;
            
            racePanel->showTimeUp();
        } else if (!m_pet_player->m_isFinishedRunning && !m_pet_player->m_isFinishedRunning){
            updateTime();
        }
        
        if (is_replay) {
            RankType rankType = m_pet_opponent->m_summaryTrackPosition > m_pet_player->m_summaryTrackPosition ? RankTypeFirst : RankTypeSecond;
            if (racePanel->rankPopup->getRankType() != rankType) {
                racePanel->rankPopup->changeRank(m_pet_player, rankType, m_rankPopoverDisplayTime);
                
                if (rankType == RankTypeFirst || rankType == RankTypeNone) {
                    racePanel->m_lblPos->setString("1ST");
                } else if (rankType == RankTypeSecond) {
                    racePanel->m_lblPos->setString("2ND");
                }
            }
            
//            racePanel->slotPanel->setRank(m_pet_opponent->sumary_distance > m_pet_player->sumary_distance ? 1 : 2);
//            racePanel->slotPanel->setSpeed(m_pet_opponent->getSpeedDisplay());
        } else {
            
            RankType rankType = racePanel->rankPopup->getRankType();
            if (m_pet_opponent->m_summaryTrackPosition > m_pet_player->m_summaryTrackPosition) {
                rankType = RankTypeSecond;
            } else if (m_pet_opponent->m_summaryTrackPosition < m_pet_player->m_summaryTrackPosition) {
                rankType = RankTypeFirst;
            }
            
            if (racePanel->rankPopup->getRankType() != rankType) {
                racePanel->rankPopup->changeRank(m_pet_player, rankType, m_rankPopoverDisplayTime);
                
                if (rankType == RankTypeFirst || rankType == RankTypeNone) {
                    racePanel->m_lblPos->setString("1ST");
                } else if (rankType == RankTypeSecond) {
                    racePanel->m_lblPos->setString("2ND");
                }
            }
            
//            float playerTrack = (float)m_pet_player->m_summaryTrack;
//            float opponentTrack = (float)m_pet_opponent->m_summaryTrack;
//            float value = (playerTrack - opponentTrack)/m_trackCount;

            CCRect rectPlayer = CCRectZero;
            rectPlayer.size = g_gameContent->tileSize;
            rectPlayer.origin = m_pet_player->m_currPos - rectPlayer.size*0.5f;
            
            CCRect rectOpponent = CCRectZero;
            rectOpponent.size = g_gameContent->tileSize;
            rectOpponent.origin = m_pet_opponent->m_currPos - rectOpponent.size*0.5f;
            
//            CCLayerColor *color1 = (CCLayerColor *)this->getChildByTag(100010);
//            if (!color1) {
//                color1 = CCLayerColor::create(ccc4(200, 200, 200, 200), rectPlayer.size.width, rectPlayer.size.height);
//                color1->setTag(100010);
//                this->addChild(color1, 1000000000);
//            }
//            
//            CCLayerColor *color2 = (CCLayerColor *)this->getChildByTag(100020);
//            if (!color2) {
//                color2 = CCLayerColor::create(ccc4(200, 0, 0, 200), rectPlayer.size.width, rectPlayer.size.height);
//                color2->setTag(100020);
//                this->addChild(color2, 1000000000);
//            }
//            
//            color1->setPosition(m_pet_player->m_currPos - rectPlayer.size*0.5f);
//            color2->setPosition(m_pet_opponent->m_currPos - rectPlayer.size*0.5f);
                        
            if (m_pet_player->usingDash()
                && !m_pet_opponent->usingDash()
                && m_pet_opponent->canBeAttacked()
                && rectPlayer.containsPoint(m_pet_opponent->m_currPos))
            {
                if (g_playerState->hasAbilityForKey(kAbilityDashForward)) {
                    Ability* _ability = g_playerState->getAbilityForKey(kAbilityDashForward);
                     m_pet_opponent->petIsAttackedWithEffect(m_pet_player, PET_FALL_A, _ability->aVo.stun_duration, SKILL_DASH);
                }
            }
            
//            value = (opponentTrack - playerTrack)/m_trackCount;
            
            if (m_pet_opponent->usingDash()
                && !m_pet_player->usingDash()
                && m_pet_player->canBeAttacked()
                && rectOpponent.containsPoint(m_pet_player->m_currPos))
            {
                if (m_pet_opponent->playerState->hasAbilityForKey(kAbilityDashForward)) {
                    Ability* _ability = m_pet_opponent->playerState->getAbilityForKey(kAbilityDashForward);
                    m_pet_player->petIsAttackedWithEffect(m_pet_opponent, PET_FALL_A, _ability->aVo.stun_duration, SKILL_DASH);
                }
            }
            
            if (m_pet_helper) {
                if (m_pet_helper->usingDash()
                    && !m_pet_opponent->usingDash()
                    && m_pet_opponent->canBeAttacked()
                    && rectOpponent.containsPoint(m_pet_helper->m_currPos))
                {
                    if (m_pet_helper->playerState->hasAbilityForKey(kAbilityDashForward)) { //TODO: g_helperState
//                        Ability* _ability = m_pet_helper->playerState->getAbilityForKey(kAbilityDashForward);
                        
                        if (m_pet_helper->m_helperStunDuration >= 0.0f &&
                            m_pet_helper->m_helperStunAmount >= 0)
                        {
                            m_pet_opponent->petIsAttackedWithEffect(m_pet_helper, PET_FALL_A, m_pet_helper->m_helperStunDuration, SKILL_DASH);
                        }
                    }
                }
            }
        }
    }
    
    CCPoint distanceBetweenPet = ccp(fabs(m_pet_player->getPositionX() - m_pet_opponent->getPositionX()), fabs(m_pet_player->getPositionY() - m_pet_opponent->getPositionY())) + ccp(80, 80);
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    CCPoint distanceBetweenPetPlayerToEdgeOfTheScreen = distanceBetweenPet + (_screenSize*0.1f);
    
    float scale = MIN(_screenSize.width*0.5f/distanceBetweenPetPlayerToEdgeOfTheScreen.x,
                      _screenSize.height*0.5f/distanceBetweenPetPlayerToEdgeOfTheScreen.y);
    
    _scene->scrollView->setCameraOnPosition(m_pet_player->getPosition() - this->getContentSize()/4.f, scale, CAMERA_ZOOMING_SPEED);
}

void IsoMapRacing::changeRacingState(MapRacingState state) {
    
    if (m_racingState != state) {
        m_racingState = state;
        CCLog_("m_racingState : %d",m_racingState);

        if(m_racingState == MRS_GO)
            submitRaceStartEvent();
    }
}

void IsoMapRacing::startRunningWithGaugeResult(GaugeRESULT result)
{
//    /*
//    if (freez) {
//        switch (arc4random()%5) {
//            case 0: m_pet_player->setAnimation(PET_FROZEN_A); break;
//            case 1: m_pet_player->setAnimation(PET_FROZEN_B); break;
//            case 2: m_pet_player->setAnimation(PET_FROZEN_C); break;
//            case 3: m_pet_player->setAnimation(PET_FROZEN_D); break;
//            case 4: m_pet_player->setAnimation(PET_FROZEN_E); break;
//            default: break;
//        }
//        should_frozen_pet_player = true;
//    } else {
//        racePanel->setLap(m_pet_player->ran_round + 1, m_pet_player->max_round);
//        m_pet_player->startRunning();
//        racePanel->slotPanel->show();
//    }
//     */
    
    m_pet_player->can_running = true;
    m_pet_opponent->can_running = true;
    
    racePanel->setLap(m_pet_player->ran_round + 1, m_pet_player->max_round);
    
    if (result == GaugeRESULT_GOOD) {
        if (!m_tutorialMode)
        {
            racePanel->showPowerUpButtons(racePanel->m_playerUsableSkills, true);
            racePanel->showGroupEnergy(true);
        }
        
        m_pet_player->startRunning();
        m_AI_on_opponent->m_startState = START_LATER;
        if (m_AI_on_player) {
            m_AI_on_player->m_startState = START_FIRST;
            m_AI_on_player->pushState(AI_START);
        }
        this->runAction(CCSequence::create(CCDelayTime::create(DELAY_START_FIRST_OR_LATER),
                                           CCCallFuncO::create(this, callfuncO_selector(IsoMapRacing::delayStartRunning), m_pet_opponent),
                                           NULL));
    } else if (result == GaugeRESULT_NORMAL) {
        if (!m_tutorialMode)
        {
            racePanel->showPowerUpButtons(racePanel->m_playerUsableSkills, true);
            racePanel->showGroupEnergy(true);
        }
        
        m_pet_player->startRunning();
        m_pet_opponent->startRunning();
        m_AI_on_opponent->pushState(AI_START);
        m_AI_on_opponent->m_startState = START_SAME;
        if (m_AI_on_player) {
            m_AI_on_player->m_startState = START_SAME;
            m_AI_on_player->pushState(AI_START);
        }
    } else {
        m_pet_opponent->startRunning();
        m_AI_on_opponent->pushState(AI_START);
        m_AI_on_opponent->m_startState = START_FIRST;
        if (m_AI_on_player) {
            m_AI_on_player->m_startState = START_LATER;
        }
        this->runAction(CCSequence::create(CCDelayTime::create(DELAY_START_FIRST_OR_LATER),
                                           CCCallFuncO::create(this, callfuncO_selector(IsoMapRacing::delayStartRunning), m_pet_player),
                                           NULL));
    }

    if(m_pet_player->isPlaybackReplayDataEnabled())
        racePanel->hideGroupEnergy(false);
}

void IsoMapRacing::delayStartRunning(Pet *pet)
{
    pet->startRunning();
    if (pet == m_pet_player) {
        if (!m_tutorialMode)
        {
            racePanel->showPowerUpButtons(racePanel->m_playerUsableSkills, true);
            racePanel->showGroupEnergy(true);
        }
        
        if (m_AI_on_player) {
            m_AI_on_player->pushState(AI_START);
        }
    } else if (pet == m_pet_opponent) {
        m_AI_on_opponent->pushState(AI_START);
    }

    if(m_pet_player->isPlaybackReplayDataEnabled())
        racePanel->hideGroupEnergy(false);
}

void IsoMapRacing::startRunningAfterFreez() {
    if (is_replay) {
        m_pet_opponent->startRunning();
    } else {
        racePanel->setLap(m_pet_player->ran_round + 1, m_pet_player->max_round);
        m_pet_player->startRunning();
    }
}

void IsoMapRacing::opponentStartRunning() {
    m_pet_player->can_running = true;
    m_pet_opponent->can_running = true;
//    scheduleOnce(schedule_selector(IsoMapRacing::startRunningAfterFreez), 2);
    if (is_replay) {
        this->runUserActivity();
        m_pet_player->startRunning();
        CCString *delay_start = (CCString *)m_pet_opponent->replayRecord->objectForKey("delay_start");
        scheduleOnce(schedule_selector(IsoMapRacing::startRunningAfterFreez), delay_start->doubleValue());
    } else {
        m_pet_opponent->startRunning();
        if (should_frozen_pet_player) {
            scheduleOnce(schedule_selector(IsoMapRacing::startRunningAfterFreez), 1);
        }
    }
}


void IsoMapRacing::updateCurrentLap(Pet *playerPet)
{
    racePanel->setLap(playerPet->ran_round+1, playerPet->max_round);
}

void IsoMapRacing::updateLapWithPetPlayer(Pet *pet)
{
    if (m_pet_player == pet) {
        g_gameAudio->playEffect(AUDIO_ENERGY_REGEN);
        
        updateCurrentLap(pet);
        addTime(_checkpointBonusTime, "CHECKPOINT");
        
        g_gameAudio->playEffect(AUDIO_FIREWORK);
        
        racePanel->animateBallEffect(BallEffectTypeFromPlayer ,racePanel->m_lblTimer, 0.45f);
        
        if (pet->ran_round < pet->max_round) {
            pet->regenCost();
            pet->showEnergyInfo();
            racePanel->checkSkillCost(m_pet_player->remainingCost());
        }
        
    } else if (m_pet_opponent == pet) {
        
        if (!m_giveUp)
        {
            if (pet->ran_round < pet->max_round) {
                pet->regenCost();
                pet->showEnergyInfo();
            }
        }
    }
    
//    if (g_gameContent->GameConfig["RESET_ALL_SKILLS_COST_AT_NEW_LAP"]) {
//        if (m_pet_player == pet) {
//            racePanel->resetAllSkillsCost();
//        } else if (m_pet_opponent == pet) {
//            m_AI_on_opponent->resetAllSkillsTimeUsed();
//        }
//    }
}

void IsoMapRacing::finishRunningWithPetPlayer(Pet *pet){
    
    racePanel->hideGetHelp(true);
    
    if (winState == UNKNOWN_WIN) {
        if (m_pet_player == pet) {
            winState = PLAYER_WIN;
            m_pet_player->petResult = WIN_RESULT;
            m_pet_opponent->petResult = LOSE_RESULT;
            
            g_gameAudio->stopAllEffect();
            g_gameAudio->playEffect(AUDIO_RACE_WIN1);
            g_gameAudio->playEffect(AUDIO_RACE_WIN2);
            g_gameAudio->playEffect(AUDIO_RACE_CHEERS);
            
        } else if (m_pet_opponent == pet) {
            winState = OPPONENT_WIN;
            m_pet_player->petResult = LOSE_RESULT;
            m_pet_opponent->petResult = WIN_RESULT;

        }
    }
}

void IsoMapRacing::stopRunningWithPetPlayer(Pet *pet){
    CCLog_("stop Running");
    if (m_pet_player == pet) {
        if (winState == PLAYER_WIN) {
            m_pet_player->setAnimation(PET_HABITAT_WIN);
        } else {
            m_pet_player->setAnimation(PET_HABITAT_LOSE);
        }
        
        racePanel->hidePowerUpButtons(true);
        racePanel->hideGroupEnergy(true);
        m_pet_player->m_isFinishedRunning = true;
        m_pet_opponent->m_isFinishedRunning = true;
        
        CCLog_("time : %f", g_gameContent->GameConfig["TIME_AFTER_FINISH"]);
        
        scheduleOnce(schedule_selector(IsoMapRacing::showResult), g_gameContent->GameConfig["TIME_AFTER_FINISH"]);
    }  else if (m_pet_opponent == pet) {
        if (winState == OPPONENT_WIN) {
            m_pet_opponent->setAnimation(PET_HABITAT_WIN);
        } else {
            m_pet_opponent->setAnimation(PET_HABITAT_LOSE);
        }
        
        m_pet_player->m_isFinishedRunning = true;
        m_pet_opponent->m_isFinishedRunning = true;
    }
    
    racePanel->hideHUD();
}

Pet* IsoMapRacing::createHelperPet(PlayerHelperPetVO helperPetVo)
{
    PlayerPetVO ppVo = PlayerPetVO();
    ppVo.top_speed = helperPetVo.pet_top_speed;
    ppVo.corner_speed = helperPetVo.pet_corner_speed;
    ppVo.jumping = helperPetVo.pet_jumping;
    ppVo.type = helperPetVo.pet_type;
    PlayerState* helper_playerState = new PlayerState();
    GDPetVO pVo = g_gameContent->getPetVO(ppVo.type);
    if (pVo.type == "") {
        CCLog_("No PetVO was found!");
        return NULL;
    }
    Pet* pet = Pet::createWithPetVO(pVo, ppVo);
    pet->retain();
    helper_playerState->Pets.push_back(pet);
    
    helper_playerState->PetRunner = pet;
    helper_playerState->playerPet = pet->ppVo;
    
    for (map<string, int>::iterator it = helperPetVo.abilities.begin(); it != helperPetVo.abilities.end(); ++it) {
        
        Ability* ablt = Ability::create();
        ablt->retain();
        ablt->aVo = GDAbilityVO(g_gameContent->getAbilityVO((*it).first, (*it).second));
        ablt->paVo = PlayerAbilityVO(ablt->aVo);
        helper_playerState->Abilities[(*it).first] = ablt;
        
    }
    
    float timeTotal = 0.0f, times = 0.0f, timeEach = 0.0f;
    g_gameContent->cal_helper_horsepower_to_stun_time(helperPetVo.pet_top_speed
                                                      + helperPetVo.pet_corner_speed
                                                      + helperPetVo.pet_jumping,
                                                      &timeTotal,
                                                      &times,
                                                      &timeEach);
    pet->m_helperStunAmount = times;
    pet->m_helperStunDuration = timeEach;
    
    pet->m_trackCount = m_trackCount;
    pet->playerState = helper_playerState;
    pet->initilizeEnergyInformation();
    pet->setZOrder(3);
    pet->isoMapRacing = this;
    pet->setVisible(false);
    this->addChild(pet);

    return pet;
}

Pet* IsoMapRacing::createHelperPet(std::string petType)
{
    PlayerHelperPetVO helperPetVO = PlayerHelperPetVO();
    helperPetVO.pet_top_speed = m_pet_player->ppVo.top_speed;
    helperPetVO.pet_corner_speed = m_pet_player->ppVo.corner_speed;
    helperPetVO.pet_jumping = m_pet_player->ppVo.jumping;
    helperPetVO.pet_type = petType;
    helperPetVO.abilities.insert(std::pair<string, int>(kAbilityBananaTrap, 1));
    helperPetVO.abilities.insert(std::pair<string, int>(kAbilityStomp, 1));
    helperPetVO.abilities.insert(std::pair<string, int>(kAbilityDashForward, 1));
    helperPetVO.abilities.insert(std::pair<string, int>(kAbilityShootForward, 1));
    helperPetVO.abilities.insert(std::pair<string, int>(kAbilityDoubleShoot, 1));

    g_playerState->HelperVo = helperPetVO;

    return createHelperPet(helperPetVO);
}

void IsoMapRacing::addHelperPet()
{
    if (m_pet_helper) {//) || m_helperHasAttacked) {
        return;
    }
    
    pauseGame();
    
    //=================================
    //Animate petSpine at GetHelp button up to the sky
    //=================================
    
    racePanel->addSpotlightBG(ZORDER_ABOVE_POWERUPBUTTONS);

    CCAssert(racePanel->m_helperPetSpine, "helper pet spine not loaded");

    CCPoint startPosition = racePanel->m_helperPetSpine->getPosition();
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint endPosition = ccp(startPosition.x, screenSize.height);
//    racePanel->m_helperPetSpine->setPosition(CCPointZero);
    
    CCSprite *lightA = CCSprite::create("common/vfx/light_fx_a.png");
    lightA->setScale(sizeMultiplier);
    lightA->setAnchorPoint(ccp(0.5f, 1.0f));
    lightA->setPosition(ccp(0, lightA->getContentSize().height*lightA->getScaleX()*0.1f));
    lightA->setFlipY(true);
    lightA->setScaleY(sizeMultiplier*0.1f);
    racePanel->m_helperPetSpine->addChild(lightA, 0);
    
    CCSprite *lightB = CCSprite::create("common/vfx/light_fx_b.png");
    lightB->setScale(sizeMultiplier);
    lightB->setPosition(ccp(0, 0));
    racePanel->m_helperPetSpine->addChild(lightB, 0);
    
    CCSize scalePetSize = CCSizeMake(racePanel->m_helperPetSpine->getScaleX(), racePanel->m_helperPetSpine->getScaleY());
    
    float moveUpDelay = 0.2f;
    float moveUp = 0.28f;
    
    lightB->runAction(CCSequence::create(CCScaleTo::create(0.5f, 0),
                                         CCRemoveSelf::create(),
                                         NULL));
    
    racePanel->m_helperPetSpine->
    runAction(CCSequence::create(CCScaleTo::create(0.1f, scalePetSize.width*1.5f, scalePetSize.height*0.5f),
                                 CCScaleTo::create(0.1f, scalePetSize.width, scalePetSize.height*1.5f),
                                 CCScaleTo::create(0.5f, scalePetSize.width, scalePetSize.height),
                                 NULL));
    
    lightA->runAction(CCSequence::create(CCDelayTime::create(moveUpDelay),
                                         CCScaleTo::create(0.2f, sizeMultiplier, sizeMultiplier*1.3f),
                                         CCRemoveSelf::create(),
                                         NULL));
    
    racePanel->m_helperPetSpine->
    runAction(CCSequence::create(CCDelayTime::create(moveUpDelay),
                                 CCMoveTo::create(moveUp, endPosition),
                                 CCHide::create(),
                                 CCCallFunc::create(racePanel, callfunc_selector(RacePanel::removeSpotlightBG)),
                                 NULL));
    
    g_gameAudio->playEffect(AUDIO_ANIMAL_PUSH);
    
    //=================================
    //Create the helper pet
    //=================================
    
    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    
    if (m_pet_helper_cache) {
        m_pet_helper = m_pet_helper_cache;
    }
    m_pet_helper->isHelperPet = true;
    
    Track *track;

    if(m_pet_player->isPlaybackReplayDataEnabled())
    {
        track = m_pet_helper->getStartingTrackFromReplayData();
    }
    else
    {
        if (m_pet_opponent->m_summaryTrack > m_pet_player->m_summaryTrack) { //if opponent ahead of player

            track = m_pet_player->m_currTrack->nextTrack->nextTrack; //two tracks ahead of m_pet_player
            m_pet_helper->m_summaryTrack = m_pet_player->m_summaryTrack + 2; //two tracks ahead of m_pet_player

        } else {

            track = m_pet_player->m_currTrack->prevTrack->prevTrack; //two tracks ahead of m_pet_player
            m_pet_helper->m_summaryTrack = m_pet_player->m_summaryTrack - 2; //two tracks ahead of m_pet_player
        }
    }
    m_pet_helper->prepareRunning(this, track, _mapPlayerState->player.racing.direction, 2);
    m_pet_helper->setZOrder(int(IsoMap::getContentSize().height-m_pet_helper->getPosition().y));
    
    m_pet_helper->opponentPet = m_pet_opponent;
    
    m_AI_on_helper = AI::create(this, m_pet_helper, m_pet_opponent);
    m_AI_on_helper->popToState(AI_SCOUT);
    m_AI_on_helper->retain();
    
    m_pet_helper->setScale(m_pet_helper->getScale() * 0.75f);
    
    m_pet_helper->setVisible(false);
    
    //=================================
    //Animate helper pet down to the track
    //=================================
    
    CCPoint endPosition_2 = m_pet_helper->getPosition();
    CCPoint startPosition_2 = ccp(endPosition_2.x, endPosition_2.y + screenSize.height);
    m_pet_helper->setPosition(startPosition_2);
    
    CCSprite *lightA_2 = CCSprite::create("common/vfx/light_fx_a.png");
    lightA_2->setScale(sizeMultiplier);
    lightA_2->setAnchorPoint(ccp(0.5f, sizeMultiplier*0.1f));
    lightA_2->setPosition(ccp(0, -lightA_2->getContentSize().height*lightA_2->getScaleX()*0.1f));
    m_pet_helper->addChild(lightA_2, 0);
    
    CCSprite *lightB_2 = CCSprite::create("common/vfx/light_fx_b.png");
    lightB_2->setScale(sizeMultiplier);
    lightB_2->setPosition(ccp(0, 0));
    lightB_2->setOpacity(0);
    m_pet_helper->addChild(lightB_2, 0);
    
    CCSize scalePetSize_2 = CCSizeMake(m_pet_helper->getScaleX(), m_pet_helper->getScaleY());
    
    float delayDuration = moveUpDelay + moveUp;
    float moveDown = 0.5f;
    
    lightA_2->runAction(CCSequence::create(CCDelayTime::create(delayDuration),
                                         CCScaleTo::create(moveDown, lightA_2->getScaleX(), lightA_2->getScaleY()*1.3f),
                                         CCRemoveSelf::create(),
                                         NULL));
    
    lightB_2->runAction(CCSequence::create(CCDelayTime::create(moveDown +delayDuration),
                                         CCScaleTo::create(moveDown, 1),
                                         CCRemoveSelf::create(),
                                         NULL));
    
    lightB_2->runAction(CCSequence::create(CCDelayTime::create(moveDown +delayDuration),
                                         CCFadeIn::create(0),
                                         CCFadeOut::create(moveDown),
                                         NULL));
    
    m_pet_helper->
    runAction(CCSpawn::create(CCSequence::create(CCDelayTime::create(delayDuration),
                                                 CCShow::create(),
                                                 CCMoveTo::create(moveDown, endPosition_2),
                                                 NULL),
                              CCSequence::create(CCDelayTime::create(delayDuration),
                                                 CCScaleTo::create(moveDown, scalePetSize_2.width, scalePetSize_2.height*1.5f),
                                                 CCCallFunc::create(this, callfunc_selector(IsoMapRacing::playDropPetSound)),
                                                 CCScaleTo::create(moveDown*0.2f, scalePetSize_2.width*1.5f, scalePetSize_2.height*0.5f),
                                                 CCScaleTo::create(moveDown*0.2f, scalePetSize_2.width, scalePetSize_2.height),
                                                 CCCallFunc::create(this, callfunc_selector(IsoMapRacing::startHelperPet)),
                                                 NULL)
                              ,
                              NULL));

    m_pet_player->callHelper(m_pet_helper);

    if(m_pet_player->isPlaybackReplayDataEnabled())
        racePanel->hideGetHelp(false);

    //=================================
    //Start up the helper pet
    //=================================
}

void IsoMapRacing::playDropPetSound()
{
    g_gameAudio->playEffect(AUDIO_DROP_ANIMAL_TO_BUILDING);
}

void IsoMapRacing::startHelperPet()
{
//    racePanel->removeSpotlightBG();
    
    resumeGame();
    
    m_pet_helper->petSpine->runAction(CCRepeatForever::create(CCSequence::create(CCTintTo::create(0.5f, 100, 100, 100),
                                                                                 CCTintTo::create(0.5f, 255, 255, 255),
                                                                                 NULL)));
    
    m_pet_helper->can_running = true;
    m_pet_helper->startRunning();
}

void IsoMapRacing::removeHelperPet()
{
    //NOTE: This function is called in update
    
    if (m_isRemovingHelperPet) return;
    m_isRemovingHelperPet = true;
    
    if (m_pet_helper) {
        
        m_pet_helper->can_running = false;
        
        CCPoint startPosition = m_pet_helper->getPosition();
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        CCPoint endPosition = ccp(startPosition.x, startPosition.y + screenSize.height);
        
        CCSprite *lightA = CCSprite::create("common/vfx/light_fx_a.png");
        lightA->setScale(sizeMultiplier);
        lightA->setAnchorPoint(ccp(0.5f, 1.0f));
        lightA->setPosition(ccp(0, lightA->getContentSize().height*lightA->getScaleX()*0.1f));
        lightA->setFlipY(true);
        lightA->setScaleY(sizeMultiplier*0.1f);
        m_pet_helper->addChild(lightA, 0);
        
        CCSprite *lightB = CCSprite::create("common/vfx/light_fx_b.png");
        lightB->setScale(sizeMultiplier);
        lightB->setPosition(ccp(0, 0));
        m_pet_helper->addChild(lightB, 0);
        
        CCSize scalePetSize = CCSizeMake(m_pet_helper->getScaleX(), m_pet_helper->getScaleY());
        
        float moveUpDelay = 0.2f;
        float moveUp = 0.5f;
        
        lightB->runAction(CCSequence::create(CCScaleTo::create(0.5f, 0),
                                             CCRemoveSelf::create(),
                                             NULL));
        
        lightA->runAction(CCSequence::create(CCDelayTime::create(moveUpDelay),
                                             CCScaleTo::create(0.2f, sizeMultiplier, sizeMultiplier*1.3f),
                                             CCRemoveSelf::create(),
                                             NULL));
        
        m_pet_helper->
        runAction(CCSequence::create(CCScaleTo::create(0.1f, scalePetSize.width*1.5f, scalePetSize.height*0.5f),
                                     CCScaleTo::create(0.1f, scalePetSize.width, scalePetSize.height*1.5f),
                                     CCScaleTo::create(moveUp, scalePetSize.width, scalePetSize.height),
                                     NULL));
        
        m_pet_helper->
        runAction(CCSequence::create(CCDelayTime::create(moveUpDelay),
                                     CCMoveTo::create(moveUp, endPosition),
                                     CCHide::create(),
                                     CCCallFunc::create(this, callfunc_selector(IsoMapRacing::deleteHelperPet)),
                                     NULL));
        
        g_gameAudio->playEffect(AUDIO_ANIMAL_PUSH);

        m_pet_player->removeHelper();
    }
}

void IsoMapRacing::deleteHelperPet()
{
    if (m_pet_helper) {

        if(!m_pet_player->isPlaybackReplayDataEnabled())
        {
            helperPetType = m_pet_helper->ppVo.type;
            helperPetReplayData = m_pet_helper->serializeReplayData();
        }

        m_pet_helper->setVisible(false);
        m_pet_helper->removeAllChildrenWithCleanup(true);
        m_pet_helper = NULL;
        
        if (m_AI_on_helper) {
            m_AI_on_helper->release();
            m_AI_on_helper = NULL;
        }
    }
}

void IsoMapRacing::showResult()
{
    bool player_lose = (_raceTimeLeft<=0.0f
                       || m_giveUp
                       || winState == OPPONENT_WIN);
    bool player_win = !player_lose;
    
    float oppo_diff_scale = m_AI_on_opponent->getNewDifficultyScale(0.0f);
    float new_ai_difficulty = g_playerState->player.racing.ai_difficulty;
    
    CCLog_("oppo_diff_scale");
    DEBUG_FLOAT(oppo_diff_scale);
    CCLog_("old_ai_difficulty");
    DEBUG_FLOAT(new_ai_difficulty);
    
    //WIN
    if (player_win) {
        
        int player_times_won = CCUserDefault::sharedUserDefault()->getIntegerForKey("player_times_won");
        CCUserDefault::sharedUserDefault()->setIntegerForKey("player_times_won", player_times_won+1);
        CCLog_("player_times_won:%d",player_times_won+1);
        
        
        
        CCLog_("LastRacedVillageIndex = %d", g_playerState->LastRacedVillageIndex);
        
        if (g_playerState->WorldMapVillages[g_playerState->LastRacedVillageIndex].uuid == "") {
            
            CCLog_("WARNING: g_playerState->WorldMapVillages[g_playerState->LastRacedVillageIndex].uuid == empty");
        } else {
            
            g_playerState->WorldMapVillages[g_playerState->LastRacedVillageIndex].status = "territory";
        }
        
        new_ai_difficulty += AI_DIFF_INCREASE_LOWEST_HP + (oppo_diff_scale * (AI_DIFF_INCREASE_HIGHEST_HP - AI_DIFF_INCREASE_LOWEST_HP));
        if (new_ai_difficulty > 1.0f) new_ai_difficulty = 1.0f;

        CustomEventsHelper::Instance().onRaceWon();
    }
    //LOSE
    else
    {
        new_ai_difficulty -= AI_DIFF_DECREASE_LOWEST_HP + (oppo_diff_scale * (AI_DIFF_DECREASE_HIGHEST_HP - AI_DIFF_DECREASE_LOWEST_HP));
        if (new_ai_difficulty < 0.0f) new_ai_difficulty = 0.0f;
    }
    
    CCLog_("new_ai_difficulty");
    DEBUG_FLOAT(new_ai_difficulty);
    
    if (isTutorialMode()) {
        delete g_playerState;
        g_playerState = g_playerState_restore;
        setTutorialMode(false);
        
        CCDictionary *ccDict = CCDictionary::create();
        if (player_win) {
            ccDict->setObject(CCString::create("Won"), "Result");
            ccDict->setObject(CCString::createWithFormat("Ahead %d", m_pet_player->m_summaryTrack - m_pet_opponent->m_summaryTrack), "Sum Track Difference");
        } else {
            if (m_giveUp) {
                ccDict->setObject(CCString::create("Gave Up"), "Result");
            } else {
                ccDict->setObject(CCString::create("Lost"), "Result");
            }
            
            ccDict->setObject(CCString::createWithFormat("Behind %d", m_pet_opponent->m_summaryTrack - m_pet_player->m_summaryTrack), "Sum Track Difference");
        }
        FlurryCpp::endTimedEvent(EVENT_TUT_TUTFIRSTRACE_FREE_PLAY, ccDict);
        
        FBAppEventsWrapper::logEvent(EVENT_TUT_TUTFIRSTRACE_FREE_PLAY);
        
        if (onShowRaceResult) {
            onShowRaceResult(onRaceResultSender, player_lose);
        }
        return;
    }
    else
    {
        submitRaceEndEvent(player_win);
    }
    
//    g_gameAudio->stopMusic();
    g_gameAudio->stopAllEffect();


    if(m_pet_player->isPlaybackReplayDataEnabled())
        return;

    if (!is_replay) {
        
        CCLog_("_raceTimeLeft : %f",_raceTimeLeft);
        CCLog_("m_giveUp : %d",m_giveUp);
        CCLog_("winState: %d",winState);
        
        Building *garage = g_playerState->getBuilding(kBuildingGarage, 0);
        int garage_level = garage ? garage->pbVo.level : -1; //-1 means no garage
        
        raceResultPanel->m_tutSecondRace = !g_playerState->Checklist[TutSecondRacePart4::key()];

        if(m_pet_helper)
        {
            helperPetType = m_pet_helper->ppVo.type;
            helperPetReplayData = m_pet_helper->serializeReplayData();
        }

        bool hasHelperPetReplayData = helperPetType != "";

        if (!player_lose) {
            g_playerState->player.info.win_streak ++;
        }
        
        if (g_opponentState->isBot()) { // don't send replay
            BackendService::getInstance()->send(APIRaceUpdate::create(m_pet_player->running_time,
                                                                      "{}",
                                                                      m_pet_opponent->running_time,
                                                                      "{}",
                                                                      "",
                                                                      "",
                                                                      player_lose, //true if player loses, false if player wins
                                                                      new_ai_difficulty,
                                                                      garage_level,
                                                                      g_sceneManager,
                                                                      SceneManager::onMapVillagesRetrieved));
        }else {
            BackendService::getInstance()->send(APIRaceUpdate::create(m_pet_player->running_time,
                                                                      m_pet_player->serializeReplayData().c_str(),
                                                                      m_pet_opponent->running_time,
                                                                      m_pet_opponent->serializeReplayData().c_str(),
                                                                      (hasHelperPetReplayData) ? helperPetType.c_str() : "",
                                                                      (hasHelperPetReplayData) ? helperPetReplayData.c_str() : "",
                                                                      player_lose, //true if player loses, false if player wins
                                                                      new_ai_difficulty,
                                                                      garage_level,
                                                                      g_sceneManager,
                                                                      SceneManager::onMapVillagesRetrieved));
        }
        
        //update_ai_difficulty
        CCLog_("new_ai_difficulty:%.2f",new_ai_difficulty);
        g_playerState->player.racing.ai_difficulty = new_ai_difficulty;
        
        CCLog_("g_playerState->player.racing.ai_difficulty:%.2f",g_playerState->player.racing.ai_difficulty);
        g_playerState->Checklist[TutSecondRacePart4::key()] = true;

        raceResultPanel->start();
        
    }
    
    CCLog_("show result");
}

CSJson::Value IsoMapRacing::createRaceStartEventParameters()
{
    int playerLevel = g_playerState->player.racing.level;
    int petLevel = m_pet_player->getLevel();
    int petHp = m_pet_player->getHorsepower();
    
    int aiLevel = g_opponentState->player.racing.level;
    int aiPetLevel = m_pet_opponent->getLevel();
    int aiPetHp = m_pet_opponent->getHorsepower();
    
    int trackLength = m_trackCount;
    
    CSJson::Value eventParameters;
    
    eventParameters["ranchLevel"] = playerLevel;
    eventParameters["petLevel"] = petLevel;
    eventParameters["petHP"] = petHp;
    eventParameters["aiLevel"] = aiLevel;
    eventParameters["aiPetLevel"] = aiPetLevel;
    eventParameters["aiPetHP"] = aiPetHp;
    eventParameters["trackLength"] = trackLength;
    
    return eventParameters;
}

CCDictionary* IsoMapRacing::createRaceStartEventParametersInDictionary()
{
    CCDictionary* parameters = CCDictionary::create();
    
    CSJson::Value eventParameters = createRaceStartEventParameters();
    
    CSJson::FastWriter writer;
    std::string output = writer.write(eventParameters);
    
    CCLog_("....test output raceStartInfo: %s", output.c_str());
    
    parameters->setObject(ccs(output.c_str()), "raceStartInfo");

    return parameters;
}

CSJson::Value IsoMapRacing::createRaceEndEventParameters(bool playerWon)
{
    CSJson::Value eventParameters;
    
    int sumTrackDifference = m_pet_player->m_summaryTrack - m_pet_opponent->m_summaryTrack;
    int remainingTrackCount = (m_trackCount * _totalLaps) - m_pet_player->m_summaryTrack;
    
    if (playerWon) { //Result Won
        
        int coins_reward = 0;
        
        PlayerState *_playerState = PlayerState::getInstance();
        
        if (Building* _garage = _playerState->getBuilding(kBuildingGarage, 0)) {
            coins_reward = g_gameContent->cal_coins_reward(_garage->pbVo.level,
                                                           g_opponentState->coinsReward,
                                                           _playerState->player.info.win_streak);
        } else {
            coins_reward = g_gameContent->cal_coins_reward(0,
                                                           g_opponentState->coinsReward,
                                                           _playerState->player.info.win_streak);
        }
        
        eventParameters["coinsReward"] = coins_reward; //Coins Reward
        
        GDItemVO itemVO = g_gameContent->getItemVO(g_opponentState->itemDrops);
        
        eventParameters["itemTypeReward"] = itemVO.type; //Item Type Reward
        eventParameters["itemCoinsValue"] = itemVO.coins_value; //Item Type Reward
    }
    
    eventParameters["resultWon"] = playerWon;
    eventParameters["resultLost"] = m_giveUp ? false : !playerWon;
    eventParameters["resultGaveUp"] = m_giveUp;
    
    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    string trackType = _mapPlayerState->Tracks[0]->tVo.type;
    string playerTrackPreference = m_pet_player->pVo.track_preference;
    string opponentTrackPreference = m_pet_opponent->pVo.track_preference;
    
    eventParameters["sumTrackDifference"] = sumTrackDifference; //Summary of Track Difference
    eventParameters["trackCount"] = m_trackCount; //Track Count
    eventParameters["remainingTrackCount"] = remainingTrackCount; //Remaining Track Count to the Finish Line
    
    eventParameters["raceTimeLeft"] = AnalyticsHelper::convertFloatTo2DecimalDouble(_raceTimeLeft); //Race Time Remaining when race ends (Rounded to 1 decimal place)
    eventParameters["petHP"] = m_pet_player->getHorsepower();
    eventParameters["enemyPetHP"] = m_pet_opponent->getHorsepower();
    eventParameters["HPDifference"] = (int)m_pet_player->getHorsepower() - (int)m_pet_opponent->getHorsepower();
    eventParameters["isPVP"] = !(g_opponentState->player.info.bot); //Is a Real Player? (Not company-designed ranches)
    
    int helperHP = 0;
    int helperLevel = 0;
    if (m_pet_helper != NULL) {
        
        helperHP = m_pet_helper->getHorsepower();
        helperLevel = m_pet_helper->getLevel();
    }
    
    eventParameters["helperHP"] = helperHP;
    eventParameters["helperLevel"] = helperLevel;
    
    eventParameters["petPrefer"] = (playerTrackPreference == trackType); //Player is using a pet that has the same track preference as the track
    eventParameters["enemyPrefer"] = (opponentTrackPreference == trackType); //Opponent is using a pet that has the same track preference as the track
    eventParameters["petType"] = m_pet_player->ppVo.type.c_str();
    eventParameters["enemyPetType"] = m_pet_opponent->ppVo.type.c_str();
    eventParameters["petLevel"] = m_pet_player->ppVo.level;
    eventParameters["enemyPetLevel"] = m_pet_opponent->ppVo.level;
    
    eventParameters["aiDifficulty"] = AnalyticsHelper::convertFloatTo2DecimalDouble(g_playerState->player.racing.ai_difficulty);
    
    //TODO: collect race duration
    //TODO: ability usage, how many times for each ability, what is the last cost for each ability, energy used, energy regenerated
    //TODO: Time you stay ahead, time you stay behind
    
    return eventParameters;
}

void IsoMapRacing::submitRaceStartEvent()
{
    CCLog_("submitRaceStartEvent");
    CCDictionary* params = createRaceStartEventParametersInDictionary();
    FlurryCpp::logEvent(FlurryEvent::RACE_START, params);
    
    CSJson::Value eventParameters;
    eventParameters["raceStart"] = createRaceStartEventParameters();
    AnalyticsClient::getInstance()->logEvent(FlurryEvent::RACE_START, eventParameters);
}

void IsoMapRacing::submitRaceEndEvent(bool playerWon)
{
    CCLog_("submitRaceEndEvent");
    
    CCDictionary* params = createRaceStartEventParametersInDictionary();
    FlurryCpp::logEvent(FlurryEvent::RACE_END, params);
    
    CSJson::Value eventParameters;
    eventParameters["raceStart"] = createRaceStartEventParameters();
    eventParameters["raceEnd"] = createRaceEndEventParameters(playerWon);
    AnalyticsClient::getInstance()->logEvent(FlurryEvent::RACE_END, eventParameters);
    
    const char * eventName = "race_error";
    
    if(playerWon)
    {
        FlurryCpp::logEvent(FlurryEvent::RACE_END_PLAYER_WON, params);
        
        eventName = EVENT_RACE_WON;
    }
    else if(m_giveUp)
    {
        FlurryCpp::logEvent(FlurryEvent::RACE_END_PLAYER_GAVEUP, params);
        
        eventName = EVENT_RACE_GAVEUP;
    }
    else
    {
        FlurryCpp::logEvent(FlurryEvent::RACE_END_AI_WON, params);
        
        eventName = EVENT_RACE_LOST;
    }
    
    FlurryCpp::endTimedEvent(EVENT_RACE_NORMAL_PLAY, NULL);
    
    FBAppEventsWrapper::logEvent(eventName);
}

CCArray* IsoMapRacing::addVisitor(int count)
{
    PlayerState *_mapPlayerState = (PlayerState*)mapPlayerState;
    
    CCPointArray *points = CCPointArray::create(_mapPlayerState->Tracks.size()*3);
    CCArray *tracks = CCArray::create();
    CCArray *flips = CCArray::create();
    for (vector<Track*>::iterator it=_mapPlayerState->Tracks.begin(); it!=_mapPlayerState->Tracks.end(); ++it) {
        if (Track* _track = (Track*)(*it)) {
            
            if (Track* startTrack = getTrackAtIndex(_mapPlayerState->player.racing.start)) {
                if (_track == startTrack) {
                    continue;
                }
            }
            
            for (int i = 0; i < 4; i++) {
                CCPoint randomPoint = _track->getPosition() + ccp((i == 0 ? 1 : -1)*_track->getContentSize().width*0.5, _track->getContentSize().height*0.5);
                CCPoint positionPoint = _track->getPosition() + ccp((i == 0 ? 1 : -1)*_track->getContentSize().width*0.25, _track->getContentSize().height*0.25);
                
                if (i == 0) {
                    randomPoint = _track->getPosition() + ccp(_track->getContentSize().width*0.5, _track->getContentSize().height*0.5);
                    positionPoint = _track->getPosition() + ccp(_track->getContentSize().width*0.25, _track->getContentSize().height*0.25);
                } else if (i == 1) {
                    randomPoint = _track->getPosition() + ccp(-_track->getContentSize().width*0.5, _track->getContentSize().height*0.5);
                    positionPoint = _track->getPosition() + ccp(-_track->getContentSize().width*0.25, _track->getContentSize().height*0.25);
                } else if (i == 2) {
                    randomPoint = _track->getPosition() + ccp(-_track->getContentSize().width*0.5, -_track->getContentSize().height*0.5);
                    positionPoint = _track->getPosition() + ccp(-_track->getContentSize().width*0.25, -_track->getContentSize().height*0.25);
                } else if (i == 3) {
                    randomPoint = _track->getPosition() + ccp(_track->getContentSize().width*0.5, -_track->getContentSize().height*0.5);
                    positionPoint = _track->getPosition() + ccp(_track->getContentSize().width*0.25, -_track->getContentSize().height*0.25);
                }
                
                int tileIndex = IsoMap::getTrackIndexByPosition(randomPoint, g_gameContent->tileSize, g_gameContent->tileShift);
                Track *tileTrack = IsoMap::getTrackAtIndex(tileIndex);
                if (tileTrack == NULL) {
                    float x = _track->getContentSize().width;
                    float y = _track->getContentSize().height;
                    float a = atan2f(y, x);
                    for (int j = 0; j < 3; j++) {
                        if (i == 0) {
                            CCPoint point = positionPoint + ccp((j - 1)*-_track->getContentSize().width*0.2*cosf(a), (j - 1)*_track->getContentSize().width*0.2*sinf(a));
                            points->addControlPoint(point);
                            flips->addObject(CCNumber::create(1));
                        } else if (i == 1){
                            CCPoint point = positionPoint + ccp((j - 1)*_track->getContentSize().width*0.2*cosf(a), (j - 1)*_track->getContentSize().width*0.2*sinf(a));
                            points->addControlPoint(point);
                            flips->addObject(CCNumber::create(0));
                        } else if (i == 2){
                            CCPoint point = positionPoint + ccp((j - 1)*-_track->getContentSize().width*0.2*cosf(a), (j - 1)*_track->getContentSize().width*0.2*sinf(a));
                            points->addControlPoint(point);
                            flips->addObject(CCNumber::create(0));
                        } else if (i == 3){
                            CCPoint point = positionPoint + ccp((j - 1)*_track->getContentSize().width*0.2*cosf(a), (j - 1)*_track->getContentSize().width*0.2*sinf(a));
                            points->addControlPoint(point);
                            flips->addObject(CCNumber::create(1));
                        }
                        tracks->addObject(_track);
                    }
                }
            }
        }
    }
    
    CCArray *_visitors = CCArray::create();
    while (count > 0 && points->count() > 0) {
        int rand = arc4random()%points->count();
        CCNumber *flip = (CCNumber *)flips->objectAtIndex(rand);
        CCPoint randomPoint = points->getControlPointAtIndex(rand);
        Track *track = (Track *)tracks->objectAtIndex(rand);
        flips->removeObjectAtIndex(rand);
        points->removeControlPointAtIndex(rand);
        tracks->removeObjectAtIndex(rand);
        
        Visitor *_visitor = Visitor::createWithRandomSpineName();
//        _visitor->setVisitorState(VTS_JUMP1);
        _visitor->startRepeatAction();
        _visitor->isoMap = this;
        if (is_replay) {
            _visitor->m_pet = m_pet_opponent;
        } else {
            _visitor->m_pet = m_pet_player;
        }
        _visitor->setScaleX((flip->getIntValue() ? 1 : -1));
        _visitor->setPosition(randomPoint);
        this->addChild(_visitor, int(IsoMap::getContentSize().height - _visitor->getPosition().y));
        _visitors->addObject(_visitor);
        
        track->addVisitor(_visitor);
        count--;
    }
    return _visitors;
}

void IsoMapRacing::petWillArriveOnTrack(Pet *pet, Track *track)
{
    if (is_replay) {
        if (m_pet_opponent == pet) {
//            track->visitorDropCoin();
        }
    } else {
        if (m_pet_player == pet) {
            track->visitorDropCoin();
        }
    }
}

void IsoMapRacing::activeGlow(float second)
{
    if (is_replay) {
        m_pet_opponent->setActiveGlowing(true);
    } else {
        m_pet_player->setActiveGlowing(true);
        this->runAction(CCSequence::create(CCDelayTime::create(second),
                                           CCCallFunc::create(this, callfunc_selector(IsoMapRacing::deActiveGlow)),
                                           NULL));
//        m_pet_player->useDash();
    }
}

void IsoMapRacing::deActiveGlow()
{
    if (is_replay) {
        m_pet_opponent->setActiveGlowing(false);
    } else {
        m_pet_player->setActiveGlowing(false);
    }
}

void IsoMapRacing::runUserActivity()
{
    /*
    if (CCArray *spins = (CCArray *)userReplayRecord->objectForKey("spins")) {
        for (int i = 0; i < spins->count(); i++) {
            CCDictionary *spin = (CCDictionary *)spins->objectAtIndex(i);
            CCNumber *start_time = (CCNumber *)spin->objectForKey("start_time");
            CCNumber *finish_time = (CCNumber *)spin->objectForKey("finish_time");
//            CCString *action = (CCString *)spin->objectForKey("action");
            
//            this->runAction(CCSequence::create(CCDelayTime::create(start_time->getDoubleValue()),
//                                               CCCallFuncO::create(this, callfuncO_selector(IsoMapRacing::startSpinWithData), spin),
//                                               NULL));
//            
//            this->runAction(CCSequence::create(CCDelayTime::create(finish_time->getDoubleValue()),
//                                               CCCallFuncO::create(this, callfuncO_selector(IsoMapRacing::finishSpinWithData), spin),
//                                               NULL));
        }
    }
     */
}

float IsoMapRacing::getTimeScale() const
{
    return timeScale;
}

void IsoMapRacing::setTimeScale(float toSet) {
    timeScale = toSet;
}

#pragma mark - Powerup

bool IsoMapRacing::useDash(cocos2d::CCObject *sender)
{
    bool useDash = m_pet_player->useDash();
    
    if (useDash)
    {
        racePanel->updateCostOnButton(sender, m_pet_player->getDashCost());
        racePanel->checkSkillCost(m_pet_player->remainingCost());
    }
    
    return useDash;
}

bool IsoMapRacing::useTrap(cocos2d::CCObject *sender)
{
    bool useTrap = m_pet_player->useTrap();
    
    if (useTrap)
    {
        racePanel->updateCostOnButton(sender, m_pet_player->getTrapCost());
        racePanel->checkSkillCost(m_pet_player->remainingCost());
    }
    
    return useTrap;
}

bool IsoMapRacing::useStomp(cocos2d::CCObject *sender)
{
    bool useStomp = m_pet_player->useStomp();
    
    if (useStomp)
    {
        racePanel->updateCostOnButton(sender, m_pet_player->getStompCost());
        racePanel->checkSkillCost(m_pet_player->remainingCost());
    }
    
    return useStomp;
}

bool IsoMapRacing::useShockwaveStraight1(cocos2d::CCObject *sender)
{
    bool useShockwave1 = m_pet_player->useShockwaveStraight1();
    
    if (useShockwave1)
    {
        racePanel->updateCostOnButton(sender, m_pet_player->getShockwave1Cost());
        racePanel->checkSkillCost(m_pet_player->remainingCost());
    }
    
    return useShockwave1;
}

bool IsoMapRacing::useShockwaveStraight2(cocos2d::CCObject *sender)
{
    bool useShockwave2 = m_pet_player->useShockwaveStraight2();
    
    if (useShockwave2)
    {
        racePanel->updateCostOnButton(sender, m_pet_player->getShockwave2Cost());
        racePanel->checkSkillCost(m_pet_player->remainingCost());
    }
    
    return useShockwave2;
}

void IsoMapRacing::petIsAttacked(Pet *attackingPet, Pet *attackedPet, float stunDuration, SKILL skillUsed)
{
    if (attackedPet == m_pet_opponent)
    {
        m_AI_on_opponent->aiIsAttacked(stunDuration, skillUsed);
        
        racePanel->animateBallEffect(BallEffectTypeFromOpponent ,racePanel->m_lblTimer, 0.45f);
        
        addTime(ATTACK_BONUS_TIME_ADDED, "ATTACK BONUS");
        
        if (m_pet_helper) {
            if (attackingPet == m_pet_helper) {
                
                if (m_pet_helper->m_helperStunAmount > 0) {
#ifndef DEBUG_HELPER_USE_SKILL_FOREVER
                    m_pet_helper->m_helperStunAmount--;
                    CCLog_("m_pet_helper->m_helperStunAmount %d->%d",m_pet_helper->m_helperStunAmount+1,m_pet_helper->m_helperStunAmount);
#endif
                }
                
                if (m_pet_helper->m_helperStunAmount <= 0) {
                    m_delayUntilHelperWillBeRemoved = stunDuration;
                    CCLog_("m_delayUntilHelperWillBeRemoved :%f",m_delayUntilHelperWillBeRemoved);
                }
            }
        }
    }
    else if (attackedPet == m_pet_player)
    {
        if (m_AI_on_player) m_AI_on_player->aiIsAttacked(stunDuration, skillUsed);
    }
    
    if (attackingPet->opponentPet == attackedPet) {
        if (attackingPet == m_pet_opponent)
        {
            m_AI_on_opponent->opponentIsAttacked(stunDuration, skillUsed);
        }
        else if (attackingPet == m_pet_player)
        {
            if (m_AI_on_player) m_AI_on_player->opponentIsAttacked(stunDuration, skillUsed);
        }
    }
}

void IsoMapRacing::skillIsUsed(Pet *usingSkillPet, SKILL skillUsed)
{
    if (usingSkillPet == m_pet_player)
    {
        m_AI_on_opponent->opponentUsedSkill(skillUsed);
    }
    else if (usingSkillPet == m_pet_opponent)
    {
        if (m_AI_on_player) m_AI_on_player->opponentUsedSkill(skillUsed);
    }
}


////////////////////////////////////////////////////////
// Callback method
////////////////////////////////////////////////////////

#pragma mark - API Callback

//void IsoMapRacing::onRaceUpdated(void *sender_, std::string resp_json_str_) {
//    
//    if (SceneRaceCommand* _command = (SceneRaceCommand*)g_currentScene) {
//        
//        if (Json *_root = Json_create(resp_json_str_.c_str())) {
//            if (Json_getItem(_root, "coins_reward")) {
//                _command->raceResultPanel->coins_reward = Json_getInt(_root, "coins_reward", 0);
//            }
//            if (Json_getItem(_root, "fans_won")) {
//                _command->raceResultPanel->fans_won = Json_getInt(_root, "fans_won", 0);
//            }
//            if (Json *_state = Json_getItem(_root, "state")) {
//                _command->raceResultPanel->win_streak = Json_getInt(_state, "win_streak", 0);
//                CCLog_("Win Streak = %d", _command->raceResultPanel->win_streak);
//            }
//            Json_dispose(_root);
//        }
//        
//    }
//    
//}


#pragma mark - CCNikScrollViewDelegate

void IsoMapRacing::zoom() {
    
    SceneRace* _scene = (SceneRace*)g_currentScene;
    _scene->scrollView->zoomOnPosition(m_pet_player->getPosition() - this->getContentSize()/4.f, RACING_MAXIMUM_SCALE_FOR_ZOOMING, RACING_ZOOM_DURATION);
    
}

void IsoMapRacing::scrollViewDidEndZoomingAnimation(CCNikScrollView *view) {
    
    SceneRace* _scene = (SceneRace*)g_currentScene;
    _scene->scrollView->setCameraMode(true);
    
    if (!m_tutorialMode) {
        racePanel->showGauge();
    }
    
    if(m_pet_player->isPlaybackReplayDataEnabled())
        racePanel->hideGauge(0.f);
    
}


