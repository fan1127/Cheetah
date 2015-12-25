/*
 * LinkSameSystemDialog.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: patrick.pamplona
 */

#include "LinkSameSystemDialog.h"
USING_NS_CC;
using namespace std;

#include "DialogSettings.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/UIHelper.h"

extern float sizeMultiplier;
extern float margin;

LinkSameSystemDialog* LinkSameSystemDialog::create(CCModalController *parentModalController_)
{
	LinkSameSystemDialog *pRet = new LinkSameSystemDialog();
    pRet->init(parentModalController_, kDialogLinkDevice);
    pRet->autorelease();
    return pRet;
}

bool LinkSameSystemDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ModalDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    //string
    std::string accountTypeIOS = string("Game Center");
    std::string accountTypeAndroid = string("Google");
    std::string accountType = string(""); //temp
    std::string descIOS = string("To play on multiple iOS devices, please make sure you have signed in to your Game Center account in the game's settings. Then, sign in with your Game Center account on the other device as well. Your progress will be automatically synchronized.");
    std::string descAndroid = string("To play on multiple Android devices, please make sure you have signed in to your Google account in the game's settings. Then, sign in with your Google account on the other device as well. Your progress will be automatically synchronized.");

    std::string str_description = string(""); //temp
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    str_description = descIOS;
    accountType = accountTypeIOS;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    str_description = descAndroid;
    accountType = accountTypeAndroid;
#endif
    std::string str_linkADevice = string("Link a device");
    std::string str_header      = string("Sync with a " + accountType + " account");
    std::string str_Okay        = string("Okay");

    //Dialog title
	addTitle(str_linkADevice.c_str());

	//Dialog header
	CCLabelBMFont *labelHeader = CCLabelHelper::createBMFont(str_header.c_str(), kFontSizeSmall + 8.0f, kTextColorDark);
	labelHeader->setAnchorPoint(ccp(0.5f, 0.5f));
	labelHeader->setAlignment(kCCTextAlignmentLeft);
	labelHeader->setWidth(this->getContentSize().width * 0.725f);
	labelHeader->setLineBreakWithoutSpace(false);
	labelHeader->setPosition(-margin, 260.0f * sizeMultiplier);
	addChild(labelHeader);

    //Dialog text
	CCLabelBMFont *labelInst = CCLabelHelper::createBMFont(str_description.c_str(), kFontSizeSmall + 2.0f, kTextColorDark);
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
	CCLabelBMFont *okayLabel = CCLabelHelper::createBMFont(str_Okay.c_str(), kFontSizeSmall + 5.0f, true);
	okayLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	okayLabel->setAlignment(kCCTextAlignmentCenter);
	okayLabel->setPosition( okayButton->getContentSize().width * 0.5f, okayButton->getContentSize().height * 0.55f);
	okayButton->addChild(okayLabel);

	CCMenuItemSprite *btnOkay = CCMenuItemScaleSprite::create(okayButton, this, menu_selector(LinkSameSystemDialog::onOkaySelected));
	CCMenu *menuOkay = CCMenu::create(btnOkay, NULL);
	menuOkay->setContentSize(btnOkay->getContentSize());
	menuOkay->setPosition(margin, labelInst->getPositionY()
                                - (margin * 8 * sizeMultiplier)
                                - labelInst->getContentSize().height * 0.5f
                                - btnOkay->getContentSize().height * 0.5f);
	addChild(menuOkay, 3);

    this->setTapBlackBGToPopView(true);
    
    return true;
}

void LinkSameSystemDialog::onOkaySelected()
{
	this->m_modalController->closeAll();
}
