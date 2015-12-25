//
//  TutBuildFeedSilo.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutBuildFeedSilo.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

string TutBuildFeedSilo::key() {
    
    return "build_bank";
    
}

string TutBuildFeedSilo::keyIntro() {
    
    return "build_bank_intro";
    
}

bool TutBuildFeedSilo::check() {
    
    if (!g_playerState->Checklist[TutBuildFeedSilo::key()] &&
        !g_playerState->getBuilding(kBuildingFoodStorage, 0)&&
        g_playerState->getBuilding(kBuildingHQ, 3)&&
        g_playerState->player.balance.food >= g_playerState->getTotalFoodStorageCapacity()) {
        return true;
    }
    
    return false;
    
}

bool TutBuildFeedSilo::checkIntro() {
    
    if (!g_playerState->Checklist[TutBuildFeedSilo::keyIntro()] &&
        g_playerState->player.balance.food >= g_playerState->getTotalFoodStorageCapacity()) {
        return true;
    }
    
    return false;
    
}

TutBuildFeedSilo* TutBuildFeedSilo::create() {
    
    if (TutBuildFeedSilo::check() ) {
        TutBuildFeedSilo* pRet = new TutBuildFeedSilo();
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


TutBuildFeedSilo* TutBuildFeedSilo::createIntro() {
    
    if (TutBuildFeedSilo::checkIntro() ) {
        TutBuildFeedSilo* pRet = new TutBuildFeedSilo();
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

bool TutBuildFeedSilo::init(bool isIntro_) {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    GDBuildingVO _garage_vo = g_gameContent->getBuildingVO(kBuildingBank, 1);
    if (_garage_vo.type == "") {
        return false;
    }
    
    this->isIntro = (bool *)isIntro_;
    if(isIntro_)
        m_key = TutBuildFeedSilo::keyIntro();
    else
        m_key = TutBuildFeedSilo::key();
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    
    GDBuildingVO bVo = g_gameContent->getBuildingVO("food_storage", 1);
    snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_build_feed_silo_speech_1"].c_str(), _garage_vo.title.c_str());
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    _buffer,
                                    CT_SPC_OPTION_NONE_WITH_PICTURE,
                                    bVo,this, TutBuildFeedSilo::onTapTheBG));
    
    return true;
    
}

void TutBuildFeedSilo::finish() {
    if(this->isIntro)
        g_playerState->checklist(TutBuildBank::keyIntro());
    
}

#pragma mark - callback functions

void TutBuildFeedSilo::onTapTheBG(void* sender_, std::string result_) {
    
    if (TutBuildFeedSilo* _this = (TutBuildFeedSilo*)sender_) {
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
