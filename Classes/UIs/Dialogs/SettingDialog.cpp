//
//  SettingDialog.cpp
//  Cheetah
//
//  Created by Avalable on 3/27/2557 BE.
//
//

#include "SettingDialog.h"
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
extern TargetDevice targetDevice;
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Extensions/CCOpenURL/CCOpenURL.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Extensions/CCAlert/CCAlert.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/Versioning.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/UIHelper.h"
#include "../../Helpers/SocialPlatform/SocialPlatform.h"
#include "LinkSelectTypeDialog.h"
#include "LinkMustSignInDialog.h"

// flag to determine if user is currently in the process of logging in to
// Facebook or GooglePlay. Prevents logging in to both at the same time.
bool _isLoggingIn = false;

SettingDialog* SettingDialog::create(CCModalController *parentModalController_)
{
    SettingDialog *pRet = new SettingDialog();
    pRet->init(parentModalController_, kDialogSetting);
    pRet->autorelease();
    return pRet;
}

bool SettingDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ModalDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    //music button
    CCMenuItemSprite *_btnMusicMenuItem = CCMenuItemScaleSprite::create(getSpriteOnOff(g_gameAudio->isMusicOn()), this, menu_selector(SettingDialog::onMusic));
    m_menu[0] = CCMenu::create(_btnMusicMenuItem, NULL);
    m_menu[0]->setContentSize(_btnMusicMenuItem->getContentSize());
    m_menu[0]->setPosition((margin*2*sizeMultiplier)+m_menu[0]->getContentSize().width*0.5f,
                           310.0f*sizeMultiplier);
    addChild(m_menu[0], 2);
    //label music
    CCLabelBMFont *_music = CCLabelHelper::createBMFont(g_gameContent->Word["music"].c_str(), kFontSizeSmall+5.0f, kTextColorDark);
    _music->setAnchorPoint(ccp(1.0f, 0.5f));
    _music->setAlignment(kCCTextAlignmentRight);
    _music->setPosition(-(margin*2*sizeMultiplier),
                        m_menu[0]->getPositionY());
    addChild(_music);

    //sound effect button
    CCMenuItemSprite *_btnSoundEffectMenuItem = CCMenuItemScaleSprite::create(getSpriteOnOff(g_gameAudio->isSoundOn()), this, menu_selector(SettingDialog::onSound));
    m_menu[1] = CCMenu::create(_btnSoundEffectMenuItem, NULL);
    m_menu[1]->setContentSize(_btnSoundEffectMenuItem->getContentSize());
    m_menu[1]->setPosition((margin*2*sizeMultiplier)+m_menu[1]->getContentSize().width*0.5f,
                           m_menu[0]->getPositionY()
                           -(margin*2*sizeMultiplier)
                           -m_menu[1]->getContentSize().height);
    addChild(m_menu[1], 3);
    //label sound effect
    CCLabelBMFont *_sound_effect = CCLabelHelper::createBMFont(g_gameContent->Word["sound_effect"].c_str(), kFontSizeSmall+5.0f, kTextColorDark);
    _sound_effect->setAnchorPoint(ccp(1.0f, 0.5f));
    _sound_effect->setAlignment(kCCTextAlignmentRight);
    _sound_effect->setPosition(-(margin*2*sizeMultiplier),
                               m_menu[1]->getPositionY());
    addChild(_sound_effect);
    
    //link device button
    CCSprite *_sprite_link = UIHelper::getSprite(UIHelper::SettingsSupportButton);
    CCLabelBMFont *_label_link = CCLabelHelper::createBMFont("Link a device", kFontSizeSmall+5.0f, true);
    _label_link->setPosition(ccp(_sprite_link->getContentSize().width*0.5f, _sprite_link->getContentSize().height*0.55));
    _sprite_link->addChild(_label_link);
    CCMenuItemSprite *_btnLinkDevice = CCMenuItemScaleSprite::create(_sprite_link, this, menu_selector(SettingDialog::onLinkDevice));
    m_linkButton = CCMenu::create(_btnLinkDevice, NULL);
    m_linkButton->setContentSize(_btnLinkDevice->getContentSize());
    m_linkButton->setPosition(margin * 0.5f,
                              m_menu[1]->getPositionY()
                              -(170.0f*sizeMultiplier));
    addChild(m_linkButton, 6);
    
    //support button
    CCSprite *_sprite_support = UIHelper::getSprite(UIHelper::SettingsSupportButton);
    CCLabelBMFont *_label_support = CCLabelHelper::createBMFont(g_gameContent->Word["support"].c_str(), kFontSizeSmall+5.0f, true);
    _label_support->setPosition(ccp(_sprite_support->getContentSize().width*0.5f, _sprite_support->getContentSize().height*0.55));
    _sprite_support->addChild(_label_support);
    CCMenuItemSprite *_btnSupportMenuItem = CCMenuItemScaleSprite::create(_sprite_support, this, menu_selector(SettingDialog::onSupport));
    m_menu[3] = CCMenu::create(_btnSupportMenuItem, NULL);
    m_menu[3]->setContentSize(_btnSupportMenuItem->getContentSize());
    m_menu[3]->setPosition((margin*2*sizeMultiplier)+m_menu[3]->getContentSize().width*0.5f,
                           m_linkButton->getPositionY()
                           -(170.0f*sizeMultiplier));
    addChild(m_menu[3], 5);

    //UUID button
    CCSprite *_sprite_credit = UIHelper::getSprite(UIHelper::SettingsSupportButton);
    CCLabelBMFont *_label_credit = CCLabelHelper::createBMFont("Credits", kFontSizeSmall+5.0f, true);
    _label_credit->setPosition(ccp(_sprite_credit->getContentSize().width*0.5f, _sprite_credit->getContentSize().height*0.55));
    _sprite_credit->addChild(_label_credit);
    CCMenuItemSprite *_btnUUIDMenuItem = CCMenuItemScaleSprite::create(_sprite_credit, this, menu_selector(SettingDialog::onCredit));
    m_menu[2] = CCMenu::create(_btnUUIDMenuItem, NULL);
    m_menu[2]->setContentSize(_btnUUIDMenuItem->getContentSize());
    m_menu[2]->setPosition(-(margin*2*sizeMultiplier)-m_menu[2]->getContentSize().width*0.5f,
                           m_menu[3]->getPositionY());
    addChild(m_menu[2], 4);

    std::ostringstream ss;
    ss << "version " << Versioning::GetBundleVersionShort();
    
    // label version
    CCLabelBMFont *version = CCLabelHelper::createBMFont(ss.str().c_str(), kFontSizeSmall+5.0f, kTextColorDark);
    version->setAnchorPoint(ccp(1.0f, 0.5f));
    version->setAlignment(kCCTextAlignmentRight);
    version->setPosition(ccp(getContentSize().width * 0.5f, -getContentSize().height * 0.5f) - ccp(150.f * sizeMultiplier, -150.f * sizeMultiplier));
    
    CCLog_("Menu3pos: %f height: %f", m_menu[3]->getPositionY(), getContentSize().height);
    
    addChild(version);

    //-----------facebook---------
    composeFacebookButton();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    composeGooglePlayButton();
#endif

    //set title
    addTitle(g_gameContent->Word["settings"].c_str());
    
    //close Dialog
    if (SceneVille *_scene = (SceneVille*)g_currentScene) {
        if (_scene->mapToScreenUI) {
            _scene->mapToScreenUI->closeAllUIs();
        }
        if (IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap) {
            if (_isoMapBuilding->entityAction) {
                _isoMapBuilding->entityAction->finish();
            }
        }
    }
    
    return true;
}

void SettingDialog::composeFacebookButton()
{

    if (m_facebookButton) {
        m_facebookButton->removeFromParent();
    }

    m_facebookButton = NULL;
    m_facebookButton = CCMenuHelper::createMenu(UIHelper::getSprite(UIHelper::FacebookConnectionFacebookButton), this, menu_selector(SettingDialog::onFacebook));
    CCNode *facebookButtonItem = CCMenuHelper::getMenuItemScaleSprite(m_facebookButton);
    m_facebookButton->setAnchorPoint(ccp(0.5f, 0.5f));

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    m_facebookButton->setPosition(0.0f,
                                  m_menu[2]->getPositionY()
                                  - (140.0f*sizeMultiplier));
    m_facebookButton->setScale(1.1f);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    m_facebookButton->setPosition(m_menu[2]->getPositionX() - (margin * 2),
                                  m_menu[2]->getPositionY()
                                  - (140.0f*sizeMultiplier));
#endif

    addChild(m_facebookButton, 5);

    CREATE_CHAR_BUFFER(_buffer, 64);
    if (g_sceneManager->facebook->isLoggedIn()) {
        strncpy(_buffer, g_gameContent->Word["facebook_logout"].c_str(), _buffer_size);
    } else {
        strncpy(_buffer, g_gameContent->Word["facebook_login"].c_str(), _buffer_size);
    }

    CCLabelBMFont *_facebookLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium, true);
    _facebookLabel->setPosition(m_facebookButton->getContentSize().width*0.59f,
                                m_facebookButton->getContentSize().height*0.5f
                                +5*sizeMultiplier);
    _facebookLabel->setAnchorPoint(ccp(0.4f, 0.5f));
    facebookButtonItem->addChild(_facebookLabel, facebookButtonItem->getChildrenCount() + 1);

    //g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, _facebookLabel->getString());
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void SettingDialog::composeGooglePlayButton()
{
    if (m_googlePlayButton)
    {
        m_googlePlayButton->removeFromParent();
    }

    m_googlePlayButton = CCMenuHelper::createMenu(UIHelper::getSprite(UIHelper::GooglePlayButton), this, menu_selector(SettingDialog::onGooglePlayButton));
    CCNode *facebookButtonItem = CCMenuHelper::getMenuItemScaleSprite(m_googlePlayButton);
    m_googlePlayButton->setAnchorPoint(ccp(0.5f, 0.5f));
    m_googlePlayButton->setPosition(m_menu[3]->getPositionX(), m_menu[2]->getPositionY() - (140.0f*sizeMultiplier));
    m_googlePlayButton->setScale(1.0f);
    addChild(m_googlePlayButton, 5);

    CREATE_CHAR_BUFFER(_buffer, 64);
    if (SocialPlatform::getInstance()->isLoggedIn())
    {
        strncpy(_buffer, /*g_gameContent->Word["facebook_logout"].c_str()*/"Logout", _buffer_size);
    }
    else
    {
        strncpy(_buffer, /*g_gameContent->Word["facebook_login"].c_str()*/"Login", _buffer_size);
    }
    
    CCLabelBMFont *_facebookLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium, true);
    _facebookLabel->setPosition(m_googlePlayButton->getContentSize().width*0.59f, m_googlePlayButton->getContentSize().height*0.5f + 5*sizeMultiplier);
    _facebookLabel->setAnchorPoint(ccp(0.4f, 0.5f));
    facebookButtonItem->addChild(_facebookLabel);
}
#endif

void SettingDialog::onMusic(CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);

    g_gameAudio->toggleMusic();
    g_gameAudio->updateAudioSetting();

    if(g_gameAudio->isMusicOn())
    {
        g_gameAudio->playEffectFadeOutAndFadeIn(AUDIO_AMBIENCE_FARM,true,0.1f);
        g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_VIL, true, 0.1, 1.0, 1.0);
    }
    
    CCPoint _position = m_menu[0]->getPosition();
    m_menu[0]->removeFromParent();
    
    CCSprite *_sprite_on_off = getSpriteOnOff(g_gameAudio->isMusicOn());
    CCMenuItemSprite *_btnMusicMenuItem = CCMenuItemScaleSprite::create(_sprite_on_off, this, menu_selector(SettingDialog::onMusic));
    m_menu[0] = CCMenu::create(_btnMusicMenuItem, NULL);
    m_menu[0]->setContentSize(_btnMusicMenuItem->getContentSize());
    m_menu[0]->setPosition(_position);
    this->addChild(m_menu[0], 2);
}

void SettingDialog::onSound(CCObject *sender_) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    g_gameAudio->toggleSound();
    g_gameAudio->updateAudioSetting();
    
    if(g_gameAudio->isSoundOn()) g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    CCPoint _position = m_menu[1]->getPosition();
    m_menu[1]->removeFromParent();
    
    CCSprite *_sprite_on_off = getSpriteOnOff(g_gameAudio->isSoundOn());
    CCMenuItemSprite *_btnSoundEffectMenuItem = CCMenuItemScaleSprite::create(_sprite_on_off, this, menu_selector(SettingDialog::onSound));
    m_menu[1] = CCMenu::create(_btnSoundEffectMenuItem, NULL);
    m_menu[1]->setContentSize(_btnSoundEffectMenuItem->getContentSize());
    m_menu[1]->setPosition(_position);
    this->addChild(m_menu[1],3);
}

void SettingDialog::onHelp(CCObject *sender_) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    this->m_modalController->popView();
}

void SettingDialog::onCredit(cocos2d::CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    CCAlert::Show("Credits:", g_gameContent->Word["credit_detail"].c_str(), g_gameContent->Word["done"].c_str(), NULL, NULL, NULL);
}

void SettingDialog::onLinkDevice(cocos2d::CCObject *sender_)
{
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    LinkSelectTypeDialog* _linkDialog = LinkSelectTypeDialog::create(this->m_modalController);
    _linkDialog->setPosition(_screenSize * 0.5f);
    
    if(this->m_modalController->getParent() == NULL)
    {
        this->getParent()->addChild(this->m_modalController, 10001);
    }
    this->m_modalController->pushView(_linkDialog);
    
//    LinkMustSignInDialog* _linkDialog = LinkMustSignInDialog::create(this->m_modalController);
//    _linkDialog->setPosition(_screenSize * 0.5f);
//    _linkDialog->setCloseLocked(true);
//
//    if(this->m_modalController->getParent() == NULL)
//    {
//        this->getParent()->addChild(this->m_modalController, 10001);
//    }
//    this->m_modalController->pushView(_linkDialog);
}

void SettingDialog::onSupport(CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    this->m_modalController->popView();
    
//    "%0A" = new line
//    "%20" = space
    
    ostringstream urlString;
    
    urlString << "mailto:" << kEmailAddress << "?subject=" << kEmailSubject;
    urlString << "&body=%0A%0A%0A%0A%0A%0A%0AGame%20ID%20is%20" << g_playerState->player.uuid.c_str();
    urlString << "%0APlease%20don't%20remove%20the%20Game%20ID%20above%20as%20we%20need%20it%20to%20identify%20your%20problem.%20Thanks!";

    CCLog_("urlString.str().c_str():%s",urlString.str().c_str());

    CCOpenURL::openURL(urlString.str().c_str());
}

void SettingDialog::onFacebook(cocos2d::CCObject *sender_)
{
	if(_isLoggingIn)
		 return;

	_isLoggingIn = true;

    if(g_sceneManager->facebook->isLoggedIn())
    {
        g_sceneManager->facebook->logout(this, SettingDialog::onFacebookLoggedOut);
    }
    else
    {
//        g_sceneManager->facebook->login(g_sceneManager, SceneManager::onFBLoggedIn);
        g_sceneManager->facebook->login(this, SettingDialog::onFacebookLoggedIn);
//        this->m_modalController->popView();
    }
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void SettingDialog::onGooglePlayButton(cocos2d::CCObject *sender_)
{
	if(_isLoggingIn)
		return;

	_isLoggingIn = true;
    
	g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
	bool isLoggedIn = SocialPlatform::getInstance()->isLoggedIn();

	if(isLoggedIn) {
		SocialPlatform::getInstance()->signOut();
		this->onGooglePlayLoggedOut(this, true);
	}
	else {
		SocialPlatform::getInstance()->setAuthenticationCallback(this, SettingDialog::onGooglePlayLoggedIn);

		SocialPlatform::getInstance()->signInFromSettings();
	}
}
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void SettingDialog::onGooglePlayLoggedIn(void* sender, bool success)
{
	_isLoggingIn = false;

    if(success)
    {
    	g_sceneManager->onGameCenterAuthenticated(&sender, true);
    }

    if (SettingDialog* _this = (SettingDialog*)sender) {
        _this->composeGooglePlayButton();
        _this->m_modalController->popView();
    }
}
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void SettingDialog::onGooglePlayLoggedOut(void* sender, bool success)
{
	_isLoggingIn = false;

    if (!success) return;

    if (SettingDialog* _this = (SettingDialog*)sender)
    {
        _this->composeGooglePlayButton();

        // Remove references to stored gcid
		string _gcid = "";
		g_playerState->player.gcid = _gcid;
		CCUserDefault::sharedUserDefault()->setStringForKey("gcid", _gcid);

		// clear ranch progress
		// g_gameBackend->user_gcid_tie(_gcid, NULL, NULL);
		// CCAlert::Show("Restart", g_gameContent->Word["loadgamealert_will_reload_now"].c_str(), "OK", NULL, g_sceneManager, alert_selector(BackendService::kick_back_to_front));

		_this->m_modalController->popView();
    }
}
#endif

void SettingDialog::onFacebookLoggedIn(void* sender_, bool success_)
{
	_isLoggingIn = false;

    if(success_)
    {
        g_sceneManager->onFBLoggedIn(g_sceneManager, true);
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, g_gameContent->Word["facebook_login_success_message"]);
    }
    
    if (SettingDialog* _this = (SettingDialog*)sender_)
    {
        _this->composeFacebookButton();
        _this->m_modalController->popView();
    }
}

void SettingDialog::onFacebookLoggedOut(void* sender_, bool success_)
{
	_isLoggingIn = false;

    if (!success_) return;
    
    if (SettingDialog* _this = (SettingDialog*)sender_)
    {
        _this->composeFacebookButton();
        _this->m_modalController->popView();
    }
}

CCSprite* SettingDialog::getSpriteOnOff(bool status)
{
    CCSprite *_sprite_on_off = NULL;
    if(status) {
        CCSprite* _sprite_on = UIHelper::getSprite(UIHelper::SettingsOnButton);
        CCLabelBMFont *_label_on = CCLabelHelper::createBMFont("On", kFontSizeSmall+5.0f, true);
        _label_on->setAnchorPoint(ccp(0.5f, 0.5f));
        _label_on->setAlignment(kCCTextAlignmentCenter);
        _label_on->setPosition( _sprite_on->getContentSize().width*0.5f, _sprite_on->getContentSize().height*0.55f);
        _sprite_on->addChild(_label_on);
        _sprite_on_off=_sprite_on;
    } else {
        CCSprite *_sprite_off = UIHelper::getSprite(UIHelper::SettingsOffButton);
        CCLabelBMFont *_label_on = CCLabelHelper::createBMFont("Off", kFontSizeSmall+5.0f, true);
        _label_on->setAnchorPoint(ccp(0.5f, 0.5f));
        _label_on->setAlignment(kCCTextAlignmentCenter);
        _label_on->setPosition( _sprite_off->getContentSize().width*0.5f, _sprite_off->getContentSize().height*0.55f);
        _sprite_off->addChild(_label_on);
        _sprite_on_off=_sprite_off;
    }

    return  _sprite_on_off;
}
