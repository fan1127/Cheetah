//
//  DialogBase.h
//  Cheetah
//
//  Created by Vaclav Samec on 10/13/2557 BE.
//
//

#ifndef __Cheetah__DialogBase__
#define __Cheetah__DialogBase__

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Settings.h"
#include "../../Extensions/CCNikButton.h"

class SettingDialog : public CCModalView {

    CCPhatTouchableSprite*  m_block_sprite;

    cocos2d::CCMenu*        m_menu[4];
    cocos2d::CCMenu*        m_facebookButton;

public:

    static SettingDialog *create(CCModalController *parentModalController_);
    bool init(CCModalController *parentModalController_);

    bool compose();
    void composeFacebookButton();

    void onClose(cocos2d::CCObject *sender_);
    void onMusic(cocos2d::CCObject *sender_);
    void onSound(cocos2d::CCObject *sender_);
    void onHelp(cocos2d::CCObject *sender_);
    void onCredit(cocos2d::CCObject *sender_);
    void onSupport(cocos2d::CCObject *sender_);
    void onFacebook(cocos2d::CCObject* sender_);

    cocos2d::CCSprite* getSpriteOnOff(bool);

    static void onFacebookLoggedIn(void* sender_, bool success_);
    static void onFacebookLoggedOut(void* sender_, bool success_);

};

#endif /* defined(__Cheetah__DialogBase__) */
