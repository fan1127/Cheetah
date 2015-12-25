//
//  LinkSelectSystemtypeDialog.h
//  Cheetah
//
//  Created by Kristian Mitra on 1/14/2015 BE.
//
//

#include "LinkSelectSystemTypeDialog.h"
USING_NS_CC;
using namespace std;

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
#include "../../Settings.h"
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
#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIUserLinkGet.h"
#include "LinkShowCodeDialog.h"

#include "LinkSameSystemDialog.h"

LinkDeviceResponseData responseData;
bool isInputBlocked = false;

LinkSelectSystemTypeDialog* LinkSelectSystemTypeDialog::create(CCModalController *parentModalController_)
{
	LinkSelectSystemTypeDialog *pRet = new LinkSelectSystemTypeDialog();
    pRet->init(parentModalController_, kDialogLinkDevice);
    pRet->autorelease();
    return pRet;
}


bool LinkSelectSystemTypeDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ModalDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    // strings
    std::string _strSameDevice = string("");
    std::string _strAnotherDevice = string("");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

    _strSameDevice = string("iPhone, iPad or iPod");
    _strAnotherDevice = string("Another Device");

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    _strSameDevice = string("Another Device");
    _strAnotherDevice = string("iPhone, iPad or iPod");

#endif


    std::string _strInstructions = string("Choose the type of device you want to link to:");
    std::string _strTitle = string("Link a device");

    
    // label instructions
    CCLabelBMFont *_labelInst = CCLabelHelper::createBMFont(_strInstructions.c_str(), kFontSizeSmall+5.0f, kTextColorDark);
    _labelInst->setAnchorPoint(ccp(0.5f, 0.5f));
    _labelInst->setAlignment(kCCTextAlignmentCenter);
    _labelInst->setWidth(this->getContentSize().width * 0.675f);
    _labelInst->setLineBreakWithoutSpace(false);
    _labelInst->setPosition(-margin, 180.0f*sizeMultiplier);
    addChild(_labelInst);
    

    //Same System button
    CCSprite* _spriteSameButton = UIHelper::getSprite(UIHelper::PossibleRewardsVisitButton);
    CCLabelBMFont *_label_sameDevice = CCLabelHelper::createBMFont(_strSameDevice.c_str(), kFontSizeSmall+5.0f, true);
    _label_sameDevice->setAnchorPoint(ccp(0.5f, 0.5f));
    _label_sameDevice->setAlignment(kCCTextAlignmentCenter);
    _label_sameDevice->setPosition( _spriteSameButton->getContentSize().width*0.5f, _spriteSameButton->getContentSize().height*0.55f);
    //_label_sameDevice->setScaleX(0.8f);
	_spriteSameButton->addChild(_label_sameDevice);

    CCMenuItemSprite *_btnSameSystem = CCMenuItemScaleSprite::create(_spriteSameButton, this, menu_selector(LinkSelectSystemTypeDialog::onSelectSameSystem));
    //_btnSameSystem->setScaleX(1.2f);
    m_sameSystemButton = CCMenu::create(_btnSameSystem, NULL);
    m_sameSystemButton->setContentSize(_btnSameSystem->getContentSize());
    m_sameSystemButton->setPosition(-margin, _labelInst->getPositionY()
                                    -(margin * 5 * sizeMultiplier)
                                    -m_sameSystemButton->getContentSize().height);
    addChild(m_sameSystemButton, 2);


    //Other System Button
    CCSprite* _spriteOtherButton = UIHelper::getSprite(UIHelper::PossibleRewardsVisitButton);
	CCLabelBMFont *_label_otherDevice = CCLabelHelper::createBMFont(_strAnotherDevice.c_str(), kFontSizeSmall+5.0f, true);
	_label_otherDevice->setAnchorPoint(ccp(0.5f, 0.5f));
	_label_otherDevice->setAlignment(kCCTextAlignmentCenter);
	_label_otherDevice->setPosition( _spriteOtherButton->getContentSize().width*0.5f, _spriteOtherButton->getContentSize().height*0.55f);
	//_label_otherDevice->setScaleX(0.8f);
	_spriteOtherButton->addChild(_label_otherDevice);

    CCMenuItemSprite *_btnAnotherSystem = CCMenuItemScaleSprite::create(_spriteOtherButton, this, menu_selector(LinkSelectSystemTypeDialog::onSelectAnotherSystem));
    //_btnAnotherSystem->setScaleX(1.2f);
    m_otherSystemButton = CCMenu::create(_btnAnotherSystem, NULL);
    m_otherSystemButton->setContentSize(_btnAnotherSystem->getContentSize());
    m_otherSystemButton->setPosition(-margin,
    						m_sameSystemButton->getPositionY()
                           -(margin * 2 * sizeMultiplier)
                           -m_otherSystemButton->getContentSize().height);
    addChild(m_otherSystemButton, 3);


    //set title
    addTitle(_strTitle.c_str());

    this->setTapBlackBGToPopView(true);
    
    return true;
}


void LinkSelectSystemTypeDialog::onSelectSameSystem(CCObject *sender_)
{
	if(isInputBlocked)
		return;

	g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);

	LinkSameSystemDialog *_sameSystemDialog = LinkSameSystemDialog::create(this->m_modalController);
	_sameSystemDialog->setPosition(this->getPositionX(), this->getPositionY());

	if(this->m_modalController->getParent() == NULL)
	{
		this->getParent()->addChild(this->m_modalController, 100001);
	}

	this->m_modalController->pushView(_sameSystemDialog);
}

void LinkSelectSystemTypeDialog::onSelectAnotherSystem(CCObject *sender_)
{
	if(isInputBlocked)
		return;

	g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);

	isInputBlocked = true;
	BackendService::getInstance()->send(APIUserLinkGet::create(this, LinkSelectSystemTypeDialog::onGetLinkCodeCallback));
}

void LinkSelectSystemTypeDialog::showCodeDialog() {

	if(this->getParent() == NULL)
		return;

	CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
	LinkShowCodeDialog *_dialog = LinkShowCodeDialog::create(this->m_modalController, responseData);
	_dialog->setPosition(_screenSize*0.5f);

	if (this->m_modalController->getParent() == NULL) {
		this->getParent()->addChild(this->m_modalController, 10001);
	}

	this->m_modalController->pushView(_dialog);
}

bool LinkSelectSystemTypeDialog::parseLinkResponseData(const char *json_str) {

	// parse string json only once
	//JsonAllocator allocator;
	Json* _root = Json_create(json_str);

	if (!_root) {
		return false;
	}

	Json* _link = Json_getItem(_root, "link");
	if(!_link) {
		return false;
	}

	responseData.key = Json_getString(_link, "key", "_BLANK_");
	responseData.timeExpire = Json_getString(_link, "expire", "_BLANK_");
	responseData.timeCurrent = Json_getString(_root, "currentTime", "_BLANK_");

//	CCLog_("QQQ Key: %s", responseData.key.c_str());
//	CCLog_("QQQ Time Expire: %s", responseData.timeExpire.c_str());
//	CCLog_("QQQ Time Current: %s", responseData.timeCurrent.c_str());

	return true;
}

void LinkSelectSystemTypeDialog::onGetLinkCodeCallback(void *sender, std::string response_json) {

	isInputBlocked = false;

	LinkSelectSystemTypeDialog* _this = (LinkSelectSystemTypeDialog*)sender;

	// parse data
	if(_this->parseLinkResponseData(response_json.c_str())) {
		_this->showCodeDialog();
	}
	else {
		// json cannot be parsed properly
	}
}



