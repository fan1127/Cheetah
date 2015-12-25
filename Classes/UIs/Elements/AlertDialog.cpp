//
//  AlertDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 7/23/2557 BE.
//
//

#include "AlertDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Extensions/Modal/CCModalController.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/UIHelper.h"
#include "../../Settings.h"

extern float sizeMultiplier;


AlertDialog *AlertDialog::create(CCModalController *parentModalController_, std::string title_, std::string message_, void *sender_, void (*callback_)(void*,bool))
{
    return create(parentModalController_, title_, message_, g_gameContent->Word["okay"].c_str(), g_gameContent->Word["cancel"].c_str(), true, sender_, callback_);
}

AlertDialog *AlertDialog::create(CCModalController *parentModalController_, std::string title_, std::string message_, bool makeOkayGreen_, void *sender_, void (*callback_)(void*,bool))
{
    return create(parentModalController_, title_, message_, g_gameContent->Word["okay"].c_str(), g_gameContent->Word["cancel"].c_str(), makeOkayGreen_, sender_, callback_);
}

AlertDialog *AlertDialog::create(CCModalController *parentModalController_, std::string title_, std::string message_, std::string okay_, std::string cancel_, bool makeOkayGreen_, void *sender_, void (*callback_)(void*,bool))
{
    
    if (AlertDialog *pRet = new AlertDialog()) {
        if (pRet->init(parentModalController_, title_, message_, okay_, cancel_, makeOkayGreen_, sender_, callback_)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool AlertDialog::init(CCModalController *parentModalController_, std::string title_, std::string message_, std::string okay_, std::string cancel_, bool makeOkayGreen_, void *sender_, void (*callback_)(void*,bool))
{
    
    if (!CCModalView::init(parentModalController_)) {
        return false;
    }
    
    m_sender = sender_;
    m_callback = callback_;
    
    CCScale9Sprite* _background = UIHelper::getScalableSprite(UIHelper::ScalableSprites(UIHelper::PaperBg));
    _background->setContentSize(kDialogSmallSize*sizeMultiplier);
    addChild(_background);
    setAnchorPoint(ccp(0, 0));
    setContentSize(_background->getContentSize());
    
    CCSprite* _tmp = blankSpriteWithSize(_background->getContentSize());
    CCPhatTouchableSprite* _block_sprite = new CCPhatTouchableSprite();
    _block_sprite->autorelease();
    _block_sprite->initWithTexture(_tmp->getTexture());
    _block_sprite->setColor(ccBLUE);
    _block_sprite->setVisible(false);
    addChild(_block_sprite);
    _tmp->removeFromParentAndCleanup(true);
    
    CCMenu *_closeButton = CCMenuHelper::createMenu(UIHelper::getSprite(UIHelper::NSCloseButton), this, menu_selector(AlertDialog::onCancel));
    _closeButton->setPosition(_background->getContentSize()*0.5f
                              + ccp(-20.0f*sizeMultiplier, -20.0f*sizeMultiplier));
    _closeButton->setAnchorPoint(ccp(0.5f, 0.5f));
    addChild(_closeButton);
    
    CCNikButton* _cancelButton = CCNikButton::create(UIHelper::getSprite(UIHelper::UISprites(makeOkayGreen_ ? UIHelper::NotificationNo : UIHelper::NotificationYes)), NULL);
    CCLabelBMFont *_cancelLabel = CCLabelHelper::createBMFont(cancel_.c_str(), StyleButtonCoinAndGems);
    _cancelLabel->setPosition(_cancelButton->getContentSize()*0.5f
                              +ccp(0.0f, 5.0f*sizeMultiplier));
    _cancelButton->addChild(_cancelLabel);
    _cancelButton->setTarget(this, menu_selector(AlertDialog::onCancel), CCNikButtonEventTouchUpInside);
    addChild(_cancelButton);
    
    CCNikButton* _okayButton = CCNikButton::create(UIHelper::getSprite(UIHelper::UISprites(makeOkayGreen_ ? UIHelper::NotificationYes : UIHelper::NotificationNo)), NULL);
    CCLabelBMFont *_okayLabel = CCLabelHelper::createBMFont(okay_.c_str(), StyleButtonCoinAndGems);
    _okayLabel->setPosition(_okayButton->getContentSize()*0.5f
                            +ccp(0.0f, 5.0f*sizeMultiplier));
    _okayButton->addChild(_okayLabel);
    _okayButton->setTarget(this, menu_selector(AlertDialog::onOkay), CCNikButtonEventTouchUpInside);
    addChild(_okayButton);
    
    UIHelper::setTwoNodesToMiddlePosition(_cancelButton, _okayButton, 100.0f*sizeMultiplier, ccp(0.0f, _background->getContentSize().height*-0.5f + 125.0f*sizeMultiplier));
    
    CCLabelBMFont *_title_label = CCLabelHelper::createBMFont(title_.c_str(), StyleDialogTitle);
    _title_label->setPosition(ccp(0.0f,
                                  _background->getContentSize().height*0.5f
                                  - 95.0f*sizeMultiplier));
    addChild(_title_label);

    CCLabelBMFont *_msg = CCLabelHelper::createBMFont(message_.c_str(), kFontSizeMedium, kTextColorDark);
    _msg->setWidth(_background->getContentSize().width * 0.8f);
    _msg->setPosition(ccp(0.0f, 38.0f*sizeMultiplier));
    addChild(_msg);
    
    return true;
}

void AlertDialog::onOkay(CCObject *sender_) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);

    if (m_callback) {
        m_callback(m_sender, true);
    }
    
    m_callback = NULL;
    m_sender = NULL;
    
    m_modalController->closeAll();
    
}

void AlertDialog::onCancel(CCObject *sender_) {
    
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    
    if (m_callback) {
        m_callback(m_sender, false);
    }
    
    m_callback = NULL;
    m_sender = NULL;
    
    m_modalController->popView();
    
}



