//
//  TutSecondRacePart3.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutSecondRacePart3.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../../Helpers/FlurryCpp.h"

string TutSecondRacePart3::key() {
    
    return TutSecondRacePart1::key();
    
}

bool TutSecondRacePart3::check() {
    
    if (!g_playerState->Checklist[TutSecondRacePart3::key()] &&
        g_playerState->getNPet() > 0) {
        return true;
    }
    
    return false;
    
}

TutSecondRacePart3* TutSecondRacePart3::create(IsoMapMatch* isoMapMatch_, MatchPanel* matchPanel_) {
    
    if (TutSecondRacePart3::check()) {
        TutSecondRacePart3* pRet = new TutSecondRacePart3();
        if (pRet) {
            pRet->isoMapMatch = isoMapMatch_;
            pRet->matchPanel = matchPanel_;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool TutSecondRacePart3::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutSecondRacePart3::key();
    
    if (matchPanel->m_menuRace) {
        //m_tuts.push_back(new CTutPointAtButton(matchPanel->m_menuRace, true, CCPointZero, -45.0f, this, TutSecondRacePart3::onButtonTapped));
    }
    
    //    if (matchPanel->m_menuRace) {
    //        m_tuts.push_back(new CTutPointAtButton(matchPanel->m_menuRace, this, TutFirstRacePart3::onTapTheBG));
    //    }
    
    return true;
    
}

void TutSecondRacePart3::start() {
    
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P3, true);
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P3_START);
}

void TutSecondRacePart3::finish() {
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUT2NDRACE_P3, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P3_END);
}

#pragma mark - callback functions

//void TutSecondRacePart3::onTapTheBG(void* sender_, std::string result_) {
//
//    if (TutSecondRacePart3* _this = (TutSecondRacePart3*)sender_) {
//        if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
//            if (typeid(*_tut) == typeid(CTutSpeech)) {
//                if (_tut->option == CT_SPC_OPTION_NONE) {
//                    _this->m_tutSelector ++;
//                }
//                return;
//            }
//        }
//    }
//
//}

void TutSecondRacePart3::onButtonTapped(void *sender_, std::string result_) {
    
    if (TutSecondRacePart3* _this = (TutSecondRacePart3*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtButton* _tut = (CTutPointAtButton*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtButton)) {
                    _tut->menuButton = NULL;
                    if (result_ == CTutPointAtButtonFailed) {
                        _this->goToPreviousTutorial();
                    }else {
                        _this->goToNextTutorial();
                    }
                }
            }
        }
    }
    
}
