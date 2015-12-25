//
//  LogDialog.cpp
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#include "LogDialog.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

USING_NS_CC;

using namespace std;

#include "../../Helpers/CCSpriteHelper.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/CreateSpriteHelper.h"

#include "../../Commands/SceneVilleCommand.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../UIs/WinStreak.h"

#include "../../AppMacros.h"

//#include "IAPConfirmDialog.h"
//#include "CCUrlSprite.h"

#include "../../Extensions/CCLoadSpriteURL.h"

extern TargetDevice targetDevice;
//extern CCPoint screenFactor;
extern float sizeMultiplier;
extern float retinaMultiplier;

LogDialog* LogDialog::create(CCModalController* parentModalController_) {
    LogDialog *pRet = new LogDialog();
    pRet->init(parentModalController_);
    pRet->autorelease();
    return pRet;
}

bool LogDialog::init(CCModalController* parentModalController_) {
    
    if (!CCModalView::init(parentModalController_)) {
        return false;
    }
    
//    m_modalController->onHitShadow = onHitShadow;
    m_modalController->userObject = this;
    _modalController = m_modalController;
    
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();

    
    CCSprite *_tmp = blankSpriteWithSize(ccp(1,1));
    _tmp->setPosition(ccp(_screenSize.width*0.5,_screenSize.height*0.3));
    addChild(_tmp);
    
    dialog = CCPopOverDialog::create(CCSizeMake(1200*retinaMultiplier, 650*retinaMultiplier), CCPopoverArrowDirectionDown);
    dialog->setAutoDismiss(true);
    dialog->presentPopover(_tmp, true);
    
    _homeLabel = CCLabelHelper::createBMFont(retinaMultiplier, LOG_WON, (kFontSizeLarge), false, kTextColorDark);
    _homeLabel->setPosition(ccp(dialog->getContentSize().width*0.05,dialog->getContentSize().height*0.8));
    _homeLabel->setAnchorPoint(ccp(0,0));
    dialog->addChild(_homeLabel);
    
//    long opFbId = 1097695967;
//    std::stringstream ss;
//    ss << "http://graph.facebook.com/" << opFbId << "/picture?type=square";
//    std::string opFname = "_avatar_default_red.png";
//    std::string opUrl = ss.str();
    
//    CCUrlSprite *opImage = CCUrlSprite::create((char*)opFname.c_str(), (char*)opUrl.c_str());
//    opImage->setPosition(ccp(dialog->getContentSize().width*0.07,dialog->getContentSize().height*0.65));
//    dialog->addChild(opImage);
    
//    CCSprite *frameAvartar = CCSprite::createWithSpriteFrameName("_avatar_frame_red.png");
//    frameAvartar->setPosition(opImage->getPosition());
//    frameAvartar->setScale(0.38);
//    dialog->addChild(frameAvartar);
    
    
//    std::string opFname = "";
//    std::string opUrl = "";
//    
//    //size default
//    CCSize sizeDefaultAvatar = ccp(102 * retinaMultiplier, 102 * retinaMultiplier);
//    
//    CCLoadSpriteURL *avartar = CCLoadSpriteURL::create((char*)"_avatar_default_map.png", sizeDefaultAvatar, (char*)opFname.c_str(), (char*)opUrl.c_str());
//    avartar->setPosition(ccp(xVill-6*retinaMultiplier,vill->getPositionY() + vill->getContentSize().height + 80*sizeMultiplier));
//    _scrollView->addChild(avartar,-1, 999999);
//    
//    CCSprite *frameAvartar = CCSprite::createWithSpriteFrameName("_avatar_frame_map.png");
//    frameAvartar->setPosition(avartar->getPosition());
//    frameAvartar->setAnchorPoint(ccp(0.51,0.5));
//    _avatarbatchNode->addChild(frameAvartar,1);
    
    
//    std::string opUrl = g_opponentState->player.info.photo.c_str();
    std::string opUrl = "";
    std::string opFname = opUrl;
    
    CCSize sizeDefaultAvatar = ccp(102 * retinaMultiplier, 102 * retinaMultiplier);
    
    CCLoadSpriteURL *avartar = CCLoadSpriteURL::create((char*)"_avatar_default_red.png", sizeDefaultAvatar, (char*)opFname.c_str(), (char*)opUrl.c_str());
    avartar->setPosition(ccp(78.0f*sizeMultiplier , opponentLabel->getPositionY() ));
    avartar->setScale(0.6f);
    this->addChild(avartar);
    
    CCSprite *frameAvartar = CCSprite::createWithSpriteFrameName("_avatar_frame_red.png");
    frameAvartar->setPosition(avartar->getPosition());
    frameAvartar->setAnchorPoint(ccp(0.51,0.5));
    frameAvartar->setScale(0.6f);
    this->addChild(frameAvartar,1);
    
    
    _name = CCLabelHelper::createBMFont(retinaMultiplier, "name opponent", (kFontSizeLarge), false, kTextColorGreen);
    _name->setPosition(ccp(dialog->getContentSize().width*0.13,dialog->getContentSize().height*0.6));
    _name->setAnchorPoint(ccp(0,0));
    dialog->addChild(_name);
    
    _time = CCLabelHelper::createBMFont(retinaMultiplier, "time ago", (kFontSizeLarge), false, kTextColorGreen);
    _time->setPosition(ccp(dialog->getContentSize().width*0.95,dialog->getContentSize().height*0.6));
    _time->setAnchorPoint(ccp(1,0));
    dialog->addChild(_time);
    
    _streak = CCLabelHelper::createBMFont(retinaMultiplier, "Winning Streak Bonus", (kFontSizeLarge), false, kTextColorGreen);
    _streak->setPosition(ccp(dialog->getContentSize().width*0.05,dialog->getContentSize().height*0.4));
    _streak->setAnchorPoint(ccp(0,0));
    dialog->addChild(_streak);
    
    CCNikButton *_closeButton = CCNikButton::create(CCSprite::createWithSpriteFrameName("_mini_button_close.png"), NULL);
    _closeButton->setTarget(this, menu_selector(LogDialog::onClose), CCNikButtonEventTouchUpInside);
    _closeButton->setContentSize(CCSizeMake(MAX(70, _closeButton->getContentSize().width), MAX(70, _closeButton->getContentSize().height)));
    _closeButton->setScale(1.5);
    _closeButton->setPosition(ccp(dialog->getContentSize().width - _closeButton->getContentSize().width*0.5f,dialog->getContentSize().height - _closeButton->getContentSize().height*0.5f));
    _closeButton->setColor(ccBLUE);
    dialog->addChild(_closeButton, 5);

    
    CCSprite *_coin = CCSprite::createWithSpriteFrameName("_icon_coin_3.png");
    _coin->setPosition(ccp(dialog->getContentSize().width*0.2,dialog->getContentSize().height*0.25));
    dialog->addChild(_coin);
    
    _coinLabel = CCLabelHelper::createBMFont(retinaMultiplier, "10,000", (kFontSizeExtraLarge), true, kTextColorWhite);
    _coinLabel->setPosition(ccp(_coin->getPositionX()+_coin->getContentSize().width,dialog->getContentSize().height*0.25));
    _coinLabel->setAnchorPoint(ccp(0,0.5));
    dialog->addChild(_coinLabel);
    
    CCNikButton *_Button = CCNikButton::create(CCSprite::createWithSpriteFrameName("_button_green_0.png"), NULL);
    _Button->setTarget(this, menu_selector(LogDialog::onButton), CCNikButtonEventTouchUpInside);
    _Button->setPosition(ccp(dialog->getContentSize().width*0.75f, dialog->getContentSize().height*0.25f));
    dialog->addChild(_Button);
    
    _buttonLabel = CCLabelHelper::createBMFont("Claim", kFontSizeMedium, true);
    _buttonLabel->setPosition(_Button->getContentSize()*0.5f);
    _Button->addChild(_buttonLabel);

    
    this->updateContent();
    
    return true;
}

void LogDialog::updateContent()
{

    if(!winstatus) {
        _homeLabel->setString("You lost the race against.");
        _streak->setString("Winning Streak Ended");
        _streak->setColor(ccRED);
        _buttonLabel->setString("Continue");
    }
    
    
    CCSprite *_fanIcon = CCSprite::createWithSpriteFrameName("_hud_icon_fans.png");
    _fanIcon->setPosition(ccp(dialog->getContentSize().width*0.65,dialog->getContentSize().height*0.85));
    _fanIcon->setAnchorPoint(ccp(1,0.5));
    _fanIcon->setScale(0.75);
    dialog->addChild(_fanIcon);
    
    CCLabelBMFont *_fanLabel = CCLabelHelper::createBMFont(retinaMultiplier, "-1", (kFontSizeExtraLarge), true, kTextColorWhite);
    _fanLabel->setPosition(ccp(_fanIcon->getPositionX()-_fanIcon->getContentSize().width*1.3,_fanIcon->getPositionY()));
    dialog->addChild(_fanLabel);
}

void LogDialog::onButton(CCObject *sender_)
{
    CCLog("Touch Button");
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    dialog->dismissPopover(true);
    this->m_modalController->popView();

}

void LogDialog::onClose(CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    
    dialog->dismissPopover(true);
    this->m_modalController->popView();
}

#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

