//
//  TutIntroMerchant.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutIntroMerchant.h"
USING_NS_CC;
using namespace std;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Helpers/FlurryCpp.h"

string TutIntoMerchant::key() {
    
    return "intro_merchant";
    
}

bool TutIntoMerchant::check() {
    
    if (!g_playerState->Checklist[TutIntoMerchant::key()]) {
        return true;
    }
    return false;
    
}

TutIntoMerchant* TutIntoMerchant::create(CCNikScrollView* scrollView,
                                         HUDPanel* hudPanel,
                                         ShopPanel* shopPanel,
                                         IsoMapBuilding* isoMapBuilding,
                                         Merchant* merchant) {
    
    if (TutIntoMerchant::check() && merchant) {
        TutIntoMerchant* pRet = new TutIntoMerchant();
        if (pRet) {
            pRet->scrollView = scrollView;
            pRet->hudPanel = hudPanel;
            pRet->shopPanel = shopPanel;
            pRet->isoMapBuilding = isoMapBuilding;
            pRet->merchant = merchant;
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool TutIntoMerchant::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = TutIntoMerchant::key();
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_MERCHANT, CT_CHR_SIDE_LEFT,
                                    kMerchantIdle, true,
                                    g_gameContent->Word["tut_merchant_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutIntoMerchant::onTapTheBG));
    
    if (scrollView && isoMapBuilding) {
        m_tuts.push_back(new CTutFocusOnNode(scrollView,
                                             isoMapBuilding,
                                             merchant->spine,
                                             this,
                                             TutIntoMerchant::onNodeFocuzed));
    }
    
    return true;
    
}

bool TutIntoMerchant::trig() {
    
    if (hudPanel->isLevelUpEffectShowed()) return false;
    return CTutUnit::trig();
    
}

void TutIntoMerchant::start() {
    
    hudPanel->setDangerButtonsEnabled(false);
    shopPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("none");
    
    FlurryCpp::logEvent(EVENT_TUT_TUTINTROMERCHANT, true);
    FlurryCpp::logEvent(EVENT_TUT_TUTINTROMERCHANT_START);
}

void TutIntoMerchant::finish() {
    
    hudPanel->setDangerButtonsEnabled(true);
    shopPanel->setDangerButtonsEnabled(true);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("");
    
    g_playerState->checklist(TutIntoMerchant::key());
    
    FlurryCpp::endTimedEvent(EVENT_TUT_TUTINTROMERCHANT, NULL);
    FlurryCpp::logEvent(EVENT_TUT_TUTINTROMERCHANT_END);
}

#pragma mark - callback functions

void TutIntoMerchant::onTapTheBG(void* sender, string result) {
    
    if (TutIntoMerchant* _this = (TutIntoMerchant*)sender) {
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

void TutIntoMerchant::onNodeFocuzed(void *sender, std::string result) {
    
    if (TutIntoMerchant* _this = (TutIntoMerchant*)sender) {
        if (_this->m_tutSelector < _this->m_tuts.size()) {
            if (CTutFocusOnNode* _tut = (CTutFocusOnNode*)_this->m_tuts[_this->m_tutSelector]) {
                if (typeid(*_tut) == typeid(CTutFocusOnNode)) {
                    _this->goToNextTutorial();
                }
            }
        }
    }
    
}
