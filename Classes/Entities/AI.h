//
//  AI.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/11/2557 BE.
//
//

#ifndef __Cheetah__AI__
#define __Cheetah__AI__

#include "cocos2d.h"
#include "../Entities/Pet.h"

#include "../Settings.h"

USING_NS_CC;

class IsoMapRacing;

#pragma mark - Macros

#define LAST_ROUND_SKILL_DELAY_RANDOM (arc4random()%2)
#define NORMAL_ROUND_SKILL_DELAY_RANDOM (arc4random()%2)
#define NORMAL_ROUND_SKILL_DELAY_AT_LEAST (1)
#define IS_LAST_ROUND (OPPONENT_IS_LAST_ROUND || PLAYER_IS_LAST_ROUND)
#define PLAYER_IS_LAST_ROUND m_pet_opponent_of_ai->ran_round >= m_pet_opponent_of_ai->max_round - 1
#define OPPONENT_IS_LAST_ROUND (m_pet_ai->ran_round >= m_pet_ai->max_round - 1)

//#define randNum(X, A, B) ((A) + ((X) % ((B)-(A))))
#define HASABILITY(X) (m_pet_ai->playerState->hasAbilityForKey(X))
#define GETABILITY(X) (m_pet_ai->playerState->getAbilityForKey(X))
#define GETABILITY_FROM_OPPONENT(X) (m_pet_opponent_of_ai->playerState->getAbilityForKey(X))


//settings
#define ATTACK_REACTION_TIME 0.16f//0.14f was the number first tested
#define DEFENSE_REACTION_TIME 0.2f
#define BASE_DEFENSE_REACTION_STOMP 0.7f
#define BASE_DEFENSE_REACTION_DASH 0.8f
#define ADDITIONAL_DEFENSE_REACTION_STOMP 0.7f
#define ADDITIONAL_DEFENSE_REACTION_DASH 0.3f
#define FAILING_ATTACK_INTERVAL 0.125f //higher value will cause each random chance fail to wait longer //higher horsepower means less interval
#define FAILING_DEFENSE_INTERVAL 0.25f
#define WAIT_TO_SKILL_TIME 0.0f //min horsepower gets 0.1, max horsepower animals gets 0.0// this affects accuracy

#define MAX_HORSEPOWER_RATIO_FOR_AI 0.85f //meaning that 470*0.85 will equal 399.5, meaning 376 or above will get a full 1 difficulty range

#define FIND_DASH_DISTANCE -1

//start first
#define DEC_DO_NOTHING 0
#define DEC_DASH_DASH 1
#define SETTING_DASH_DASH_maxDashTries 2

//start same
#define DEC_STOMP_AFTER_RAND_TIME 1
#define DEC_SHOOT_STRAIGHT_1_AFTER_RAND_TIME 2

#define AI_STOMP_AREA_RATIO_TO_TILESIZE 0.75f*STOMP_AREA_RATIO_TO_TILESIZE
#define AI2_STOMP_AREA_RATIO_TO_TILESIZE 1.0f*STOMP_AREA_RATIO_TO_TILESIZE

#define AI_SHOCKWAVE_TILE_PERCENT_SHOCKWAVE1 0.7f//0.9f//0.7f
#define AI_SHOCKWAVE_TILE_PERCENT_SHOCKWAVE2 0.65f//1.0f//0.65f

#define AI_SHOCKWAVE_DEFEND_DASH_HOW_FAR 0.65f
#define AI_SHOCKWAVE_DEFEND_STOMP_HOW_BIG_IS_THE_AREA 1.5f

#define IF_HASNT_USED_SKILL_YET__THEN_DO if (_useSkill == SKILL_NONE) _useSkill =
#define IF_HASNT_USED_SKILL_YET__THEN if (_useSkill == SKILL_NONE)

#define AI_DELTA 0.016666667f

//abilities parameters
#define STOMP_DELAY_TIME aVo.param_1
#define STOMP_SPEED_PERCENT aVo.param_2
#define DASH_DISTANCE aVo.param_1
#define DASH_TIME aVo.param_2
#define SHOCKWAVE1_DELAY1 aVo.param_1
#define SHOCKWAVE2_DELAY1 aVo.param_1
#define SHOCKWAVE1_DELAY2 aVo.param_2
#define SHOCKWAVE2_DELAY2 aVo.param_2


//time to do at least something
#define TIME_TO_DO_SOMETHING_MIN 1.0f
#define TIME_TO_DO_SOMETHING_MAX 4.0f

//skill usage frequency, let's call it quota
#define MAX_QUOTA_SLOT_MINHP 3 //from lowest horsepower
#define MAX_QUOTA_SLOT_MAXHP 10 //to highest horsepower
#define TIME_TO_RELEASE_ONE_QUOTA_SLOT_FROM_MINHP 0.75f
#define TIME_TO_RELEASE_ONE_QUOTA_SLOT_TO_MINHP 1.5f

#define TIME_TO_RELEASE_ONE_QUOTA_SLOT_FROM_MAXHP 0.125f //maxhp will get lower value (stronger) 2.0f
#define TIME_TO_RELEASE_ONE_QUOTA_SLOT_TO_MAXHP 0.25f


#pragma mark - Find Position Macros


#pragma mark A

//PRE_FIND_POSITION_A_DEFAULT
#define PRE_FIND_POSITION_A_DEFAULT(PET)\
PRE_FIND_POSITION_A(PET->m_currSpeed,PET->m_currTrackPosition,PET->m_currTrack,PET->m_dashTime,PET->m_stompTime,PET->m_shockwave1Time,PET->m_shockwave2Time,PET->m_usingDash,PET->m_usingStomp,PET->m_usingShockwaveStraight1,PET->m_usingShockwaveStraight2,PET->m_dashSpeed,PET->m_stompSpeed,PET->m_shockwave1Speed,PET->m_shockwave2Speed)

//PRE_FIND_POSITION_A
#define PRE_FIND_POSITION_A(current_speed,trackPosition,curr_track,dashTime,stompTime,shockwave1Time,shockwave2Time,usingDash,usingStomp,usingShockwave1,usingShockwave2,dashSpeed,stompSpeed,shockwave1Speed,shockwave2Speed)\
float a_current_speed = current_speed;float a_new_speed = 0.0f;float a_trackPosition = trackPosition;Track *a_curr_track = curr_track;float a_dashTime = dashTime;float a_stompTime = stompTime;float a_shockwave1Time = shockwave1Time;float a_shockwave2Time = shockwave2Time;bool a_usingDash = usingDash;bool a_usingStomp = usingStomp;bool a_usingShockwave1 = usingShockwave1;bool a_usingShockwave2 = usingShockwave2;bool a_dashSpeed = dashSpeed;bool a_stompSpeed = stompSpeed;bool a_shockwave1Speed = shockwave1Speed;bool a_shockwave2Speed = shockwave2Speed;

//FIND_POSITION_A
#define FIND_POSITION_A(PET)\
PET->getNewSpeed(AI_DELTA,a_current_speed,&a_new_speed,a_curr_track,&a_dashTime,&a_stompTime,&a_shockwave1Time,&a_shockwave2Time,&a_usingDash,&a_usingStomp,&a_usingShockwave1,&a_usingShockwave2,a_dashSpeed,a_stompSpeed,a_shockwave1Speed,a_shockwave2Speed);a_current_speed = a_new_speed;a_trackPosition += (a_current_speed * AI_DELTA);int a_trackCount = 0;a_curr_track = PET->getTrackFromTrackPosition(a_curr_track, &a_trackPosition, &a_trackCount);

//POST_FIND_POSITION_A
#define POST_FIND_POSITION_A(PET)PET->getPositionFromTrack(a_curr_track, a_trackPosition);

#pragma mark B

//PRE_FIND_POSITION_A_DEFAULT
#define PRE_FIND_POSITION_B_DEFAULT(PET)\
PRE_FIND_POSITION_B(PET->m_currSpeed,PET->m_currTrackPosition,PET->m_currTrack,PET->m_dashTime,PET->m_stompTime,PET->m_shockwave1Time,PET->m_shockwave2Time,PET->m_usingDash,PET->m_usingStomp,PET->m_usingShockwaveStraight1,PET->m_usingShockwaveStraight2,PET->m_dashSpeed,PET->m_stompSpeed,PET->m_shockwave1Speed,PET->m_shockwave2Speed)

//PRE_FIND_POSITION_B
#define PRE_FIND_POSITION_B(current_speed,trackPosition,curr_track,dashTime,stompTime,shockwave1Time,shockwave2Time,usingDash,usingStomp,usingShockwave1,usingShockwave2,dashSpeed,stompSpeed,shockwave1Speed,shockwave2Speed)\
float b_current_speed = current_speed;float b_new_speed = 0.0f;float b_trackPosition = trackPosition;Track *b_curr_track = curr_track;float b_dashTime = dashTime;float b_stompTime = stompTime;float b_shockwave1Time = shockwave1Time;float b_shockwave2Time = shockwave2Time;bool b_usingDash = usingDash;bool b_usingStomp = usingStomp;bool b_usingShockwave1 = usingShockwave1;bool b_usingShockwave2 = usingShockwave2;bool b_dashSpeed = dashSpeed;bool b_stompSpeed = stompSpeed;bool b_shockwave1Speed = shockwave1Speed;bool b_shockwave2Speed = shockwave2Speed;

//FIND_POSITION_B
#define FIND_POSITION_B(PET)\
PET->getNewSpeed(AI_DELTA,b_current_speed,&b_new_speed,b_curr_track,&b_dashTime,&b_stompTime,&b_shockwave1Time,&b_shockwave2Time,&b_usingDash,&b_usingStomp,&b_usingShockwave1,&b_usingShockwave2,b_dashSpeed,b_stompSpeed,b_shockwave1Speed,b_shockwave2Speed);b_current_speed = b_new_speed;b_trackPosition += (b_current_speed * AI_DELTA);int b_trackCount = 0;b_curr_track = PET->getTrackFromTrackPosition(b_curr_track, &b_trackPosition, &b_trackCount);

//POST_FIND_POSITION_B
#define POST_FIND_POSITION_B(PET)\
PET->getPositionFromTrack(b_curr_track, b_trackPosition);


#define TRAP_RESERVE_RANDOM_RANGE 6


#pragma mark - Enums

enum AI_STATE {
    AI_NOTHING = 0,
    AI_START, //1
    AI_START_FROM_FRONT, //2
    AI_START_FROM_BEHIND, //3
    AI_START_FROM_NEAR, //4
    AI_SCOUT, //5
    AI_ATTACK_FROM_FRONT, //6
    AI_ATTACK_FROM_BEHIND, //7
    AI_ATTACK_FROM_NEAR, //8
    AI_DEFEND_FROM_FRONT, //9
    AI_DEFEND_FROM_BEHIND, //10
    
    TUTORIAL_IDLE,
    TUTORIAL_DASH,
    TUTORIAL_DASH_OVER_TRAP,
    TUTORIAL_DASH_OVER_TRAP_PRACTICE,
    TUTORIAL_STOMP
};

enum START_STATE {
    NOT_STARTED = 0,
    START_FIRST,
    START_SAME,
    START_LATER
};

enum SKILL_USAGE {
    USAGE_UNKNOWN,
    TO_ATTACK,
    TO_DEFEND
};

#pragma mark - AI Class

class AI : public CCObject {
    
    IsoMapRacing *m_isoMapRacing;
    
    Pet *m_pet_opponent_of_ai;
    Pet *m_pet_ai;
#ifdef DEBUG_AI
    cocos2d::CCLabelBMFont *debugLabel;
#endif
    
#define kAbilityDashForward         "dash_forward"
#define kAbilityBananaTrap          "banana_trap"
#define kAbilityStomp               "stomp"
#define kAbilityShootForward        "shoot_forward"
#define kAbilityDoubleShoot         "double_shoot"
    
    double m_randOnce1;
    double m_randOnce2;
    float m_float1;
    float m_int1;
    int m_decision1;
    
    bool m_hasStartedRunning;
    
    float m_attackReactionTime;
    
    float m_waitToUseSkill;
    SKILL m_useSkill;
    
    SKILL m_skillToDefend;
    
    //from 0 to 1
    //0 easiest, 1 hardest
    float m_horsePowerScale;
    float m_attackScale;
    float m_defenseScale;
    
    float m_attackInterval;
    float m_defenseInterval;
    
    
    //to not do nothing for a long time, if behind and nothing can be done for a long time
    float m_timeSinceCantDoNothing;
    float m_timeToDoSomethingAfterCantDoNothing;
    
    
    //skill usage frequency, let's call it quota slot
    int m_maxQuotaSlot;
    int m_quotaSlotCurrentlyInUse;
    float m_timeRemainingToReleaseOneQuotaSlot;
    
    
    //contact IsoMapRacing that and action can be made when set to true
    bool m_tutorialMode;
    bool m_tutorialLearner; //player
    bool m_tutorialTeacher; //player opponent
    
    //turn off all AI actions when set to false (default to true)
    bool m_isActive;
    
    //turn off all handicaps when set to true
    bool m_fullPower;
    
    //disable defensive skills when set to true
    bool m_disableDefensiveSkills;
    
    float m_aiDifficulty; //get from g_playerState->player.racing.ai_difficulty
    
    float m_idleTime;
    float m_idleChance;
    float m_maxIdleTime;
    float m_minIdleTime;
    int m_stayIdleCount;
    int m_useSkillCount;
    void adjustIdleChanceAndDuration();

    float m_shockwaveInaccuracy;
    float m_goodShotThreshold;
    int m_goodShotCount;
    int m_badShotCount;
    void adjustShockwaveAccuracy();

    void adjustSpeed();

    bool shouldAILetPlayerWin() const;
    bool playerMustWin() const;

    bool isLastLap() const;

public:
    
    bool m_canRecommend;
    
#pragma mark - Public Functions
    
    static AI* create(IsoMapRacing *isoMapRacing, Pet *pet_ai, Pet *pet_opponent_of_ai);
    
    bool init(IsoMapRacing *isoMapRacing, Pet *pet_ai, Pet *pet_opponent_of_ai);
    
    void updateAI(float delta);
    
    
    bool isTutorialMode() { return m_tutorialMode; };
    void setTutorialMode(bool tutorialMode) { m_tutorialMode = tutorialMode; };
    bool isTutorialLearner() { return m_tutorialLearner; };
    void setTutorialLearner(bool tutorialLearner) { m_tutorialLearner = tutorialLearner; };
    bool isTutorialTeacher() { return m_tutorialTeacher; };
    void setTutorialTeacher(bool tutorialTeacher) { m_tutorialTeacher = tutorialTeacher; };
    bool isActive() { return m_isActive; };
    void setActive(bool isActive) { m_isActive = isActive; };
    bool fullPower() { return m_fullPower; };
    void setFullPower(bool fullPower) { m_fullPower = fullPower; };
    bool isDefensiveSkillsDisabled() { return m_disableDefensiveSkills; };
    void setDisableDefensiveSkills(bool disableDefensiveSkills) { m_disableDefensiveSkills = disableDefensiveSkills; };
    float aiDifficulty() { return m_aiDifficulty; };
    void setAiDifficulty(float aiDifficulty) {
        m_aiDifficulty = aiDifficulty;
        setMaxQuotaSlot();
    };
    
    
    START_STATE m_startState;
    
    std::vector<AI_STATE> m_stateStack;
    void pushState(AI_STATE state);
    void popState(AI_STATE state);
    AI_STATE getCurrentState();
    
    void popToState(AI_STATE state);
    void runStateStart(float delta);
    void runStateStartFromFront(float delta);
    void runStateStartFromBehind(float delta);
    void runStateStartFromNear(float delta);
    void runStateScout(float delta);
    void runStateAttackFromFront(float delta); //exit when no longer front
    void runStateAttackFromBehind(float delta); //exit when no longer behind
    void runStateAttackFromNear(float delta); //exit when no longer near
    void runStateDefendFromFront(float delta);
    void runStateDefendFromBehind(float delta);
    void runStateTutorial(float delta);
    
#pragma mark - Check Skills
    
    SKILL checkDash(Pet *attackingPet, Pet *defendingPet, int distance);
    SKILL checkStomp(Pet *attackingPet, Pet *defendingPet, float tileSizeRatio);
    SKILL checkStomp(Pet *attackingPet, Pet *defendingPet, float tileSizeRatio, SKILL defendingPetUseSkill);
    SKILL checkStomp(Pet *attackingPet, Pet *defendingPet, float tileSizeRatio, SKILL defendingPetUseSkill, SKILL attackingPetUseSkill);
    SKILL checkShockwave(Pet *attackingPet, Pet *defendingPet, float tilePercentShockwave1, float tilePercentShockwave2);
    SKILL checkShockwave(Pet *attackingPet, Pet *defendingPet, float tilePercentShockwave1, float tilePercentShockwave2, float *timeUntilHit);
    
    void opponentIsAttacked(float stunDuration, SKILL skillUsed);
    
#pragma mark - Defend Skills
    void opponentUsedSkill(SKILL usedSkill);
    void aiIsAttacked(float stunDuration, SKILL skillUsed);
    SKILL defendTrap();
    SKILL defendStomp(float *timeUntilHit);
    SKILL defendShockwave(float *timeUntilHit);
    
#pragma mark - Try Skills
    SKILL tryDash(float distance);
    SKILL tryTrap();
    SKILL tryStomp();
    SKILL tryShockwave();
    SKILL tryLastResortDash();
    SKILL tryLastResortTrap();
    
#pragma mark - Difficulty Values Getter
    
    float getNewDifficultyScale();
    float getNewDifficultyScale(float randPercent);
    float getNewAttackReactionTime();
    float getNewDefenseReactionTime();
    
#pragma mark - Helper Functions
    
    bool useSkill(SKILL useSkill_);
    
    float randNum();
    float randNum(float min, float max);
    float randNum(double randNum, float min, float max);
    
    void recalculateTimeToDoSomething();
    void recalculateTimeRemainingToReleaseOneQuotaSlot();
    void setHorsePowerScale(int aiHorsePower);
    void setMaxQuotaSlot();
    
    CCPoint getNextPosition(Pet *pet,
                            float delta,
                            float current_speed,
                            float *new_speed,
                            float *trackPosition,
                            Track *curr_track,
                            float *dashTime,
                            float *stompTime,
                            float *shockwave1Time,
                            float *shockwave2Time,
                            bool *usingDash,
                            bool *usingStomp,
                            bool *usingShockwave1,
                            bool *usingShockwave2,
                            float dashSpeed,
                            float stompSpeed,
                            float shockwave1Speed,
                            float shockwave2Speed
                            );
    
#pragma mark - Use Skill
    
    SKILL useDash();
    SKILL useTrap();
    SKILL useStomp();
    SKILL useShockwaveStraight1();
    SKILL useShockwaveStraight2();
    
#pragma mark - Callback functions
    
    void* sender;
    void (*onRecommendSkill)(void* sender_, SKILL recommendedSkill);

};

#endif /* defined(__Cheetah__AI__) */
