//
//  TutBuildBank.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutBuildBank.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

string TutBuildBank::key() {
    
    return "build_bank";
    
}

string TutBuildBank::keyIntro() {
    
    return "build_bank_intro";
    
}

bool TutBuildBank::check() {
    if (!g_playerState->Checklist[TutBuildBank::key()] &&
        !g_playerState->getBuilding(kBuildingBank, 0) &&
        g_playerState->player.balance.gold >= g_playerState->getTotalCoinStorageCapacity()) {
        return true;
    }
    
    return false;
    
}


bool TutBuildBank::checkIntro() {
    if (!g_playerState->Checklist[TutBuildBank::keyIntro()] &&
        g_playerState->player.balance.gold >= g_playerState->getTotalCoinStorageCapacity()) {
        return true;
    }
    
    return false;
    
}

TutBuildBank* TutBuildBank::create() {
    
    if (TutBuildBank::check()) {
        TutBuildBank* pRet = new TutBuildBank();
        if (pRet) {
            if (pRet->init(false)) {
                pRet->autorelease();
                return pRet;
            }
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}



TutBuildBank* TutBuildBank::createIntro() {
    
    if (TutBuildBank::checkIntro()) {
        TutBuildBank* pRet = new TutBuildBank();
        if (pRet) {
            if (pRet->init(true)) {
                pRet->autorelease();
                return pRet;
            }
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool TutBuildBank::init(bool isIntro_) {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    GDBuildingVO _garage_vo = g_gameContent->getBuildingVO(kBuildingBank, 1);
    if (_garage_vo.type == "") {
        return false;
    }
    
    this->isIntro = (bool *) isIntro_;
    if(isIntro_)
        m_key = TutBuildBank::keyIntro();
    else
        m_key = TutBuildBank::key();
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    
    GDBuildingVO bVo = g_gameContent->getBuildingVO("bank", 1);
    snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_build_bank_speech_1"].c_str(), _garage_vo.title.c_str());
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    _buffer,
                                    CT_SPC_OPTION_NONE_WITH_PICTURE,
                                    bVo,this, TutBuildBank::onTapTheBG));
    
    return true;
    
}

void TutBuildBank::finish() {
    if(this->isIntro)
        g_playerState->checklist(TutBuildBank::keyIntro());
    
}

#pragma mark - callback functions

void TutBuildBank::onTapTheBG(void* sender_, std::string result_) {
    
    if (TutBuildBank* _this = (TutBuildBank*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutSpeech)) {
                    if (_tut->option == CT_SPC_OPTION_NONE || _tut->option == CT_SPC_OPTION_NONE_WITH_PICTURE) {
                        _this->m_tutSelector ++;
                        CCLog_("__finish test callback");
                    }
                    return;
                }
            }
        }
    }
    
}
