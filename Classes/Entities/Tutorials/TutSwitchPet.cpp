//
//  TutSwitchPet.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutSwitchPet.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

string TutSwitchPet::key() {
    
    return "switch_pet";
    
}

bool TutSwitchPet::check() {
    
    if (!g_playerState->Checklist[TutSwitchPet::key()]) {
        if (g_playerState->PetRunner) {
            //            if (g_playerState->PetRunner->getSecondsLeftToFullEnergy() > 0) {
            for (vector<Pet*>::iterator it = g_playerState->Pets.begin(); it != g_playerState->Pets.end(); ++it) {
                if (Pet* _pet = (Pet*)(*it)) {
                    if (_pet->ppVo.located_in != "") {
                        if (_pet != g_playerState->PetRunner && _pet->getSecondsLeftToFullEnergy() == 0) {
                            return true;
                        }
                    }
                }
            }
            //            }
        }
    }
    
    return false;
    
}

TutSwitchPet* TutSwitchPet::create(MatchPanel* matchPanel_) {
    
    if (TutSwitchPet::check()) {
        TutSwitchPet* pRet = new TutSwitchPet();
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

bool TutSwitchPet::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutSwitchPet::key();
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, true,
                                    g_gameContent->Word["tut_switch_pet_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutSwitchPet::onTapTheBG));
    
    m_switch_pet_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL, this, TutSwitchPet::onPetSwitched));
    
    return true;
    
}

void TutSwitchPet::update(float delta_) {
    
    CTutUnit::update(delta_);
    
    if (m_tutSelector == m_switch_pet_btn_tut_index) {
        
        if (CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector]) {
            if (!_tut->menuButton) {
                
                int select_index = -1;
                int i = 0;
                for (vector<Pet*>::iterator it = g_playerState->Pets.begin(); it != g_playerState->Pets.end(); ++it) {
                    if (Pet* _pet = (Pet*)(*it)) {
                        if (_pet->ppVo.located_in != "") {
                            if (_pet->getSecondsLeftToFullEnergy() == 0 && _pet != g_playerState->PetRunner) {
                                select_index = i;
                                break;
                            }
                        }
                    }
                    i++;
                }
                
                if (select_index >= 0) {
                    if (CCMenu* _menu = (CCMenu*)matchPanel->m_selectPetGroup->getChildByTag(select_index)) {
                        _tut->menuButton = _menu;
                    }
                }
                
            }
        }
        
    }
    
}

void TutSwitchPet::finish() {
    
    g_playerState->checklist(TutSwitchPet::key());
    
}

#pragma mark - callback functions

void TutSwitchPet::onTapTheBG(void* sender_, std::string result_) {
    
    if (TutSwitchPet* _this = (TutSwitchPet*)sender_) {
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

void TutSwitchPet::onPetSwitched(void *sender_, std::string result_) {
    
    if (TutSwitchPet* _this = (TutSwitchPet*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutPointAtButton* _tut = (CTutPointAtButton*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutPointAtButton)) {
                    
                    if (result_ != CTutPointAtButtonFailed) {
                        _this->goToNextTutorial();
                    }
                    _tut->menuButton = NULL;
                    
                }
            }
        }
    }
    
}
