//
//  TutFirstRace.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutFirstRace.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Scenes/SceneVille.h"
#include "../../Scenes/SceneRace.h"
extern CCScene* g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../UIs/GaugePanel.h"
#include "../../UIs/RacePanel.h"

#include "../../Entities/AI.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Helpers/FlurryCpp.h"
#include "../../Helpers/CustomEventsHelper.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

extern float sizeMultiplier;

string TutFirstRace::key() {
    
    return "first_race";
    
}

bool TutFirstRace::check() {
    
    if (!g_playerState->Checklist[TutFirstRace::key()]) {
        return true;
    }
    
    return false;
    
}

TutFirstRace* TutFirstRace::create(IsoMapRacing* isoMapRacing_) {
    
    if (TutFirstRace::check()) {
        TutFirstRace* pRet = new TutFirstRace();
        if (pRet) {
            pRet->isoMapRacing = isoMapRacing_;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool TutFirstRace::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_state = TUTSTATE_NONE;
    
    m_key = TutFirstRace::key();
    
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    
    
    stompPracticeTime = 0.0f;
    stompPracticeTimeOver = false;
    numberOfDashesDuringDashOverTrapPractice = 0;
    
    
    //???: do we need to disable these later on? (especially the Tutorial mode on pet
    
    isoMapRacing->setTutorialMode(true);
    
    isoMapRacing->m_pet_player->setTutorialMode(true);
    isoMapRacing->m_pet_opponent->setTutorialMode(true);
    isoMapRacing->m_pet_player->setStatStraightSpeed(100); //actual stats gets overrided later
    isoMapRacing->m_pet_player->setStatCornerSpeed(100); //actual stats gets overrided later
    isoMapRacing->m_pet_opponent->setStatStraightSpeed(100); //actual stats gets overrided later
    isoMapRacing->m_pet_opponent->setStatCornerSpeed(100); //actual stats gets overrided later
    isoMapRacing->m_pet_player->reload(true, false, "cat1_7_1");
    isoMapRacing->m_pet_opponent->reload(true, false, "dragon1");
    
    //set energy here, this is not overrided
    isoMapRacing->m_pet_player->setStatEnergy(100);
    isoMapRacing->m_pet_opponent->setStatEnergy(50);
    
    isoMapRacing->addAIToPlayer();
    
    isoMapRacing->m_AI_on_player->sender = this;
    isoMapRacing->m_AI_on_player->onRecommendSkill = TutFirstRace::onRecommendSkill;
    isoMapRacing->m_AI_on_player->m_canRecommend = true;
    
    isoMapRacing->racePanel->onUseSkillSender = this;
    isoMapRacing->racePanel->onUseSkill = TutFirstRace::onUseSkill;
    
    isoMapRacing->onRaceResultSender = this;
    isoMapRacing->onShowRaceResult = TutFirstRace::onShowRaceResult;
    
    isoMapRacing->m_AI_on_player->setActive(false);
    isoMapRacing->m_AI_on_opponent->setActive(false);
    isoMapRacing->m_AI_on_player->setTutorialLearner(true);
    isoMapRacing->m_AI_on_player->setTutorialMode(true);
    isoMapRacing->m_AI_on_opponent->setTutorialTeacher(true);
    isoMapRacing->m_AI_on_opponent->setTutorialMode(true);
    
    isoMapRacing->m_AI_on_player->setFullPower(true);
    
    isoMapRacing->m_AI_on_opponent->setAiDifficulty(0.85f);
    isoMapRacing->m_AI_on_opponent->setHorsePowerScale(75);
    
    m_race_tutor = g_sceneManager->sharedSkeletonData->getSkeleton(kTutorialRaceSrcDir, "tutorial_pwr_up");
    m_race_tutor->setPosition(screenSize*0.5f);
    m_race_tutor->setVisible(false);
    m_race_tutor->setScale(2.0f * sizeMultiplier);
    m_race_tutor->timeScale = 0.5f;
    this->addChild(m_race_tutor);
    
    if (isoMapRacing->racePanel)
    {
        isoMapRacing->racePanel->disableTouchDownGaugeFunction(true);
        isoMapRacing->racePanel->forceHideGetHelpButton(true);
        isoMapRacing->racePanel->forceHideGiveUpButton(true);
        isoMapRacing->racePanel->forceVeryEasyGauge(true);
    }
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleHappy, false,
                                    g_gameContent->Word["tut_first_race_intro_1"],
                                    CT_SPC_OPTION_NONE,
                                    this, TutFirstRace::onTapToProgressFirstSpeech));
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_SARAH, CT_CHR_SIDE_RIGHT,
                                    kSarahHappy, false,
                                    g_gameContent->Word["tut_first_race_intro_3"],
                                    CT_SPC_OPTION_NONE,
                                    this, TutFirstRace::onTapToProgressSpeech));
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    g_gameContent->Word["tut_first_race_intro_4"],
                                    CT_SPC_OPTION_NONE,
                                    this, TutFirstRace::onTapToShowGuagePanel)); //last speech should point here
    
    
    
    if (isoMapRacing->racePanel->m_btnGo) {
        
        m_pointerAtGo = new CTutPointAtNikButton(isoMapRacing->racePanel->m_btnGo, true, isoMapRacing->racePanel->m_btnGo->getContentSize()*0.5f, 0.0f, this, TutFirstRace::onTapGo);
        m_pointerAtGo->autoInactive = false;
        
        m_tuts.push_back(m_pointerAtGo);
    }
    
    m_pointerAtDash = new CTutPointAtPowerupButtons(isoMapRacing->racePanel, SKILL_DASH, CCPointZero, 0.0f, true, this, TutFirstRace::onTapDash);
    m_tuts.push_back(m_pointerAtDash);
    
    m_manualDashPractice = new CTutManualControl();
    m_tuts.push_back(m_manualDashPractice);
    
    m_pointerAtDashOverTrap = new CTutPointAtPowerupButtons(isoMapRacing->racePanel, SKILL_DASH, CCPointZero, 0.0f, true, this, TutFirstRace::onTapDashOverTrap);
    m_tuts.push_back(m_pointerAtDashOverTrap);
    
    m_manualDashOverTrapPractice = new CTutManualControl();
    m_tuts.push_back(m_manualDashOverTrapPractice);
    
    m_pointerAtStomp = new CTutPointAtPowerupButtons(isoMapRacing->racePanel, SKILL_STOMP, CCPointZero, 0.0f, true, this, TutFirstRace::onTapStomp);
    m_tuts.push_back(m_pointerAtStomp);
    
    m_manualStompPractice = new CTutManualControl();
    m_tuts.push_back(m_manualStompPractice);
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    g_gameContent->Word["tut_first_race_before_freeplay_1"],
                                    CT_SPC_OPTION_NONE,
                                    this, TutFirstRace::onTapToStartTheRealRace));
    
    m_manualFreePlay = new CTutManualControl();
    m_tuts.push_back(m_manualFreePlay);
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleHappy, false,
                                    g_gameContent->Word["tut_first_race_after_freeplay_1"],
                                    CT_SPC_OPTION_NONE,
                                    this, TutFirstRace::onTapToEndTheRace));
    
    return true;
    
}

void TutFirstRace::setState(TUTSTATE state_)
{
    if (m_state != state_) {
        
        //exiting old state
        switch (m_state) {
            case TUTSTATE_GAUGEPANEL:
            {
                isoMapRacing->racePanel->m_tutorialLabel->setString("");
                isoMapRacing->racePanel->m_tutorialLabel->setVisible(false);
                
                FlurryCpp::endTimedEvent(EVENT_TUT_TUTFIRSTRACE_GAUGEPANEL, NULL);
            }
                break;
                
            case TUTSTATE_DASH:
            {
                isoMapRacing->m_AI_on_player->popToState(TUTORIAL_IDLE);
                isoMapRacing->m_AI_on_opponent->popToState(TUTORIAL_IDLE);
                
                FlurryCpp::endTimedEvent(EVENT_TUT_TUTFIRSTRACE_DASH, NULL);
            }
                break;
                
            case TUTSTATE_DASH_PRACTICE:
            {
                FlurryCpp::endTimedEvent(EVENT_TUT_TUTFIRSTRACE_DASH_PRACTICE, NULL);
            }
                break;
                
            case TUTSTATE_DASH_OVER_TRAP:
            {
                isoMapRacing->m_AI_on_player->popToState(TUTORIAL_IDLE);
                isoMapRacing->m_AI_on_opponent->popToState(TUTORIAL_IDLE);
                
                FlurryCpp::endTimedEvent(EVENT_TUT_TUTFIRSTRACE_DASH_OVER_TRAP, NULL);
            }
                break;
                
            case TUTSTATE_DASH_OVER_TRAP_PRACTICE:
            {
                FlurryCpp::endTimedEvent(EVENT_TUT_TUTFIRSTRACE_DASH_OVER_TRAP_PRACTICE, NULL);
            }
                break;
                
            case TUTSTATE_STOMP:
            {
                isoMapRacing->m_AI_on_player->popToState(TUTORIAL_IDLE);
                isoMapRacing->m_AI_on_opponent->popToState(TUTORIAL_IDLE);
                
                FlurryCpp::endTimedEvent(EVENT_TUT_TUTFIRSTRACE_STOMP, NULL);
            }
                break;
                
            case TUTSTATE_STOMP_PRACTICE:
            {
                FlurryCpp::endTimedEvent(EVENT_TUT_TUTFIRSTRACE_STOMP_PRACTICE, NULL);
            }
                break;
                
            default:
                break;
        }
        
        
        //changes to new state
        m_state = state_;
        
        //entering new state
        switch (m_state) {
            case TUTSTATE_GAUGEPANEL:
            {
                isoMapRacing->racePanel->m_tutorialLabel->setString(g_gameContent->Word["tut_first_race_go_1"].c_str());
                isoMapRacing->racePanel->m_tutorialLabel->setVisible(true);
                
                FlurryCpp::logEvent(EVENT_TUT_TUTFIRSTRACE_GAUGEPANEL, true);
            }
                break;
                
            case TUTSTATE_DASH:
            {
                isoMapRacing->m_AI_on_player->pushState(TUTORIAL_DASH);
                isoMapRacing->m_AI_on_opponent->pushState(TUTORIAL_DASH);
                
                FlurryCpp::logEvent(EVENT_TUT_TUTFIRSTRACE_DASH, true);
            }
                break;
                
            case TUTSTATE_DASH_PRACTICE:
            {
                isoMapRacing->m_AI_on_player->pushState(TUTORIAL_DASH_OVER_TRAP);
                isoMapRacing->m_AI_on_opponent->pushState(TUTORIAL_DASH_OVER_TRAP);
                
                FlurryCpp::logEvent(EVENT_TUT_TUTFIRSTRACE_DASH_PRACTICE, true);
            }
                break;
                
            case TUTSTATE_DASH_OVER_TRAP:
            {
                
                FlurryCpp::logEvent(EVENT_TUT_TUTFIRSTRACE_DASH_OVER_TRAP, true);
            }
                break;
                
            case TUTSTATE_DASH_OVER_TRAP_PRACTICE:
            {
                isoMapRacing->m_AI_on_player->pushState(TUTORIAL_DASH_OVER_TRAP_PRACTICE);
                isoMapRacing->m_AI_on_opponent->pushState(TUTORIAL_DASH_OVER_TRAP_PRACTICE);
                
                FlurryCpp::logEvent(EVENT_TUT_TUTFIRSTRACE_DASH_OVER_TRAP_PRACTICE, true);
            }
                break;
                
            case TUTSTATE_STOMP:
            {
                isoMapRacing->racePanel->forceEnabledPowerupButton(SKILL_DASH, false);
                
                isoMapRacing->m_AI_on_player->pushState(TUTORIAL_STOMP);
                isoMapRacing->m_AI_on_opponent->pushState(TUTORIAL_STOMP);
                
                FlurryCpp::logEvent(EVENT_TUT_TUTFIRSTRACE_STOMP, true);
            }
                break;
                
            case TUTSTATE_STOMP_PRACTICE:
            {
                
                FlurryCpp::logEvent(EVENT_TUT_TUTFIRSTRACE_STOMP_PRACTICE, true);
            }
                break;
                
            case TUTSTATE_FREE_PLAY:
            {
                FlurryCpp::logEvent(EVENT_TUT_TUTFIRSTRACE_FREE_PLAY, true);
                
                isoMapRacing->racePanel->showHintCurrentEnergy();
                
                isoMapRacing->racePanel->setAllPowerupButtonsCostVisible(true);
                isoMapRacing->racePanel->turnOffAllForceEnabledPowerupButtons();
                isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_A);
                isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_A);
                //                isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_A -5);
                //                isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_A -5);
                
                
                //                isoMapRacing->m_AI_on_player->setActive(false);
                isoMapRacing->m_AI_on_opponent->setActive(true);
                //                isoMapRacing->m_AI_on_player->setTutorialLearner(true);
                //                isoMapRacing->m_AI_on_player->setTutorialMode(true);
                isoMapRacing->m_AI_on_opponent->setTutorialTeacher(false);
                isoMapRacing->m_AI_on_opponent->setTutorialMode(false);
                
                isoMapRacing->m_AI_on_opponent->setDisableDefensiveSkills(true);
                
                isoMapRacing->m_pet_player->setTutorialMode(false);
                isoMapRacing->m_pet_opponent->setTutorialMode(false);
                
                isoMapRacing->m_AI_on_opponent->popToState(AI_SCOUT);
                
                isoMapRacing->_raceTimeLeft = 45.0f;
                //                int maxRound = isoMapRacing->m_pet_player->ran_round + 1;
                int maxRound = 3;
                isoMapRacing->m_pet_player->max_round = maxRound;
                isoMapRacing->m_pet_opponent->max_round = maxRound;
                isoMapRacing->updateCurrentLap(isoMapRacing->m_pet_player);
                
                isoMapRacing->racePanel->showGroupEnergy(true);
                
                isoMapRacing->pauseGame(); //resume after tapping onTapToStartTheRealRace
            }
                
            default:
                break;
        }
    }
}

void TutFirstRace::update(float delta_) {
    CTutUnit::update(delta_);
    
    if (m_state == TUTSTATE_DASH_OVER_TRAP_PRACTICE) {
        if (numberOfDashesDuringDashOverTrapPractice >= 1) { //allow dash only once after tutorial (if player doesn't dash, tutorial progresses anyway)
            this->goToNextTutorial();
            this->setState(TUTSTATE_STOMP);
        }
    }
    
    if (m_manualStompPractice->isRunningUpdate()) {
        if (!stompPracticeTimeOver) { //call once
            stompPracticeTime += delta_;
            if (stompPracticeTime >= 5.0f) { //5 seconds to see stomp animation and try stomp a few more times
                this->goToNextTutorial();
                this->setState(TUTSTATE_FREE_PLAY);
                m_manualStompPractice->setToInActiveNow();
                stompPracticeTimeOver = true;
            }
        }
    }
    
    switch (m_state) {
        case TUTSTATE_GAUGEPANEL:
        {
            if (m_pointerAtGo) {
                if (isoMapRacing->racePanel->gaugePanel->getCurrentPinResult() == GaugeRESULT_GOOD)
                {
                    m_pointerAtGo->pointer->setVisible(true);
                    if (m_pointerAtGo->pointer->numberOfRunningActions() == 0) {
                        m_pointerAtGo->pointer->
                        runAction(CCRepeatForever::create(CCSequence::create(CCEaseIn::create(CCMoveTo::create(0.25f, ccp(0.0f, m_pointerAtGo->button->getContentSize().height*0.3f)), 3.5f),
                                                                             CCEaseOut::create(CCMoveTo::create(0.25f, ccp(0.0f, m_pointerAtGo->button->getContentSize().height*0.6f)), 3.5f),
                                                                             NULL)));
                    }
                }
                else
                {
                    m_pointerAtGo->pointer->setVisible(false);
                    if (m_pointerAtGo->pointer->numberOfRunningActions() > 0) {
                        m_pointerAtGo->pointer->stopAllActions();
                    }
                }
            }
        }
            break;
            
        case TUTSTATE_DASH:
        {
            if (isoMapRacing->m_pet_opponent->m_summaryTrack <= isoMapRacing->m_pet_player->m_summaryTrack + 1)
            {
                if (!isoMapRacing->m_pet_player->m_usingDash) {
                    isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_B);
                    isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_B);
                }
                if (!isoMapRacing->m_pet_opponent->m_usingDash) {
                    isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_S);
                    isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_S);
                }
            }
            else
            {
                if (!isoMapRacing->m_pet_player->m_usingDash) {
                    isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_A);
                    isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_A);
                }
                if (!isoMapRacing->m_pet_opponent->m_usingDash) {
                    isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_F);
                    isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_F);
                }
            }
        }
            break;
            
        case TUTSTATE_DASH_PRACTICE:
        case TUTSTATE_DASH_OVER_TRAP:
        {
            if (isoMapRacing->m_pet_opponent->m_summaryTrack <= isoMapRacing->m_pet_player->m_summaryTrack + 4)
            {
                if (!isoMapRacing->m_pet_player->m_usingDash) {
                    isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_B);
                    isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_B);
                }
                if (!isoMapRacing->m_pet_opponent->m_usingDash) {
                    isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_S);
                    isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_S);
                }
            }
            else
            {
                if (!isoMapRacing->m_pet_player->m_usingDash) {
                    isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_A);
                    isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_A);
                }
                if (!isoMapRacing->m_pet_opponent->m_usingDash) {
                    isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_D);
                    isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_D);
                }
            }
        }
            break;
            
        case TUTSTATE_DASH_OVER_TRAP_PRACTICE:
        case TUTSTATE_STOMP:
        {
            if (isoMapRacing->m_pet_player->m_summaryTrack == isoMapRacing->m_pet_opponent->m_summaryTrack)
            {
                if (!isoMapRacing->m_pet_player->m_usingDash) {
                    isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_A);
                    isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_A);
                }
                if (!isoMapRacing->m_pet_opponent->m_usingDash) {
                    isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_B);
                    isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_B);
                }
            }
            else
                if (isoMapRacing->m_pet_player->m_summaryTrack > isoMapRacing->m_pet_opponent->m_summaryTrack)
                {
                    if (!isoMapRacing->m_pet_player->m_usingDash) {
                        isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_D);
                        isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_D);
                    }
                    if (!isoMapRacing->m_pet_opponent->m_usingDash) {
                        isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_A);
                        isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_A);
                    }
                }
                else
                {
                    if (!isoMapRacing->m_pet_player->m_usingDash) {
                        isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_S);
                        isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_S);
                    }
                    if (!isoMapRacing->m_pet_opponent->m_usingDash) {
                        isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_C);
                        isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_C);
                    }
                }
        }
            break;
            
        case TUTSTATE_FREE_PLAY:
        {
            if (!isoMapRacing->m_pet_player->m_usingDash) {
                isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_A);
                isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_A);
            }
            
            if (isoMapRacing->m_pet_opponent->m_summaryTrack <= isoMapRacing->m_pet_player->m_summaryTrack - 4)
            {
                if (!isoMapRacing->m_pet_opponent->m_usingDash) {
                    isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_A +50);
                    isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_A +50);
                    isoMapRacing->m_AI_on_opponent->setActive(true);
                }
            }
            else
            {
                if (!isoMapRacing->m_pet_opponent->m_usingDash) {
                    isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_B);
                    isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_B);
                    isoMapRacing->m_AI_on_opponent->setActive(false);
                }
            }
            
            
            //            if (!isoMapRacing->m_pet_player->m_usingDash) {
            //                isoMapRacing->m_pet_player->setStatStraightSpeed(SPEED_A);
            //                isoMapRacing->m_pet_player->setStatCornerSpeed(SPEED_A);
            //            }
            //            if (!isoMapRacing->m_pet_opponent->m_usingDash) {
            //                isoMapRacing->m_pet_opponent->setStatStraightSpeed(SPEED_B);
            //                isoMapRacing->m_pet_opponent->setStatCornerSpeed(SPEED_B);
            //            }
        }
            break;
            
        default:
            break;
    }
    
}

#pragma mark - callback functions

void TutFirstRace::onTapToProgressFirstSpeech(void* sender_, std::string result_) {
    
    if (TutFirstRace* _this = (TutFirstRace*)sender_) {
        if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
            if (typeid(*_tut) == typeid(CTutSpeech)) {
                if (_tut->option == CT_SPC_OPTION_NONE) {
                    
                    FlurryCpp::logEvent(EVENT_TUT_TUTFIRSTRACE_FIRST_SPEECH_TAPPED, (CCDictionary*)NULL);
                    
                    FBAppEventsWrapper::logEvent(EVENT_TUT_TUTFIRSTRACE_FIRST_SPEECH_TAPPED);
                    
                    CustomEventsHelper::Instance().doWhenTutorialsBegins();
                    
                    _this->goToNextTutorial();
                }
                return;
            }
        }
    }
}

void TutFirstRace::onTapToProgressSpeech(void* sender_, std::string result_) {
    
    if (TutFirstRace* _this = (TutFirstRace*)sender_) {
        if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
            if (typeid(*_tut) == typeid(CTutSpeech)) {
                if (_tut->option == CT_SPC_OPTION_NONE) {
                    _this->goToNextTutorial();
                }
                return;
            }
        }
    }
}

void TutFirstRace::onTapToShowGuagePanel(void* sender_, std::string result_) {
    
    if (TutFirstRace* _this = (TutFirstRace*)sender_) {
        if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
            if (typeid(*_tut) == typeid(CTutSpeech)) {
                if (_tut->option == CT_SPC_OPTION_NONE) {
                    _this->goToNextTutorial();
                    
                    _this->setState(TUTSTATE_GAUGEPANEL);
                    
                    _this->isoMapRacing->racePanel->showGauge();
                }
                return;
            }
        }
    }
}

void TutFirstRace::onTapToStartTheRealRace(void* sender_, std::string result_) {
    
    if (TutFirstRace* _this = (TutFirstRace*)sender_) {
        if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
            if (typeid(*_tut) == typeid(CTutSpeech)) {
                if (_tut->option == CT_SPC_OPTION_NONE) {
                    _this->goToNextTutorial();
                    
                    _this->isoMapRacing->racePanel->removeAllHints();
                    
                    _this->isoMapRacing->resumeGame();
                    
                    _this->isoMapRacing->racePanel->showHUD();
                    _this->isoMapRacing->racePanel->showPowerUpButtons(_this->isoMapRacing->racePanel->m_playerUsableSkills, true);
                }
                return;
            }
        }
    }
}

void TutFirstRace::onTapGo(void* sender_, std::string result_) {
    
    if (TutFirstRace* _this = (TutFirstRace*)sender_) {
        if (CTutPointAtNikButton* _tut = (CTutPointAtNikButton*)_this->m_tuts[_this->m_tutSelector]) {
            if (typeid(*_tut) == typeid(CTutPointAtNikButton)) {
                
                GaugeRESULT gresult = _this->isoMapRacing->racePanel->gaugePanel->getCurrentPinResult();
                
                if (gresult == GaugeRESULT_GOOD)
                {
                    _tut->clearGraphics();
                    
                    _this->goToNextTutorial();
                    _this->setState(TUTSTATE_DASH);
                    
                    _this->isoMapRacing->racePanel->respondToGoButton(GaugeRESULT_GOOD);
                    
                    return;
                } else {
                    
                    //                    _this->isoMapRacing->racePanel->gaugePanel->restart();
                    g_gameAudio->playEffect(AUDIO_ERROR);
                    
                    _this->isoMapRacing->racePanel->m_tutorialLabel->stopAllActions();
                    _this->isoMapRacing->racePanel->m_tutorialLabel->
                    runAction(CCSequence::create(CCScaleTo::create(0.075f, 1.14f),
                                                 CCScaleTo::create(0.075f, 1.0f),
                                                 NULL));
                    
                    return;
                }
                
            }
        }
    }
}

void TutFirstRace::onTapDash(void* sender_, std::string result_) {
    
    if (TutFirstRace* _this = (TutFirstRace*)sender_) {
        
        _this->m_race_tutor->setVisible(false);
        _this->m_race_tutor->clearAnimation();
        _this->isoMapRacing->resumeGame();
        _this->isoMapRacing->m_AI_on_player->m_canRecommend = true;
        _this->isoMapRacing->racePanel->removeSpotlightBG();
        
        _this->isoMapRacing->racePanel->m_tutorialLabel->setString("");
        _this->isoMapRacing->racePanel->m_tutorialLabel->setVisible(false);
        
        //hack
        if (CTutPointAtPowerupButtons* _tut = (CTutPointAtPowerupButtons*)_this->m_tuts[_this->m_tutSelector]) {
            _tut->clearGraphics();
        }
        _this->isoMapRacing->racePanel->forceEnabledPowerupButton(SKILL_DASH, false);
        
        _this->goToNextTutorial();
        _this->setState(TUTSTATE_DASH_PRACTICE);
        
        //hack
        if (CCNikButton* button = (CCNikButton*)_this->isoMapRacing->racePanel->m_groupPowerupButtons->getChildByTag(SKILL_DASH)) { //last button
            button->setButtonEvent(CCNikButtonEventTouchUpInside);
        }
        
    }
}

void TutFirstRace::onTapDashOverTrap(void* sender_, std::string result_) {
    
    if (TutFirstRace* _this = (TutFirstRace*)sender_) {
        
        _this->m_race_tutor->setVisible(false);
        _this->m_race_tutor->clearAnimation();
        _this->isoMapRacing->resumeGame();
        _this->isoMapRacing->m_AI_on_player->m_canRecommend = true;
        _this->isoMapRacing->racePanel->removeSpotlightBG();
        
        _this->isoMapRacing->racePanel->m_tutorialLabel->setString("");
        _this->isoMapRacing->racePanel->m_tutorialLabel->setVisible(false);
        
        //hack
        if (CTutPointAtPowerupButtons* _tut = (CTutPointAtPowerupButtons*)_this->m_tuts[_this->m_tutSelector]) {
            _tut->clearGraphics();
        }
        //        _this->isoMapRacing->racePanel->hidePowerUpButtons(true);
        
        _this->goToNextTutorial();
        _this->setState(TUTSTATE_DASH_OVER_TRAP_PRACTICE);
        
        //hack
        if (CCNikButton* button = (CCNikButton*)_this->isoMapRacing->racePanel->m_groupPowerupButtons->getChildByTag(SKILL_DASH)) { //last button
            button->setButtonEvent(CCNikButtonEventTouchUpInside);
        }
    }
}

void TutFirstRace::onTapStomp(void* sender_, std::string result_) {
    
    if (TutFirstRace* _this = (TutFirstRace*)sender_) {
        
        _this->m_race_tutor->setVisible(false);
        _this->m_race_tutor->clearAnimation();
        _this->isoMapRacing->resumeGame();
        _this->isoMapRacing->m_AI_on_player->m_canRecommend = true;
        _this->isoMapRacing->racePanel->removeSpotlightBG();
        
        _this->isoMapRacing->racePanel->m_tutorialLabel->setString("");
        _this->isoMapRacing->racePanel->m_tutorialLabel->setVisible(false);
        
        //hack
        if (CTutPointAtPowerupButtons* _tut = (CTutPointAtPowerupButtons*)_this->m_tuts[_this->m_tutSelector]) {
            _tut->clearGraphics();
        }
        //        _this->isoMapRacing->racePanel->hidePowerUpButtons(true);
        
        _this->goToNextTutorial();
        _this->setState(TUTSTATE_STOMP_PRACTICE);
        
        //hack
        if (CCNikButton* button = (CCNikButton*)_this->isoMapRacing->racePanel->m_groupPowerupButtons->getChildByTag(SKILL_DASH)) { //last button
            button->setButtonEvent(CCNikButtonEventTouchUpInside);
        }
    }
}


void TutFirstRace::showDashButton1()
{
    CCLog_("showDashButton1");
    
    SKILL skills_[5] = {SKILL_DASH, SKILL_NONE, SKILL_NONE, SKILL_NONE, SKILL_NONE};
    
    isoMapRacing->racePanel->showPowerUpButtons(skills_, true);
    isoMapRacing->racePanel->setAllPowerupButtonsCostVisible(false);
}

void TutFirstRace::showDashButton2()
{
    CCLog_("showDashButton2");
    
    SKILL skills_[5] = {SKILL_DASH, SKILL_NONE, SKILL_NONE, SKILL_NONE, SKILL_NONE};
    
    isoMapRacing->racePanel->showPowerUpButtons(skills_, true);
    isoMapRacing->racePanel->forceEnabledPowerupButton(SKILL_DASH, true);
    isoMapRacing->racePanel->setAllPowerupButtonsCostVisible(false);
}

void TutFirstRace::showStompButton()
{
    CCLog_("showStompButton");
    
    SKILL skills_[5] = {SKILL_DASH, SKILL_STOMP, SKILL_NONE, SKILL_NONE, SKILL_NONE};
    
    isoMapRacing->racePanel->showPowerUpButtons(skills_, true);
    isoMapRacing->racePanel->forceEnabledPowerupButton(SKILL_DASH, false);
    isoMapRacing->racePanel->setAllPowerupButtonsCostVisible(false);
}

void TutFirstRace::onRecommendSkill(void* sender_, SKILL skill_)
{
    //    CCLog_("onRecommendSkill:%d",skill_);
    
    if (TutFirstRace* _this = (TutFirstRace*)sender_) {
        
        if (skill_ == SKILL_DASH)
        {
            SKILL skills_[5] = {SKILL_NONE, SKILL_NONE, SKILL_NONE, SKILL_NONE, SKILL_NONE};
            skills_[0] = skill_;
            
            if (_this->getState() == TUTSTATE_DASH)
            {
                _this->m_race_tutor->setVisible(true);
                _this->m_race_tutor->setAnimation("learn_dash", true);
                
                _this->isoMapRacing->pauseGame();
                
                _this->scheduleOnce(schedule_selector(TutFirstRace::showDashButton1), SPINE_TUTORIAL_SHOW_BUTTON_DELAY);
                
                _this->isoMapRacing->racePanel->addSpotlightBG();
                
                _this->isoMapRacing->racePanel->m_tutorialLabel->setString("Tap to use the Dash ability!");
                _this->isoMapRacing->racePanel->m_tutorialLabel->setVisible(true);
                _this->isoMapRacing->racePanel->m_tutorialLabel->setPositionY(300.0f * sizeMultiplier);
            }
            
            bool startDashOverTrapTutorialNow = false;
            
            if (_this->m_state == TUTSTATE_DASH_PRACTICE)
            {
                _this->goToNextTutorial();
                _this->setState(TUTSTATE_DASH_OVER_TRAP);
                
                startDashOverTrapTutorialNow = true;
            }
            
            if (_this->m_state == TUTSTATE_DASH_OVER_TRAP || startDashOverTrapTutorialNow)
            {
                _this->m_race_tutor->setVisible(true);
                _this->m_race_tutor->setAnimation("learn_skip_banana", true);
                
                _this->isoMapRacing->pauseGame();
                
                _this->isoMapRacing->racePanel->hidePowerUpButtons(false);
                
                _this->scheduleOnce(schedule_selector(TutFirstRace::showDashButton2), SPINE_TUTORIAL_SHOW_BUTTON_DELAY);
                
                _this->isoMapRacing->racePanel->addSpotlightBG();
                
                _this->isoMapRacing->racePanel->m_tutorialLabel->setString("Dash over the bananas!");
                _this->isoMapRacing->racePanel->m_tutorialLabel->setVisible(true);
                _this->isoMapRacing->racePanel->m_tutorialLabel->setPositionY(300.0f * sizeMultiplier);
            }
        }
        else if (skill_ == SKILL_STOMP)
        {
            bool startStompTutorialNow = false;
            
            if (_this->m_state == TUTSTATE_DASH_OVER_TRAP_PRACTICE)
            {
                _this->goToNextTutorial();
                _this->setState(TUTSTATE_STOMP);
                
                startStompTutorialNow = true;
            }
            
            if (_this->m_state == TUTSTATE_STOMP || startStompTutorialNow)
            {
                SKILL skills_[5] = {SKILL_NONE, SKILL_NONE, SKILL_NONE, SKILL_NONE, SKILL_NONE};
                skills_[0] = SKILL_DASH;
                skills_[1] = SKILL_STOMP;
                
                if (_this->getState() == TUTSTATE_STOMP)
                {
                    _this->m_race_tutor->setVisible(true);
                    _this->m_race_tutor->setAnimation("learn_stomp", true);
                    
                    _this->isoMapRacing->pauseGame();
                    
                    _this->scheduleOnce(schedule_selector(TutFirstRace::showStompButton), SPINE_TUTORIAL_SHOW_BUTTON_DELAY);
                    
                    _this->isoMapRacing->racePanel->addSpotlightBG();
                    
                    _this->isoMapRacing->racePanel->m_tutorialLabel->setString("Tap to use the Jumping Smash ability!");
                    _this->isoMapRacing->racePanel->m_tutorialLabel->setVisible(true);
                    _this->isoMapRacing->racePanel->m_tutorialLabel->setPositionY(300.0f * sizeMultiplier);
                }
            }
        }
    }
}


void TutFirstRace::onUseSkill(void* sender_, SKILL skill_)
{
    TutFirstRace *_this = (TutFirstRace*)sender_;
    
    if (_this->m_manualDashOverTrapPractice &&
        _this->m_state == TUTSTATE_DASH_OVER_TRAP_PRACTICE) {
        _this->numberOfDashesDuringDashOverTrapPractice++;
    }
}

void TutFirstRace::onShowRaceResult(void* sender_, bool playerLose)
{
    TutFirstRace *_this = (TutFirstRace*)sender_;
    
    _this->goToNextTutorial();
    
}

void TutFirstRace::onTapToEndTheRace(void* sender_, std::string result_)
{
        TutFirstRace *_this = (TutFirstRace*)sender_;
    if (SceneRace* _scene = (SceneRace*)g_currentScene)
    {
        if (g_sceneManager->sceneSelector != SCENE_VILLE) {
            
            g_gameAudio->stopMusic();
            g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
            g_gameAudio->clearEffectFadeOut(3.0f);
            g_gameAudio->playEffectFadeOutAndFadeIn(AUDIO_AMBIENCE_FARM,true,2.5f);
            g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_VIL, true, 2.0, 2.5f, 3.0);
            _scene->fadeOutThisSceneAndChangeTo(SCENE_VILLE);
            
#ifndef DEBUG_WORKING_ON_TUTFIRSTRACE
            g_playerState->checklist(TutFirstRace::key());
#endif
        }
        
                _this->goToNextTutorial();
    }
}
