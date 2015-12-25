//
//  WaitForRaceToEndDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/21/2557 BE.
//
//

#include "WaitForRaceToEndDialog.h"
extern float sizeMultiplier;

#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/UIDisplayHelper.h"

#define kWaitForRaceToEndDialog_x_pos (193.0f*sizeMultiplier)

WaitForRaceToEndDialog* WaitForRaceToEndDialog::create(CCModalController *parentModalController, int seconds) {
    
    WaitForRaceToEndDialog* dialog = new WaitForRaceToEndDialog();
    dialog->m_seconds = seconds;
    dialog->init(parentModalController);
    dialog->autorelease();
    return dialog;
    
}

bool WaitForRaceToEndDialog::init(CCModalController *parentModalController) {
    
    if (!ModalDialog::init(parentModalController, cocos2d::CCSize(1536.0f, 800.0f)))
        return false;
    
    CCSprite *m_merchantSay = CCSprite::create("common/character/merchant_say.png");
    m_merchantSay->setScale(sizeMultiplier);
    m_merchantSay->setAnchorPoint(ccp(0.5f, 0.0f));
    m_merchantSay->setPosition(ccp(-448.0f * sizeMultiplier, m_background->getContentSize().height*0.5f*-1.0f+75.0f*sizeMultiplier));
    this->addChild(m_merchantSay);
    
    CCNode* nodesGroup = CCNode::create();
    this->addChild(nodesGroup);
    
    CCLabelBMFont *header = CCLabelHelper::createBMFont("Racing in Progress!", kFontSizeVeryLarge, true);
    header->setAnchorPoint(ccp(0.5f, 0.5f));
    header->setAlignment(kCCTextAlignmentCenter);
    header->setWidth(500.0f*sizeMultiplier);
    header->setPosition(kWaitForRaceToEndDialog_x_pos, 193.0f*sizeMultiplier);
    nodesGroup->addChild(header);
    
    CCLabelBMFont *details = CCLabelHelper::createBMFont("Someone is racing on your ranch.\nI wonder if we will win or lose?\nLet's wait and see!",
                                                         kFontSizeMedium, kTextColorDark);
    details->setAnchorPoint(ccp(0.5f, 0.5f));
    details->setAlignment(kCCTextAlignmentCenter);
    details->setWidth(750.0f*sizeMultiplier);
    details->setPosition(kWaitForRaceToEndDialog_x_pos, 0);
    nodesGroup->addChild(details);
    
    CREATE_CHAR_BUFFER(_buf1, 16)
    CREATE_CHAR_BUFFER(_buf2, 128);
    UIDisplayHelper::convertToDisplayTime2(m_seconds, _buf1, _buf1_size);
    snprintf(_buf2, _buf2_size, "Estimate time left: %s", _buf1);
        
    m_footer = CCLabelHelper::createBMFont(_buf2, kFontSizeVeryLarge, true);
    m_footer->setAnchorPoint(ccp(0.5f, 0.5f));
    m_footer->setAlignment(kCCTextAlignmentCenter);
    m_footer->setWidth(800.0f*sizeMultiplier);
    m_footer->setPosition(kWaitForRaceToEndDialog_x_pos, -200.0f*sizeMultiplier);
    nodesGroup->addChild(m_footer);
    
    schedule(schedule_selector(WaitForRaceToEndDialog::updateAtSecond), 1.0f);
    
    return true;
    
}

void WaitForRaceToEndDialog::updateAtSecond(float delta) {
    
    m_seconds --;
    
    if (m_seconds < 0) {
        unschedule(schedule_selector(WaitForRaceToEndDialog::updateAtSecond));
        m_modalController->closeAll();
        return;
    }
    
    if (!m_footer) return;
        
    CREATE_CHAR_BUFFER(_buf1, 16);
    CREATE_CHAR_BUFFER(_buf2, 128);
    UIDisplayHelper::convertToDisplayTime2(m_seconds, _buf1, _buf1_size);
    snprintf(_buf2, _buf2_size, "Estimate time left: %s", _buf1);
    
    m_footer->setString(_buf2);
    
}
