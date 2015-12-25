//
//  CCPhatButton.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/24/56 BE.
//
//

#include "CCPhatButton.h"
USING_NS_CC;

CCPhatButton* CCPhatButton::createWithSpriteFileNames(const char* btnFileName, const char* btnPressedFileName) {
    
//    CCPhatButton* pRet = new CCPhatButton();
//    pRet->initWithSpriteFileNames(btnFileName, btnPressedFileName, NULL);
//    pRet->autorelease();
    return createWithSpriteFileNames(btnFileName, btnPressedFileName, NULL, NULL);
    
}

CCPhatButton* CCPhatButton::createWithSpriteFileNames(const char* btnFileName, const char* btnPressedFileName, void* instance) {
    
//    CCPhatButton* pRet = new CCPhatButton();
//    pRet->initWithSpriteFileNames(btnFileName, btnPressedFileName, instance);
//    pRet->autorelease();
    return createWithSpriteFrameNames(btnFileName, btnPressedFileName, instance, NULL);
    
}

CCPhatButton* CCPhatButton::createWithSpriteFileNames(const char* btnFileName, const char* btnPressedFileName, void* instance, void *preferenceRootParent_) {
    
    CCPhatButton* pRet = new CCPhatButton();
    pRet->preferenceRootParent = preferenceRootParent_;
    pRet->initWithSpriteFileNames(btnFileName, btnPressedFileName, instance);
    pRet->autorelease();
    return pRet;
    
}

CCPhatButton* CCPhatButton::createWithSpriteFrameNames(const char* btnFrameName, const char* btnPressedFrameName) {
    
    CCPhatButton* pRet = new CCPhatButton();
    pRet->initWithSpriteFrameNames(btnFrameName, btnPressedFrameName, NULL);
    pRet->autorelease();
    return pRet;
    
}

CCPhatButton* CCPhatButton::createWithSpriteFrameNames(const char* btnFrameName, const char* btnPressedFrameName, void* instance) {
    
    CCPhatButton* pRet = new CCPhatButton();
    pRet->initWithSpriteFrameNames(btnFrameName, btnPressedFrameName, instance);
    pRet->autorelease();
    return pRet;
    
}

CCPhatButton* CCPhatButton::createWithSpriteFrameNames(const char* btnFrameName, const char* btnPressedFrameName, void* instance, void *preferenceRootParent_) {
    
    CCPhatButton* pRet = new CCPhatButton();
    pRet->preferenceRootParent = preferenceRootParent_;
    pRet->initWithSpriteFrameNames(btnFrameName, btnPressedFrameName, instance);
    pRet->autorelease();
    return pRet;
    
}

bool CCPhatButton::initWithSpriteFileNames(const char* btnFileName, const char* btnPressedFileName, void* instance) {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    this->setTouchPriority(kCCMenuHandlerPriority);
    this->setTouchEnabled(true);
    this->setAnchorPoint(ccp(0.0f, 0.0f));
    
    m_btnSprite = CCSprite::create(btnFileName);
    this->addChild(m_btnSprite);
    this->setContentSize(m_btnSprite->getContentSize());

    m_btnPressedSprite = CCSprite::create(btnPressedFileName);
    this->addChild(m_btnPressedSprite);
    
    updateButtonState(PBS_UNPRESSED);
    setButtonSensitive(true);
    
    if (instance) {
        m_instance = instance;
    }else {
        m_instance = this;
    }
    m_btnPressed = false;
    m_btnTabbed = false;
    
    onPressed = NULL;
    onPressing = NULL;
    onReleased = NULL;
    onTabbed = NULL;
    
    scheduleUpdate();
    
    return true;
    
}

bool CCPhatButton::initWithSpriteFrameNames(const char* btnFrameName, const char* btnPressedFrameName, void* instance) {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    this->setTouchPriority(kCCMenuHandlerPriority);
    this->setTouchEnabled(true);
    this->setAnchorPoint(ccp(0.0f, 0.0f));
    
    if (btnFrameName) {
        m_btnSprite = CCSprite::createWithSpriteFrameName(btnFrameName);
        this->setContentSize(m_btnSprite->getContentSize());
    } else {
        m_btnSprite = CCSprite::create();
    }
    
    //m_btnSprite->preferenceRootParent = preferenceRootParent;
    this->addChild(m_btnSprite);

    if (btnPressedFrameName) {
        m_btnPressedSprite = CCSprite::createWithSpriteFrameName(btnPressedFrameName);
        this->setContentSize(m_btnSprite->getContentSize());
    } else {
        m_btnPressedSprite = CCSprite::create();
    }
    
    //m_btnPressedSprite->preferenceRootParent = preferenceRootParent;
    this->addChild(m_btnPressedSprite);

    
    updateButtonState(PBS_UNPRESSED);
    setButtonSensitive(true);
    
    if (instance) {
        m_instance = instance;
    }else {
        m_instance = this;
    }
    m_btnPressed = false;
    m_btnTabbed = false;

    onPressed = NULL;
    onPressing = NULL;
    onReleased = NULL;
    onTabbed = NULL;
    
    scheduleUpdate();
    
    return true;
    
}

void CCPhatButton::setContentSize(cocos2d::CCSize size)
{
    CCLayer::setContentSize(size);
    if (m_btnSprite && m_btnPressedSprite) {
        m_btnSprite->setContentSize(size);
        m_btnPressedSprite->setContentSize(size);
    }
}

void CCPhatButton::update(float delta) {
    
    if (m_btnState == PBS_PRESSED) {
        if (onPressing) {
            onPressing(m_instance, m_sender);
        }
    }
    
}

void CCPhatButton::updateButtonState(PhatButtonState state) {
    
    m_btnState = state;
    
    switch (m_btnState) {
            
        default:
        case PBS_UNPRESSED:
            
            if (m_btnSprite) {
                m_btnSprite->setVisible(true);
                m_btnSprite->resumeSchedulerAndActions();
            }
            
            if (m_btnPressedSprite) {
                m_btnPressedSprite->setVisible(false);
                m_btnPressedSprite->pauseSchedulerAndActions();
            }
            
            break;
            
        case PBS_PRESSED:
            
            if (m_btnPressedSprite) {
                
                m_btnPressedSprite->setVisible(true);
                m_btnPressedSprite->resumeSchedulerAndActions();
                
                if (m_btnSprite) {
                    m_btnSprite->setVisible(false);
                    m_btnSprite->pauseSchedulerAndActions();
                }
                
            }else {
                
                if (m_btnSprite) {
                    m_btnSprite->setVisible(true);
                    m_btnSprite->resumeSchedulerAndActions();
                }
                
            }
            
            break;
    }
    
}

void CCPhatButton::setButtonSensitive(bool sensitive) {
    
    m_btnSensitive = sensitive;
    
}

void CCPhatButton::setSender(void *sender_) {
    
    m_sender = sender_;
    
}

void CCPhatButton::setTouchMargin(float margin_) {
    
    m_touchMargin = margin_;
    
}

void CCPhatButton::ccTouchesBegan(CCSet *touches, CCEvent *event) {
    
    CCTouch *touch = (CCTouch*)touches->anyObject();
    CCPoint location = touch->getLocation();
    
    if (m_btnSprite) {
        
        CCPoint t_btnPosition = convertToWorldSpace(m_btnSprite->getPosition());
        
        CCRect buttonRect_ = CCRect(t_btnPosition.x - (m_btnSprite->getContentSize().width/2) - m_touchMargin,
                                    t_btnPosition.y - (m_btnSprite->getContentSize().height/2) - m_touchMargin,
                                    m_btnSprite->getContentSize().width + m_touchMargin*2,
                                    m_btnSprite->getContentSize().height + m_touchMargin*2);
        
        if (buttonRect_.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
            
            if (m_btnState != PBS_PRESSED) {
                updateButtonState(PBS_PRESSED);
                
                if (onPressed) {
                    onPressed(m_instance, m_sender);
                }
                
                m_btnPressed = true;
                m_btnTabbed = true;
            }
            
        }
        
    }
    
}

void CCPhatButton::ccTouchesMoved(CCSet *touches, CCEvent *event) {
    
    m_btnTabbed = false;
    
}

void CCPhatButton::ccTouchesEnded(CCSet *touches, CCEvent *event) {
    
    CCTouch *touch = (CCTouch*)touches->anyObject();
    CCPoint location = touch->getLocation();
    
    if (m_btnSprite) {
        
        CCPoint t_btnPosition = convertToWorldSpace(m_btnSprite->getPosition());
        
        CCRect buttonRect_ = CCRect(t_btnPosition.x - (m_btnSprite->getContentSize().width/2) - m_touchMargin,
                                    t_btnPosition.y - (m_btnSprite->getContentSize().height/2) - m_touchMargin,
                                    m_btnSprite->getContentSize().width + m_touchMargin*2,
                                    m_btnSprite->getContentSize().height + m_touchMargin*2);
        
        
        
        if (!m_btnSensitive && m_btnPressed) {
            CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
            buttonRect_ = CCRect(0, 0, screenSize.width, screenSize.height);
        }
        
        if (m_btnState != PBS_UNPRESSED) {
            updateButtonState(PBS_UNPRESSED);
            if (onReleased) {
                onReleased(m_instance, m_sender);
            }
        }
        
        if (buttonRect_.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
            
            
            
            
            
            if (m_btnTabbed) {
                if (onTabbed) {
                    onTabbed(m_instance, m_sender);
                }
            }
            
            m_btnPressed = false;
            m_btnTabbed = false;
            
            
            
        }
        
    }
    
}

void CCPhatButton::ccTouchesCancelled(CCSet *touches, CCEvent *event) {
    
    CCTouch *touch = (CCTouch*)touches->anyObject();
    CCPoint location = touch->getLocation();
    
    if (m_btnSprite) {
        
        CCPoint t_btnPosition = convertToWorldSpace(m_btnSprite->getPosition());
        
        CCRect buttonRect_ = CCRect(t_btnPosition.x - (m_btnSprite->getContentSize().width/2) - m_touchMargin,
                                    t_btnPosition.y - (m_btnSprite->getContentSize().height/2) - m_touchMargin,
                                    m_btnSprite->getContentSize().width + m_touchMargin*2,
                                    m_btnSprite->getContentSize().height + m_touchMargin*2);
        
        if (!m_btnSensitive && m_btnPressed) {
            CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
            buttonRect_ = CCRect(0, 0, screenSize.width, screenSize.height);
        }
        
        if (buttonRect_.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
            
            if (m_btnState != PBS_UNPRESSED) {
                updateButtonState(PBS_UNPRESSED);
                
                if (onReleased) {
                    onReleased(this, m_sender);
                }
                
                m_btnPressed = false;
            }
            
        }
        
    }
}

/*
void CCPhatButton::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kButtonPriority, true);
}

bool CCPhatButton::ccTouchBegan(cocos2d::CCTouch *touch_, cocos2d::CCEvent *event_) {
    
    CCPoint location = touch_->getLocation();
    
    if (m_btnSprite) {
        
        CCPoint t_btnPosition = convertToWorldSpace(m_btnSprite->getPosition());
        
        CCRect buttonRect_ = CCRect(t_btnPosition.x - (m_btnSprite->getContentSize().width/2),
                                    t_btnPosition.y - (m_btnSprite->getContentSize().height/2),
                                    m_btnSprite->getContentSize().width,
                                    m_btnSprite->getContentSize().height);
        
        if (buttonRect_.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
            
            if (m_btnState != PBS_PRESSED) {
                updateButtonState(PBS_PRESSED);
                
                if (onPressed) {
                    onPressed(m_instance, m_sender);
                }
                
                m_btnPressed = true;
                m_btnTabbed = true;
            }
            
            return true;
            
        }
        
    }
    
    return false;
    
}

void CCPhatButton::ccTouchMoved(CCTouch *touch_, CCEvent *event_) {
    
    m_btnTabbed = false;
    
}

void CCPhatButton::ccTouchEnded(CCTouch *touch_, CCEvent *event_) {
    
    CCPoint location = touch_->getLocation();
    
    if (m_btnSprite) {
        
        CCPoint t_btnPosition = convertToWorldSpace(m_btnSprite->getPosition());
        
        CCRect buttonRect_ = CCRect(t_btnPosition.x - (m_btnSprite->getContentSize().width/2),
                                    t_btnPosition.y - (m_btnSprite->getContentSize().height/2),
                                    m_btnSprite->getContentSize().width,
                                    m_btnSprite->getContentSize().height);
        
        if (!m_btnSensitive && m_btnPressed) {
            CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
            buttonRect_ = CCRect(0, 0, screenSize.width, screenSize.height);
        }
        
        if (m_btnState != PBS_UNPRESSED) {
            updateButtonState(PBS_UNPRESSED);
            if (onReleased) {
                onReleased(m_instance, m_sender);
            }
        }
        
        if (buttonRect_.intersectsRect(CCRect(location.x, location.y, 1, 1))) {
            
            if (m_btnTabbed) {
                if (onTabbed) {
                    onTabbed(m_instance, m_sender);
                }
            }
            
            m_btnPressed = false;
            m_btnTabbed = false;
            
        }
        
    }
    
}

void CCPhatButton::ccTouchCancelled(CCTouch *touch_, CCEvent *event_) {
    
    this->ccTouchEnded(touch_, event_);
    
}
*/
