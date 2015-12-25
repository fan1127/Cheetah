//
//  CCModalView.h
//  Cheetah
//
//  Created by Kaweerut on 10/29/56 BE.
//
//

#ifndef __Cheetah__CCModalView__
#define __Cheetah__CCModalView__

#include <iostream>
#include "cocos2d.h"

class CCModalController;

class CCModalView: public cocos2d::CCLayer {
    
public:
    
    CCModalController               *m_modalController;
    cocos2d::CCSize                 m_contentSize;
    float                           m_scaleFactor;
    int                             m_currentIndex;
    CCObject                        *m_prevModal;
    bool                            m_close_locked;
    bool                            m_tap_blackbg_to_popview;
    
    ////////////////////////////////////////////////////////
    // core method
    virtual bool init(CCModalController* modalController);
    virtual ~CCModalView();
    
    ////////////////////////////////////////////////////////
    // Control method
    void setCloseLocked(bool locked_);
    void setTapBlackBGToPopView(bool popview_);
    void adjustScaleSize(bool add_padding_);
    void adjustScaleSize();
    
    ////////////////////////////////////////////////////////
    // callback method
    void (*callback)(void* sender, void* prevModal);
    
};

#endif /* defined(__Cheetah__CCModalView__) */
