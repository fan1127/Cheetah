//
//  TutIntroAndAskName.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutIntroAndAskName.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIUserNameChange.h"

#include "../../Helpers/FlurryCpp.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

string TutIntroAndAskName::key() {
    
    return "intro_and_ask_name";
    
}

bool TutIntroAndAskName::check() {
    
    if (!g_playerState->Checklist[TutIntroAndAskName::key()]) {
        return true;
    }
    
    return false;
    
}

TutIntroAndAskName* TutIntroAndAskName::create(IsoMapBuilding* isoMapBuilding_, HUDPanel* hudPanel_) {
    
    if (TutIntroAndAskName::check()) {
        if (TutIntroAndAskName* pRet = new TutIntroAndAskName()) {
            pRet->isoMapBuilding = isoMapBuilding_;
            pRet->hudPanel = hudPanel_;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
        }
    }
    return NULL;
    
}

bool TutIntroAndAskName::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutIntroAndAskName::key();
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleNote, false,
                                    g_gameContent->Word["tut_intro_and_ask_name_speech_2"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutIntroAndAskName::onTapTheBG));
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleNote, false,
                                    g_gameContent->Word["tut_intro_and_ask_name_speech_2"].c_str(),
                                    CT_SPC_OPTION_FILL_NAME,
                                    this, TutIntroAndAskName::onNameFilled));
    
    return true;
    
}

void TutIntroAndAskName::start() {
    
    isoMapBuilding->entityAction->setEntityTypeCanBeTouched("none");
    
    FlurryCpp::logEvent(EVENT_TUT_TUTINTROASKNAME, true);
    FlurryCpp::logEvent(EVENT_TUT_TUTINTROASKNAME_START);
    
    FBAppEventsWrapper::logEvent(EVENT_TUT_TUTINTROASKNAME_START);
    
    hudPanel->setVisibilityAll(false);
}

void TutIntroAndAskName::finish() {
    
    isoMapBuilding->entityAction->setEntityTypeCanBeTouched(""); // "" = any
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUTINTROASKNAME, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUTINTROASKNAME_END);
    
    FBAppEventsWrapper::logEvent(EVENT_TUT_TUTINTROASKNAME_END);
}

#pragma mark - callback functions

void TutIntroAndAskName::onTapTheBG(void* sender, std::string result) {
    
    if (TutIntroAndAskName* _this = (TutIntroAndAskName*)sender) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
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
    
}

void TutIntroAndAskName::onNameFilled(void *sender, std::string result) {
    
    if (TutIntroAndAskName* _this = (TutIntroAndAskName*)sender) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutSpeech)) {
                    if (_tut->option == CT_SPC_OPTION_FILL_NAME) {
                        BackendService::getInstance()->send(APIUserNameChange::create(result,
                                                                                      _this,
                                                                                      TutIntroAndAskName::onUserNameChanged));
                        _this->goToNextTutorial();
                    }
                }
            }
        }
    }
    
}

void TutIntroAndAskName::onUserNameChanged(void *sender, std::string json_str) {
    
    //JsonAllocator allocator;
    Json* root = Json_create(json_str.c_str());
    g_playerState->parseCurrentTime(root);
    g_playerState->parseUserProfile(root);
    g_playerState->parseChecklistData(root);
    Json_dispose(root);
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille))
        return;
    
    if (!_scene->hudPanel)
        return;
    
    _scene->hudPanel->updateHUD();
    
}
