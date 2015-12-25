/*
 * LinkDeviceDialog.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: patrick.pamplona
 */

#include "LinkSelectTypeDialog.h"

USING_NS_CC;
using namespace std;

#include "DialogSettings.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/UIHelper.h"

#include "LinkSelectSystemTypeDialog.h"
#include "LinkInputCodeDialog.h"

extern float sizeMultiplier;
extern float margin;

LinkSelectTypeDialog* LinkSelectTypeDialog::create(CCModalController *parentModalController_)
{
	LinkSelectTypeDialog *pRet = new LinkSelectTypeDialog();
    pRet->init(parentModalController_, kDialogLinkDevice);
    pRet->autorelease();
    return pRet;
}

bool LinkSelectTypeDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ModalDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    //string
    std::string str_linkADevice = string("Link a device");
    std::string str_description = string("Select an option to synchronize your current data to a different device:");
    std::string str_oldDevice = string("This is the OLD device");
    std::string str_newDevice = string("This is the NEW device");

    //Dialog title
	addTitle(str_linkADevice.c_str());

    //Dialog text
	CCLabelBMFont *labelInst = CCLabelHelper::createBMFont(str_description.c_str(), kFontSizeSmall + 2.0f, kTextColorDark);
	labelInst->setAnchorPoint(ccp(0.5f, 0.5f));
	labelInst->setAlignment(kCCTextAlignmentLeft);
	labelInst->setWidth(this->getContentSize().width * 0.675f);
	labelInst->setLineBreakWithoutSpace(false);
	labelInst->setPosition(-margin, 180.0f * sizeMultiplier);
	addChild(labelInst);

	//Old device button
    CCSprite *oldDeviceButton = UIHelper::getSprite(UIHelper::PossibleRewardsVisitButton);
    CCLabelBMFont *oldDeviceLabel = CCLabelHelper::createBMFont(str_oldDevice.c_str(), kFontSizeSmall + 5.0f, true);
    oldDeviceLabel->setAnchorPoint(ccp(0.5f, 0.5f));
    oldDeviceLabel->setAlignment(kCCTextAlignmentCenter);
    oldDeviceLabel->setPosition( oldDeviceButton->getContentSize().width * 0.5f, oldDeviceButton->getContentSize().height * 0.55f);
    oldDeviceButton->addChild(oldDeviceLabel);

    CCMenuItemSprite *btnOldDevice = CCMenuItemScaleSprite::create(oldDeviceButton, this, menu_selector(LinkSelectTypeDialog::onOldDeviceSelected));
	CCMenu *menuOldDevice = CCMenu::create(btnOldDevice, NULL);
	menuOldDevice->setContentSize(btnOldDevice->getContentSize());
	menuOldDevice->setPosition(-margin, labelInst->getPositionY() - (margin * 5 * sizeMultiplier) - menuOldDevice->getContentSize().height);
	addChild(menuOldDevice, 2);

	//New device button
	CCSprite *newDeviceButton = UIHelper::getSprite(UIHelper::PossibleRewardsVisitButton);
	CCLabelBMFont *newDeviceLabel = CCLabelHelper::createBMFont(str_newDevice.c_str(), kFontSizeSmall + 5.0f, true);
	newDeviceLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	newDeviceLabel->setAlignment(kCCTextAlignmentCenter);
	newDeviceLabel->setPosition( newDeviceButton->getContentSize().width * 0.5f, newDeviceButton->getContentSize().height * 0.55f);
	newDeviceButton->addChild(newDeviceLabel);

	CCMenuItemSprite *btnNewDevice = CCMenuItemScaleSprite::create(newDeviceButton, this, menu_selector(LinkSelectTypeDialog::onNewDeviceSelected));
	CCMenu *menuNewDevice = CCMenu::create(btnNewDevice, NULL);
	menuNewDevice->setContentSize(btnNewDevice->getContentSize());
	menuNewDevice->setPosition(-margin, menuOldDevice->getPositionY() - (margin * 2 * sizeMultiplier) - menuNewDevice->getContentSize().height);
	addChild(menuNewDevice, 3);

    this->setTapBlackBGToPopView(true);
    
    return true;
}

void LinkSelectTypeDialog::onOldDeviceSelected()
{
	LinkSelectSystemTypeDialog *_systemTypeDialog = LinkSelectSystemTypeDialog::create(this->m_modalController);
	_systemTypeDialog->setPosition(this->getPositionX(), this->getPositionY());

	if(this->m_modalController->getParent() == NULL)
	{
		this->getParent()->addChild(this->m_modalController, 100001);
	}

	this->m_modalController->pushView(_systemTypeDialog);
}

void LinkSelectTypeDialog::onNewDeviceSelected()
{
	LinkInputCodeDialog *_inputCodeDialog = LinkInputCodeDialog::create(this->m_modalController);
	_inputCodeDialog->setPosition(this->getPositionX(), this->getPositionY());

	if(this->m_modalController->getParent() == NULL)
	{
		this->getParent()->addChild(this->m_modalController, 100001);
	}

	this->m_modalController->pushView(_inputCodeDialog);
}
