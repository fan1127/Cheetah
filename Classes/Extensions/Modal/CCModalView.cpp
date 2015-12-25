//
//  CCModalView.cpp
//  Cheetah
//
//  Created by Kaweerut on 10/29/56 BE.
//
//

#include "CCModalView.h"
#include "CCModalController.h"
USING_NS_CC;

////////////////////////////////////////////////////////
// core method
bool CCModalView::init(CCModalController* modalController) {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    this->m_modalController = modalController;
    this->m_scaleFactor = 1.0f;
    
    m_prevModal = NULL;
    
    m_close_locked = false;
    m_tap_blackbg_to_popview = false;
    
    return true;
    
}

CCModalView::~CCModalView() {

}

////////////////////////////////////////////////////////
// Control method
void CCModalView::setCloseLocked(bool locked_) {
    
    m_close_locked = locked_;
    
}

void CCModalView::setTapBlackBGToPopView(bool popview_) {
    
    m_tap_blackbg_to_popview = popview_;
    
}

void CCModalView::adjustScaleSize(bool add_padding_) {
    
    // force 0.5 for normal iPad
    if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS &&
        (m_modalController->m_screenSize.height == 1024.0 ||
         m_modalController->m_screenSize.height == 768.0)) {
            
            m_scaleFactor = 0.5f;
            
    } else {
        CCSize contentSize = this->getContentSize();
        float _padding = 0.0f;
        if (add_padding_) {
            _padding = m_modalController->m_screenSize.height*0.1f;
        }
        
        this->m_scaleFactor = (m_modalController->m_screenSize.height) / (contentSize.height  + _padding);
    }
    
    this->setScale(m_scaleFactor);
    
}

void CCModalView::adjustScaleSize() {
    
    adjustScaleSize(true);
    
}















