//
//  HUDMenuButton.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/30/2557 BE.
//
//

#include "HUDMenuButton.h"
USING_NS_CC;

#include "../../Extensions/CCLabelHelper.h"
#include "../../Helpers/UIHelper.h"
#include "../../Settings.h"

extern float retinaMultiplier;

HUDMenuButton *HUDMenuButton::create(const char *button_bg_filename_,
                                     const char *button_icon_filename_,
                                     const char *button_label_,
                                     int badge_number_) {
    
    if (HUDMenuButton *pRet = new HUDMenuButton()) {
        if (pRet->init(button_bg_filename_, button_icon_filename_, button_label_, badge_number_)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool HUDMenuButton::init(const char *button_bg_filename_,
                         const char *button_icon_filename_,
                         const char *button_label_,
                         int badge_number_) {
    
    if (!button_bg_filename_ && !button_icon_filename_) return false;
    
    m_buttonSprite = NULL;
    CCSprite *_buttonIconSprite = NULL;
    
    if (button_bg_filename_ != NULL) {
        m_buttonSprite = CCSprite::createWithSpriteFrameName(button_bg_filename_);
    }
    
    if (button_icon_filename_ != NULL) {
        _buttonIconSprite = CCSprite::createWithSpriteFrameName(button_icon_filename_);
    }
    
    if (m_buttonSprite && _buttonIconSprite) {
        _buttonIconSprite->setPosition(m_buttonSprite->getContentSize()*0.5f);
        m_buttonSprite->addChild(_buttonIconSprite);
    }
    
    if (!m_buttonSprite && _buttonIconSprite) {
        m_buttonSprite = _buttonIconSprite;
        _buttonIconSprite = NULL;
    }
    
    if (!CCMenuItemScaleSprite::initWithSprite(m_buttonSprite, NULL, NULL)) {
        return false;
    }

    setContentSize(m_buttonSprite->getContentSize());

    m_label = NULL;
    m_badgeSprite = NULL;
    m_badgeLabel = NULL;
    
    setLabel(button_label_);
    setBadge(badge_number_);
    setEnabled(true);
    
    return true;
}

void HUDMenuButton::setAnimation(UIHelper::UIAnimations anim, float delay)
{
    CCAnimation *animation = CCAnimation::create();

    auto frames = UIHelper::getAnimationFrames(anim);

    for (auto &frame : frames)
    {
        animation->addSpriteFrame(frame);
    }

    animation->setLoops(1);
    animation->setDelayPerUnit(0.05f);
    CCAnimate *animate = CCAnimate::create(animation);
    m_buttonSprite->runAction(CCRepeatForever::create(CCSequence::create(animate,CCDelayTime::create(delay), NULL)));
}

void HUDMenuButton::setPrePositionShowOnly(cocos2d::CCPoint showPos_)
{
    m_showPos = showPos_;
    m_hidePos = showPos_;
}

void HUDMenuButton::setPrePosition(cocos2d::CCPoint showPos_, cocos2d::CCPoint hidePos_) {
    
    m_showPos = showPos_;
    m_hidePos = hidePos_;
    
}

void HUDMenuButton::show(bool show_) {
    
    bool _animated = true;
    show(show_, _animated);
    
}

void HUDMenuButton::show(bool show_, bool animated_) {
    
    show(show_, animated_, 0.0f);
    
}

void HUDMenuButton::show(bool show_, bool animated_, float delay_) {
    
    CCPoint _moveToPoint = m_hidePos;
    if (show_) {
        _moveToPoint = m_showPos;
    }
    
    if (animated_) {
        stopAllActions();
        if (delay_ > 0.0f) {
            runAction(CCSequence::create(CCDelayTime::create(delay_),
                                         CCMoveTo::create(kHUDMenuButtonMoveDuration*0.5f, _moveToPoint),
                                         NULL));
        }else {
            runAction(CCMoveTo::create(kHUDMenuButtonMoveDuration, _moveToPoint));
        }
    }else {
        setPosition(_moveToPoint);
    }
    
    m_show =show_;
}

void HUDMenuButton::setLabel(const char *label_str_)
{
    if (label_str_ == NULL) return;
    
    if (!m_label) {
        m_label = CCLabelHelper::createBMFont(retinaMultiplier, label_str_, kFontSizeVeryLarge, true);
        m_buttonSprite->addChild(m_label, 100);
    }
    m_label->setPosition(ccp(getContentSize().width*0.5f, m_label->getContentSize().height*0.5f));
    m_label->setString(label_str_);
}

void HUDMenuButton::setBadge(int badge_number_) {
    
    if (badge_number_ <= 0) {
        if (m_badgeSprite)
            m_badgeSprite->setVisible(false);
        return;
    }
    
    if (!m_badgeSprite) {
        m_badgeSprite = UIHelper::getSprite(UIHelper::HUDNewMarkerBig);
        m_badgeSprite->setPosition(m_buttonSprite->getContentSize()-m_badgeSprite->getContentSize()*0.5f);
        m_buttonSprite->addChild(m_badgeSprite);
    }
    
    if (!m_badgeLabel) {
        m_badgeLabel = CCLabelHelper::createBMFont("0", kFontSizeLarge-5.0f, true);
        m_badgeLabel->setPosition(m_badgeSprite->getContentSize()*0.5f);
        m_badgeSprite->addChild(m_badgeLabel);
    }
    
    CREATE_CHAR_BUFFER(_buffer, 8);
    snprintf(_buffer, _buffer_size, "%d", badge_number_);
    
    if (m_badgeLabel)
        m_badgeLabel->setString(_buffer);
    if (m_badgeSprite)
        m_badgeSprite->setVisible(true);
    
}

void HUDMenuButton::setBadgePosition(CCPoint position_) {
    
    if (!m_badgeSprite) return;
    m_badgeSprite->setPosition(position_);
    
}

void HUDMenuButton::setBadgePositionX(float pos_x_) {
    
    if (!m_badgeSprite) return;
    m_badgeSprite->setPositionX(pos_x_);
    
}

void HUDMenuButton::setBadgePositionY(float pos_y_) {
    
    if (!m_badgeSprite) return;
    m_badgeSprite->setPositionY(pos_y_);
    
}

