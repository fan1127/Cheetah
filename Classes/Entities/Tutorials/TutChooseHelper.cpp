//
//  TutChooseHelper.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutChooseHelper.h"
USING_NS_CC;
using namespace std;

#include "SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../../Helpers/FlurryCpp.h"

string TutChooseHelper::key() {
    
    return "choose_helper";
    
}

bool TutChooseHelper::check() {
    
    bool hasntCompletedThisTutorial = !g_playerState->Checklist[TutChooseHelper::key()];
    int levelRequirement = (int)g_gameContent->GameConfig["PET_HELPER_UNLOCKS_AT_LEVEL"];
    bool levelIsAboveTheRequirement = g_playerState->player.racing.level >= levelRequirement;
    
    if (hasntCompletedThisTutorial && levelIsAboveTheRequirement) {
        return true;
    }
    
    return false;
    
}

TutChooseHelper* TutChooseHelper::create(MatchPanel* matchPanel_) {
    
    if (TutChooseHelper::check()) {
        TutChooseHelper* pRet = new TutChooseHelper();
        if (pRet) {
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

bool TutChooseHelper::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutChooseHelper::key();
    
    string message = g_gameContent->Word["tut_facebook_login_speech_1"];
    
    if (g_sceneManager->facebook->isLoggedIn()) {
        message = g_gameContent->Word["tut_facebook_login_speech_1_loggedin"];
    }
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    message.c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutChooseHelper::onTapTheBG));
    
    m_didTryToLaunch = false;
    
    matchPanel->onWillLaunchChooseHelper = TutChooseHelper::onWillLaunchChooseHelper;
    matchPanel->onWillLaunchChooseHelperSender = this;
    
    return true;
    
}

bool TutChooseHelper::trig() {
    
    if (!m_tutActive &&
        !m_tutFinish) {
        if (m_didTryToLaunch) {
            m_tutActive = true;
            start();
            return true;
        }
    }
    return false;
    
}

void TutChooseHelper::start() {
    
    FlurryCpp::logEvent(EVENT_TUT_TUTCHOOSEHELPER, true);
    FlurryCpp::logEvent(EVENT_TUT_TUTCHOOSEHELPER_START);
}

void TutChooseHelper::finish() {
    
    matchPanel->onWillLaunchChooseHelper = NULL;
    matchPanel->onWillLaunchChooseHelperSender = NULL;
    matchPanel->setChooseHelperEnabled(true);
    matchPanel->launchChooseHelper();
    
    g_playerState->checklist(TutChooseHelper::key());
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUTCHOOSEHELPER, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUTCHOOSEHELPER_END);
}

#pragma mark - callback functions

void TutChooseHelper::onWillLaunchChooseHelper(void *sender) {
    
    if (TutChooseHelper* _this = (TutChooseHelper*)sender) {
        _this->matchPanel->setChooseHelperEnabled(false);
        _this->m_didTryToLaunch = true;
    }
    
}

void TutChooseHelper::onTapTheBG(void* sender_, std::string result_) {
    
    if (TutChooseHelper* _this = (TutChooseHelper*)sender_) {
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
