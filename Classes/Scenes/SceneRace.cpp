//
//  SceneRace.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/13/56 BE.
//
//

#include "SceneRace.h"
USING_NS_CC;
using namespace std;

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Entities/PlayerState.h"
extern PlayerState* g_opponentState;
extern PlayerState* g_playerState;
PlayerState* g_playerState_restore = NULL;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Settings.h"
#include "../Helpers/CustomEventsHelper.h"

#include "../Extensions/WeatherEffects/WeatherEnvironment.h"
#include "../UIs/BlockingLayer.h"

bool SceneRace::init() {
    
    if (!CCScene::init()) {
        return false;
    }
    
    if (!g_playerState->Checklist[TutFirstRace::key()]) {
        
        PlayerPetVO ppVo = PlayerPetVO();
        ppVo.top_speed = 100; //will be overrided later at TutFirstRace
        ppVo.corner_speed = 100; //will be overrided later at TutFirstRace
        ppVo.jumping = 100; //will be overrided later at TutFirstRace
        ppVo.type = "cat1_7_1"; //will be overrided later at TutFirstRace
        PlayerState* t_playerState = new PlayerState();
        GDPetVO pVo = g_gameContent->getPetVO(ppVo.type);
        if (pVo.type == "") {
            CCLog_("No PetVO was found!");
        }
        Pet* pet = Pet::createWithPetVO(pVo, ppVo);
        pet->retain();
        t_playerState->Pets.push_back(pet);
        
        t_playerState->PetRunner = pet;
        t_playerState->playerPet = pet->ppVo;
        
    
        Ability* ablt_dash = Ability::create();
        ablt_dash->retain();
        ablt_dash->aVo = GDAbilityVO(g_gameContent->getAbilityVO("dash_forward", 1));
        ablt_dash->paVo = PlayerAbilityVO(ablt_dash->aVo);
        t_playerState->Abilities["dash_forward"] = ablt_dash;
        
        Ability* ablt_trap = Ability::create();
        ablt_trap->retain();
        ablt_trap->aVo = GDAbilityVO(g_gameContent->getAbilityVO("banana_trap", 1));
        ablt_trap->paVo = PlayerAbilityVO(ablt_trap->aVo);
        t_playerState->Abilities["banana_trap"] = ablt_trap;
        
        Ability* ablt_stomp = Ability::create();
        ablt_stomp->retain();
        ablt_stomp->aVo = GDAbilityVO(g_gameContent->getAbilityVO("stomp", 1));
        ablt_stomp->paVo = PlayerAbilityVO(ablt_stomp->aVo);
        t_playerState->Abilities["stomp"] = ablt_stomp;
        
        g_playerState_restore = g_playerState;
        g_playerState = t_playerState;
        
        
        for (map<string, Ability*>::iterator it = g_opponentState->Abilities.begin(); it != g_opponentState->Abilities.end(); ++it) {
            Ability* _old_ablt = (*it).second;
            _old_ablt->release();
        }
        g_opponentState->Abilities.clear();
        
        {
            Ability* ablt_dash = Ability::create();
            ablt_dash->retain();
            ablt_dash->aVo = GDAbilityVO(g_gameContent->getAbilityVO("dash_forward", 1));
            ablt_dash->paVo = PlayerAbilityVO(ablt_dash->aVo);
            g_opponentState->Abilities["dash_forward"] = ablt_dash;
            
            Ability* ablt_trap = Ability::create();
            ablt_trap->retain();
            ablt_trap->aVo = GDAbilityVO(g_gameContent->getAbilityVO("banana_trap", 1));
            ablt_trap->paVo = PlayerAbilityVO(ablt_trap->aVo);
            g_opponentState->Abilities["banana_trap"] = ablt_trap;
            
            Ability* ablt_stomp = Ability::create();
            ablt_stomp->retain();
            ablt_stomp->aVo = GDAbilityVO(g_gameContent->getAbilityVO("stomp", 1));
            ablt_stomp->paVo = PlayerAbilityVO(ablt_stomp->aVo);
            g_opponentState->Abilities["stomp"] = ablt_stomp;
            
        }
        
        if (Pet *_pet = g_playerState->PetRunner) { //g_playerState->getPet(g_playerState->player.racing.host)) {
            
            g_opponentState->opponentInfo = g_opponentState->player.info;
            
            if (Pet* host_pet = g_opponentState->getPet(g_opponentState->player.racing.host)) {
                g_opponentState->opponentPet = host_pet->ppVo;
            }else {
                //** warning can not race and return
                g_opponentState->opponentPet = g_opponentState->Pets[0]->ppVo;
            }
            g_opponentState->opponentTime = 0.0f;
            
            g_opponentState->playerInfo =g_playerState->player.info;
            g_opponentState->playerPet = _pet->ppVo;
            g_opponentState->playerTime = 0.0f;
            
            g_opponentState->replayGuestRaw = "";
            g_opponentState->replayHostRaw = "";
        }
    } else if(g_opponentState->replayGuestRaw != "") {
        g_opponentState->playerPet = g_opponentState->getPet(g_opponentState->player.racing.host)->ppVo;
    }

    racePanel = RacePanel::create();
    this->addChild(racePanel, 10);
    
    IsoMapRacing *_isoMapRacing = IsoMapRacing::create(racePanel);
    
    scrollView = CCNikScrollView::create();
    scrollView->setTouchEnabled(false);
    scrollView->setDelegate(_isoMapRacing);
    _isoMapRacing->setPosition(_isoMapRacing->getContentSize()*-1/4.f);
    scrollView->addChild(_isoMapRacing);
    scrollView->setContentView(_isoMapRacing->getContentSize()/2.f);
    scrollView->setMaximumScale(2.f);
    scrollView->setMinimumScale(scrollView->getContentSize().width/(_isoMapRacing->getContentSize().width/2.f));
    scrollView->zoomOnPosition(_isoMapRacing->m_pet_player->getPosition() -
                               this->getContentSize()/4.f, RACING_MINIMUM_SCALE_FOR_ZOOMING_2, RACING_INSTANT_ZOOM_DURATION);
    scrollView->setTouchEnabled(false);
    this->addChild(scrollView, 1);
    
    raceResultPanel = RaceResultPanel::create(_isoMapRacing->m_pet_player);
    this->addChild(raceResultPanel, 11);
    _isoMapRacing->raceResultPanel = raceResultPanel;
    _isoMapRacing->compose();
    
    tutMgr = CTutManager::create();
#ifndef DEBUG_SKIP_FIRST_RACE
    tutMgr->addTut(TutFirstRace::create(_isoMapRacing));
#endif
    tutMgr->addTut(TutSecondRacePart4::create(_isoMapRacing, racePanel));
    tutMgr->addTut(TutRaceResult::create(_isoMapRacing));
    addChild(tutMgr, 2000);
    
    g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_RACE, true, 0.25f, 0.25f, 0.25);
    
    isoMap = _isoMapRacing;
    
//#ifdef ENABLE_SNOW
//    addChild(WeatherEnvironment::create(scrollView, isoMap, PARTICLE_TYPE_SNOW, SNOW_DEFAULT_AMOUNT), 2);
//#endif

    CustomEventsHelper::Instance().onEnterRaceScene();

    g_gameAudio->preloadEffects(GameAudioService::Race);

    return true;
}

void SceneRace::fadeOutThisSceneAndChangeTo(int sceneIndex) {

    if (SceneManager *_sceneManager = (SceneManager*)g_sceneManager) {
        _sceneManager->sceneSelector = (SceneList)sceneIndex;
        if (_sceneManager->transition) {
            _sceneManager->transition->fadeSceneOut(_sceneManager);
        }
    }
}
