//
//  TutRetrievePet.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutRetrievePet.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

extern CCScene* g_currentScene;

#include "../../UIs/Dialogs/AnimalBarnDialog.h"
#include "../../UIs/Dialogs/AnimalDialog.h"

#include "../../Helpers/FlurryCpp.h"
#include "../../Scenes/SceneVille.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

extern float sizeMultiplier;

string TutRetrievePet::key() {
    
    return "retrieve_pet";
    
}

bool TutRetrievePet::check() {
    
    if (!g_playerState->Checklist[TutRetrievePet::key()] &&
        g_playerState->getNPet() > 1) { // has more than one pet
        return true;
    }
    
    return false;
    
}

TutRetrievePet* TutRetrievePet::create(CCNikScrollView* scrollView_, HUDPanel* hudPanel_, ShopPanel* shopPanel_,IsoMapBuilding* isoMapBuilding_, CCModalController* modalController_) {
    
    if (TutRetrievePet::check()) {
        if (TutRetrievePet* pRet = new TutRetrievePet()) {
            pRet->scrollView = scrollView_;
            pRet->hudPanel = hudPanel_;
            pRet->shopPanel = shopPanel_;
            pRet->isoMapBuilding = isoMapBuilding_;
            pRet->modalController = modalController_;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
        }
    }
    return NULL;
    
}

bool TutRetrievePet::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    Building* _animal_barn = g_playerState->getBuilding(kBuildingAnimal, 0);
    if (!_animal_barn) {
        return false;
    }
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    text = CCLabelHelper::createBMFont(g_gameContent->Word["tut_guide_tap_to_retrieve"].c_str(), kFontSizeExtraLarge, true, kTextColorWhite);
    text->setPosition(_screenSize.width*0.5f, 290.0f*sizeMultiplier);
    text->setVisible(false);
    this->addChild(text);
    
    m_key = TutRetrievePet::key();
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    
    if (scrollView && isoMapBuilding) {
        m_tuts.push_back(new CTutFocusOnBuilding(scrollView, isoMapBuilding, _animal_barn, this, TutRetrievePet::onBuildingFocuzed));
    }
    
    snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_guide_tap_building_text"].c_str(), _animal_barn->bVo.title.c_str());
    m_tuts.push_back(new CTutPointAtBuilding(_animal_barn,
                                             isoMapBuilding->entityAction,
                                             _buffer,
                                             this, TutRetrievePet::onBuildingTapped));
    
    m_menu_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL,
                                           false, //effectEnabled = false
                                           this,
                                           TutRetrievePet::onMenuButtonTapped));
    
    m_retrieve_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL,
                                           true,
                                           CCPointZero,
                                           45.0f,
                                           this,
                                           TutRetrievePet::onTradeButtonTapped));
    
    m_tuts.push_back(new TutRetrievePetValidate(this, TutRetrievePet::onAllModalClosed));
    
    m_retrievePet = NULL;
    
    return true;
    
}

bool TutRetrievePet::trig() {
    
    if (!m_tutActive &&
        !m_tutFinish) {
        Building* _habitat1 = (Building*)g_playerState->getBuilding(kBuildingHabitat, 0);
        Building* _habitat2 = (Building*)g_playerState->getBuilding(kBuildingHabitat, 1);
        if (_habitat1 && _habitat2) {
            if (_habitat1->pbVo.status == BUILDING_STATUS_DELIVERED &&
                _habitat2->pbVo.status == BUILDING_STATUS_DELIVERED) {
                if (_habitat1->pbVo.puid == "" || _habitat2->pbVo.puid == "") {
                    m_tutActive = true;
                    start();
                    return true;
                }
            }
        }
    }
    
    return false;
    
}

void TutRetrievePet::start() {
    
    //Disable all components that could break the tutorial
    hudPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned(kBuildingAnimal);
    modalController->setHitBGToCloseAllEnabled(false);
    
    m_didSeeAnimalDialog = false;
    
    FlurryCpp::logEvent(EVENT_TUT_TUTRETRIEVEPET, true);
    FlurryCpp::logEvent(EVENT_TUT_TUTRETRIEVEPET_START);
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuMap, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuHome, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuGacha, true);
}

void TutRetrievePet::finish() {
    
    //Enable all components after finish the tutorial
    hudPanel->setDangerButtonsEnabled(true);
    hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned(""); //any
    modalController->setHitBGToCloseAllEnabled(true);
    
    g_playerState->checklist(key());
    
    if (SceneVille* _sceneVille = (SceneVille*)g_currentScene)
        _sceneVille->addAchievementTutorials();
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUTRETRIEVEPET, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUTRETRIEVEPET_END);
    
    FBAppEventsWrapper::logEvent(EVENT_TUT_TUTRETRIEVEPET_END);
}

void TutRetrievePet::update(float delta_) {
    
    CTutUnit::update(delta_);
    
    if (m_tutSelector == m_menu_btn_tut_index) {
        
        if (g_playerState->getNHomelessPet() == 0) {
            isoMapBuilding->entityAction->finish();
            skipThisTutorial();
            return;
        }
        
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
    
    if (m_tutSelector == m_retrieve_btn_tut_index) {
        
        if (modalController->m_views.size() > 1) {
            if (AnimalDialog* _dialog = (AnimalDialog*)modalController->m_views[modalController->m_views.size()-1]) {
                if (_dialog->_button_action2) {
                    _dialog->setSellBtnEnabled(false);
                    if (CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector]) {
                        if (!_tut->menuButton) {
                            text->setVisible(false);
                            m_didSeeAnimalDialog = true;
                            
                            if (Pet *_retrievedPet = (Pet*)_dialog->getEntity()) {
                                m_retrievePet = _retrievedPet;
                            }
                            
                            _tut->menuButton = _dialog->_button_action2;
                        }
                    }
                }
            }
        }else if (modalController->m_views.size() > 0) {
            if (AnimalBarnDialog* _dialog = (AnimalBarnDialog*)modalController->m_views[modalController->m_views.size()-1]) {
                _dialog->setCloseButtonEnabled(false);
            }
        }
        
        if (m_retrieve_btn_tut_index && modalController->m_views.size() == 1) {
            text->setVisible(true);
            
            if (m_didSeeAnimalDialog) {
                m_didSeeAnimalDialog = false;
                if (CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector]) {
                    _tut->clearGraphics();
                }
                onTradeButtonTapped(this, CTutPointAtButtonFailed);
            }
        }
        
    }
    
}

#pragma mark - callback functions

void TutRetrievePet::onTapTheBG(void* sender_, string result_) {
    
    if (TutRetrievePet* _this = (TutRetrievePet*)sender_) {
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

void TutRetrievePet::onBuildingFocuzed(void *sender_, std::string result_) {
    
    if (TutRetrievePet* _this = (TutRetrievePet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutFocusOnBuilding* _tut = (CTutFocusOnBuilding*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutFocusOnBuilding)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}

void TutRetrievePet::onBuildingTapped(void *sender_, string result_) {
    
    if (TutRetrievePet* _this = (TutRetrievePet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtBuilding* _tut = (CTutPointAtBuilding*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtBuilding)) {
                    
                    _this->goToNextTutorial();
                    
                }
            }
        }
    }
    
}

void TutRetrievePet::onMenuButtonTapped(void *sender_, std::string result_) {
    
    if (TutRetrievePet* _this = (TutRetrievePet*)sender_) {
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

void TutRetrievePet::onTradeButtonTapped(void *sender_, string result_) {
    
    if (TutRetrievePet* _this = (TutRetrievePet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtButton* _tut = (CTutPointAtButton*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtButton)) {
                    
                    _tut->menuButton = NULL;
                    if (result_ == CTutPointAtButtonFailed) {
                    }else {
                        _this->goToNextTutorial();
                    }
                    
                }
            }
        }
    }
    
}

void TutRetrievePet::onAllModalClosed(void *sender_,string result) {
    
    if (TutRetrievePet* _this = (TutRetrievePet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (TutRetrievePetValidate* _tut = (TutRetrievePetValidate*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(TutRetrievePetValidate)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}

#pragma mark - TutFusePetValidate

void TutRetrievePetValidate::update(float delta_) {
    
    if (!callback) return;
    
    if (TutRetrievePet* _tut = (TutRetrievePet*)sender) {
        
        if (Building *_habitat = g_playerState->getBuilding(_tut->m_retrievePet->ppVo.located_in)) {
            
            if (!_habitat->addPetAnimationRunningWithSmallDelay)
            {
                callback(sender, "");
            }
        }
    }
    
}
