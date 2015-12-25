//
//  CCPhatMsgBox.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/21/56 BE.
//
//

#include "CCPhatMsgBox.h"
USING_NS_CC;
using namespace std;

extern float scaleFactor;

CCPhatMsgBox* CCPhatMsgBox::createWithAllFrameNames(const char *bodyFrameName_, const char *btnOkFrameName_, const char *btnCancelFrameName_, const char *btnCloseFrameName_, string title_, string msg_) {
    
    CCPhatMsgBox* pRet = new CCPhatMsgBox();
    pRet->initWithAllFrameNames(bodyFrameName_, btnOkFrameName_, btnCancelFrameName_, btnCloseFrameName_, title_, msg_);
    pRet->retain();
    pRet->autorelease();
    
    return pRet;
    
}

bool CCPhatMsgBox::initWithAllFrameNames(const char *bodyFrameName_, const char *btnOkFrameName_, const char *btnCancelFrameName_, const char *btnCloseFrameName_, std::string title_, std::string msg_) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_boxSprite = CCSprite::createWithSpriteFrameName(bodyFrameName_);
    this->addChild(m_boxSprite);
    this->setContentSize(m_boxSprite->getContentSize());
    
    if (btnOkFrameName_) {
        btnOK = CCPhatButton::createWithSpriteFrameNames(btnOkFrameName_, btnOkFrameName_, this);
        btnOK->setPosition(CCPoint(this->getContentSize().width*0.68f, this->getContentSize().height*0.125f)-(this->getContentSize()/2));
        this->addChild(btnOK);
    }
    
    if (btnCancelFrameName_) {
        btnCancel = CCPhatButton::createWithSpriteFrameNames(btnCancelFrameName_, btnCancelFrameName_, this);
        btnCancel->setPosition(CCPoint(this->getContentSize().width*0.88f, this->getContentSize().height*0.125f)-(this->getContentSize()/2));
        this->addChild(btnCancel);
    }
    
    if (btnCloseFrameName_) {
        btnClose = CCPhatButton::createWithSpriteFrameNames(btnCloseFrameName_, btnCloseFrameName_, this);
        btnClose->setPosition(CCPoint(this->getContentSize().width*0.97f, this->getContentSize().height*0.93f)-(this->getContentSize()/2));
        this->addChild(btnClose);
    }
    
    if (title_ != "") {
        title = CCLabelTTF::create(title_.c_str(), kDefaultFont, 32*scaleFactor);
        title->setAnchorPoint(CCPoint(0.0f, 0.5f));
        title->setPosition(CCPoint(this->getContentSize().width*0.03f, this->getContentSize().height*0.93f)-(this->getContentSize()/2));
        this->addChild(title);
    }
    
    if (msg_ != "") {
        msg = CCLabelTTF::create(msg_.c_str(), kDefaultFont, 32*scaleFactor);
        msg->setColor(ccBLACK);
        this->addChild(msg);
    }
    
    return true;

}

void CCPhatMsgBox::open(string title_, string msg_) {
    
    open(title_, msg_, NULL);

}

void CCPhatMsgBox::open(string title_, string msg_, void* sender) {
    
    if (title) {
        title->setString(title_.c_str());
    }
    if (msg) {
        msg->setString(msg_.c_str());
    }
    
    this->setVisible(true);
    this->resumeSchedulerAndActions();
    
}

void CCPhatMsgBox::close() {
    
    this->setVisible(false);
    this->pauseSchedulerAndActions();
    
}



