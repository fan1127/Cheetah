//
//  LoadGameDialog.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 5/28/2557 BE.
//
//

#include "LoadGameDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Extensions/CCAlert/CCAlert.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIUserGcidTie.h"

#include "../../Entities/PlayerState.h"
extern PlayerState *g_playerState;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Helpers/CustomEventsHelper.h"

extern string gcid;
extern float sizeMultiplier;

LoadGameDialog *LoadGameDialog::create(CCModalController *parentModalController_, std::string name_, int level_) {
    
    LoadGameDialog *pRet = new LoadGameDialog();
    pRet->init(parentModalController_, name_, level_, cocos2d::CCSize(1388.0f, 980.0f));
    pRet->autorelease();
    return pRet;
    
}

bool LoadGameDialog::init(CCModalController *parentModalController_, std::string name_, int level_, CCSize size) {
    
    if (!ModalDialog::init(parentModalController_, size))
    {
        return false;
    }

    CREATE_CHAR_BUFFER(_buffer, 128);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["loadgamedialog_title"].c_str());
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["loadgamedialog_title_gp"].c_str());
#endif
    
    CCLabelBMFont *_title = CCLabelHelper::createBMFont(_buffer, kFontSizeVeryLarge, true);
    _title->setPosition(ccp(0.0f, m_background->getContentSize().height*0.5f - 180.f * sizeMultiplier));
    _title->setAnchorPoint(ccp(0.5f, 0.5f));
    addChild(_title);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    snprintf(_buffer, _buffer_size, g_gameContent->Word["loadgamedialog_message"].c_str(), level_);//g_playerState->player.racing.level);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    snprintf(_buffer, _buffer_size, g_gameContent->Word["loadgamedialog_message_gp"].c_str(), level_);//g_playerState->player.racing.level);
#endif

    CCLabelBMFont *_message = CCLabelHelper::createBMFont(_buffer, kFontSizeLarge, false, kTextColorDark, m_background->getContentSize().width*0.8f, kCCTextAlignmentCenter);
    _message->setPosition(ccp(0.0f, _title->getPositionY() + (-140.0f*sizeMultiplier)));
    _message->setAnchorPoint(ccp(0.5f, 0.5f));
    addChild(_message);
    
    CCLabelBMFont *_warningMsg = CCLabelHelper::createBMFont(g_gameContent->Word["loadgamedialog_warning_message"].c_str(), kFontSizeLarge, false, kTextColorOrange, m_background->getContentSize().width*0.8f, kCCTextAlignmentCenter);
    _warningMsg->setPosition(ccp(0.0f, _title->getPositionY() + (-314.0f*sizeMultiplier)));
    _warningMsg->setAnchorPoint(ccp(0.5f, 0.5f));
    addChild(_warningMsg);
    
    
    CCNikButton *_btnLoadGame = CCNikButton::create(UIHelper::getSprite(UIHelper::RankingVisitButton), NULL);
    _btnLoadGame->setTarget(this, menu_selector(LoadGameDialog::onLoadGameButton), CCNikButtonEventTouchUpInside);
    _btnLoadGame->setPosition(ccp(m_background->getContentSize().width*0.24f,
                                  m_background->getContentSize().height*-0.5f
                                  + _btnLoadGame->getContentSize().height + 100.f * sizeMultiplier));
    _btnLoadGame->setAnchorPoint(ccp(0.5f, 0.5f));
    _btnLoadGame->setTouchPriority(-3001);
    
    addChild(_btnLoadGame);
    
    CCLabelBMFont *_label_button_loadgame = CCLabelHelper::createBMFont(g_gameContent->Word["loadgamedialog_button_loadgame_label_button"].c_str(), kFontSizeLarge, true);
    _label_button_loadgame->setPosition(_btnLoadGame->getContentSize()*0.5f + ccp(0.0f, 5.0f*sizeMultiplier));
    _btnLoadGame->addChild(_label_button_loadgame);
    
    CCLabelBMFont *_label_above_button_loadgame = CCLabelHelper::createBMFont(g_gameContent->Word["loadgamedialog_button_loadgame_label_above_button"].c_str(), kFontSizeMedium, kTextColorDark);
    _label_above_button_loadgame->setPosition(ccp(_btnLoadGame->getPositionX(), _btnLoadGame->getPositionY() + 225.0f*sizeMultiplier));
    addChild(_label_above_button_loadgame);
        
    //------
    
    
    CCNikButton *_btnDoNotLoad = CCNikButton::create(UIHelper::getSprite(UIHelper::RankingVisitButton), NULL);
    
    _btnDoNotLoad->setTarget(this, menu_selector(LoadGameDialog::onDoNotLoadGameButton), CCNikButtonEventTouchUpInside);
    _btnDoNotLoad->setPosition(ccp(m_background->getContentSize().width*-0.24f,
                                  m_background->getContentSize().height*-0.5f
                                  + _btnDoNotLoad->getContentSize().height + 100.f * sizeMultiplier));
    _btnDoNotLoad->setAnchorPoint(ccp(0.5f, 0.5f));
    _btnDoNotLoad->setTouchPriority(-3001);
    addChild(_btnDoNotLoad);
    
    CCLabelBMFont *_label_button_donotload = CCLabelHelper::createBMFont(g_gameContent->Word["loadgamedialog_button_donotload_label_button"].c_str(), kFontSizeLarge, true);
    _label_button_donotload->setPosition(_btnDoNotLoad->getContentSize()*0.5f + ccp(0.0f, 5.0f*sizeMultiplier));
    _btnDoNotLoad->addChild(_label_button_donotload);
    
    CCLabelBMFont *_label_above_button_donotload = CCLabelHelper::createBMFont(g_gameContent->Word["loadgamedialog_button_donotload_label_above_button"].c_str(), kFontSizeMedium, kTextColorDark);
    _label_above_button_donotload->setPosition(ccp(_btnDoNotLoad->getPositionX(), _btnDoNotLoad->getPositionY() + 225.0f*sizeMultiplier));
    addChild(_label_above_button_donotload);
    
    
    //------
    
    snprintf(_buffer, _buffer_size, g_gameContent->Word["loadgamedialog_ville_name_and_level"].c_str(), name_.c_str(), level_);
    
    CCLabelBMFont *_loadgame_ville_name = CCLabelHelper::createBMFont(_buffer, kFontSizeVeryLarge, kTextColorDark);
    _loadgame_ville_name->setPosition(ccp(_btnLoadGame->getPositionX(), _btnLoadGame->getPositionY() + 120.0f*sizeMultiplier));
    addChild(_loadgame_ville_name);
    
    
    snprintf(_buffer, _buffer_size, g_gameContent->Word["loadgamedialog_ville_name_and_level"].c_str(), g_playerState->player.info.name.c_str(), g_playerState->player.racing.level);
    
    CCLabelBMFont *_do_not_load_ville_name = CCLabelHelper::createBMFont(_buffer, kFontSizeVeryLarge, kTextColorDark);
    _do_not_load_ville_name->setPosition(ccp(_btnDoNotLoad->getPositionX(), _btnDoNotLoad->getPositionY() + 120.0f*sizeMultiplier));
    addChild(_do_not_load_ville_name);
    
    
    return true;
    
}

void LoadGameDialog::onLoadGameButton(cocos2d::CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    g_playerState->player.gcid = gcid;
    CCUserDefault::sharedUserDefault()->setStringForKey("gcid", g_playerState->player.gcid);
    CCAlert::Show("Restart", g_gameContent->Word["loadgamealert_will_reload_now"].c_str(), "OK", NULL, (CCObject*)BackendService::getInstance(), alert_selector(BackendService::kick_back_to_front));
    
    m_modalController->popView();
}

void LoadGameDialog::onDoNotLoadGameButton(cocos2d::CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    BackendService::getInstance()->send(APIUserGcidTie::create(gcid, this, LoadGameDialog::onUserGcidTied));
    CCUserDefault::sharedUserDefault()->setStringForKey("gcid", gcid);
    
    m_modalController->popView();
}

void LoadGameDialog::onUserGcidTied(void *sender, string json_str) {
    
    //JsonAllocator allocator;
    Json *_root = Json_create(json_str.c_str());
    PlayerState::getInstance()->parseUserInfo(_root);
    Json_dispose(_root);
    
}

