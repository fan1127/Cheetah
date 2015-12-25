/*
 * LinkInputCodeDialog.cpp
 *
 *  Created on: Jan 20, 2015
 *      Author: patrick.pamplona
 */

#include "LinkInputCodeDialog.h"
USING_NS_CC;
using namespace std;

#include "DialogSettings.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/UIHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIUserLink.h"
#include "../../Extensions/CCAlert/CCAlert.h"

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

extern string gcDisplayAlias;
extern float sizeMultiplier;
extern float margin;
extern string uuid;

bool isInputAllowed = true;

LinkInputCodeDialog* LinkInputCodeDialog::create(CCModalController *parentModalController_)
{
	LinkInputCodeDialog *pRet = new LinkInputCodeDialog();
    pRet->init(parentModalController_, kDialogLinkDevice);
    pRet->autorelease();
    return pRet;
}

bool LinkInputCodeDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ModalDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    std::string str_linkADevice = string("Link a device");
    std::string str_header      = string("Enter the link code here:");
    std::string str_description = string("(To get a link code: on the old device, go to the Link a Device screen and tap the \"This is the OLD device\" button.)");
    
    std::string str_warningAndroid = string("Make sure you are logged out of Google Play before linking.");
    std::string str_warningIOS = string("Make sure you are logged out Game Center before linking.");
    
    std::string str_Okay        = string("Okay");

    //Dialog title
	addTitle(str_linkADevice.c_str());

	//Dialog header
	CCLabelBMFont *labelHeader = CCLabelHelper::createBMFont(str_header.c_str(), kFontSizeSmall + 8.0f, kTextColorDark);
	labelHeader->setAnchorPoint(ccp(0.5f, 0.5f));
	labelHeader->setAlignment(kCCTextAlignmentLeft);
	labelHeader->setWidth(this->getContentSize().width * 0.725f);
	labelHeader->setLineBreakWithoutSpace(false);
	labelHeader->setPosition(0 * 0.5f, 260.0f * sizeMultiplier);
	addChild(labelHeader);

	//Text Field
	m_codeBox = CCEditBox::create(ccp(this->getContentSize().width  * 0.5f,
									  this->getContentSize().height * 0.1f),
									  CCScale9Sprite::createWithSpriteFrameName("Scalable/input_field.png"));
	m_codeBox->setDelegate(this);
	m_codeBox->setPosition(ccp(margin * 0.5f, labelHeader->getPositionY()
                                            - labelHeader->getContentSize().height
                                            - (margin * 2 * sizeMultiplier)));
	m_codeBox->setFontColor(ccBLACK);
	m_codeBox->setFontSize(20);
	m_codeBox->setPlaceHolder("");
	//m_codeBox->setText(gcDisplayAlias.c_str());
	m_codeBox->setMaxLength(9);
	m_codeBox->setReturnType(kKeyboardReturnTypeDone);
	this->addChild(m_codeBox);

	//m_codeBox->touchDownAction(m_codeBox, CCControlEventTouchUpInside);
	//m_codeBox->setInputFlag(kEditBoxInputFlagInitialCapsAllCharacters);
    m_codeBox->setInputMode(kEditBoxInputModeNumeric);
    
	CCSprite* _codeBoxColiderSprite = blankSpriteWithSize(m_codeBox->getContentSize());
	_codeBoxColiderSprite->setColor(ccRED);
	_codeBoxColiderSprite->setOpacity(0);
	CCMenuItemSprite *_codeBoxColiderItem = CCMenuItemScaleSprite::create(_codeBoxColiderSprite, this, menu_selector(LinkInputCodeDialog::editBoxShouldOpen));
	CCMenu *_codeBoxColider = CCMenu::create(_codeBoxColiderItem, NULL);
	_codeBoxColider->setContentSize(_codeBoxColider->getContentSize());
	_codeBoxColider->setPosition(m_codeBox->getPosition());
	_codeBoxColider->setTouchPriority(kCCMenuHandlerPriority - 1);
	addChild(_codeBoxColider, -1);

    //Dialog text
	CCLabelBMFont *labelInst = CCLabelHelper::createBMFont(str_description.c_str(), kFontSizeSmall + 2.0f, kTextColorDark);
	labelInst->setAnchorPoint(ccp(0.5f, 0.5f));
	labelInst->setAlignment(kCCTextAlignmentLeft);
	labelInst->setWidth(this->getContentSize().width * 0.625f);
	labelInst->setLineBreakWithoutSpace(false);
	labelInst->setPosition(0, m_codeBox->getPositionY()
                                    - (m_codeBox->getContentSize().height * 0.5f)
                                    - (labelInst->getContentSize().height * 0.5f)
                                    - (margin * 4 * sizeMultiplier));
	addChild(labelInst);

    // warning text
/*    CCLabelBMFont *labelInst2;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    labelInst2 = CCLabelHelper::createBMFont(str_warningAndroid.c_str(), kFontSizeSmall + 2.0f, kTextColorDark);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    labelInst2 = CCLabelHelper::createBMFont(str_warningIOS.c_str(), kFontSizeSmall + 2.0f, kTextColorDark);
#endif
    labelInst2->setAnchorPoint(ccp(0.5f, 0.5f));
    labelInst2->setAlignment(kCCTextAlignmentLeft);
    labelInst2->setWidth(this->getContentSize().width * 0.625f);
    labelInst2->setLineBreakWithoutSpace(false);
    labelInst2->setPosition(0, labelInst->getPositionY()
                                    - (labelInst->getContentSize().height * 0.5f)
                                    - (labelInst2->getContentSize().height * 0.5f)
                                    - (margin * 2 * sizeMultiplier));
    addChild(labelInst2);
*/
    
    //Okay button
	CCSprite *okayButton = UIHelper::getSprite(UIHelper::SettingsOnButton);
	CCLabelBMFont *okayLabel = CCLabelHelper::createBMFont(str_Okay.c_str(), kFontSizeSmall + 5.0f, true);
	okayLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	okayLabel->setAlignment(kCCTextAlignmentCenter);
	okayLabel->setPosition( okayButton->getContentSize().width * 0.5f, okayButton->getContentSize().height * 0.55f);
	okayButton->addChild(okayLabel);

	CCMenuItemSprite *btnOkay = CCMenuItemScaleSprite::create(okayButton, this, menu_selector(LinkInputCodeDialog::onOkaySelected));
	CCMenu *menuOkay = CCMenu::create(btnOkay, NULL);
	menuOkay->setContentSize(btnOkay->getContentSize());
    menuOkay->setPosition(0, labelInst->getPositionY()
                          - (labelInst->getContentSize().height * 0.5f)
                          - (menuOkay->getContentSize().height * 0.5f)
                          - (margin * 10 * sizeMultiplier));
    
    addChild(menuOkay, 3);

    this->setTapBlackBGToPopView(true);
    
    return true;
}

void LinkInputCodeDialog::onOkaySelected()
{
	if(!isInputAllowed)
		return;

    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
	isInputAllowed = false;
	std::string key = string(m_codeBox->getText());
    
    if(key.empty()) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, "Cannot be empty.");
        isInputAllowed = true;
        return;
    }
    
	BackendService::getInstance()->send(APIUserLink::create(key, this, LinkInputCodeDialog::onSendCodeResponse));
}

void LinkInputCodeDialog::editBoxReturn(cocos2d::extension::CCEditBox *editBox)
{

}

void LinkInputCodeDialog::editBoxTextChanged(CCEditBox* editBox, const std::string& text)
{

}

void LinkInputCodeDialog::editBoxShouldOpen(cocos2d::CCObject *object_)
{
    m_codeBox->touchDownAction(m_codeBox, CCControlEventTouchDown);
}

bool LinkInputCodeDialog::parseLinkResponseData(const char *json_str) {

	// parse string json only once
	//JsonAllocator allocator;
	Json* _root = Json_create(json_str);

	if (!_root) {
		return false;
	}

	Json* _warning = Json_getItem(_root, "warning");
	if(_warning) {
		return false;
	}

	std::string newUUID = Json_getString(_root, "uuid", "_BLANK_");
	PlayerState::getInstance()->setUUID(newUUID);

	CCAlert::Show("Restart", g_gameContent->Word["loadgamealert_will_reload_now"].c_str(), "OK", NULL, (CCObject*)BackendService::getInstance(), alert_selector(BackendService::kick_back_to_front));
	//CCAlert::Show("Restart", newUUID.c_str(), "OK", NULL, (CCObject*)BackendService::getInstance(), alert_selector(BackendService::kick_back_to_front));
	m_modalController->popView();
    
	return true;
}

void LinkInputCodeDialog::onSendCodeResponse(void *sender, std::string response_json) {

	isInputAllowed = true;

	LinkInputCodeDialog *_this = (LinkInputCodeDialog *)sender;

	// parse data
	if(_this->parseLinkResponseData(response_json.c_str()) == false) {
		// Linking failed. backend class will show alert box.
        g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, "Link code is invalid or has already expired");
	}
}
