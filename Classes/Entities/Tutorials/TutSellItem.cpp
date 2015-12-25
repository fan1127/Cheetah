//
//  TutSellItem.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutSellItem.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

string TutSellItem::key() {
    
    return "sell_item";
    
}

bool TutSellItem::check() {
    
    if (!g_playerState->Checklist[TutSellItem::key()]) {
        if (g_playerState->isItemFull()) {
            return true;
        }
    }
    return false;
    
}

TutSellItem* TutSellItem::create(CCNikScrollView* scrollView_, HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_) {
    
    if (TutSellItem::check()) {
        TutSellItem* pRet = new TutSellItem();
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

bool TutSellItem::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    Building* _item_barn = g_playerState->getBuilding(kBuildingItem, 0);
    if (!_item_barn) {
        return false;
    }
    
    m_key = TutSellItem::key();
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    
    snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_sell_item_speech_2"].c_str(), _item_barn->bVo.title.c_str());
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    _buffer,
                                    CT_SPC_OPTION_NONE,
                                    this, TutSellItem::onTapTheBG));
    
    if (scrollView && isoMapBuilding) {
        m_tuts.push_back(new CTutFocusOnBuilding(scrollView, isoMapBuilding, _item_barn, this, TutSellItem::onBuildingFocuzed));
    }
    
    return true;
    
}

void TutSellItem::start() {
    
    hudPanel->setDangerButtonsEnabled(false);
    shopPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("none");
    
}

void TutSellItem::finish() {
    
    hudPanel->setDangerButtonsEnabled(true);
    shopPanel->setDangerButtonsEnabled(true);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("");
    
    g_playerState->checklist(TutSellItem::key());
    
}

#pragma mark - callback functions

void TutSellItem::onTapTheBG(void* sender_, string result_) {
    
    if (TutSellItem* _this = (TutSellItem*)sender_) {
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

void TutSellItem::onBuildingFocuzed(void *sender_, std::string result_) {
    
    if (TutSellItem* _this = (TutSellItem*)sender_) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutFocusOnBuilding* _tut = (CTutFocusOnBuilding*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutFocusOnBuilding)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}
