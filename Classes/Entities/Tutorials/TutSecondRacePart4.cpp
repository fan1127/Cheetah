//
//  TutSecondRacePart4.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutSecondRacePart4.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

//#include "../../Scenes/SceneVille.h"
//extern CCScene* g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Helpers/FlurryCpp.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

string TutSecondRacePart4::key() {
    
    return TutSecondRacePart1::key();
    
}

bool TutSecondRacePart4::check() {
    
    if (!g_playerState->Checklist[TutSecondRacePart4::key()] &&
        g_playerState->getNPet() > 0) {
        return true;
    }
    
    //    if (TutChooseFirstPet::check()) { //follow TutChooseFirstPet
    //        return true;
    //    }
    
    return false;
    
}

TutSecondRacePart4* TutSecondRacePart4::create(IsoMapRacing* isoMapRacing_, RacePanel* racePanel_) {
    
    if (TutSecondRacePart4::check()) {
        TutSecondRacePart4* pRet = new TutSecondRacePart4();
        if (pRet) {
            pRet->isoMapRacing = isoMapRacing_;
            pRet->racePanel = racePanel_;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool TutSecondRacePart4::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutSecondRacePart4::key();
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    g_gameContent->Word["tut_second_race_p4_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutSecondRacePart4::onTapTheBG));
    
    return true;
    
}

void TutSecondRacePart4::start() {
    
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P4, true);
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P4_START);
    
    if (isoMapRacing->racePanel)
    {
        isoMapRacing->racePanel->forceHideGetHelpButton(true);
        isoMapRacing->racePanel->forceHideGiveUpButton(true);
        isoMapRacing->racePanel->forceVeryEasyGauge(true);
    }
}

void TutSecondRacePart4::finish() {
    
    //end at TutRaceResult
    FlurryCpp::endTimedEvent(EVENT_TUT_TUT2NDRACE_P4, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P4_END);
    
    FBAppEventsWrapper::logEvent(EVENT_TUT_TUT2NDRACE_P4_END);
}

#pragma mark - callback functions

void TutSecondRacePart4::onTapTheBG(void* sender_, std::string result_) {
    
    if (TutSecondRacePart4* _this = (TutSecondRacePart4*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutSpeech)) {
                    if (_tut->option == CT_SPC_OPTION_NONE) {
                        
                        if (_this->isoMapRacing->racePanel)
                        {
                            _this->isoMapRacing->racePanel->forceHideGetHelpButton(true);
                            _this->isoMapRacing->racePanel->forceHideGiveUpButton(true);
                            _this->isoMapRacing->racePanel->forceVeryEasyGauge(true);
                        }
                        
                        _this->goToNextTutorial();
                    }
                    return;
                }
            }
        }
    }
    
}

//void TutFirstRacePart4::onContinue(void *sender_, std::string result_) {
//
//    if (TutFirstRacePart4* _this = (TutFirstRacePart4*)sender_) {
//        if (result_ == "") {
//            _this->m_tutSelector ++;
//        }else {
//            _this->m_tutSelector --;
//        }
//    }
//    
//}
