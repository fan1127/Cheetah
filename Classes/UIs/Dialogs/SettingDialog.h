//
//  SettingDialog.h
//  Cheetah
//
//  Created by Avalable on 3/27/2557 BE.
//
//

#ifndef __Cheetah__SettingDialog__
#define __Cheetah__SettingDialog__

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Settings.h"
#include "../../Extensions/CCNikButton.h"
#include "ModalDialog.h"
#include "../../AppMacros.h"

class SettingDialog : public ModalDialog {
    
    cocos2d::CCMenu*        m_menu[4];
    cocos2d::CCMenu*        m_facebookButton;
    cocos2d::CCMenu*        m_googlePlayButton;
    cocos2d::CCMenu*		m_linkButton;
    
public:
    
    static SettingDialog *create(CCModalController *parentModalController_);

    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);
    void composeFacebookButton();
    void onMusic(cocos2d::CCObject *sender_);
    void onSound(cocos2d::CCObject *sender_);
    void onHelp(cocos2d::CCObject *sender_);
    void onCredit(cocos2d::CCObject *sender_);
    void onSupport(cocos2d::CCObject *sender_);
    void onFacebook(cocos2d::CCObject* sender_);
    void onLinkDevice(cocos2d::CCObject* sender_);
    
    cocos2d::CCSprite* getSpriteOnOff(bool);

    static void onFacebookLoggedIn(void* sender_, bool success_);
    static void onFacebookLoggedOut(void* sender_, bool success_);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    void composeGooglePlayButton();
    void onGooglePlayButton(cocos2d::CCObject* sender_);
    static void onGooglePlayLoggedIn(void* sender_, bool success_);
    static void onGooglePlayLoggedOut(void* sender_, bool success_);
#endif
};

#endif /* defined(__Cheetah__SettingDialog__) */
