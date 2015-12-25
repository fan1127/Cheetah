//
//  FBLoginDialog.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 9/4/2557 BE.
//
//

#ifndef __Cheetah__FBLoginDialog__
#define __Cheetah__FBLoginDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Settings.h"
#include "../../Extensions/CCNikButton.h"
#include "ModalDialog.h"

#define kFBLoginDialogAchievementType   "facebook_1"

class FBLoginDialog : public ModalDialog {
    
    CCSprite                *m_merchantSay;
    CCNode                  *m_nodesGroup;
    
    cocos2d::CCPoint        posTouch;
    
public:
    
    static FBLoginDialog *create(CCModalController *parentModalController,
                                 bool lastChance);
    
    bool init(CCModalController *parentModalController,
              bool lastChance,
              CCSize size);
    void composeClaimReward();
    
    void onConnectFacebook(cocos2d::CCObject* sender);
    void onClaim(cocos2d::CCObject *sender);
    
    static void onAchievementClaimed(void *sender, std::string resp_str);
    static void onFacebookLoggedIn(void* sender, bool success);
    static void onCollectGemsEffectFinished(void* sender);
    
};

#endif /* defined(__Cheetah__FBLoginDialog__) */
