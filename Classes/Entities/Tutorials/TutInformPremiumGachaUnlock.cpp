//
//  TutInformPremiumGachaUnlock.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutInformPremiumGachaUnlock.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
#include "../../Services/GameContentService.h"
#include "../../Helpers/FlurryCpp.h"

#include "../../UIs/GachaPanel.h"

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

string TutInformPremiumGachaUnlock::key() {
    
    return "inform_premium_gacha_unlock";
    
}

bool TutInformPremiumGachaUnlock::check() {
    
    if (PlayerState::getInstance()->Checklist[TutInformPremiumGachaUnlock::key()])
        return false;
    
    if (!GachaPanel::isPremiumBoxUnlocked()) {
        return false;
    }
    
    return true;
    
}

TutInformPremiumGachaUnlock* TutInformPremiumGachaUnlock::create(HUDPanel* hudPanel,
                                                                 ShopPanel* shopPanel,
                                                                 IsoMapBuilding* isoMapBuilding) {
    
    if (!TutInformPremiumGachaUnlock::check()) return NULL;
    
    TutInformPremiumGachaUnlock* p = new TutInformPremiumGachaUnlock();
    p->hudPanel = hudPanel;
    p->shopPanel = shopPanel;
    p->isoMapBuilding = isoMapBuilding;
    if (p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return NULL;
    
}

bool TutInformPremiumGachaUnlock::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutInformPremiumGachaUnlock::key();
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE,
                                    CT_CHR_SIDE_LEFT,
                                    kMerchantIdle,
                                    true,
                                    GameContentService::getInstance()->Word["tut_inform_premium_gacha_unlock_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutInformPremiumGachaUnlock::onTapTheBG));
    
    m_point_at_gacha_btn_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(hudPanel->mainMenu,
                                           hudPanel->menuButtons[HUDPanelMenuGacha],
                                           true, CCPointZero, 45.0f,
                                           this, TutInformPremiumGachaUnlock::onGachaButtonTapped));
    
    return true;
    
}

bool TutInformPremiumGachaUnlock::trig() {
    
    if (hudPanel->isLevelUpEffectShowed()) return false;
    return CTutUnit::trig();
    
}

void TutInformPremiumGachaUnlock::start() {
    
    hudPanel->setDangerButtonsEnabled(false);
    hudPanel->menuButtons[HUDPanelMenuGacha]->setEnabled(true);
    hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    shopPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("none");
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuMap, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuHome, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuGacha, true);
    
    FlurryCpp::logEvent(EVENT_TUT_INFORMPREMIUMGACHAUNLOCK, true);
    FlurryCpp::logEvent(EVENT_TUT_INFORMPREMIUMGACHAUNLOCK_START);
    
}

void TutInformPremiumGachaUnlock::finish() {
    
    hudPanel->setDangerButtonsEnabled(true);
    hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(true);
    shopPanel->setDangerButtonsEnabled(true);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("");
    
    hudPanel->setVisibilityAll(true);
    
    PlayerState::getInstance()->checklist(TutInformPremiumGachaUnlock::key());
    
    FlurryCpp::endTimedEvent(EVENT_TUT_INFORMPREMIUMGACHAUNLOCK, NULL);
    FlurryCpp::logEvent(EVENT_TUT_INFORMPREMIUMGACHAUNLOCK_END);
    
}

void TutInformPremiumGachaUnlock::update(float delta) {
    
    CTutUnit::update(delta);
    
    if(m_tutSelector == m_point_at_gacha_btn_index) {
        SceneVille* _sceneVille = (SceneVille*)g_currentScene;
        if(_sceneVille && _sceneVille->gachaPanel)
            goToNextTutorial();
        return;
    }
    
}

#pragma mark - callback functions

void TutInformPremiumGachaUnlock::onTapTheBG(void* sender, string result) {
    
    TutInformPremiumGachaUnlock* _this = (TutInformPremiumGachaUnlock*)sender;
    if (!_this || typeid(*_this) != typeid(TutInformPremiumGachaUnlock))
        return;
    
    if (_this->m_tutSelector >= _this->m_tuts.size())
        return;
    
    CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector];
    if (!_tut || typeid(*_tut) != typeid(CTutSpeech))
        return;
    
    if (_tut->option != CT_SPC_OPTION_NONE)
        return;
    
    _this->goToNextTutorial();
    
}

void TutInformPremiumGachaUnlock::onGachaButtonTapped(void *sender, std::string result) {
    
    TutInformPremiumGachaUnlock* _this = (TutInformPremiumGachaUnlock*)sender;
    if (!_this || _this->isOver()) return;
    
    CTutPointAtButton* _tut = (CTutPointAtButton*)_this->m_tuts[_this->m_tutSelector];
    if (!_tut || typeid(*_tut) != typeid(CTutPointAtButton)) return;
    
    if (result == CTutPointAtButtonFailed) return;
    
    _this->hudPanel->setDangerButtonsEnabled(true);
    _this->hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    _this->goToNextTutorial();
    
    FlurryCpp::logEvent(EVENT_TUT_INFORMPREMIUMGACHAUNLOCK_PRESS_BUTTON);
    
}
