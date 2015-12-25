
//  AbilityDialog.h
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#ifndef __Cheetah__AbilityDialog__
#define __Cheetah__AbilityDialog__


#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCClippingLayer.h"
//#include "../../Extensions/InAppPurchase/InAppPurchase.h"
#include "AbilityUpgradeDialog.h"

#include "../../Settings.h"
#include "ScrollDialog.h"

//has to be all negatives, because positives mean unlocks at that level
#define kButtonLockedReasonUpgradeInProgress        -1
#define kButtonLockedReasonReachMaxLevel            -2
#define kButtonLockedReasonUpgrading                -3
#define kButtonLockedReasonLibraryUnderConstruction -4

class AbilityDialog : public ScrollDialog
{
    CCModalController           *_modalController;
    
    cocos2d::CCLabelBMFont*     m_label_foot;
    
    cocos2d::CCMenu*            m_noteButton;
    
    int                         m_buttonLockedReason;
    PlayerState*                m_playerState;
    
public:
    
    AbilityUpgradeDialog*       abilityUpgradeDialog;
    
    static AbilityDialog *create(CCModalController *parentModalController_, PlayerState* playerState_);

    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

    virtual void updateContent();
    
    void onButton(cocos2d::CCObject *sender_);
    
    virtual void scrollViewWillBeginDragging(CCNikScrollView *view);
    
    static void onHitShadow(cocos2d::CCPoint&, void* sender_);
  
    void onNote(cocos2d::CCObject* obj_);
    
    virtual void onClose(cocos2d::CCObject *sender_);
    
};


#endif /* defined(__Cheetah__AbilityDialog__) */
