//
//  LogDialog.h
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#ifndef __Cheetah__LogDialog__
#define __Cheetah__LogDialog__


#include "../../Extensions/Modal/CCModalView.h"
//#include "../../Extensions/CCPhatTouchableSprite.h"
//#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCLabelHelper.h"
//#include "../../Extensions/CCClippingLayer.h"
#include "../../Extensions/CCPopOverDialog.h"
#include "../../Settings.h"

#define LOG_WON             "You won the race against."
#define LOG_LOST            "You lost the race against."

class LogDialog : public CCModalView {
    
    CCModalController           *_modalController;
    
    CCPopOverDialog*            dialog;
    bool                        winstatus;
    
    CCLabelBMFont *_homeLabel;
    CCLabelBMFont *_name;
    CCLabelBMFont *_time;
    CCLabelBMFont *_streak;
    CCLabelBMFont *_coinLabel;
    CCLabelBMFont *_buttonLabel;
    
    cocos2d::extension::CCSkeletonAnimation *m_pet_spine;
public:
    
    static LogDialog *create(CCModalController *parentModalController_);
    bool init(CCModalController* parentModalController_);
    void updateContent();
    
    void onClose(cocos2d::CCObject *sender_);
    void onButton(cocos2d::CCObject *sender_);
//    static void onHitShadow(void *sender_);
//    virtual void scrollViewWillBeginDragging(CCNikScrollView *view);
    
};


#endif /* defined(__Cheetah__LogDialog__) */
