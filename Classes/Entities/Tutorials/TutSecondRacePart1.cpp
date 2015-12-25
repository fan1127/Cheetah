//
//  TutSecondRacePart1.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutSecondRacePart1.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Helpers/FlurryCpp.h"
#include "TutFeedPet.h"
#include "../../Services/BackendService.h"

string TutSecondRacePart1::key() {
    
    return "second_race";
    
}

bool TutSecondRacePart1::check() {
    
    return ((!g_playerState->Checklist[TutSecondRacePart1::key()]) &&
            (g_playerState->Checklist[TutFeedPet::key()]) &&
            (g_playerState->player.racing.level >= 2));
    
}

TutSecondRacePart1* TutSecondRacePart1::create(HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_) {
    
    if (TutSecondRacePart1::check()) {
        TutSecondRacePart1* pRet = new TutSecondRacePart1();
        if (pRet) {
            pRet->hudPanel = hudPanel_;
            pRet->shopPanel = shopPanel_;
            pRet->isoMapBuilding = isoMapBuilding_;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool TutSecondRacePart1::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutSecondRacePart1::key();
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleHappy, true,
                                    g_gameContent->Word["tut_second_race_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutSecondRacePart1::onTapTheBG));
    
    m_tuts.push_back(new CTutPointAtButton(hudPanel->mainMenu,
                                           hudPanel->menuButtons[HUDPanelMenuMap],
                                           true, CCPointZero, -45.0f,
                                           this, TutSecondRacePart1::onButtonTapped));
    
    return true;
    
}

void TutSecondRacePart1::start() {
    
    if (g_playerState->getNPet() <= 0 ||
        g_playerState->player.racing.host == "") {
        skipThisTutorial();
        return;
    }
    
    hudPanel->setDangerButtonsEnabled(false);
    hudPanel->menuButtons[HUDPanelMenuMap]->setEnabled(true);
    shopPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("none");
    
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P1, true);
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P1_START);
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuMap, true);
}

void TutSecondRacePart1::finish() {
    
    hudPanel->setDangerButtonsEnabled(true);
    hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    shopPanel->setDangerButtonsEnabled(true);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("");
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUT2NDRACE_P1, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P1_END);
}

#pragma mark - callback functions

void TutSecondRacePart1::onTapTheBG(void* sender_, string result_) {
    
    if (TutSecondRacePart1* _this = (TutSecondRacePart1*)sender_) {
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

void TutSecondRacePart1::onButtonTapped(void *sender_, string result_) {
    
    if (TutSecondRacePart1* _this = (TutSecondRacePart1*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtButton* _tut = (CTutPointAtButton*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtButton)) {
                    
                    if (BackendService::getInstance()->isWaitingForQueue() || BackendService::getInstance()->isWaitingForResponse()) {
                        _this->m_tuts.push_back(new CTutPointAtButton(_this->hudPanel->mainMenu,
                                                                      _this->hudPanel->menuButtons[HUDPanelMenuMap],
                                                                      true, CCPointZero, -45.0f,
                                                                      _this, TutSecondRacePart1::onButtonTapped));
                    }
                    
                    if (result_ != CTutPointAtButtonFailed) {
                        _this->hudPanel->setDangerButtonsEnabled(true);
                        _this->goToNextTutorial();
                    }
                    
                }
            }
        }
    }
    
}
