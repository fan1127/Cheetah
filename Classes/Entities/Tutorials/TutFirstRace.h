//
//  TutFirstRace.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutFirstRace__
#define __Cheetah__TutFirstRace__

#include "CTutManager.h"
#include "../../Entities/IsoMapRacing.h"

#define SPEED_S 200 //300
#define SPEED_A 100 //150
#define SPEED_B 75 //125
#define SPEED_C 50
#define SPEED_D 25
#define SPEED_F 15

#define SPINE_TUTORIAL_SHOW_BUTTON_DELAY 1.0f


enum TUTSTATE {
    TUTSTATE_NONE = 0,
    TUTSTATE_GAUGEPANEL,
    TUTSTATE_DASH,
    TUTSTATE_DASH_PRACTICE,
    TUTSTATE_DASH_OVER_TRAP,
    TUTSTATE_DASH_OVER_TRAP_PRACTICE,
    TUTSTATE_STOMP,
    TUTSTATE_STOMP_PRACTICE,
    TUTSTATE_FREE_PLAY
};

class TutFirstRace: public CTutUnit {
    
    
    TUTSTATE m_state;
    
    cocos2d::extension::CCSkeletonAnimation*    m_race_tutor;
    
    float stompPracticeTime;
    bool stompPracticeTimeOver;
    int numberOfDashesDuringDashOverTrapPractice;
    
public:
    
    /** required instance */
    IsoMapRacing*   isoMapRacing;
    
    CTutPointAtNikButton *m_pointerAtGo;
    CTutPointAtPowerupButtons *m_pointerAtDash;
    CTutPointAtPowerupButtons *m_pointerAtDashOverTrap;
    CTutPointAtPowerupButtons *m_pointerAtStomp;
    CTutManualControl *m_manualDashPractice;
    CTutManualControl *m_manualDashOverTrapPractice;
    CTutManualControl *m_manualStompPractice;
    
    
    CTutManualControl *m_manualFreePlay;
    
    static std::string key();
    static bool check();
    static TutFirstRace* create(IsoMapRacing* isoMapRacing_);
    
    TUTSTATE getState() { return m_state; };
    void setState(TUTSTATE state_);
    
    
    void showDashButton1();
    void showDashButton2();
    void showStompButton();
    
    virtual bool init();
    
    virtual void update(float delta_);
    
    static void onTapToProgressFirstSpeech(void* sender_, std::string result_);
    static void onTapToProgressSpeech(void* sender_, std::string result_);
    static void onTapToShowGuagePanel(void* sender_, std::string result_);
    static void onTapToStartTheRealRace(void* sender_, std::string result_);
    static void onTapGo(void* sender_, std::string result_);
    static void onTapDash(void* sender_, std::string result_);
    static void onTapDashOverTrap(void* sender_, std::string result_);
    static void onTapStomp(void* sender_, std::string result_);
    
    static void onRecommendSkill(void* sender_, SKILL skill_);
    
    static void onUseSkill(void* sender_, SKILL skill_);
    
    static void onShowRaceResult(void* sender_, bool playerLose);
    
    static void onTapToEndTheRace(void* sender_, std::string result_);
};

#endif /* defined(__Cheetah__TutFirstRace__) */
