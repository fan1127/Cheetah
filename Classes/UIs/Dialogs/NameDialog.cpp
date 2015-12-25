//
//  NameDialog.cpp
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#include "NameDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include "../../Services/BackendService.h"
extern BackendService*      g_gameBackend;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

extern string gcDisplayAlias;

#include "../../Scenes/SceneVille.h"
extern CCScene* g_currentScene;
extern float sizeMultiplier;

#include "../../Helpers/UIDisplayHelper.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Helpers/CCSpriteHelper.h"

NameDialog* NameDialog::create(void *sender_, void (*callback_)(void *, std::string)) {
    
    NameDialog* pRet = new NameDialog();
    if (pRet) {
        pRet->sender = sender_;
        pRet->callback = callback_;
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool NameDialog::init() {
    
    if (!CCNode::init()) {
        return false;
    }
    
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    CCScale9Sprite* _background = UIHelper::getScalableSprite(UIHelper::PaperBg);
    _background->setContentSize(kNameDialogSize*sizeMultiplier);
    _background->setPosition(ccp(_screenSize.width*kNameDialogPosRatio.x,
                                 _screenSize.height*kNameDialogPosRatio.y));
    this->addChild(_background);
    
    CCLabelBMFont *_tutorialTxt = CCLabelHelper::createBMFont(g_gameContent->Word["name_dialog_title"].c_str(), kFontSizeMedium, true, kTextColorWhite);
    _tutorialTxt->setPosition(ccp(_background->getContentSize().width*0.5,
                                  _background->getContentSize().height
                                  -_tutorialTxt->getContentSize().height*0.6));
    _background->addChild(_tutorialTxt);
    
    m_nameBox = CCEditBox::create(ccp(_background->getContentSize().width*0.714f,
                                      _background->getContentSize().height*0.262f),
                                  CCScale9Sprite::createWithSpriteFrameName("Scalable/input_field.png"));
    m_nameBox->setDelegate(this);
    m_nameBox->setPosition(ccp(_background->getContentSize().width*0.5,_background->getContentSize().height*0.63));
    m_nameBox->setFontColor(ccBLACK);
    m_nameBox->setFontSize(24);
    m_nameBox->setPlaceHolder("");
    m_nameBox->setText(gcDisplayAlias.c_str());
    m_nameBox->setMaxLength(12);
    m_nameBox->setReturnType(kKeyboardReturnTypeDone);
    _background->addChild(m_nameBox);

    CCLog_("NameDialog::init");
    m_nameBox->setInputFlag(kEditBoxInputFlagInitialCapsSentence);
    
    SceneVille *_sceneVille = (SceneVille *)g_currentScene;
    if(_sceneVille->modalController->getNStackView() == 0) {
        m_nameBox->touchDownAction(m_nameBox, CCControlEventTouchUpInside);
    }
    
    
    CCSprite* _nameBoxColiderSprite = blankSpriteWithSize(m_nameBox->getContentSize());
    _nameBoxColiderSprite->setColor(ccRED);
    _nameBoxColiderSprite->setOpacity(0);
    CCMenuItemSprite *_nameBoxColiderItem = CCMenuItemScaleSprite::create(_nameBoxColiderSprite, this, menu_selector(NameDialog::editBoxShouldOpen));
    CCMenu *_nameBoxColider = CCMenu::create(_nameBoxColiderItem, NULL);
    _nameBoxColider->setContentSize(_nameBoxColiderSprite->getContentSize());
    _nameBoxColider->setPosition(m_nameBox->getPosition());
    _nameBoxColider->setTouchPriority(kCCMenuHandlerPriority - 1);
    _background->addChild(_nameBoxColider);
    
    m_warnTxt = CCLabelHelper::createBMFont(g_gameContent->Word["name_dialog_warning"].c_str(), kFontSizeSmall, false, kTextColorDark);
    m_warnTxt->setPosition(ccp(_background->getContentSize().width*0.5,
                               _background->getContentSize().height*0.42));
    _background->addChild(m_warnTxt);
    
    CCSprite *_button = UIHelper::getSprite(UIHelper::UISprites(UIHelper::NotificationYes));
    float buttonScale = 0.9f;
    _button->setScale(buttonScale);
    CCLabelBMFont *_buttonLabel = CCLabelHelper::createBMFont(g_gameContent->Word["done"].c_str(), kFontSizeLarge, true);
    _buttonLabel->setPosition(_button->getContentSize()*0.5f + ccp(0.0f, 5.0f*sizeMultiplier));
    _button->addChild(_buttonLabel);
    CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_button, this, menu_selector(NameDialog::onSaveNameButton));
    CCMenu *_button_upgrade = CCMenu::create(_menuItem, NULL);
    _button_upgrade->setPosition(ccp(_background->getContentSize().width*0.5f,
                                     _background->getContentSize().height*0.2f));
    _button_upgrade->setPosition(_button_upgrade->getPosition() + _button->getContentSize()*(1.0f-buttonScale));
    _button_upgrade->setContentSize(_button->getContentSize());
    _button_upgrade->setTouchPriority(kCCMenuHandlerPriority - 1);
    _background->addChild(_button_upgrade);
    
    m_backupInput = "";
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//    m_nameBox->touchDownAction(m_nameBox, CCControlEventTouchDown);
#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    return true;
    
}

void NameDialog::onSaveNameButton(cocos2d::CCObject *sender_) {

    SceneVille *_sceneVille = (SceneVille *)g_currentScene;
    if(_sceneVille->modalController->getNStackView() > 0) {
        // Do not process button taps when there is a modal dialog displayed.
        return;
    }
    
    std::string nameString = m_nameBox->getText();
    if(nameString.length() < kNameDialogLengthMin) {
        m_warnTxt->setString(g_gameContent->Word["name_dialog_short_name_msg"].c_str());
        m_backupInput = "";
        CCLog_("NameDialog::onSaveNameButton");
        m_nameBox->touchDownAction(m_nameBox, CCControlEventTouchUpInside);
    } else {
        m_warnTxt->setString("");
        if (callback) {
            callback(sender, nameString);
        }
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        stopAllActions();
        runAction(CCSequence::create(CCEaseBackOut::create(CCMoveBy::create(1.0f, ccp(0, _screenSize.height*0.7))),
                                     CCRemoveSelf::create(), NULL));
    }
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
}

void NameDialog::editBoxReturn(cocos2d::extension::CCEditBox *editBox) {
    
    std::string nameString = m_nameBox->getText();
    if(nameString.length() < kNameDialogLengthMin) {
        m_warnTxt->setString(g_gameContent->Word["name_dialog_short_name_msg"].c_str());
    } else {
        m_warnTxt->setString(g_gameContent->Word["name_dialog_name_confirm_msg"].c_str());
    }
    
    g_gameAudio->playEffect(AUDIO_TYPING);
    
}

void NameDialog::editBoxTextChanged(CCEditBox* editBox, const std::string& text) {

    int n = strlen(kNameDialogCharacters);

    for (int i=0; i<text.length(); i++) {
        
        bool match = false;
        for (int j=0; j<n; j++) {
            if (int(text[i]) == int(kNameDialogCharacters[j])) {
                match = true;
            }
        }
        
        if(!match) {
            editBox->setText(m_backupInput.c_str());
            m_warnTxt->setString(g_gameContent->Word["name_dialog_language_msg"].c_str());
            break;
        }
    }
    
    m_backupInput = editBox->getText();
    m_warnTxt->setString(g_gameContent->Word["name_dialog_warning"].c_str());
        
    g_gameAudio->playEffect(AUDIO_TYPING);
    
}

void NameDialog::editBoxShouldOpen(cocos2d::CCObject *object_) {

    CCLog_("NameDialog::editBoxShouldOpen");

    SceneVille *_sceneVille = (SceneVille *)g_currentScene;
    if(_sceneVille->modalController->getNStackView() > 0) {
        // Do not allow Text Input when there are modal dialogs displayed
        return;
    }
    
    m_nameBox->touchDownAction(m_nameBox, CCControlEventTouchDown);
    
}


