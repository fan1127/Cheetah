//
//  Pet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/10/56 BE.
//
//

#ifndef __Cheetah__Pet__
#define __Cheetah__Pet__

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>

#include "../VOs/PlayerPetVO.h"
#include "Track.h"
#include "../Extensions/CCProgressBar9Sprite.h"
#include "../Extensions/CCPhatTouchableSprite.h"
#include "../Extensions/CCLabelHelper.h"

#include "../Settings.h"
#include "../Helpers/ReplayData.h"
#include "Ability.h"


#define PET_SCALE               0.85f //adjust this
#define PET_STATIC_SCALE        (0.75f *PET_SCALE) //don't adjust this
#define PET_STATIC_SCALE_HEAD   (1.5f *PET_SCALE) //don't adjust this
#define TIME_SCALE_FACTOR       (0.533333333f *PET_SCALE)
#define PET_FALLING_TIME_SCALE  1.0f

#define SHOCKWAVE_EFFECT_SCALE  0.4f

#define RUN_SPEED_TO_SPEED_DISPLAY_RATIO 1.0f

#define PET_SPEED_FOR_FAST_FALL 2.0f


#define GROUP_ENERGY_ORIGINAL_SCALE 0.5f

//probably need to change to some dynamic values
#define ATTACK_BONUS_TIME_ADDED         5.0f

#define PET_IS_RUNNING   (m_currPetAction == PET_RUN)
#define PET_IS_JUMPING   (m_currPetAction == PET_JUMP)



#define BOT_MAX_SKILL_COST_MULT_HANDICAP 0.4f

//actual target_speed
//max 7.5
//min 1.5-1.75

#define PPVO_STRAIGHT_SPEED         ppVo.top_speed
#define PPVO_CORNER_SPEED           ppVo.corner_speed
#define PPVO_ENERGY_STAT            ppVo.jumping

#define STAT_3_MIN_ADDITION         0.20f
#define STAT_3_RATE_TO_VALUE        0.005f


#define POWERUP_1_SPEED             9.0f
#define POWERUP_1_TAG               1009

#define SHOCKWAVE_AREA              260
#define STOMP_AREA_RATIO_TO_TILESIZE 1.55f
#define STOMP_Y_OFFSET ccp(0.0f, g_gameContent->tileSize.height * -0.25f)
#define STOMP_CIRCLE_Y_LENGTH_PETCENT 0.79f //1.0f is circle, less value means more oval in the Y

#define POWERUP_3_SCALE_START       3.0f
#define POWERUP_3_SCALE_END         4.0f
#define POWERUP_3_SCALE_TIME        0.1f
#define POWERUP_3_FADEOUT_TIME      0.15f

#define POWERUP_4_STAGE1_SPEED      0.f

//STAT TO TARGET_SPEED CONVERSION
//....
//....

#define PET_JUMPING_FAILED_SPEED    0.5f
#define PET_JUMPING_FAILED_ANIMATION_TIME   1.5f
#define DELAY_TIME_TO_RESULT_SCREEN_AFTER_WINNING    1.5f
#define DELAY_TIME_TO_RESULT_SCREEN_AFTER_LOSING     0.75f

#define TRACK_TYPE_STRAIGHT (m_currTrack->ptVo.no == 1 || m_currTrack->ptVo.no == 3)

//how many seconds will the animal need to deaccelerate to target speed
#define SECONDS_TO_ACCELERATE_TO_NORMAL_SPEED       1.0f
#define SECONDS_TO_BRAKE_TO_TARGET_SPEED            -0.5f
#define SECONDS_TO_BRAKE_TO_FULL_STOP               1.2f //animation length is 1 sec

//acceleration = speedstat / seconds_to_full_speed;


#define T_S_UP_LEFT (track->ptVo.no == TTYPE_STRAIGHT_UP_LEFT)
#define T_S_UP_RIGHT (track->ptVo.no == TTYPE_STRAIGHT_UP_RIGHT)

#define D_UP_LEFT 1
#define D_UP_RIGHT 2
#define D_DOWN_LEFT 3
#define D_DOWN_RIGHT 4

#define TOPSPEED 1
#define ACCELERATION 0.1
#define STARTSPEED 1.5f
#define MINSPEED 1.5f
#define MINSPEED_FOR_SPEED_DISPLAY 1.2f

#define CONVERSION_RATE 0.75f

#define TWOPLAYER_PERCENT_SHIFT 0.18f

#define RECOVERY_TIME_BY_TRAP 0.2f

#define CRITICAL_STUN_DURATION_MULT 1.5f

#define EFFECT_DASH_SCALE (1.8f)//(2.0f)//(0.8f)
#define EFFECT_DASH_MIN_FRAME 2//0
#define EFFECT_DASH_MAX_FRAME 6//19
#define EFFECT_DASH_FILE_FORMAT "oldversion/_effect_dash_%d.png"
#define EFFECT_DASH_OFFSET_Y_PERCENT (0.2f)

enum PetAction {
    
    PET_IDLE_WALK,                  //0
    PET_IDLE_STAND,
    PET_IDLE_HEAD_UP,
    PET_IDLE_HEAD_DOWN,
    PET_IDLE_HEAD_SHAKE,
    PET_IDLE_HEAD_GORE,             //5
    PET_IDLE_SLEEP_TO_STAND,
    PET_HABITAT_SITDOWN,
    PET_HABITAT_JUMP,
    PET_HABITAT_STAND2LEGS,
    PET_HABITAT_SCRABBLE,           //10
    PET_HABITAT_2LEGS,
    PET_HABITAT_STAND_SMOKE,
    PET_HABITAT_FIRE,
    PET_HABITAT_FLY,
    PET_HABITAT_SMOKE,              //15
    PET_HABITAT_EAT,
    PET_HABITAT_EAT_2,
    PET_HABITAT_HUNGRY,
    PET_HABITAT_HUNGRY_LAID,
    PET_HABITAT_HUNGRY_SMOKE,       //20

    PET_HABITAT_SLEEP,
    
    PET_STANDBY,                    //22
    PET_TOUCH,                      //23
    PET_READY,                      //24
    PET_RUN,                        //25
    PET_FLY,                        //26
    PET_JUMP,                       //27
    PET_FALL_A,                     //28
    PET_FALL_B,                     //29
    PET_SLIDE,                      //30
    PET_FROZEN_A,
    PET_FROZEN_B,
    PET_FROZEN_C,
    PET_FROZEN_D,
    PET_FROZEN_E,                   //35
    PET_STOMP,
};

#define kPetActionRandomRange       18

enum PetResult {
    UNKNOWN_RESULT,
    WIN_RESULT,
    LOSE_RESULT
};

class IsoMapRacing;
class PlayerState;

#define kExpLabelMax                10

class Pet: public cocos2d::CCNode {
    
#ifdef DEBUG_TEST_STOMP_AREA
    float testAngle;
    float testTime;
#endif
    float                   m_order;
    bool                    m_active;
    float                   m_delta = 0.0f;
    float                   m_hunger_subtract = 0.0f;
    
    int                     m_currTrackIndex, m_prevTrackIndex;
    
    bool                    m_flip;
    float                   m_walkSpeed;
    float                   m_beforeUsingSkillSpeed;
    float                   m_beforeIsAttackedSpeed;
    float                   m_beforeIsFinishedRunningSpeed;
    bool                    m_didFall;
    
    bool                    m_isArriveOnNewTrack;
    bool                    m_hasObstacleOnTrack;
    bool                    m_hasObstacleOnNextTrack;
    bool                    m_needStopOnNextTrack;
    bool                    m_needSlideOnNextTrack;
    
    bool                    m_isAttacked = false;
    
    float                   m_finishedRunningTime;
    
    Track* track;
    Track* nextTrack;
    
    cocos2d::CCSprite *m_effectGlow;
    
    int                     m_currentEffectDashFrame;
    int                     m_effectDashAnimationFlipper;
//    float                   m_effectDashTimePerFrame;
//    float                   m_currentDashTimeFrame;
    
    int                     m_hunger_timer;
    
    float                   m_spd_a;
    float                   m_spd_b;
    float                   m_spd_c;
    float                   m_spd_mult;
    float                   m_acc_a;
    float                   m_acc_b;
    float                   m_acc_c;
    float                   m_acc_mult;
    
    int                     m_energy_timer;
    float                   m_energy_current;
    
//    CCLabelBMFont*          m_label_exp[kExpLabelMax];
//    CCLabelBMFont*          m_label_food[kExpLabelMax];
//    cocos2d::CCArray*       m_label_exp;
//    cocos2d::CCArray*       m_label_food;
    
    int                     m_label_exp_count;
    int                     m_label_food_count;
    
    
    unsigned int m_dashTimeUsed;
    unsigned int m_bananaTrapTimeUsed;
    unsigned int m_stompTimeUsed;
    unsigned int m_shockwave1TimeUsed;
    unsigned int m_shockwave2TimeUsed;
    
    unsigned int m_dashCost;
    unsigned int m_bananaTrapCost;
    unsigned int m_stompCost;
    unsigned int m_shockwave1Cost;
    unsigned int m_shockwave2Cost;
    
    unsigned int m_dashStartingCost;
    unsigned int m_bananaTrapStartingCost;
    unsigned int m_stompStartingCost;
    unsigned int m_shockwave1StartingCost;
    unsigned int m_shockwave2StartingCost;
    
    float m_dashIncreasingCost;
    float m_bananaTrapIncreasingCost;
    float m_stompIncreasingCost;
    float m_shockwave1IncreasingCost;
    float m_shockwave2IncreasingCost;
    
    float m_dashAddingIncreasingCost;
    float m_bananaTrapAddingIncreasingCost;
    float m_stompAddingIncreasingCost;
    float m_shockwave1AddingIncreasingCost;
    float m_shockwave2AddingIncreasingCost;
    
    unsigned int m_allCost;
    unsigned int m_usedCost;
    
    bool                    m_tutorialMode;
    bool                    m_isPaused;
    
    CCNode *                m_groupSpineEffectsBelowPet;
    
    float                   m_beforePausedTimeScale;
    
    CCSkeletonAnimation *   m_stompSpine;
    
    unsigned int            m_stat_straight_speed;
    unsigned int            m_stat_corner_speed;
    unsigned int            m_stat_energy;
    
    CCSprite                *m_effectDash;

    enum ReplayState
    {
        REPLAY_STATE_ANIM = 0,
        REPLAY_STATE_FLIP,
        REPLAY_STATE_SKILL,
        REPLAY_STATE_TRACK,
        REPLAY_STATE_SUM_TRACK
    };
    enum ReplayFloat
    {
        REPLAY_FLOAT_SPEED = 0,
        REPLAY_FLOAT_TRACK,
        REPLAY_FLOAT_SUM_TRACK
    };
    enum ReplayEvent
    {
        REPLAY_EVENT_CALL_HELPER = SKILL_COUNT_PLUS_ONE,
        REPLAY_EVENT_REMOVE_HELPER = SKILL_COUNT_PLUS_ONE + 1
    };

    bool m_playbackReplayData;
    float m_lastSkillTime;
    ReplayData m_replayData;
    void collectVisualReplayData();
    void collectSimReplayData();
    void playbackReplayData(float delta);
    int flipSpriteDirection();
    void initializeReplayData();

public:
    
    void setStatStraightSpeed(unsigned int stat_straight_speed_) { m_stat_straight_speed = stat_straight_speed_; };
    void setStatCornerSpeed(unsigned int stat_corner_speed_) {m_stat_corner_speed = stat_corner_speed_; } ;
    void setStatEnergy(unsigned int stat_energy_) { m_stat_energy = stat_energy_; };
    unsigned int getStatStraightSpeed(){ return m_stat_straight_speed; };
    unsigned int getStatCornerSpeed(){ return m_stat_corner_speed; };
    unsigned int getStatEnergy(){ return m_stat_straight_speed; };
    unsigned int getHorsepower(){ return m_stat_energy + m_stat_corner_speed + m_stat_straight_speed; };
    
    float                   m_stunnedTime;
    float                   m_recoveryTime;
    
    bool                    m_usingSkill;
    bool                    m_usingDash;
    bool                    m_usingStomp;
    bool                    m_usingShockwaveStraight1;
    bool                    m_usingShockwaveStraight2;
    //--
    float                   m_dashTime;
    float                   m_stompTime;
    float                   m_shockwave1Time;
    float                   m_shockwave2Time;
    //--
    float                   m_dashSpeed;
    float                   m_stompSpeed;
    float                   m_shockwave1Speed;
    float                   m_shockwave2Speed;
    
    //------
    float                   m_stompCollideTime;
    
    //Constants
    //--
//    float                   m_constant_dashTime;
//    float                   m_constant_stompTime;
//    float                   m_constant_shockwave1Time;
//    float                   m_constant_shockwave2Time;
    //--
//    float                   m_constant_dashSpeed;
//    float                   m_constant_stompSpeed;
//    float                   m_constant_shockwave1Speed;
//    float                   m_constant_shockwave2Speed;
    
    float m_currDelta;
    float m_currTrackPosition;
    float m_currSpeed;
    float m_currAcc;
    Track *firstTrack;
    Track *m_currTrack;
    float m_summaryTrackPosition;
    
    bool isBot;
    
    
    float petScaleFactor;
    
    float                   m_food_fed;
    bool                    m_feed_done;
    
    cocos2d::CCPoint        m_currPos;
    
    cocos2d::extension::CCSkeletonAnimation* m_levelUp;
    cocos2d::extension::CCSkeletonAnimation* m_expUp;
    
    cocos2d::CCNode*        m_groupEnergy;
    cocos2d::CCSprite*      m_bgEnergy;
    cocos2d::CCSprite*      _energyIcon;
    cocos2d::CCLabelBMFont* m_label_energy_info;
    
    cocos2d::CCLabelBMFont* m_critLabel;
    
    void                    *preferenceRootParent = NULL;
    
    PetAction               m_currPetAction, m_prevPetAction;
    
    IsoMapRacing *isoMapRacing;
    
    PetResult petResult;
    std::string message;
    std::string message2;
    
    bool                    m_isFinishedRunning;
    
    bool m_trackDirection;
    double m_distanceBetweenTrack;
    double m_distancePerRound;
    
    int m_previousTrackIndex;
    
    bool start_running;
    bool can_running;
    
    bool end_running;
    
//    bool is_replay;
    
    int ran_round;
    int max_round;
    
    float running_time;
    float after_running_time;
    
    bool is_active_grow = false;
    
    int m_summaryTrack;
    bool m_canAttack;
    int m_trackCount;
    
    bool isPlayerPet;
    bool isHelperPet;
    bool isActive;
    
    float m_helperStunDuration;
    int m_helperStunAmount;
    
    GDPetVO         pVo;
    PlayerPetVO     ppVo;
    
    PlayerState *playerState;
    
    Pet *opponentPet;
    
    cocos2d::CCDictionary *replayRecord = NULL;
    
    cocos2d::extension::CCSkeletonAnimation *petSpine;

    static Pet* createWithPetVO(GDPetVO vo, PlayerPetVO ppvo);

    bool initWithPetVO(GDPetVO vo, PlayerPetVO ppv);
    
    virtual ~Pet();
    
    void initilizeEnergyInformation();
    
    void showEnergyInfo();
    void showEnergyInfo(const cocos2d::ccColor3B &color);
    void showEnergyInfo(int energy, const cocos2d::ccColor3B &color);
    
    void showCritical();
    
    virtual void draw();
    
    virtual void update(float delta_);
    void updatePet(float delta_);
    static void onPetUpdateCompleted(void* sender,
                                     std::string resp_str);
    
    void reload();
    void reload(bool force_);
    void reload(bool force_, bool save_, std::string type_);
    void resumeAction();
    void setAnimation(PetAction action);
    void playShowOffAnimation();
    bool trySetAnimation(PetAction action);
    std::string getAnimationName(PetAction action);
    void updateAnimation();
    void updateHunger();
    void updateCoinsProduced();
    void zeroCoinsProduced();
    
    void setActive(bool active_);
    void flip(bool flip);
    bool isFlip() { return m_flip; };
    void updateFlip();
    void randomIdleAction();
    float getTargetSpdFromStat(int stat_);
    float getSecToAccToTargetSpdFromStat(int stat_);
    
    void startRunning();
    float getExpectedExp();
    
    void prepareRunning(void* ptr_isomap, Track* startTrack, bool trackDirection, int order);
    void updatePosition();
    void setPositionToTrack(Track *track, float trackPosition);
    CCPoint getPositionFromTrack(Track *track, float trackPosition);
    CCPoint getPositionFromTrack(Track *track, float trackPosition, bool doFlip);
    void getNewSpeed(float delta, float curr_speed, float *new_speed, Track* curr_track);
    void getNewSpeed(float delta, float curr_speed, float *new_speed, Track* curr_track, float *dashTime, float *stompTime, float *shockwave1Time, float *shockwave2Time, bool *usingDash, bool *usingStompSpeed, bool *usingShockwave1Speed, bool *usingShockwave2Speed);
    void getNewSpeed(float delta, float curr_speed, float *new_speed, Track* curr_track, float *dashTime, float *stompTime, float *shockwave1Time, float *shockwave2Time, bool *usingDash, bool *usingStomp, bool *usingShockwave1, bool *usingShockwave2, float dashSpeed, float stompSpeed, float shockwave1Speed, float shockwave2Speed);
    void getNewSpeed(float delta, float curr_speed, float *new_speed, Track* curr_track, float *dashTime, float *stompTime, float *shockwave1Time, float *shockwave2Time, bool *usingDash, bool *usingStomp, bool *usingShockwave1, bool *usingShockwave2, float dashSpeed, float stompSpeed, float shockwave1Speed, float shockwave2Speed, bool *usingSkill);
    Track* getTrackFromTrackPosition(Track *startingTrack, float *trackPosition, int *trackCount);
    void updateRunning(void* ptr_isomap ,float delta);
    void setTimeScaleDependingOnSpeed();
    void pauseRacing();
    void resumeRacing();
    CCPoint getNextPositionFromDelta(float delta);
    CCPoint getNextPositionAfterSeconds(float seconds);
    
    void setDefaultSpeed(CCObject *sender);
    float getMinSpeed();
    float getTopSpeed();
    float getSpeedDisplay();
    
    int getLevel();
    float getExpValue();
    float getExpMax();
    float getExpRatio();
    int getExpGain();
    float getExpGainRatio();
    float getHungerValue();
    float getHungerMax();
    float getHungerRatio();
    float getHungerFoodToFull();
    float getHungerFoodToFullRatio();
    int getSecondsLeftToHungerDrain();
    int getRarityStars();
    float getCurrentEnergy();
    int getSecondsLeftToFullEnergy();
    float getCoinsProducedPerSecond();
    float getCoinsProducedCapacity();
    bool isInAnimalPen();

    bool currPetActionIsSleeping() { return m_currPetAction == PET_HABITAT_SLEEP; }

    void printMessage();
    
    //Emotion while in habitat functions
//    cocos2d::CCSprite* emotionHungry;
    CCPhatTouchableSprite* emotionHungry;
    static void onHungryIconTouched(void* inst_, void* sender_);
    bool isHungry();
    bool isSleeping();
    void showHungry();
    void hideHungry();
    void delHungry(cocos2d::CCObject* sender_); //release sprite object callback for an popup
    
    //Hunger bar while pet being fed
    void setHungerTimer(int timer_);
    void feedBegin();
    bool feed();
    void feedEnd();
    
    //Show Exp
    void showExp(int feedAmount);
//    void releaseExp(cocos2d::CCObject* sender_);
    
    //Show LevelUp
    void playLevelUpEffect(bool expUp = false);
    
#pragma mark - UsePowerup
    
    bool useDash();
    void startDashEffect();
    void updateDashEffect();
    void stopDashEffect();
    void stopDashing();
    bool usingDash() { return m_usingDash; };
    bool useTrap();
    void spawnTrap();

    void updateCostForAllSkills();
    unsigned int getDashCost() { return m_dashCost; };
    unsigned int getTrapCost() { return m_bananaTrapCost; };
    unsigned int getStompCost() { return m_stompCost; };
    unsigned int getShockwave1Cost() { return m_shockwave1Cost; };
    unsigned int getShockwave2Cost() { return m_shockwave2Cost; };
    unsigned int getDashTimeUsed() { return m_dashTimeUsed; };
    unsigned int getTrapTimeUsed() { return m_bananaTrapTimeUsed; };
    unsigned int getStompTimeUsed() { return m_stompTimeUsed; };
    unsigned int getShockwave1TimeUsed() { return m_shockwave1TimeUsed; };
    unsigned int getShockwave2TimeUsed() { return m_shockwave2TimeUsed; };
    unsigned int getUsedCost() { return m_usedCost; };
    unsigned int getAllCost() { return m_allCost; };
    bool canDash();
    bool canTrap();
    bool canStomp();
    bool canShockwave1();
    bool canShockwave2();
    
    float isCritical();
    void petIsAttacked(Pet *attackingPet, PetAction petAction, float stunDuration, SKILL skillUsed);
    void petIsAttacked(Pet *attackingPet, PetAction petAction, float stunDuration, SKILL skillUsed, float critical);
    void petIsAttackedRecovered(CCObject *sender);
    void petIsAttackedWithEffect(Pet *attackingPet, PetAction petAction, float stunDuration, SKILL skillUsed);
    void petIsAttackedWithEffect(Pet *attackingPet, PetAction petAction, float stunDuration, SKILL skillUsed, float critical);
    
    void smokeFallingAnimation();
    
    int startSkillCost();
    void regenCost();
    void decreaseCost(int cost);
    unsigned int remainingCost();
    
    bool useStomp();
    void effectStomp();
    void playEffectStomp();
    void stompCollide();
    void useStompFinished();
    bool usingStomp() { return m_usingStomp; };
    
    bool useShockwaveStraight1();
    void shockwaveStraight1Stage2();
    void shockwaveStraight1Stage3();
    
    bool useShockwaveStraight2();
    void shockwaveStraight2Stage2();
    void shockwaveStraight2Stage3();

    void spawnShockwave1();
    void spawnShockwave2();

    void setActiveGlowing(bool active);
    bool isActiveGlowing();

    bool isFalling() const;

    bool canAttack();
    bool canBeAttacked();

    void callHelper(Pet* pet);
    void removeHelper();
    Track* getStartingTrackFromReplayData();

    void setTutorialMode(bool tut_) { m_tutorialMode = tut_; }
    bool isTutorialMode() { return m_tutorialMode; }
    
    std::string serializeReplayData();
    void enablePlaybackReplayData(bool enable, const char* replayData);
    bool isPlaybackReplayDataEnabled() const;
    bool isPlaybackFinished() const;
    float getReplayDataDuration() const;
    float getRemainingReplayTime() const;

    Ability* createAbility(const char* abilityName);
    
    ////////////////////////////////////////////////////////
    // Callback method
    ////////////////////////////////////////////////////////
    
#pragma mark - Clean up
    
    void cleanEffect();
    void cleanEffectLevelUp();
    
#pragma mark - Callback
    
    static void onPetFed(void* sender_, std::string respn_json_str_);
    
};

#endif /* defined(__Cheetah__Pet__) */
