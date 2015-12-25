//
//  TutSetLeader.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutSetLeader.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../UIs/Dialogs/AnimalDialog.h"

#include "../../Helpers/FlurryCpp.h"

extern float sizeMultiplier;

#include "TutRetrievePet.h"

string TutSetLeader::key() {
    
    return "set_leader";
    
}

bool TutSetLeader::check() {
    
    if (
        //TODO: check if the user has just started download at version 1.0.7 or later
        //TODO: skip this tutorial for version 1.0.6 or before???
        
        //        !g_playerState->Checklist[TutRetrievePet::key()] &&
        !g_playerState->Checklist[TutSetLeader::key()] &&
        g_playerState->getNPet() > 1) { // has more than one pet
        return true;
    }
    
    return false;
    
}

TutSetLeader* TutSetLeader::create(CCNikScrollView* scrollView_, HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_, CCModalController* modalController_) {
    
    if (TutSetLeader::check()) {
        if (TutSetLeader* pRet = new TutSetLeader()) {
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

bool TutSetLeader::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutSetLeader::key();
    
    //    CREATE_CHAR_BUFFER(_buffer, 128);
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_SARAH, CT_CHR_SIDE_RIGHT,
                                    kSarahHappy, false,
                                    g_gameContent->Word["tut_set_leader_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutSetLeader::onTapTheBG));
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    g_gameContent->Word["tut_set_leader_speech_2"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutSetLeader::onTapTheBG));
    
    if (scrollView && isoMapBuilding) {
        m_focus_tut_index = m_tuts.size();
        m_tuts.push_back(new CTutFocusOnBuilding(scrollView, isoMapBuilding, NULL, this, TutSetLeader::onBuildingFocuzed));
    }
    
    m_pointatbuilding_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtBuilding(NULL,
                                             isoMapBuilding->entityAction,
                                             "",
                                             this, TutSetLeader::onBuildingTapped));
    
    m_menu_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL,
                                           false, //effectEnabled = false
                                           CCPointZero,
                                           45.0f,
                                           this,
                                           TutSetLeader::onMenuButtonTapped));
    
    m_setleader_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL,
                                           true,
                                           CCPointZero,
                                           -45.0f,
                                           this,
                                           TutSetLeader::onSetLeaderButtonTapped));
    
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_SARAH, CT_CHR_SIDE_RIGHT,
                                    kUncleHappy, false,
                                    g_gameContent->Word["tut_set_leader_speech_3"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutSetLeader::onTapTheBG));
    return true;
    
}

bool TutSetLeader::trig() {
    
    if (!m_tutActive &&
        !m_tutFinish) {
        m_tutActive = true;
        start();
        return true;
    }
    
    return false;
    
}

void TutSetLeader::start() {
    
    //Disable all components that could break the tutorial
    hudPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned(kBuildingHabitat);
    modalController->setHitBGToCloseAllEnabled(false);
    
}

void TutSetLeader::finish() {
    
    //Enable all components after finish the tutorial
    hudPanel->setDangerButtonsEnabled(true);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned(""); //any
    modalController->setHitBGToCloseAllEnabled(true);
    g_playerState->checklist(TutSetLeader::key());
    
}

void TutSetLeader::update(float delta_) {
    
    CTutUnit::update(delta_);
    
    
    if (scrollView && isoMapBuilding) {
        if (m_tutSelector == m_focus_tut_index) {
            
            
            if (CTutFocusOnBuilding* _tut = (CTutFocusOnBuilding*)m_tuts[m_tutSelector]) {
                
                if (!_tut->building) {
                    Pet *highestHPPet = g_playerState->getPetWithHighestHorsePower();
                    Building* _targetHabitat = g_playerState->getBuilding(highestHPPet->ppVo.located_in);
                    
                    _tut->building = _targetHabitat;
                }
            }
            
        }
    }
    
    if (m_tutSelector == m_pointatbuilding_tut_index) {
        
        Pet *highestHPPet = g_playerState->getPetWithHighestHorsePower();
        CREATE_CHAR_BUFFER(_buffer, 128);
        snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_guide_tap_animal_pet_text"].c_str(), highestHPPet->pVo.title.c_str());
        
        if (CTutPointAtBuilding* _tut = (CTutPointAtBuilding*)m_tuts[m_tutSelector]) {
            
            Pet *highestHPPet = g_playerState->getPetWithHighestHorsePower();
            Building* _targetHabitat = g_playerState->getBuilding(highestHPPet->ppVo.located_in);
            
            _tut->building = _targetHabitat;
            _tut->message = _buffer;
        }
    }
    
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
    
    if (m_tutSelector == m_setleader_btn_tut_index
        && modalController->m_views.size() > 0) {
        
        if (AnimalDialog* _dialog = (AnimalDialog*)modalController->m_views[modalController->m_views.size()-1]) {
            _dialog->setCloseButtonEnabled(false);
            _dialog->setSendToBarnButtonEnabled(false);
            
            if (_dialog->menuSetLeader) {
                if (CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector]) {
                    if (!_tut->menuButton) {
                        //                        text->setVisible(false);
                        _tut->menuButton = _dialog->menuSetLeader;
                    }
                }
            }
        }
    }
    
}

#pragma mark - callback functions

void TutSetLeader::onTapTheBG(void* sender_, string result_) {
    
    if (TutSetLeader* _this = (TutSetLeader*)sender_) {
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

void TutSetLeader::onBuildingFocuzed(void *sender_, std::string result_) {
    
    if (TutSetLeader* _this = (TutSetLeader*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutFocusOnBuilding* _tut = (CTutFocusOnBuilding*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutFocusOnBuilding)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}

void TutSetLeader::onBuildingTapped(void *sender_, string result_) {
    
    if (TutSetLeader* _this = (TutSetLeader*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtBuilding* _tut = (CTutPointAtBuilding*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtBuilding)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}

void TutSetLeader::onMenuButtonTapped(void *sender_, std::string result_) {
    
    if (TutSetLeader* _this = (TutSetLeader*)sender_) {
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

void TutSetLeader::onSetLeaderButtonTapped(void *sender_, string result_) {
    
    if (TutSetLeader* _this = (TutSetLeader*)sender_) {
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
