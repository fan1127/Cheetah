//
//  TutFusePet.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutFusePet.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../UIs/Dialogs/AnimalMarketDialog.h"
#include "../../UIs/Dialogs/AnimalTradeDialog.h"

#include "../../Helpers/FlurryCpp.h"
#include "../../Scenes/SceneVille.h"

extern float sizeMultiplier;

string TutFusePet::key() {
    
    return "fuse_pet";
    
}

bool TutFusePet::check() {
    
    if (!g_playerState->Checklist[TutFusePet::key()] &&
        g_playerState->getNPet() == 0) { // has no pet
        return true;
    }
    
    return false;
    
}

TutFusePet* TutFusePet::create(CCNikScrollView* scrollView,
                               HUDPanel* hudPanel,
                               ShopPanel* shopPanel,
                               IsoMapBuilding* isoMapBuilding,
                               CCModalController* modalController) {
    
    if (TutFusePet::check()) {
        if (TutFusePet* pRet = new TutFusePet()) {
            pRet->scrollView = scrollView;
            pRet->hudPanel = hudPanel;
            pRet->shopPanel = shopPanel;
            pRet->isoMapBuilding = isoMapBuilding;
            pRet->modalController = modalController;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
        }
    }
    return NULL;
    
}

bool TutFusePet::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    Building* _petshop = g_playerState->getBuilding(kBuildingFusion, 0);
    if (!_petshop) {
        return false;
    }
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    text = CCLabelHelper::createBMFont(g_gameContent->Word["tut_guide_tap_to_choose"].c_str(),
                                       kFontSizeExtraLarge,
                                       true,
                                       kTextColorWhite);
    text->setPosition(_screenSize.width*0.5f, 290.0f*sizeMultiplier);
    text->setVisible(false);
    this->addChild(text);
    
    m_key = TutFusePet::key();
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    g_gameContent->Word["tut_fuse_pet_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutFusePet::onTapTheBG));
    
    if (scrollView && isoMapBuilding) {
        m_tuts.push_back(new CTutFocusOnBuilding(scrollView,
                                                 isoMapBuilding,
                                                 _petshop,
                                                 this,
                                                 TutFusePet::onBuildingFocuzed));
    }
    
    snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_guide_tap_building_text"].c_str(), _petshop->bVo.title.c_str());
    m_tuts.push_back(new CTutPointAtBuilding(_petshop,
                                             isoMapBuilding->entityAction,
                                             _buffer,
                                             this, TutFusePet::onBuildingTapped));
    
    m_menu_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL,
                                           false, //effectEnabled = false
                                           this,
                                           TutFusePet::onMenuButtonTapped));
    
    m_trade_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL,
                                           true,
                                           CCPointZero,
                                           45.0f,
                                           this,
                                           TutFusePet::onTradeButtonTapped));
    
    m_tuts.push_back(new TutFusePetValidate(this, TutFusePet::onAllModalClosed));
    
    return true;
    
}

bool TutFusePet::trig() {
    
    if (!m_tutActive &&
        !m_tutFinish) {
        if (Building* _habitat = (Building*)g_playerState->getBuilding(kBuildingHabitat, 0)) {
            if (_habitat->pbVo.status == BUILDING_STATUS_DELIVERED && _habitat->pbVo.puid == "") {
                
                m_tutActive = true;
                start();
                
                return true;
            }
        }
    }
    
    return false;
    
}

void TutFusePet::start() {
    
    //Disable all components that could break the tutorial
    hudPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned(kBuildingFusion);
    modalController->setHitBGToCloseAllEnabled(false);
    
    m_didSeeAnimalTradeDialog = false;
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    
    FlurryCpp::logEvent(EVENT_TUT_TUTFUSEPET, true);
    FlurryCpp::logEvent(EVENT_TUT_TUTFUSEPET_START);
    
}

void TutFusePet::finish() {
    
    //Enable all components after finish the tutorial
    hudPanel->setDangerButtonsEnabled(true);
    hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned(""); //any
    modalController->setHitBGToCloseAllEnabled(true);
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUTFUSEPET, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUTFUSEPET_END);
    
    if (SceneVille* _sceneVille = (SceneVille*)g_currentScene) {
        _sceneVille->addFoodTutorials();
        _sceneVille->addSecondRaceTutorial();
    }
    
}

void TutFusePet::update(float delta_) {
    
    CTutUnit::update(delta_);
    
    if (m_tutSelector == m_menu_btn_tut_index) {
        if (isoMapBuilding->entityAction) {
            if (CCMenu* _menu = (CCMenu*)isoMapBuilding->entityAction->m_menuLayer->getChildByTag(2)) {
                if (CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector]) {
                    if (!_tut->menuButton) {
                        _tut->menuButton = _menu;
                    }
                }
            }
        }
    }
    
    //disabled close button of the dialog
    if (m_tutSelector == m_trade_btn_tut_index) {
        if (modalController->m_views.size() > 0) { //first dialog
            if (AnimalMarketDialog* _dialog = (AnimalMarketDialog*)modalController->m_views[modalController->m_views.size()-1]) {
                _dialog->setCloseButtonEnabled(false);
            }
        }
    }
    
    if (m_tutSelector == m_trade_btn_tut_index && modalController->m_views.size() > 1) { //second dialog on stack
        if (AnimalTradeDialog* _dialog = (AnimalTradeDialog*)modalController->m_views[modalController->m_views.size()-1]) {
            if (_dialog->menuTrade) {
                _dialog->setCloseButtonEnabled(true);
                if (CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector]) {
                    if (!_tut->menuButton) {
                        text->setVisible(false);
                        m_didSeeAnimalTradeDialog = true;
                        _tut->menuButton = _dialog->menuTrade;
                    }
                }
            }
        }
    }
    
    if (m_tutSelector == m_trade_btn_tut_index && modalController->m_views.size() == 1) {
        text->setVisible(true);
        if (m_didSeeAnimalTradeDialog) {
            m_didSeeAnimalTradeDialog = false;
            if (CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector]) {
                _tut->clearGraphics();
            }
            onTradeButtonTapped(this, CTutPointAtButtonFailed);
        }
    }
    
}

#pragma mark - callback functions

void TutFusePet::onTapTheBG(void* sender_, string result_) {
    
    if (TutFusePet* _this = (TutFusePet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutSpeech)) {
                    if (_tut->option == CT_SPC_OPTION_NONE) {
                        _this->goToNextTutorial();
                        //end speech tut_fuse_pet_speech_3 -> enable select button
                        if (_this->m_tutSelector == _this->m_trade_btn_tut_index) {
                            if (AnimalMarketDialog* _dialog = (AnimalMarketDialog*)_this->modalController->m_views[_this->modalController->m_views.size()-1]) {
                                _dialog->setSelectButtonEnabled(true);
                            }
                        }
                    }
                }
            }
        }
    }
    
}

void TutFusePet::onBuildingFocuzed(void *sender_, std::string result_) {
    
    if (TutFusePet* _this = (TutFusePet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutFocusOnBuilding* _tut = (CTutFocusOnBuilding*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutFocusOnBuilding)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}

void TutFusePet::onBuildingTapped(void *sender_, string result_) {
    
    if (TutFusePet* _this = (TutFusePet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtBuilding* _tut = (CTutPointAtBuilding*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtBuilding)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}

void TutFusePet::onMenuButtonTapped(void *sender_, std::string result_) {
    
    if (TutFusePet* _this = (TutFusePet*)sender_) {
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

void TutFusePet::onTradeButtonTapped(void *sender_, string result_) {
    
    if (TutFusePet* _this = (TutFusePet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtButton* _tut = (CTutPointAtButton*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtButton)) {
                    
                    _tut->menuButton = NULL;
                    if (result_ == CTutPointAtButtonFailed) {
                        //_this->goToPreviousTutorial();
                    }else {
                        _this->goToNextTutorial();
                    }
                    
                }
            }
        }
    }
    
}

void TutFusePet::onAllModalClosed(void *sender_,string result) {
    
    if (TutFusePet* _this = (TutFusePet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (TutFusePetValidate* _tut = (TutFusePetValidate*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(TutFusePetValidate)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}

#pragma mark - TutFusePetValidate

void TutFusePetValidate::update(float delta_) {
    
    if (!callback) return;
    
    if (TutFusePet* _tut = (TutFusePet*)sender) {
        if (_tut->modalController->getNStackView() == 0) {
            callback(sender, "");
        }
    }
    
}
