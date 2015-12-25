//
//  TutRaceResult.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutRaceResult.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Helpers/FlurryCpp.h"

#include "TutFirstRace.h"

string TutRaceResult::key(int k) {
    
    if (k == 1) {
        return "race_win";
    }else if (k == 2) {
        return "race_lose";
    }
    return "race_result";
    
}

bool TutRaceResult::check() {
    
    if ((!g_playerState->Checklist[TutRaceResult::key(1)] ||
         !g_playerState->Checklist[TutRaceResult::key(2)]) &&
        !TutFirstRace::check()) {
        return true;
    }
    return false;
    
}

TutRaceResult* TutRaceResult::create(IsoMapRacing* isoMapRacing_) {
    
    if (TutRaceResult::check()) {
        TutRaceResult* pRet = new TutRaceResult();
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

bool TutRaceResult::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutRaceResult::key(0);
    
    isoMapRacing->raceResultPanel->setStartEnabled(false);
    
    return true;
    
}

bool TutRaceResult::trig() {
    
    if (!m_tutActive &&
        !m_tutFinish) {
        
        if (isoMapRacing->raceResultPanel->raceResultState == RaceResultPanel::LOSE) {
            if (!g_playerState->Checklist[TutRaceResult::key(2)]) {
                m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                                kUncleNote, false,
                                                g_gameContent->Word["tut_race_lose_speech_1"].c_str(),
                                                CT_SPC_OPTION_NONE,
                                                this, TutRaceResult::onTapTheBG));
            }
            m_tutActive = true;
            m_key = TutRaceResult::key(2);
            start();
            return true;
        }
        
        if (isoMapRacing->raceResultPanel->raceResultState == RaceResultPanel::WIN) {
            if (!g_playerState->Checklist[TutRaceResult::key(1)]) {
                m_tuts.push_back(new CTutSpeech(CT_CHR_FANS, CT_CHR_SIDE_LEFT,
                                                kFanIdle, false,
                                                g_gameContent->Word["tut_race_win_speech_1"].c_str(),
                                                CT_SPC_OPTION_NONE,
                                                this, TutRaceResult::onTapTheBG));
                
                m_tuts.push_back(new CTutSpeech(CT_CHR_SARAH, CT_CHR_SIDE_RIGHT,
                                                kSarahScared, false,
                                                g_gameContent->Word["tut_race_win_speech_3"].c_str(),
                                                CT_SPC_OPTION_NONE,
                                                this, TutRaceResult::onTapTheBG));
            }
            m_tutActive = true;
            m_key = TutRaceResult::key(1);
            start();
            return true;
        }
        
    }
    
    return false;
    
}

void TutRaceResult::start() {
    
    //    isoMapRacing->raceResultPanel->setButtonEnabled(false);
    
}

void TutRaceResult::finish() {
    
    isoMapRacing->raceResultPanel->setStartEnabled(true);
    isoMapRacing->raceResultPanel->start();
    //    isoMapRacing->raceResultPanel->setButtonEnabled(true);
    g_playerState->checklist(m_key);
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUT2NDRACE_P4, NULL);
}

#pragma mark - callback functions

void TutRaceResult::onTapTheBG(void* sender_, std::string result_) {
    
    if (TutRaceResult* _this = (TutRaceResult*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutSpeech)) {
                    if (_tut->option == CT_SPC_OPTION_NONE) {
                        _this->goToNextTutorial();
                    }
                }
            }
        }
    }
    
}
