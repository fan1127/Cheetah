//
//  TutSecondRacePart2.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutSecondRacePart2.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../../Helpers/FlurryCpp.h"

string TutSecondRacePart2::key() {
    
    return "second_race";
    
}

bool TutSecondRacePart2::check() {
    
    if (!g_playerState->Checklist[TutSecondRacePart2::key()] &&
        g_playerState->getNPet() > 0) {
        return true;
    }
    
    return false;
    
}

TutSecondRacePart2* TutSecondRacePart2::create(WorldMap* worldMap_, HUDPanel* hudPanel_) {
    
    if (TutSecondRacePart2::check()) {
        TutSecondRacePart2* pRet = new TutSecondRacePart2();
        if (pRet) {
            pRet->worldMap = worldMap_;
            pRet->hudPanel = hudPanel_;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool TutSecondRacePart2::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutSecondRacePart2::key();
    
    if (worldMap) {
        if (worldMap->getVillages()->objectAtIndex(0)) {
            CCSprite* tmp = (CCSprite*)worldMap->getVillages()->objectAtIndex(0);
            m_tuts.push_back(new CTutPointAtVillage(worldMap, tmp, this, TutSecondRacePart2::onVillageTapped));
        }
        
        m_visit_button_tut_index = m_tuts.size();
        //m_tuts.push_back(new CTutPointAtButton(worldMap->visitButton, false, CCPointZero, 0.0f, this, TutSecondRacePart2::onButtonTapped));
        
    }
    
    return true;
    
}

void TutSecondRacePart2::start() {
    
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P2, true);
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P2_START);
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    if (hudPanel->menuButtons[HUDPanelMenuShop])
        hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuMap, true);
}

void TutSecondRacePart2::finish() {
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUT2NDRACE_P2, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUT2NDRACE_P2_END);
}

void TutSecondRacePart2::update(float delta_) {
    
    CTutUnit::update(delta_);
    
    if (m_tutSelector == m_visit_button_tut_index) {
        if (CTutPointAtButton* tut = (CTutPointAtButton*)m_tuts[m_tutSelector]) {
            if (worldMap->visitButton) {
                tut->menuButton = worldMap->visitButton;
            }
        }
    }
    
}

#pragma mark - callback functions

void TutSecondRacePart2::onTapTheBG(void* sender_, std::string result_) {
    
    if (TutSecondRacePart2* _this = (TutSecondRacePart2*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutSpeech)) {
                    if (_tut->option == CT_SPC_OPTION_NONE) {
                        _this->m_tutSelector ++;
                    }
                    return;
                }
            }
        }
    }
    
}

void TutSecondRacePart2::onVillageTapped(void *sender_, std::string result_) {
    
    if (TutSecondRacePart2* _this = (TutSecondRacePart2*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtVillage* _tut = (CTutPointAtVillage*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtVillage)) {
                    
                    _this->goToNextTutorial();
                    
                }
            }
        }
    }
    
}

void TutSecondRacePart2::onButtonTapped(void *sender_, std::string result_) {
    
    if (TutSecondRacePart2* _this = (TutSecondRacePart2*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtButton* _tut = (CTutPointAtButton*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtButton)) {
                    
                    if (result_ == CTutPointAtNikButtonFailed) {
                        _this->goToPreviousTutorial();
                    }else {
                        _this->goToNextTutorial();
                    }
                    
                }
            }
        }
    }
    
}
