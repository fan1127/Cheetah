//
//  CTutUnit.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutUnit.h"
USING_NS_CC;
using namespace std;

extern float sizeMultiplier;

bool CTutUnit::init() {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_tutActive         = false;
    m_tutFinish         = false;
    m_key               = "undefined";
    
    _cleanAllTuts();
    m_tutSelected       = -1;
    m_tutSelector       = 0;
    
    characterSpineUncle     = NULL;
    characterSpineSarah     = NULL;
    characterSpineMerchant  = NULL;
    characterSpineFans      = NULL;
    characterLast     = CT_CHR_NONE;
    characterLastAni  = "";
    currentTut        = NULL;
    
    m_bg                = NULL;
    
    return true;
    
}

CTutUnit::~CTutUnit() {
    
    if (m_bg) {
        m_bg->removeFromParent();
        m_bg = NULL;
    }
    
    if (messageDialogSprite) {
        messageDialogSprite->removeFromParent();
        messageDialogSprite = NULL;
    }
    if (characterSpineUncle) {
        characterSpineUncle->removeFromParent();
        characterSpineUncle = NULL;
    }
    if (characterSpineSarah) {
        characterSpineSarah->removeFromParent();
        characterSpineSarah = NULL;
    }
    if (characterSpineMerchant) {
        characterSpineMerchant->removeFromParent();
        characterSpineMerchant = NULL;
    }
    if (characterSpineFans) {
        characterSpineFans->removeFromParent();
        characterSpineFans = NULL;
    }
    if (chooseDialogSprite) {
        chooseDialogSprite->removeFromParent();
        chooseDialogSprite = NULL;
    }
    
    _cleanAllTuts();
    
}

void CTutUnit::_cleanAllTuts() {
    
    for (vector<CTut*>::iterator it=m_tuts.begin(); it!=m_tuts.end(); ++it) {
        if (CTut* _dat = (CTut*)(*it)) {
            delete _dat;
        }
    }
    m_tuts.clear();
    
}

void CTutUnit::update(float delta_) {
    
    if (m_tutActive) {
        
        if (m_tutSelected != m_tutSelector) {
            if (m_tutSelector < m_tuts.size()) {
                m_tutSelected = m_tutSelector;
                m_tuts[m_tutSelector]->perform(this);
                m_tuts[m_tutSelector]->doAfterPerform();
            }else {
                if (!m_tutFinish) {
                    m_tutFinish = true;
                    finish();
                }
            }
        }
        
        if (m_tutSelector < m_tuts.size()) {
            m_tuts[m_tutSelector]->update(delta_);
        }
        
    }
    
}

bool CTutUnit::trig() {
    
    if (!m_tutActive &&
        !m_tutFinish) {
        m_tutActive = true;
        start();
        return true;
    }
    
    return false;
    
}

void CTutUnit::addSpotlightBG(void *sender_, void (*callback_)(void *, void *)) {
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (m_bg == NULL) {
        CCSprite *sprite = CCSprite::createWithSpriteFrameName("oldversion/_bg_spotlight.png");
        sprite->setScaleX((_screenSize.width+40.0f*sizeMultiplier)/sprite->getContentSize().width);
        sprite->setScaleY((_screenSize.height+40.0f*sizeMultiplier)/sprite->getContentSize().height);
        m_bg = CCNikButton::create(sprite, NULL);
        m_bg->setPosition(_screenSize*0.5f);
        m_bg->setContentSize(_screenSize);
        m_bg->setTouchPriority(kCCMenuHandlerPriority-1);
        m_bg->setEnableActiveScale(false);
        addChild(m_bg);
    }
    
    m_bg->setVisible(true);
    m_bg->setSender(sender_);
    m_bg->onHit = callback_;
}

void CTutUnit::removeSpotlightBG() {
    
    if (m_bg) m_bg->setVisible(false);
    
}

void CTutUnit::removeChooseDialogSprite() {
    
    if (chooseDialogSprite) {
        chooseDialogSprite->removeFromParent();
        chooseDialogSprite = NULL;
    }
    
}

void CTutUnit::removeAllUIs() {
    
    if (messageDialogSprite) messageDialogSprite->setVisible(false);
    if (characterSpineUncle) characterSpineUncle->setVisible(false);
    if (characterSpineSarah) characterSpineSarah->setVisible(false);
    if (characterSpineMerchant) characterSpineMerchant->setVisible(false);
    if (characterSpineFans) characterSpineFans->setVisible(false);
    if (chooseDialogSprite) chooseDialogSprite->setVisible(false);
    
}
