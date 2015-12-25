//
//  LoadGameDialog.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 5/28/2557 BE.
//
//

#ifndef __Cheetah__LoadGameDialog__
#define __Cheetah__LoadGameDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "ModalDialog.h"

class LoadGameDialog : public ModalDialog
{

public:
    
    static LoadGameDialog *create(CCModalController *parentModalController_, std::string name_, int level_);
    bool init(CCModalController *parentModalController_, std::string name_, int level_, CCSize size);
    
    void onLoadGameButton(cocos2d::CCObject *sender_);
    void onDoNotLoadGameButton(cocos2d::CCObject *sender_);

    static void onUserGcidTied(void* sender, std::string json_str);
    
};

#endif /* defined(__Cheetah__LoadGameDialog__) */
