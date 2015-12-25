//
//  AlertDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 7/23/2557 BE.
//
//

#ifndef __Cheetah__AlertDialog__
#define __Cheetah__AlertDialog__

#include <cocos-ext.h>
#include "../../Extensions/Modal/CCModalView.h"


class AlertDialog : public CCModalView {
    
    void *m_sender;
    void (*m_callback)(void*,bool);
    
public:
    
    static AlertDialog *create(CCModalController *parentModalController_, std::string title_, std::string message_, void *sender_, void (*callback_)(void*,bool));
    
    static AlertDialog *create(CCModalController *parentModalController_, std::string title_, std::string message_, bool makeOkayGreen_, void *sender_, void (*callback_)(void*,bool));
    
    static AlertDialog *create(CCModalController *parentModalController_, std::string title_, std::string message_, std::string okay_, std::string cancel_, bool makeOkayGreen_, void *sender_, void (*callback_)(void*,bool));
    bool init(CCModalController *parentModalController_, std::string title_, std::string message_, std::string okay_, std::string cancel_, bool makeOkayGreen_, void *sender_, void (*callback_)(void*,bool));
    
    void onOkay(cocos2d::CCObject *sender_);
    void onCancel(cocos2d::CCObject *sender_);
    
};

#endif /* defined(__Cheetah__AlertDialog__) */
