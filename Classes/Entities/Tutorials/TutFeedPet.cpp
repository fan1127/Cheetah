//
//  TutFeedPet.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutFeedPet.h"
USING_NS_CC;
using namespace std;

#include "../../Services/BackendService.h"
extern BackendService* g_gameBackend;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "TutSecondRacePart1.h"

#include "../../Helpers/FlurryCpp.h"

string TutFeedPet::key() {
    
    return "feed_pet";
    
}

bool TutFeedPet::check() {
    
    if (g_playerState->Checklist[TutFeedPet::key()] || !g_playerState->Checklist[TutFusePet::key()])
        return false;
    
    CCAssert(g_playerState->getNPet() > 0, "missing a pet?");
    
    int n = g_playerState->getNBuilding(kBuildingHabitat);
    for (int i=0; i<n; i++) {
        if (Building* _habitat = g_playerState->getBuilding(kBuildingHabitat, i)) {
            if (Pet* _pet = _habitat->pet) {
                if (_pet->isHungry() && !_pet->isSleeping()) {
                    return true;
                }
            }
        }
    }
    
    return false;
    
}

TutFeedPet* TutFeedPet::create(CCNikScrollView* scrollView_, HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_) {
    
    if (TutFeedPet::check()) {
        TutFeedPet* pRet = new TutFeedPet();
        if (pRet) {
            pRet->scrollView = scrollView_;
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

bool TutFeedPet::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    Building* _habitat = NULL;
    int n = g_playerState->getNBuilding(kBuildingHabitat);
    for (int i=0; i<n; i++) {
        if (Building* __habitat = g_playerState->getBuilding(kBuildingHabitat, i)) {
            if (Pet* _pet = __habitat->pet) {
                CCLog_("getHungerRatio: %f/%f", _pet->getHungerRatio(), g_gameContent->GameConfig["HUNGER_ANIMATION_ACTIVE"]);
                if (_pet->isHungry() && !_pet->isSleeping()) {
                    _habitat = __habitat;
                    break;
                }
            }
        }
    }
    if (!_habitat) return false;
    
    m_key = TutFeedPet::key();
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleHappy, false,
                                    g_gameContent->Word["tut_feed_pet_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutFeedPet::onTapTheBG));
    
    m_tuts.push_back(new CTutFocusOnBuilding(scrollView, isoMapBuilding, _habitat, this, TutFeedPet::onBuildingDidFocuzed));
    
    snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_guide_tap_building_text"].c_str(), _habitat->bVo.title.c_str());
    m_tuts.push_back(new CTutPointAtBuilding(_habitat, isoMapBuilding->entityAction, _buffer, this, TutFeedPet::onBuildingDidTouched));
    
    if (isoMapBuilding->entityAction) {
        CTutDragFoodToPet* _tut = new CTutDragFoodToPet(isoMapBuilding->entityAction);
        _tut->message = g_gameContent->Word["tut_feed_pet_tutorial_message_1"].c_str();
        _tut->callback = TutFeedPet::onPetFeeded;
        _tut->sender = this;
        m_tuts.push_back(_tut);
    }
    
    return true;
    
}

void TutFeedPet::start() {
    
    //Disable all components that could break the tutorial
    hudPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned(kBuildingHabitat);
    isoMapBuilding->entityAction->setFeedTutorialRunning(true);
    
    FlurryCpp::logEvent(EVENT_TUT_TUTFEEDPET, true);
    FlurryCpp::logEvent(EVENT_TUT_TUTFEEDPET_START);
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
}

void TutFeedPet::finish() {
    
    //Enable all components after finish the tutorial
    hudPanel->setDangerButtonsEnabled(true);
    hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned(""); //any
    isoMapBuilding->entityAction->setFeedTutorialRunning(false);
    
    g_playerState->checklist(key());
    
    if (SceneVille* _sceneVille = (SceneVille*)g_currentScene) {
        _sceneVille->addFoodTutorials();
        _sceneVille->addSecondRaceTutorial();
    }
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUTFEEDPET, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUTFEEDPET_END);
}

#pragma mark - callback functions

void TutFeedPet::onTapTheBG(void *sender_, std::string result_) {
    
    if (TutFeedPet* _this = (TutFeedPet*)sender_) {
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

void TutFeedPet::onBuildingDidFocuzed(void* sender_, std::string result_) {
    
    if (TutFeedPet* _this = (TutFeedPet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutFocusOnBuilding* _tut = (CTutFocusOnBuilding*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutFocusOnBuilding)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}

void TutFeedPet::onBuildingDidTouched(void *sender_, std::string result_) {
    
    if (TutFeedPet* _this = (TutFeedPet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtBuilding* _tut = (CTutPointAtBuilding*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtBuilding)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}

void TutFeedPet::onPetFeeded(void* sender_, std::string result_) { //result = "feeded" --> success, otherwise failed
    
    if (TutFeedPet* _this = (TutFeedPet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutDragFoodToPet* _tut = (CTutDragFoodToPet*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutDragFoodToPet)) {
                    if (result_ == CTutDragFoodToPetFailed) {
                        _this->goToPreviousTutorial();
                    }else {
                        _this->goToNextTutorial();
                    }
                }
            }
        }
    }
    
}
