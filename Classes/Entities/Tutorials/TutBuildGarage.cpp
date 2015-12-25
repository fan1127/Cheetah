//
//  TutBuildGarage.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutBuildGarage.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;

string TutBuildGarage::key() {
    
    return "build_garage";
    
}

bool TutBuildGarage::check() {
    
    if (!g_playerState->Checklist[TutBuildGarage::key()] &&
        !g_playerState->getBuilding(kBuildingGarage, 0)) {
        return true;
    }
    
    return false;
    
}

TutBuildGarage* TutBuildGarage::create(WorldMap* worldMap_, HUDPanel* hudPanel_) {
    
    if (TutBuildGarage::check() && worldMap_->isAllLegalCloudsOpenned()) {
        TutBuildGarage* pRet = new TutBuildGarage();
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

bool TutBuildGarage::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    GDBuildingVO _garage_vo = g_gameContent->getBuildingVO(kBuildingGarage, 1);
    if (_garage_vo.type == "") {
        return false;
    }
    
    m_key = TutBuildGarage::key();
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    
    GDBuildingVO bVo = g_gameContent->getBuildingVO("garage", 1);
    snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_build_garage_speech_1"].c_str(), _garage_vo.title.c_str());
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    _buffer,
                                    CT_SPC_OPTION_NONE_WITH_PICTURE,
                                    bVo,this, TutBuildGarage::onTapTheBG));
    
    return true;
    
}

void TutBuildGarage::finish() {
    
    g_playerState->checklist(TutBuildGarage::key());
    
}

#pragma mark - callback functions

void TutBuildGarage::onTapTheBG(void* sender_, std::string result_) {
    
    if (TutBuildGarage* _this = (TutBuildGarage*)sender_) {
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
