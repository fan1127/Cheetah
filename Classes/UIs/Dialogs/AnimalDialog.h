//
//  AnimalDialog.h
//  Cheetah
//
//  Created by Kaweerut on 2/24/57 BE.
//
//

#ifndef __Cheetah__AnimalDialog__
#define __Cheetah__AnimalDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCProgressBar9Sprite.h"
#include "../../Extensions/CCNikButton.h"

#include "../../Settings.h"
#include "ModalDialog.h"

enum AnimalDialogType {
    AnimalDialogType_1,
    AnimalDialogType_2
};


class AnimalDialog : public ModalDialog
{
    AnimalDialogType        m_type;
    bool                    m_sellBtnEnabled;
    bool                    m_sendToBarnButtonEnabled;

public:
    
    int                     viewMode;
    CCMenu *_button_action2;
    
    cocos2d::CCMenu*        menuSetLeader;
    
    static AnimalDialog *create(CCModalController *parentModalController_, void *entity_, AnimalDialogType t);
    static AnimalDialog *create(CCModalController *parentModalController_, void *entity_, AnimalDialogType t, int viewMode_);
    bool init(CCModalController *parentModalController_, void *entity_, AnimalDialogType t, CCSize size);
    
    void addButton();
    void setSellBtnEnabled(bool enabled_) { m_sellBtnEnabled = enabled_; }
    void setSendToBarnButtonEnabled(bool enabled_) { m_sendToBarnButtonEnabled = enabled_; }
    
    void onButton(cocos2d::CCObject *sender_);
    void onTopSpeed(cocos2d::CCObject *sender_);
    void onCornerSpeed(cocos2d::CCObject *sender_);
    void onJumping(cocos2d::CCObject *sender_);
    void onFavoriteType(cocos2d::CCObject *sender_);

#pragma mark - callback functions
    
    static void onPetHosted(void *sender_, std::string resp_json_str_);
    
    void onNote(cocos2d::CCObject* obj_);
    
};

#endif /* defined(__Cheetah__AnimalDialog__) */
