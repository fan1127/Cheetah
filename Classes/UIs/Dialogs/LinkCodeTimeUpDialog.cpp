/*
 * LinkSameSystemDialog.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: patrick.pamplona
 */

#include "LinkCodeTimeUpDialog.h"
USING_NS_CC;
using namespace std;

#include "DialogSettings.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/UIHelper.h"

extern float sizeMultiplier;
extern float margin;

LinkCodeTimeUpDialog* LinkCodeTimeUpDialog::create(CCModalController *parentModalController_)
{
	LinkCodeTimeUpDialog *pRet = new LinkCodeTimeUpDialog();
    pRet->init(parentModalController_, kDialogLinkDevice);
    pRet->autorelease();
    return pRet;
}

bool LinkCodeTimeUpDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ModalDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    //string
    std::string newLine = "\n";

    std::string str_linkADevice = string("Link a device");
    std::string str_header      = string("Link code expired");
    std::string str_description = string("The link code has expired. The code must be entered into the new device within 2m."
    									 + newLine + "You can get a new code easily just by re-entering the link code screen.");
    std::string str_Okay        = string("Okay");

    //Dialog title
	addTitle(str_linkADevice.c_str());

	//Dialog header
	CCLabelBMFont *labelHeader = CCLabelHelper::createBMFont(str_header.c_str(), kFontSizeSmall + 8.0f, kTextColorDark);
	labelHeader->setAnchorPoint(ccp(0.5f, 0.5f));
	labelHeader->setAlignment(kCCTextAlignmentLeft);
	labelHeader->setWidth(this->getContentSize().width * 0.725f);
	labelHeader->setLineBreakWithoutSpace(false);
	labelHeader->setPosition(-margin * 0.5f, 220.0f * sizeMultiplier);
	addChild(labelHeader);

    //Dialog text
	CCLabelBMFont *labelInst = CCLabelHelper::createBMFont(str_description.c_str(), kFontSizeSmall + 2.0f, kTextColorDark);
	labelInst->setAnchorPoint(ccp(0.5f, 0.5f));
	labelInst->setAlignment(kCCTextAlignmentLeft);
	labelInst->setWidth(this->getContentSize().width * 0.725f);
	labelInst->setLineBreakWithoutSpace(false);
	labelInst->setPosition(-margin, labelHeader->getPositionY()
                                - (margin * 2 * sizeMultiplier)
                                - (labelHeader->getContentSize().height * 0.5f)
                                - (labelInst->getContentSize().height * 0.5f));
	addChild(labelInst);

    //Okay button
	CCSprite *okayButton = UIHelper::getSprite(UIHelper::SettingsOnButton);
	CCLabelBMFont *okayLabel = CCLabelHelper::createBMFont(str_Okay.c_str(), kFontSizeSmall + 5.0f, true);
	okayLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	okayLabel->setAlignment(kCCTextAlignmentCenter);
	okayLabel->setPosition( okayButton->getContentSize().width * 0.5f, okayButton->getContentSize().height * 0.55f);
	okayButton->addChild(okayLabel);

	CCMenuItemSprite *btnOkay = CCMenuItemScaleSprite::create(okayButton, this, menu_selector(LinkCodeTimeUpDialog::onOkaySelected));
	CCMenu *menuOkay = CCMenu::create(btnOkay, NULL);
	menuOkay->setContentSize(btnOkay->getContentSize());
	menuOkay->setPosition(-margin, labelInst->getPositionY()
                                - (margin * 2 * sizeMultiplier)
                                - (labelInst->getContentSize().height * 0.75f)
                                - (menuOkay->getContentSize().height * 0.5f));
	addChild(menuOkay, 3);
    
    this->setTapBlackBGToPopView(true);
    
    return true;
}

void LinkCodeTimeUpDialog::onOkaySelected()
{
	this->m_modalController->popView();
	this->m_modalController->popView();
	this->m_modalController->popView();
	this->m_modalController->popView();
}
