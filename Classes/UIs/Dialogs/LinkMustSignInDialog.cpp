//
//  LinkMustSignInDialog.cpp
//  Cheetah
//
//  Created by Kristian Mitra on 2/10/15.
//
//

#include "LinkMustSignInDialog.h"
USING_NS_CC;
using namespace std;

#include "DialogSettings.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/UIHelper.h"

extern float sizeMultiplier;
extern float margin;

LinkMustSignInDialog* LinkMustSignInDialog::create(CCModalController *parentModalController_)
{
    LinkMustSignInDialog *pRet = new LinkMustSignInDialog();
    pRet->init(parentModalController_, kDialogLinkDevice);
    pRet->autorelease();
    return pRet;
}

bool LinkMustSignInDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ModalDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }
    
    //string
    std::string systemType = string("Android");
    std::string accountType = string("Google");
    std::string descIOS = string("Please log-in to Game Center to perform the device link. To do this, launch the Game Center app and login from there. Then try again.");
    std::string descAndroid = string("Please log-in to Google Play to perform the device link. To do this, open the Settings Window and select the Login Button for Google Play. Then try again.");
    
    std::string headerAndroid = string("Not Logged in to Google Play");
    std::string headerIOS = string("Not Logged in to Game Center");
    
    std::string _strDescription = string("temp description");
    std::string _strHeader      = string("temp header");
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    _strDescription = descIOS;
    _strHeader = headerIOS;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _strDescription = descAndroid;
    _strHeader = headerAndroid;
#endif
    std::string _strLinkADevice = string("Link a device");
    std::string _strOkay        = string("Okay");
    
    //Dialog title
    addTitle(_strLinkADevice.c_str());
    
    //Dialog header
    CCLabelBMFont *labelHeader = CCLabelHelper::createBMFont(_strHeader.c_str(), kFontSizeSmall + 8.0f, kTextColorDark);
    labelHeader->setAnchorPoint(ccp(0.5f, 0.5f));
    labelHeader->setAlignment(kCCTextAlignmentLeft);
    labelHeader->setWidth(this->getContentSize().width * 0.725f);
    labelHeader->setLineBreakWithoutSpace(false);
    labelHeader->setPosition(-margin, 260.0f * sizeMultiplier);
    addChild(labelHeader);
    
    //Dialog text
    CCLabelBMFont *labelInst = CCLabelHelper::createBMFont(_strDescription.c_str(), kFontSizeSmall + 2.0f, kTextColorDark);
    labelInst->setAnchorPoint(ccp(0.5f, 0.5f));
    labelInst->setAlignment(kCCTextAlignmentLeft);
    labelInst->setWidth(this->getContentSize().width * 0.675f);
    labelInst->setLineBreakWithoutSpace(false);
    labelInst->setPosition(-margin, labelHeader->getPositionY()
                           - (margin * 2 * sizeMultiplier)
                           - labelHeader->getContentSize().height * 0.5f
                           - labelInst->getContentSize().height * 0.5f);
    addChild(labelInst);
    
    //Okay button
    CCSprite *okayButton = UIHelper::getSprite(UIHelper::SettingsOnButton);
    CCLabelBMFont *okayLabel = CCLabelHelper::createBMFont(_strOkay.c_str(), kFontSizeSmall + 5.0f, true);
    okayLabel->setAnchorPoint(ccp(0.5f, 0.5f));
    okayLabel->setAlignment(kCCTextAlignmentCenter);
    okayLabel->setPosition( okayButton->getContentSize().width * 0.5f, okayButton->getContentSize().height * 0.55f);
    okayButton->addChild(okayLabel);
    
    CCMenuItemSprite *btnOkay = CCMenuItemScaleSprite::create(okayButton, this, menu_selector(LinkMustSignInDialog::onOkaySelected));
    CCMenu *menuOkay = CCMenu::create(btnOkay, NULL);
    menuOkay->setContentSize(btnOkay->getContentSize());
    menuOkay->setPosition(margin, labelInst->getPositionY()
                          - (margin * 2 * sizeMultiplier)
                          - labelInst->getContentSize().height * 0.5f
                          - btnOkay->getContentSize().height * 0.5f);
    addChild(menuOkay, 3);

    this->setTapBlackBGToPopView(true);
    
    return true;
}

void LinkMustSignInDialog::onOkaySelected()
{
    this->m_modalController->popView();
}