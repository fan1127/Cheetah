//
//  IsoMapRacing.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/20/56 BE.
//
//

#ifndef __Cheetah__IsoMapRacing__
#define __Cheetah__IsoMapRacing__

#include "IsoMap.h"

#include "Pet.h"
#include "../UIs/RacePanel.h"
#include "../UIs/RaceResultPanel.h"

#include "../../extensions/CCArmature/external_tool/Json/lib_json/value.h"
//#include "reader.h"
#include "../../extensions/CCArmature/external_tool/Json/lib_json/writer.h"

#include "PlayerState.h"

//#define MAX_PET 2

// PET_RUNNING_ONE_STROKE_PER_TIME / (2^(n-1))
#define PET_RUNNING_STROKE_PER_TIME                     ((16.0f*2.0f)/100.0f) // (16*2) 30 frame per seconds
#define PET_RUNNING_STROKE_EMITTER_REUSE_COUNT          1

#define RANKCHANGED_POPOVER_DISPLAY_TIME_DIFF_PET       3.0f
#define RANKCHANGED_POPOVER_DISPLAY_TIME_SAME_PET       10000.0f

#define DELAY_START_FIRST_OR_LATER                      1.5f

#define CAMERA_ZOOMING_SPEED                            0.5f //lower is faster

#define HELPER_TIME_TO_REMOVE   5.0f

#define EMISSION_RATE_A (-0.002030456853)
#define EMISSION_RATE_B (0.0330456852)

#define AI_DIFF_INCREASE_HIGHEST_HP 0.18f
#define AI_DIFF_INCREASE_LOWEST_HP 0.1f
#define AI_DIFF_DECREASE_HIGHEST_HP 0.22f
#define AI_DIFF_DECREASE_LOWEST_HP 0.15f

#define RACING_MINIMUM_SCALE_FOR_ZOOMING (_scene->scrollView->getMinimumScale()*1.2f)
#define RACING_MINIMUM_SCALE_FOR_ZOOMING_2 (scrollView->getMinimumScale()*1.2f)
#define RACING_MAXIMUM_SCALE_FOR_ZOOMING (_scene->scrollView->getMaximumScale())
#define RACING_ZOOM_DURATION (0.8f)//(0.1)
#define RACING_INSTANT_ZOOM_DURATION (0.0f) //less than 0.001

enum MapRacingState {
    MRS_PREPARE,
    MRS_READY,
    MRS_GO,
    MRS_FINISH,
};

enum WinState {
    UNKNOWN_WIN,
    PLAYER_WIN,
    OPPONENT_WIN,
};

class ParticleEmitter;
class AI;

class IsoMapRacing: public IsoMap, public CCNikScrollViewDelegate {
    
    MapRacingState m_racingState;
    MapRacingState m_racingStatePlayed;
    
    WinState winState;
    
    bool is_replay;
    bool should_frozen_pet_player;
    
    double time;
    float timeScale;

//    void *m_camControl = NULL;
    
    float m_helperTime;
//    bool m_helperHasAttacked;
    float m_delayUntilHelperWillBeRemoved;
    
    float m_rankPopoverDisplayTime;
    
    bool m_gameIsPaused;
    bool m_tutorialMode;
    
    bool m_isRemovingHelperPet;
    
    CCNode *m_groupSpineEffectsAboveAnimals;
    CCNode *m_groupNodeEffects;
    
    CCArray *m_effectsArray;
    CCArray *m_shockwaveArray;

    void submitRaceStartEvent();
    void submitRaceEndEvent(bool playerWon);
    cocos2d::CCDictionary* createRaceStartEventParametersInDictionary();
    CSJson::Value createRaceStartEventParameters();
    CSJson::Value createRaceEndEventParameters(bool playerWon);

    std::string helperPetReplayData;
    std::string helperPetType;

public:

    // Effect
    //cocos2d::CCLayer        *_effectLayer                           = NULL;
    
    ParticleEmitter         *_emitter;
    ParticleEmitter         *_emitter2;
    ParticleEmitter         *_emitter3;
    
    bool                    m_giveUp;
    
    bool                    _timeUpDisplayed;
    float                   _raceTimeLeft                           = 0;
    float                   _checkpointBonusTime                    = 0;
    float                   _totalLaps                              = 0;
    
    int m_trackCount;
    
    cocos2d::CCDictionary *userReplayRecord = NULL;
    
    AI *m_AI_on_opponent;
    AI *m_AI_on_player;
    AI *m_AI_on_helper;
    
    Pet *m_pet_opponent;
    Pet *m_pet_player;
    Pet *m_pet_helper;
    
    Pet *m_pet_helper_cache;
    
    RacePanel *racePanel;
    RaceResultPanel *raceResultPanel;
    
    CCNode *groupSpineEffectsAboveAnimals(){ return m_groupSpineEffectsAboveAnimals; };
    CCNode *groupNodeEffect(){ return m_groupNodeEffects; };
    CCArray *effectsArray(){ return m_effectsArray; };
    CCArray *shockwaveArray(){ return m_shockwaveArray; };
    
    
    void setTutorialMode(bool tutorialMode_) { m_tutorialMode = tutorialMode_; }
    bool isTutorialMode() { return m_tutorialMode; }
    
    bool gameIsPaused() { return m_gameIsPaused; };
    
    static IsoMapRacing *create(RacePanel *racePanel_);
    
    virtual bool init();
    virtual void cleanup();

    void compose();
    
    ~IsoMapRacing();
    
    void startUpdate();
    
    void addAIToPlayer();
    
    void pauseGame();
    void resumeGame();
    
    virtual void update(float delta);
    
    //Timer label
    void addTime(float timeToAdd, const char* title);
    void updateTime();
    
    void playReplay();
    
    void changeRacingState(MapRacingState state);
    void startRunningWithGaugeResult(GaugeRESULT result);
    void opponentStartRunning();
    void delayStartRunning(Pet *pet);
    void startRunningAfterFreez();
    void updateCurrentLap(Pet *playerPet);
    void updateLapWithPetPlayer(Pet *pet);
    void finishRunningWithPetPlayer(Pet *pet);
    void stopRunningWithPetPlayer(Pet *pet);
    
    Pet* createHelperPet(PlayerHelperPetVO helperPetVo);
    Pet* createHelperPet(std::string petType);
    void addHelperPet();
    void playDropPetSound();
    void startHelperPet();
    void removeHelperPet();
    void deleteHelperPet();
    
    void showResult();
    
    bool useDash(cocos2d::CCObject *sender);
    bool useTrap(cocos2d::CCObject *sender);
    bool useStomp(cocos2d::CCObject *sender);
    bool useShockwaveStraight1(cocos2d::CCObject *sender);
    bool useShockwaveStraight2(cocos2d::CCObject *sender);
    
    void petIsAttacked(Pet *attackingPet, Pet *attackedPet, float stunDuration, SKILL skillUsed);
    void skillIsUsed(Pet *usingSkillPet, SKILL skillUsed);
    
    cocos2d::CCArray* addVisitor(int count);
    void petWillArriveOnTrack(Pet *pet, Track *track);

    void activeGlow(float second);
    void deActiveGlow();
    
    void runUserActivity();

    float getTimeScale() const;
    void setTimeScale(float toSet);

    ////////////////////////////////////////////////////////
    // Callback method
    ////////////////////////////////////////////////////////
    
    void zoom();
    
//    static void onRaceUpdated(void* sender_, std::string resp_json_str_);
    
    virtual void scrollViewDidEndZoomingAnimation(CCNikScrollView *view);
    
    virtual void draw();
    
    
#pragma mark - Callback functions
    
    void* onRaceResultSender;
    void (*onShowRaceResult)(void* sender_, bool playerLose);
};

#endif /* defined(__Cheetah__IsoMapRacing__) */
