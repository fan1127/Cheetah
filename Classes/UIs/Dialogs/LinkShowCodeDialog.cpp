//
//  LinkSelectSystemtypeDialog.h
//  Cheetah
//
//  Created by Kristian Mitra on 1/14/2015 BE.
//
//

#include "LinkShowCodeDialog.h"
USING_NS_CC;
using namespace std;

#include<math.h>

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState *g_playerState;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Services/BackendService.h"
extern BackendService* g_gameBackend;

extern float sizeMultiplier;
extern float margin;

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/UIHelper.h"
#include "../../Helpers/SocialPlatform/SocialPlatform.h"

#include "LinkCodeTimeUpDialog.h"

LinkShowCodeDialog* LinkShowCodeDialog::create(CCModalController *parentModalController_, LinkDeviceResponseData responseData)
{
	LinkShowCodeDialog *pRet = new LinkShowCodeDialog();
    pRet->init(parentModalController_, kDialogLinkDevice, responseData);
    pRet->autorelease();
    return pRet;
}


bool LinkShowCodeDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize, LinkDeviceResponseData responseData)
{
    if (!ModalDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    long longTimeExpire = std::atol(responseData.timeExpire.c_str());
    long longTimeCurrent = std::atol(responseData.timeCurrent.c_str());
    codeTimer = (longTimeExpire - longTimeCurrent);
    
    CCLog_("codeTImer: %f", codeTimer);

    // strings
	std::string _strInstructions1 = string("Now, leave this screen open. On the other device, go to the \"Link a device\" screen and tap \"This is the NEW device\" button.");
	std::string _strInstructions2 = string("Enter this code in your new device:");
	std::string _strWarning = string("Warning: Linking this device will override the current ranch data.");
	std::string _strCodeValid = string("Code Valid:");
	std::string _strTitle = string("Link a device");


	// label instructions 1
	CCLabelBMFont *_labelInst = CCLabelHelper::createBMFont(_strInstructions1.c_str(), kFontSizeSmall+2.0f, kTextColorDark);
	_labelInst->setAnchorPoint(ccp(0.5f, 0.5f));
	_labelInst->setAlignment(kCCTextAlignmentCenter);
	_labelInst->setWidth(this->getContentSize().width * 0.675f);
	_labelInst->setLineBreakWithoutSpace(false);
	_labelInst->setPosition(-margin, 220.0f*sizeMultiplier);
	addChild(_labelInst);


	// label instructions 2
	CCLabelBMFont *_labelInst2 = CCLabelHelper::createBMFont(_strInstructions2.c_str(), kFontSizeSmall+4.0f, kTextColorDark);
	_labelInst2->setAnchorPoint(ccp(0.5f, 0.5f));
	_labelInst2->setAlignment(kCCTextAlignmentCenter);
	_labelInst2->setWidth(this->getContentSize().width * 0.675f);
	_labelInst2->setLineBreakWithoutSpace(false);
	_labelInst2->setPosition(-margin, _labelInst->getPositionY()
                                    - (margin * 4 * sizeMultiplier)
                                    - _labelInst->getContentSize().height * 0.5f
                                    - _labelInst2->getContentSize().height * 0.5f);
	addChild(_labelInst2);


	// generated code
	CCLabelBMFont *_labelGenCode = CCLabelHelper::createBMFont(responseData.key.c_str(), StyleDialogTitle);
	_labelGenCode->setAnchorPoint(ccp(0.5f, 0.5f));
	_labelGenCode->setAlignment(kCCTextAlignmentCenter);
	_labelGenCode->setWidth(this->getContentSize().width);
	_labelGenCode->setLineBreakWithoutSpace(false);
	_labelGenCode->setPosition(-margin, _labelInst2->getPositionY()
                                        - (margin * 2 * sizeMultiplier)
                                        - _labelInst2->getContentSize().height * 0.5f
                                        - _labelGenCode->getContentSize().height * 0.5f);
	addChild(_labelGenCode);


	// Code Valid:
	CCLabelBMFont *_LabelCodeValid = CCLabelHelper::createBMFont(_strCodeValid.c_str(), kFontSizeSmall+4.0f, kTextColorDark);
	_LabelCodeValid->setAnchorPoint(ccp(0.5f, 0.5f));
	_LabelCodeValid->setAlignment(kCCTextAlignmentRight);
	_LabelCodeValid->setWidth(this->getContentSize().width);
	_LabelCodeValid->setLineBreakWithoutSpace(false);
	_LabelCodeValid->setPosition(-(margin*2*sizeMultiplier) - (_LabelCodeValid->getContentSize().width * 0.5f),
                                 _labelGenCode->getPositionY()
                                 - (margin * 2 * sizeMultiplier)
                                 - _labelGenCode->getContentSize().height * 0.5f
                                 - _LabelCodeValid->getContentSize().height * 0.5f);
	addChild(_LabelCodeValid);
 

	// Valid Time:
	CREATE_CHAR_BUFFER(buffer, 32)
	UIDisplayHelper::convertToDisplayTime2(codeTimer, buffer, buffer_size);
	_labelValidTime = CCLabelHelper::createBMFont(buffer, kFontSizeSmall+4.0f, kTextColorDark);
	_labelValidTime->setAnchorPoint(ccp(0.5f, 0.5f));
	_labelValidTime->setAlignment(kCCTextAlignmentLeft);
	_labelValidTime->setWidth(this->getContentSize().width);
	_labelValidTime->setLineBreakWithoutSpace(false);
	_labelValidTime->setPosition((margin * 7 * sizeMultiplier) + (_labelValidTime->getContentSize().width * 0.5f),
                                 _LabelCodeValid->getPositionY());
	addChild(_labelValidTime);


	// Warning
	CCLabelBMFont *_labelWarning = CCLabelHelper::createBMFont(_strWarning.c_str(), kFontSizeSmall+4.0f, kTextColorRed);
	_labelWarning->setAnchorPoint(ccp(0.5f, 0.5f));
	_labelWarning->setAlignment(kCCTextAlignmentCenter);
	_labelWarning->setWidth(this->getContentSize().width * 0.675f);
	_labelWarning->setLineBreakWithoutSpace(false);
	_labelWarning->setPosition(-margin,
                               (this->getContentSize().height * 0.4f * -1)
                               + (_labelWarning->getContentSize().height));
	addChild(_labelWarning);


	//set title
	addTitle(_strTitle.c_str());

	initTimer();

    this->setTapBlackBGToPopView(true);
    
	return true;
}

void LinkShowCodeDialog::initTimer()
{
    unschedule(schedule_selector(LinkShowCodeDialog::updateCodeValidityTimer));
	schedule(schedule_selector(LinkShowCodeDialog::updateCodeValidityTimer), 1.0f);
}

void LinkShowCodeDialog::updateCodeValidityTimer()
{
	CREATE_CHAR_BUFFER(buffer, 16)
	codeTimer -= 1.0f;
	UIDisplayHelper::convertToDisplayTime2(codeTimer, buffer, buffer_size);
	_labelValidTime->setString(buffer);
    
    if(codeTimer <= 0) {
        onCodeValidityEnded();
    }
}

void LinkShowCodeDialog::onCodeValidityEnded()
{
	unschedule(schedule_selector(LinkShowCodeDialog::updateCodeValidityTimer));

	LinkCodeTimeUpDialog *_timeUpDialog = LinkCodeTimeUpDialog::create(this->m_modalController);
	_timeUpDialog->setPosition(this->getPositionX(), this->getPositionY());

	if(this->m_modalController->getParent() == NULL)
	{
		this->getParent()->addChild(this->m_modalController, 100001);
	}

	this->m_modalController->pushView(_timeUpDialog);
}

void LinkShowCodeDialog::onClose(cocos2d::CCObject *sender_) {
    
    unschedule(schedule_selector(LinkShowCodeDialog::updateCodeValidityTimer));
    
    ModalDialog::onClose(sender_);
}

