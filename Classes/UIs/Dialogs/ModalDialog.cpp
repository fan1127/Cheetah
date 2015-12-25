//
//  ModalDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#include "ModalDialog.h"
#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;
#include "../../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/BackendService.h"
extern BackendService *g_gameBackend;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Settings.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/UIHelper.h"
#include "../../DialogSettings.h"

ModalDialog::ModalDialog() :
        CCModalView(),
        m_block_sprite(NULL),
        m_background(NULL),
        m_frontBackground(NULL),
        m_title(NULL),
        m_help(NULL),
        m_header(NULL),
        m_closeButton(NULL),
        m_entity(NULL),
        m_loadingLayer(NULL)
{}

ModalDialog::~ModalDialog()
{
    CC_SAFE_DELETE(m_closeButton);
    CC_SAFE_DELETE(m_title);
    CC_SAFE_DELETE(m_help);
    CC_SAFE_DELETE(m_block_sprite);
    CC_SAFE_DELETE(m_frontBackground);
    CC_SAFE_DELETE(m_background);
    CC_SAFE_DELETE(m_loadingLayer);
}

ModalDialog *ModalDialog::create(CCModalController *parentModalController_,cocos2d::CCSize dialogSize) {
    ModalDialog *pRet = new ModalDialog();
    pRet->init(parentModalController_, dialogSize);
    pRet->autorelease();
    return pRet;
}

bool ModalDialog::init(CCModalController *parentModalController_,cocos2d::CCSize dialogSize) {

    if (!CCModalView::init(parentModalController_)) {
        return false;
    }

    m_closeButtonEnabled = true;

    m_background = UIHelper::getScalableSprite(UIHelper::ScreenForSlice);
    CCLog_("m_background.w:%f",m_background->getContentSize().width);
    CCLog_("m_background.h:%f",m_background->getContentSize().height);
    CCLog_("sizeMultiplier:%f",sizeMultiplier);
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

    m_closeButton = CCMenuHelper::createMenu(UIHelper::getSprite(UIHelper::NSCloseButton), this, menu_selector(ModalDialog::onClose));
    m_closeButton->setPosition(ccp(m_background->getContentSize().width*0.5f- m_closeButton->getContentSize().width  , m_background->getContentSize().height*0.5f- m_closeButton->getContentSize().height*0.6));
    m_closeButton->setAnchorPoint(ccp(0.5f, 0.5f));

    switch (targetDevice) {
        case TD_IPAD_RETINA:    {
            m_closeButton->setPosition(ccp(m_background->getContentSize().width*0.5f- m_closeButton->getContentSize().width  , m_background->getContentSize().height*0.5f- m_closeButton->getContentSize().height*0.55f)*sizeMultiplier);
        }  break;
        default:
            break;
    }
    this->addChild(m_closeButton);

    return true;
}

void ModalDialog::onClose(CCObject *sender_)
{
    if (!m_closeButtonEnabled) return;
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    if (this->numberOfRunningActions() > 0) return;
    this->m_modalController->popView();
}

void ModalDialog::onButton(CCObject *sender_)
{
    if (g_playerState->player.balance.gold < 3)
    {
//       SceneVille *_scene = (SceneVille*)g_currentScene;
       return;
    }

    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    m_modalController->closeAll();
}

void ModalDialog::addTitle(const char *icon_filename_)
{
    m_title = CCLabelHelper::createBMFont(icon_filename_, StyleDialogTitle);

    m_header = UIHelper::getScalableSprite(UIHelper::Header);
    m_header->setPositionY(m_closeButton->getPositionY() + 30*sizeMultiplier);

    addChild(m_header);
    m_header->addChild(m_title);

    setTitle(icon_filename_);
}

void ModalDialog::addHelpButton(const char* noteId, void (CCObject::*SEL_MenuHandler)(CCObject*))
{
    if (!m_help)
    {
        if (g_gameContent->Word[noteId] != "")
        {
            m_help = CCMenuHelper::createMenu(UIHelper::getSpritePath(UIHelper::NSHelpButton), this, SEL_MenuHandler);
            m_help->setPosition(ccp(getContentSize().width*0.5f - kDialogPadding*sizeMultiplier*2.5 - m_help->getContentSize().width*0.5f,
                                   - getContentSize().height*0.5f + kDialogPadding*sizeMultiplier*2 + m_help->getContentSize().height*0.5f));
            addChild(m_help);
        }
    }
}

void ModalDialog::setTitle(const char* name)
{
    CCAssert(m_title != NULL, "Call AddTitle first!");
    CCAssert(m_header != NULL, "Call AddTitle first!");

    m_title->setString(name);
    m_header->setContentSize(CCSizeMake(m_title->getContentSize().width + TITLE_HEADER_OFFSET, m_header->getContentSize().height*1.0f));
    m_title->setPosition(ccp(m_header->getContentSize().width*0.5f, m_header->getContentSize().height*0.5f));
}

void ModalDialog::moveBy(float x, float y)
{
    m_background->setPosition(CCPointMake(m_background->getPositionX() + x, m_background->getPositionY() - y));
    m_closeButton->setPosition(CCPointMake(m_closeButton->getPositionX() + x, m_closeButton->getPositionY() - y));

    if (m_title)
    {
        m_title->setPosition(CCPointMake(m_title->getPositionX() + x, m_title->getPositionY() - y));
    }
}

void ModalDialog::createContent(float leftMargin, float rightMargin, float topMargin, float bottomMargin)
{
    createContent(leftMargin, rightMargin, topMargin, bottomMargin, 0.0f, 0.0f);
}

void ModalDialog::createContent(float leftMargin, float rightMargin, float topMargin, float bottomMargin, float contentXShift, float contentYShift)
{
    if (!m_frontBackground)
    {
        m_frontBackground = new CCNode();
        addChild(m_frontBackground);
    }
    
    if (leftMargin >= 0.0f && leftMargin <= 1.0f
        &&
        rightMargin >= 0.0f && rightMargin <= 1.0f
        &&
        topMargin >= 0.0f && topMargin <= 1.0f
        &&
        bottomMargin >= 0.0f && bottomMargin <= 1.0f) //method 1, set margins by percentage. All margin values have to be 0 to 1
    {
        m_frontBackground->setContentSize(CCSizeMake(m_background->getContentSize().width *  (1.0f - (leftMargin+rightMargin)),
                                                     m_background->getContentSize().height * (1.0f - (topMargin + bottomMargin))));
        
        m_frontBackground->setPosition(CCPointMake(-m_background->getContentSize().width * 0.5f
                                                   + m_frontBackground->getContentSize().width * 0.5f
                                                   + m_background->getContentSize().width * leftMargin,
                                                   -m_background->getContentSize().height * 0.5f
                                                   + m_frontBackground->getContentSize().height * 0.5f
                                                   + m_background->getContentSize().height * bottomMargin));
    } else {
        m_frontBackground->setContentSize(CCSizeMake(m_background->getContentSize().width - leftMargin - rightMargin,
                                                     m_background->getContentSize().height - topMargin - bottomMargin));
        m_frontBackground->setPosition(ccp(-m_background->getContentSize().width * 0.5f
                                           + m_frontBackground->getContentSize().width * 0.5f
                                           + leftMargin,
                                           -m_background->getContentSize().height * 0.5f
                                           + m_frontBackground->getContentSize().height * 0.5f
                                           + bottomMargin));
    }
}

void ModalDialog::createFrontBackground(UIHelper::ScalableSprites sprite)
{
    auto newBcg = UIHelper::getScalableSprite(sprite);
    CCNode* parent = this;

    if (m_frontBackground)
    {
        parent = m_frontBackground->getParent();
    }

    m_frontBackground->removeFromParent();
    CC_SAFE_DELETE(m_frontBackground);

    m_frontBackground = newBcg;
    parent->addChild(newBcg);
}

void ModalDialog::showLoading()
{
    CCAssert(!m_loadingLayer, "Loading layer should be NULL!");

    m_loadingLayer = LoadingLayer::create();
    m_loadingLayer->showInDialog(this);
}

void ModalDialog::hideLoading()
{
    if (m_loadingLayer)
    {
        m_loadingLayer->hide();
        m_loadingLayer = NULL;
    }
}
