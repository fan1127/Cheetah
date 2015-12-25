//
//  CCPhatButton.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/24/56 BE.
//
//

#ifndef __Cheetah__CCPhatButton__
#define __Cheetah__CCPhatButton__

#include "cocos2d.h"

#define kButtonPriority     -1024
#define kButtonTextColour   ccc3(0, 99, 133)

enum PhatButtonState {
    
    PBS_UNPRESSED,
    PBS_PRESSED,
    
};

class CCPhatButton: public cocos2d::CCLayer {
    
    ////////////////////////////////////////////////////////
    // button sprites
    cocos2d::CCSprite       *m_btnSprite;
    cocos2d::CCSprite       *m_btnPressedSprite;
    
    ////////////////////////////////////////////////////////
    // button properties
    void                    *m_instance;
    void                    *m_sender;
    PhatButtonState         m_btnState;
    bool                    m_btnSensitive;
    bool                    m_btnPressed;
    bool                    m_btnTabbed = false;
    float                   m_touchMargin = 0.0f;
    
public:
    
    void                    *preferenceRootParent   = NULL;
    void                    *userData               = NULL;
    
    void setContentSize(cocos2d::CCSize size);
    
    ////////////////////////////////////////////////////////
    // callback functions
    void (*onPressed)(void *inst, void* sender);
    void (*onReleased)(void *inst, void* sender);
    void (*onPressing)(void *inst, void* sender);
    void (*onTabbed)(void *inst, void* sender);
    
    ////////////////////////////////////////////////////////
    // core functions
    static CCPhatButton *createWithSpriteFileNames(const char *btnFileName_, const char *btnPressedFileName_);
    static CCPhatButton *createWithSpriteFileNames(const char *btnFileName_, const char *btnPressedFileName_, void *instance_);
    static CCPhatButton *createWithSpriteFileNames(const char *btnFileName_, const char *btnPressedFileName_, void *instance_, void *preferenceRootParent_);
    static CCPhatButton *createWithSpriteFrameNames(const char *btnFrameName_, const char *btnPressedFrameName_);
    static CCPhatButton *createWithSpriteFrameNames(const char *btnFrameName_, const char *btnPressedFrameName_, void *instance_);
    static CCPhatButton *createWithSpriteFrameNames(const char *btnFrameName_, const char *btnPressedFrameName_, void *instance_, void *preferenceRootParent_);
    
    bool initWithSpriteFileNames(const char *btnFileName_, const char *btnPressedFileName_, void *instance_);
    bool initWithSpriteFrameNames(const char *btnFrameName_, const char *btnPressedFrameName_, void *instance_);
    virtual void update(float delta_);
    
    void updateButtonState(PhatButtonState state_);
    void setButtonSensitive(bool sensitive_);
    
    void setSender(void *sender_);
    void setTouchMargin(float margin_);

    ////////////////////////////////////////////////////////
    
    // touch delegate functions
    void ccTouchesBegan(cocos2d::CCSet *touches_, cocos2d::CCEvent *event_);
    void ccTouchesMoved(cocos2d::CCSet *touches_, cocos2d::CCEvent *event_);
    void ccTouchesEnded(cocos2d::CCSet *touches_, cocos2d::CCEvent *event_);
    void ccTouchesCancelled(cocos2d::CCSet *touches_, cocos2d::CCEvent *event_);
    
//    virtual void registerWithTouchDispatcher();
//    virtual bool ccTouchBegan(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_);
//    virtual void ccTouchMoved(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_);
//    virtual void ccTouchEnded(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_);
//    virtual void ccTouchCancelled(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_);
    
};

#endif /* defined(__Cheetah__CCPhatButton__) */
