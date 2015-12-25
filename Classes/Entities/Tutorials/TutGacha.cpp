//
//  TutGacha.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutGacha.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

#include "../../Helpers/FlurryCpp.h"
#include "../../Helpers/PushNotifications/PushNotifications.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

string TutGacha::key() {
    
    return "premium_gacha";
    
}

bool TutGacha::check() {
    
    if (!g_playerState->Checklist[TutGacha::key()] &&
        g_playerState->Checklist[TutSecondRacePart1::key()] &&
        (g_playerState->getBuilding(kBuildingHabitat, 1) || g_playerState->getBuilding(kBuildingAnimal, 0))) { // must have 2 habitats or 1 animal barn
        return true;
    }
    
    return false;
    
}

TutGacha* TutGacha::create(HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_) {
    
    if (TutGacha::check()) {
        if (TutGacha* pRet = new TutGacha()) {
            pRet->hudPanel = hudPanel_;
            pRet->shopPanel = shopPanel_;
            pRet->isoMapBuilding = isoMapBuilding_;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
        }
    }
    return NULL;
    
}

bool TutGacha::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutGacha::key();
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleHappy, false,
                                    g_gameContent->Word["tut_gacha_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutGacha::onTapTheBG));
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleNote, false,
                                    g_gameContent->Word["tut_gacha_speech_2"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutGacha::onTapTheBG));
    
    m_point_at_gacha_btn_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(hudPanel->mainMenu,
                                           hudPanel->menuButtons[HUDPanelMenuGacha],
                                           true, CCPointZero, 45.0f,
                                           this, TutGacha::onGachaButtonTapped));
    
    m_gacha_spin_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtNikButton(NULL, true,
                                              CCPointZero, 0.0f,
                                              this, TutGacha::onGachaSpinned));
    
    
    GDBuildingVO _animal_barn_vo = g_gameContent->getBuildingVO(kBuildingAnimal, 1);
    if (_animal_barn_vo.type == "") {
        return false;
    }
    
    return true;
    
}

void TutGacha::start() {
    
    hudPanel->setDangerButtonsEnabled(false);
    hudPanel->menuButtons[HUDPanelMenuGacha]->setEnabled(true);
    shopPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("none");
    
    FlurryCpp::logEvent(EVENT_TUT_TUTGACHA, true);
    FlurryCpp::logEvent(EVENT_TUT_TUTGACHA_START);
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuMap, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuHome, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuGacha, true);
}

void TutGacha::finish() {
    
    hudPanel->setDangerButtonsEnabled(true);
    hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    shopPanel->setDangerButtonsEnabled(true);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("");
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUTGACHA, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUTGACHA_END);
    
    FBAppEventsWrapper::logEvent(EVENT_TUT_TUTGACHA_END);
}

void TutGacha::update(float delta_) {
    
    CTutUnit::update(delta_);
    
    if (m_tutSelector == m_gacha_spin_btn_tut_index) {
        SceneVille* _scene = (SceneVille*)g_currentScene;
        if (!_scene || typeid(*_scene) != typeid(SceneVille) || !_scene->gachaPanel) return;
        CTutPointAtNikButton* _tut = (CTutPointAtNikButton*)m_tuts[m_tutSelector];
        if (!_tut) return;
        if (_tut->button) return;
        _tut->button = _scene->gachaPanel->normalButton;
        _tut->shift = _tut->button->getContentSize()*0.5f;
        _scene->gachaPanel->setBackButtonEnabled(false);
        _scene->gachaPanel->setFreeSpinButtonEnabled(false);
        return;
    }
    
    if(m_tutSelector == m_point_at_gacha_btn_index) {
        SceneVille* _sceneVille = (SceneVille*)g_currentScene;
        if(_sceneVille->gachaPanel)
            goToNextTutorial();
        return;
    }
    
}

#pragma mark - callback functions

void TutGacha::onTapTheBG(void* sender, std::string result) {
    
    TutGacha* _this = (TutGacha*)sender;
    if (!_this || _this->isOver()) return;
    
    CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector];
    if (!_tut || typeid(*_tut) != typeid(CTutSpeech) || _tut->option != CT_SPC_OPTION_NONE) return;
    
    _this->goToNextTutorial();
    
}

void TutGacha::onGachaButtonTapped(void *sender, std::string result) {
    
    TutGacha* _this = (TutGacha*)sender;
    if (!_this || _this->isOver()) return;
    
    CTutPointAtButton* _tut = (CTutPointAtButton*)_this->m_tuts[_this->m_tutSelector];
    if (!_tut || typeid(*_tut) != typeid(CTutPointAtButton)) return;
    
    if (result != CTutPointAtButtonFailed) {
        _this->hudPanel->setDangerButtonsEnabled(true);
        _this->hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
        _this->goToNextTutorial();
        FlurryCpp::logEvent(EVENT_TUT_TUTGACHA_PRESS_BUTTON);
        return;
    }
    
    _this->goToPreviousTutorial();
    
}

void TutGacha::onGachaSpinned(void *sender, std::string result) {
    
    TutGacha* _this = (TutGacha*)sender;
    if (!_this || _this->isOver());
    
    CTutPointAtNikButton* _tut = (CTutPointAtNikButton*)_this->m_tuts[_this->m_tutSelector];
    if (!_tut || typeid(*_tut) != typeid(CTutPointAtNikButton)) return;
    
    if (result != CTutPointAtNikButtonFailed) {
        SceneVille* _scene = (SceneVille*)g_currentScene;
        if (!_scene || !_scene->gachaPanel) return;
        _scene->gachaPanel->setBackButtonEnabled(true);
        _scene->gachaPanel->setFreeSpinButtonEnabled(true);
        _this->goToNextTutorial();
        return;
    }
    
    _this->goToPreviousTutorial();
    
}
