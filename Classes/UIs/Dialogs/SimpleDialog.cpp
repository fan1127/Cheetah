//
//  SimpleDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#include "SimpleDialog.h"
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

#include "AppMacros.h"

extern float sizeMultiplier;
extern GameAudioService *g_gameAudio;
extern CCScene *g_currentScene;
extern GameContentService *g_gameContent;
extern PlayerState* g_playerState;
extern TargetDevice targetDevice;

SimpleDialog::SimpleDialog() :
        CCModalView(),
        m_block_sprite(NULL),
        m_background(NULL),
        m_title(NULL),
        m_description(NULL),
        m_iconLine(NULL),
        m_closeButton(NULL)
{}

SimpleDialog::~SimpleDialog()
{
    CC_SAFE_DELETE(m_block_sprite);
    CC_SAFE_DELETE(m_background);
    CC_SAFE_DELETE(m_title);
    CC_SAFE_DELETE(m_description);
    CC_SAFE_DELETE(m_iconLine);
    CC_SAFE_DELETE(m_closeButton);
}

SimpleDialog* SimpleDialog::create(CCModalController* parent)
{
    return SimpleDialog::create(parent, kDialogSimpleDefault);
}

SimpleDialog *SimpleDialog::create(CCModalController* parent, cocos2d::CCSize dialogSize)
{
    SimpleDialog *pRet = new SimpleDialog();
    pRet->init(parent, dialogSize);
    pRet->autorelease();
    return pRet;
}

bool SimpleDialog::init(CCModalController* parent, cocos2d::CCSize dialogSize)
{
    if (!CCModalView::init(parent))
    {
        return false;
    }

    m_background = UIHelper::getScalableSprite(UIHelper::PaperBg);
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

    m_closeButton = CCMenuHelper::createMenu(UIHelper::getSprite(UIHelper::NSCloseButton), this, menu_selector(SimpleDialog::onClose));
    m_closeButton->setPosition(ccp(m_background->getContentSize().width*0.5f- m_closeButton->getContentSize().width*0.3f, m_background->getContentSize().height*0.5f - m_closeButton->getContentSize().height*0.3));
    m_closeButton->setAnchorPoint(ccp(0.5f, 0.5f));

    this->addChild(m_closeButton);

    return true;
}

void SimpleDialog::autoResize(float minWidth, float minHeight)
{
    auto maxWidth = 0.0f;
    auto maxHeight = 0.0f;

    if (m_title)
    {
        maxWidth = m_title->getContentSize().width;
    }

    if (m_description)
    {
        maxWidth = MAX(maxWidth, m_description->getContentSize().width);
    }

//DIALOG_SIMPLE_MARGIN_BOTTOM

    maxWidth = MAX(maxWidth + DIALOG_SIMPLE_MARGIN_LEFT * 2.0f, minWidth);

    auto btnPosY = FLT_MAX;

    for (auto &btn : m_buttons)
    {
        btnPosY = MIN(btnPosY, btn->getPositionY() - btn->getContentSize().height);
    }

    maxHeight = m_background->getPositionY() + m_background->getContentSize().height*0.5f - btnPosY;
    maxHeight = MAX(maxHeight + DIALOG_SIMPLE_MARGIN_BOTTOM, minWidth);

    m_background->setContentSize(CCSizeMake(maxWidth, maxHeight));
    setContentSize(m_background->getContentSize());

    m_block_sprite->removeFromParent();
    CC_SAFE_DELETE(m_block_sprite);

    CCSprite *_tmp = blankSpriteWithSize(m_background->getContentSize());
    m_block_sprite = new CCPhatTouchableSprite();
    m_block_sprite->autorelease();
    m_block_sprite->initWithTexture(_tmp->getTexture());
    m_block_sprite->setColor(ccBLUE);
    m_block_sprite->setVisible(false);
    addChild(m_block_sprite);
    _tmp->removeFromParentAndCleanup(true);

    m_closeButton->setPosition(ccp(m_background->getContentSize().width * 0.5f - m_closeButton->getContentSize().width * 0.3f, m_background->getContentSize().height * 0.5f - m_closeButton->getContentSize().height * 0.3));
    m_closeButton->setAnchorPoint(ccp(0.5f, 0.5f));

    m_title->setPositionY(m_closeButton->getPositionY() + m_title->getContentSize().height*0.5f - DIALOG_SIMPLE_TITLE_OFFSET);

    auto posY = m_title->getPositionY() -  m_title->getContentSize().height - DIALOG_SIMPLE_BUTTON_OFFSET;

    if (m_description)
    {
        m_description->setPositionY(m_title->getPositionY() - m_title->getContentSize().height - DIALOG_SIMPLE_DESCRIPTION_OFFSET);
        posY = m_description->getPositionY() - m_description->getContentSize().height - DIALOG_SIMPLE_BUTTON_OFFSET;
    }

    if (m_iconLine)
    {
        m_iconLine->setPositionY(m_description->getPositionY() - m_description->getContentSize().height - DIALOG_SIMPLE_TITLE_OFFSET);
        posY = m_iconLine->getPositionY() - m_iconLine->getContentSize().height - DIALOG_SIMPLE_BUTTON_OFFSET;
    }

    for (auto& btn : m_buttons)
    {
        btn->setPositionY(posY);
    }
}

void SimpleDialog::createIconLine(const char* text, ButtonIcon icon)
{
    m_iconLine = new CCNode();

    CCSprite *iconSprite = NULL;
    
    switch (icon)
    {
        case SimpleDialog::Gem:
        {
            iconSprite = UIHelper::getSprite(UIHelper::NSClaimGemBlue);
        }
            break;
            
        case SimpleDialog::Coin:
        {
            iconSprite = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);
        }
            break;
            
        case SimpleDialog::Food:
        {
            iconSprite = UIHelper::getSprite(UIHelper::RaceElementsFoodIcon);
        }
            break;
        case SimpleDialog::None:
        {
            iconSprite = UIHelper::getSprite(UIHelper::NSClaimGemBlue);
        }
            break;
        default:
            break;
    }

    CCAssert(iconSprite != NULL, "SimpleDialog::Icon is not valid!");

    CCLabelBMFont* label = CCLabelHelper::createBMFont(text, StyleButtonCoinAndGems);
    m_iconLine->addChild(label);
    m_iconLine->addChild(iconSprite);
    UIHelper::setTwoNodesToMiddlePosition(iconSprite, label, 10*sizeMultiplier, m_iconLine->getContentSize()*0.5f);
    m_iconLine->setAnchorPoint(ccp(0.5f, 1.0f));
    m_iconLine->setPositionY(m_description->getPositionY() - m_description->getContentSize().height - DIALOG_SIMPLE_TITLE_OFFSET);
    addChild(m_iconLine);
}

void SimpleDialog::createButton(CCLabelUpdater* labelUpdater, ButtonAllign align, ButtonIcon icon)
{
    CCSprite *btnSprite = NULL;
    CCSprite *iconSprite = NULL;

    switch (icon)
    {
        case SimpleDialog::Gem:
        {
            btnSprite = UIHelper::getSprite(UIHelper::UpgradeInstantBuyButton);
            iconSprite = UIHelper::getSprite(UIHelper::NSClaimGemBlue);
        }
            break;

        case SimpleDialog::Coin:
        {
            btnSprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);
            iconSprite = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);
        }
            break;

        case SimpleDialog::Food:
        {
            btnSprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);
            iconSprite = UIHelper::getSprite(UIHelper::RaceElementsFoodIcon);
        }
            break;
        case SimpleDialog::None:
        {
            btnSprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);
            iconSprite = UIHelper::getSprite(UIHelper::NSClaimGemBlue);
        }
            break;
        default:
            break;
    }

    CCAssert(btnSprite != NULL, "SimpleDialog::Button is not valid!");

    auto size = 0.0f;

    size += labelUpdater->getLabel()->getContentSize().width;
    labelUpdater->getLabel()->setPosition(ccp(btnSprite->getContentSize().width*0.5f, btnSprite->getContentSize().height*0.5f));
    btnSprite->addChild(labelUpdater);

    if (iconSprite)
    {
        iconSprite->setPosition(ccp(btnSprite->getContentSize().width * 0.5f - iconSprite->getContentSize().width, btnSprite->getContentSize().height * 0.5f));
        iconSprite->setAnchorPoint(ccp(1.0f, 0.5f));
        labelUpdater->getLabel()->setAnchorPoint(ccp(0.0f, 0.5f));
        size += iconSprite->getContentSize().width;
        btnSprite->addChild(iconSprite);

        auto offset = 5*sizeMultiplier;

        iconSprite->setPositionX(btnSprite->getContentSize().width*0.5f - offset);
        labelUpdater->getLabel()->setPositionX(btnSprite->getContentSize().width*0.5f + offset);

        UIHelper::setTwoNodesToMiddlePosition(iconSprite, labelUpdater->getLabel(), 10*sizeMultiplier, btnSprite->getContentSize()*0.5f);
    }

    auto menuButton = CCMenuItemScaleSprite::create(btnSprite, this, menu_selector(SimpleDialog::onButton));
    auto menu = CCMenu::create(menuButton, NULL);
    menu->setContentSize(menuButton->getContentSize());
    menu->setAnchorPoint(ccp(0.5f, 1.0f));

    auto posY = m_title->getPositionY() -  m_title->getContentSize().height - DIALOG_SIMPLE_BUTTON_OFFSET;

    if (m_description)
    {
        m_description->setPositionY(m_title->getPositionY() - m_title->getContentSize().height - DIALOG_SIMPLE_DESCRIPTION_OFFSET);
        posY = m_description->getPositionY() - m_description->getContentSize().height - DIALOG_SIMPLE_BUTTON_OFFSET;
    }

    if (m_iconLine)
    {
        m_iconLine->setPositionY(m_description->getPositionY() - m_description->getContentSize().height - DIALOG_SIMPLE_TITLE_OFFSET);
        posY = m_iconLine->getPositionY() - m_iconLine->getContentSize().height - DIALOG_SIMPLE_BUTTON_OFFSET;
    }

    menu->setPosition(ccp(0.0f, posY));
    addChild(menu);
    m_buttons.push_back(menu);
}

void SimpleDialog::createButton(const char* title, ButtonAllign align, ButtonIcon icon)
{
    CCSprite *btnSprite = NULL;
    CCSprite *iconSprite = NULL;

    switch (icon)
    {
        case SimpleDialog::Gem:
        {
            btnSprite = UIHelper::getSprite(UIHelper::UpgradeInstantBuyButton);
            iconSprite = UIHelper::getSprite(UIHelper::NSClaimGemBlue);
        }
            break;

        case SimpleDialog::Coin:
        {
            btnSprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);
            iconSprite = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);
        }
            break;

        case SimpleDialog::Food:
        {
            btnSprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);
            iconSprite = UIHelper::getSprite(UIHelper::RaceElementsFoodIcon);
        }
            break;
        case SimpleDialog::None:
        {
            btnSprite = UIHelper::getSprite(UIHelper::NotificationYes);
        }
            break;
        default:
            break;
    }

    CCAssert(btnSprite != NULL, "SimpleDialog::Button is not valid!");

    auto size = 0.0f;

    auto label = CCLabelHelper::createBMFont(title, StyleButtonCoinAndGems);
    size += label->getContentSize().width;
    label->setPosition(ccp(btnSprite->getContentSize().width*0.5f,
                           btnSprite->getContentSize().height*0.5f
                           + 6.0f*sizeMultiplier));
    btnSprite->addChild(label);

    if (iconSprite)
    {
        iconSprite->setPosition(ccp(btnSprite->getContentSize().width * 0.5f - iconSprite->getContentSize().width, btnSprite->getContentSize().height * 0.5f));
        iconSprite->setAnchorPoint(ccp(1.0f, 0.5f));
        label->setAnchorPoint(ccp(0.0f, 0.5f));
        size += iconSprite->getContentSize().width;
        btnSprite->addChild(iconSprite);

        auto offset = 5*sizeMultiplier;

        iconSprite->setPositionX(btnSprite->getContentSize().width*0.5f - offset);
        label->setPositionX(btnSprite->getContentSize().width*0.5f + offset);

        UIHelper::setTwoNodesToMiddlePosition(iconSprite, label, 10*sizeMultiplier, btnSprite->getContentSize()*0.5f);
    }

    auto menuButton = CCMenuItemScaleSprite::create(btnSprite, this, menu_selector(SimpleDialog::onButton));
    auto menu = CCMenu::create(menuButton, NULL);
    menu->setContentSize(menuButton->getContentSize());
    menu->setAnchorPoint(ccp(0.5f, 1.0f));

    auto posY = m_title->getPositionY() -  m_title->getContentSize().height - DIALOG_SIMPLE_BUTTON_OFFSET;

    if (m_description)
    {
        m_description->setPositionY(m_title->getPositionY() - m_title->getContentSize().height - DIALOG_SIMPLE_DESCRIPTION_OFFSET);
        posY = m_description->getPositionY() - m_description->getContentSize().height - DIALOG_SIMPLE_BUTTON_OFFSET;
    }

    if (m_iconLine)
    {
        m_iconLine->setPositionY(m_description->getPositionY() - m_description->getContentSize().height - DIALOG_SIMPLE_TITLE_OFFSET);
        posY = m_iconLine->getPositionY() - m_iconLine->getContentSize().height - DIALOG_SIMPLE_BUTTON_OFFSET;
    }

    menu->setPosition(ccp(0.0f, posY));
    addChild(menu);
    m_buttons.push_back(menu);
}

void SimpleDialog::onClose(CCObject *sender)
{
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    if (this->numberOfRunningActions() > 0) return;
    this->m_modalController->popView();
}

void SimpleDialog::onButton(CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    m_modalController->closeAll();
}

void SimpleDialog::createDescription(const char *description)
{
    CCAssert(m_description == NULL, "Description is already created!");
    m_description = CCLabelHelper::createBMFont(description, 40.0f, false, kTextColorBrown);
    m_description->setAlignment(kCCTextAlignmentCenter);
    m_description->setAnchorPoint(ccp(0.5f, 1.0f));
    m_description->setWidth(DESCRIPTION_TEXT_MAX_LENGTH);
    addChild(m_description);
    setDescription(description);
}

void SimpleDialog::setDescription(const char *desc)
{
    CCAssert(m_description != NULL, "Call CreateDescription first!");
    m_description->setString(desc);
    m_description->setPositionY(m_title->getPositionY() - m_title->getContentSize().height - DIALOG_SIMPLE_DESCRIPTION_OFFSET);
}

void SimpleDialog::createTitle(const char *name)
{
    m_title = CCLabelHelper::createBMFont(name, StyleDialogTitle);
    m_title->setAnchorPoint(ccp(0.5f, 1.0f));
    addChild(m_title);
    setTitle(name);
}

void SimpleDialog::setTitle(const char* name)
{
    CCAssert(m_title != NULL, "Call CreateTitle first!");
    m_title->setString(name);
    m_title->setPositionY(m_closeButton->getPositionY() + m_title->getContentSize().height*0.5f - DIALOG_SIMPLE_TITLE_OFFSET);
}

void SimpleDialog::moveBy(float x, float y)
{
    m_background->setPosition(CCPointMake(m_background->getPositionX() + x, m_background->getPositionY() - y));
    m_closeButton->setPosition(CCPointMake(m_closeButton->getPositionX() + x, m_closeButton->getPositionY() - y));

    if (m_title)
    {
        m_title->setPosition(CCPointMake(m_title->getPositionX() + x, m_title->getPositionY() - y));
    }

    for (auto& btn : m_buttons)
    {
        btn->setPosition(CCPointMake(btn->getPositionX() + x, btn->getPositionY() - y));
    }
}
