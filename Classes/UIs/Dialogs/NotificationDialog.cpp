//
//  NotificationDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#include "NotificationDialog.h"
#include "../../Settings.h"
#include "../../DialogSettings.h"

#include "../../Services/GameAudioService.h"
#include "../../Scenes/SceneVille.h"
#include "../../Services/GameContentService.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/UIHelper.h"
#include "../../Helpers/CCLabelUpdater.h"
#include "../../Helpers/UIDisplayHelper.h"

#include "../../AppMacros.h"
#include "../../Helpers/PushNotifications/PushNotifications.h"

extern float sizeMultiplier;
extern GameAudioService *g_gameAudio;
extern CCScene *g_currentScene;
extern GameContentService *g_gameContent;
extern PlayerState* g_playerState;
extern TargetDevice targetDevice;

NotificationDialog::NotificationDialog() :
        CCModalView(),
        m_block_sprite(NULL),
        m_background(NULL),
        m_title(NULL),
        m_description(NULL),
        m_buttonNo(NULL),
        m_buttonYes(NULL)
{}

NotificationDialog::~NotificationDialog()
{
    CC_SAFE_DELETE(m_block_sprite);
    CC_SAFE_DELETE(m_background);
    CC_SAFE_DELETE(m_title);
    CC_SAFE_DELETE(m_description);
    CC_SAFE_DELETE(m_buttonNo);
    CC_SAFE_DELETE(m_buttonYes);
}

NotificationDialog* NotificationDialog::create(CCModalController* parent)
{
    return NotificationDialog::create(parent, kDialogNotificationSize);
}

NotificationDialog *NotificationDialog::create(CCModalController* parent, cocos2d::CCSize dialogSize)
{
    NotificationDialog *pRet = new NotificationDialog();
    pRet->init(parent, dialogSize);
    pRet->autorelease();
    return pRet;
}

bool NotificationDialog::init(CCModalController* parent, cocos2d::CCSize dialogSize)
{
    if (!CCModalView::init(parent))
    {
        return false;
    }

    m_background = UIHelper::getScalableSprite(UIHelper::AchievementBGSolid);
    m_background->setOpacity(255);
    m_background->setContentSize(dialogSize*sizeMultiplier);
    addChild(m_background);
    setAnchorPoint(ccp(0, 0));
    setContentSize(m_background->getContentSize());

    CCSprite *_tmp = blankSpriteWithSize(m_background->getContentSize());
    m_block_sprite = new CCPhatTouchableSprite();
    m_block_sprite->autorelease();
    m_block_sprite->initWithTexture(_tmp->getTexture());
    m_block_sprite->setColor(ccBLUE);
    m_block_sprite->setVisible(false);
    addChild(m_block_sprite);
    _tmp->removeFromParentAndCleanup(true);

//    createTitle("All your animals are sleeping");
//    createDescription("Would you like to be alerted when your pet will awake?");
//    createButtons();

    parent->setHitBGToCloseAllEnabled(false);

    return true;
}

cocos2d::CCMenu* NotificationDialog::createButton(bool yesButton, const char* title, void (CCObject::*SEL_MenuHandler)(CCObject*))
{
    CCSprite *btnSprite = NULL;

    if (yesButton)
    {
        btnSprite = UIHelper::getSprite(UIHelper::NotificationYes);
    }
    else
    {
        btnSprite = UIHelper::getSprite(UIHelper::NotificationNo);
    }

    auto label = CCLabelHelper::createBMFont(title, 60, true);
    btnSprite->addChild(label);

    label->setPosition(btnSprite->getContentSize()*0.5f);

    auto menuButton = CCMenuItemScaleSprite::create(btnSprite, this, SEL_MenuHandler);
    auto menu = CCMenu::create(menuButton, NULL);
    menu->setContentSize(menuButton->getContentSize());
    menu->setAnchorPoint(ccp(0.5f, 0.5f));

    return menu;
}

void NotificationDialog::createButtons(const char* yesTitle, const char* noTitle)
{
    m_buttonYes = createButton(true, yesTitle, menu_selector(NotificationDialog::onButtonYes));
    m_buttonNo = createButton(false, noTitle, menu_selector(NotificationDialog::onButtonNo));

    UIHelper::setTwoNodesToMiddlePosition(m_buttonNo, m_buttonYes, 20*sizeMultiplier, ccp(0, -m_background->getContentSize().height*0.5f +
            m_buttonNo->getContentSize().height*0.5f + 20*sizeMultiplier));

    addChild(m_buttonYes);
    addChild(m_buttonNo);
}

void NotificationDialog::createOkButton(const char* title)
{
    CCAssert(!m_buttonYes && !m_buttonNo, "Buttons already exits!");
    m_buttonYes = createButton(true, title, menu_selector(NotificationDialog::onButtonYes));
    m_buttonYes->setPosition(ccp(0, -m_background->getContentSize().height*0.5f + m_buttonYes->getContentSize().height*0.5f + 20*sizeMultiplier));
    addChild(m_buttonYes);
}

void NotificationDialog::onButtonYes(CCObject *sender)
{
    CCLog_("NotificationDialog::onButtonYes");

    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    if (this->numberOfRunningActions() > 0) return;
    this->m_modalController->popView();
}

void NotificationDialog::onButtonNo(CCObject *sender)
{
    CCLog_("NotificationDialog::onButtonNo");

    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    if (this->numberOfRunningActions() > 0) return;
    this->m_modalController->popView();
}

void NotificationDialog::createDescription(const char *description)
{
    CCAssert(m_description == NULL, "Description is already created!");
    m_description = CCLabelHelper::createBMFont(description, 50.0f, false, kTextColorBrown);
    m_description->setAlignment(kCCTextAlignmentCenter);
    m_description->setAnchorPoint(ccp(0.5f, 1.0f));
    m_description->setWidth(DESCRIPTION_TEXT_NOTIFY_MAX_LENGTH);
    addChild(m_description);
    setDescription(description);
}

void NotificationDialog::setDescription(const char *desc)
{
    CCAssert(m_description != NULL, "Call CreateDescription first!");
    m_description->setString(desc);
    m_description->setPositionY(m_title->getPositionY() - m_title->getContentSize().height*1.4f);
}

void NotificationDialog::createTitle(const char *name)
{
    m_title = CCLabelHelper::createBMFont(name, StyleDialogTitle);
    m_title->setAnchorPoint(ccp(0.5f, 1.0f));
    addChild(m_title);
    setTitle(name);
}

void NotificationDialog::setTitle(const char* name)
{
    CCAssert(m_title != NULL, "Call CreateTitle first!");
    m_title->setString(name);
    m_title->setPositionY(m_background->getContentSize().height*0.5f - m_title->getContentSize().height*0.5f);
}
