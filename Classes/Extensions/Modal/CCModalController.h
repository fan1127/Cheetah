//
//  CCModalController.h
//  Cheetah
//
//  Created by Kaweerut, Plus Pingya on 10/29/56 BE.
//
//

#ifndef __Cheetah__CCModalController__
#define __Cheetah__CCModalController__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "../../Extensions/CCPhatTouchableSprite.h"


#define kCCModalViewTransitionDuration  0.1f
#define kCCModalViewMinimizeScale       0.5f
class CCModalView;

class CCModalController: public cocos2d::CCLayer {
    
    void                                *m_sender;
    CCPhatTouchableSprite               *m_fade_sprite;
    bool                                m_hitBgToCloseAllEnabled;
    
    void clearAllViews();
    void removeModalView(cocos2d::CCNode *node_, void* sender_);
    void removeLastModalView(cocos2d::CCNode *node_, void* sender_);
    
    void _showView(CCModalView *view_);
    void _hideView(CCModalView *view_);
    void _hideViewRelease(CCModalView *view_);
    void _hideAllViews(CCModalView *view_);
    
public:
    
    cocos2d::CCSize                     m_screenSize;
    std::vector<CCModalView *>          m_views;
    cocos2d::CCSequence*                m_actionPoppingUp;
    void *userObject;

    ////////////////////////////////////////////////////////
    // core method
    static CCModalController* create();
    virtual bool init();
    void setHitBGToCloseAllEnabled(bool enabled_) { m_hitBgToCloseAllEnabled = enabled_; }
    
    void setSender(void *sender_);
    void (*onWillPushView)(void *sender_);
    void (*onWillPopView)(void *sender_, void* view_);
    void (*onPushView)(void *sender_);
    void (*onPopView)(void *sender_);
    void (*onCloseAll)(void *sender_);
    void (*onHitShadow)(cocos2d::CCPoint&, void *sender_);
    
    
    ////////////////////////////////////////////////////////
    // Control method
    void pushView(CCModalView *v);
    void popView(bool trig_callback);
    void popView();
    void closeAll();
//    void removeFromMyParent(CCNode* sender);
    int getNStackView();
    static void hitShadow(void *inst, void *sender);
    
    ////////////////////////////////////////////////////////
    // callback functions
    void (*willPushViewAtIndex)(int ind);
    void (*willPopViewAtIndex)(int ind);
    void (*didPushViewAtIndex)(int ind);
    void (*didPopViewAtIndex)(int ind);
    
    static void hideAllChildrensAnimated(cocos2d::CCNode* root, float duration);
    static void hideAllChildrensAnimated(cocos2d::CCNode* root, float duration, void* except);
    static void showAllChildrensAnimated(cocos2d::CCNode* root, float duration);
    static void showAllChildrensAnimated(cocos2d::CCNode* root, float duration, void* except);
    
};

#endif /* defined(__Cheetah__CCModalController__) */
