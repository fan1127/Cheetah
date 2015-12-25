//
//  ScrollDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//
#include "time.h"
#include "ScrollDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;

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
extern PlayerState* g_opponentState;

#include "../../Scenes/SceneWorldMap.h"
extern CCScene* g_currentScene;

#include "../../Settings.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../VOs/HistoryItemVO.h"
#include "../../Helpers/UIHelper.h"

ScrollDialog::ScrollDialog() :
    ModalDialog(),
    m_scrollView(NULL),
    m_clippingLayer(NULL),
    m_sprite(NULL),
    m_tag(0)
{}

ScrollDialog::~ScrollDialog()
{
    CC_SAFE_DELETE(m_scrollView);
    CC_SAFE_DELETE(m_clippingLayer);
    CC_SAFE_DELETE(m_sprite);
}

ScrollDialog *ScrollDialog::create(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    ScrollDialog *pRet = new ScrollDialog();
    pRet->init(parentModalController_, dialogSize);
    pRet->autorelease();
    return pRet;
}

void ScrollDialog::createScrollContent(BounceOptions bounceOptions, float leftMargin, float rightMargin, float topMargin, float bottomMargin)
{
    createScrollContent(bounceOptions, leftMargin, rightMargin, topMargin, bottomMargin, 0.0f, 0.0f);
}

void ScrollDialog::createScrollContent(BounceOptions bounceOptions, float leftMargin, float rightMargin, float topMargin, float bottomMargin, float contentXShift, float contentYShift)
{
    ModalDialog::createContent(leftMargin, rightMargin, topMargin, bottomMargin, contentXShift, contentYShift);

    m_clippingLayer->setContentSize(CCSizeMake(m_frontBackground->getContentSize().width
                                               - contentXShift * m_background->getContentSize().height,
                                               m_frontBackground->getContentSize().height));// - contentYShift * m_background->getContentSize().width));

    m_clippingLayer->setPosition(CCPointMake(-m_frontBackground->getContentSize().width * 0.5f
                                             + m_frontBackground->getPositionX()
                                             + contentXShift * m_background->getContentSize().height,
                                             -m_frontBackground->getContentSize().height * 0.5f
                                             + m_frontBackground->getPositionY()
                                             - contentYShift * m_background->getContentSize().width));

    switch (bounceOptions)
    {
        case BounceHorizontal:
            m_scrollView->setAlwaysBounceHorizontal(true);
            break;

        case BounceVertical:
            m_scrollView->setAlwaysBounceVertical(true);
            break;
    }

    m_scrollView->setContentSize(m_clippingLayer->getContentSize());
}

bool ScrollDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ModalDialog::init(parentModalController_,dialogSize))
    {
        return false;
    }

    if (!m_frontBackground)
    {
        m_frontBackground = new CCNode();
        addChild(m_frontBackground);
    }

    m_clippingLayer = CCClippingLayer::create();
    m_clippingLayer->setAnchorPoint(ccp(0, 0));
    m_clippingLayer->preferenceRootParent = m_frontBackground;
    this->addChild(m_clippingLayer, 4);

    m_scrollView = CCNikScrollView::create();
    m_scrollView->setBounce(true);
    m_scrollView->setTouchMode(kCCTouchesOneByOne);
    m_scrollView->setDelegate(this);
    m_clippingLayer->addChild(m_scrollView);

    return true;
}

void ScrollDialog::replaceControlsParent(CCNode *parent)
{
    m_background->removeFromParent();
    parent->addChild(m_background);
    m_frontBackground->removeFromParent();
    parent->addChild(m_frontBackground);
    m_block_sprite->removeFromParent();
    parent->addChild(m_block_sprite);
    m_closeButton->removeFromParent();
    parent->addChild(m_closeButton);
    m_clippingLayer->removeFromParent();
    parent->addChild(m_clippingLayer);
}

void ScrollDialog::moveBy(float x, float y)
{
    ModalDialog::moveBy(x, y);

    m_frontBackground->setPosition(CCPointMake(m_frontBackground->getPositionX() + x, m_frontBackground->getPositionY() - y));
    m_block_sprite->setPosition(CCPointMake(m_block_sprite->getPositionX() + x, m_block_sprite->getPositionY() - y));
    m_clippingLayer->setPosition(CCPointMake(m_clippingLayer->getPositionX() + x, m_clippingLayer->getPositionY() - y));
}

void ScrollDialog::updateContent()
{
}

bool ScrollDialog::compose()
{
      return true;
}

CCNikScrollViewButton* ScrollDialog::createButton(char const *buttonStyle_, int tag_)
{
    CCNikScrollViewButton *_button = CCNikScrollViewButton::create(CCSprite::createWithSpriteFrameName(buttonStyle_), NULL);
    _button->setDelegate(m_scrollView);
    _button->setAnchorPoint(ccp(0.5,0.5));
    _button->setTag(m_tag+tag_);
    return _button;
}

void ScrollDialog::addTextToButton(char const *buttonText_, CCNode *button_)
{
    CCLabelBMFont *_text = CCLabelHelper::createBMFont(buttonText_, kFontSizeMedium, true);
    _text->setPosition(ccp(button_->getContentSize().width * 0.5, button_->getContentSize().height * 0.57));
    _text->setAnchorPoint(ccp(0.5, 0.5));
    button_->addChild(_text);
}

CCNikScrollViewButton* ScrollDialog::addButtonScroll(const CCPoint &newPosition_, int tag_, char const *buttonStyle_, char const *buttonText_, float scale_) {
    
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(m_tag);
    
    CCNikScrollViewButton *_Button = createButton(buttonStyle_, tag_);
    _Button->setPosition(ccp(tempFrame->getContentSize().width*newPosition_.x,tempFrame->getPositionY()+tempFrame->getContentSize().height*newPosition_.y));
    _Button->setScale(scale_);
    m_scrollView->addChild(_Button);
    
    if(buttonText_ != NULL) {
        addTextToButton(buttonText_, _Button);
    }
    
    return _Button;
    
}

CCLabelBMFont*ScrollDialog::addLabelScroll(const CCPoint &newPosition_, int tag_, char const *buttonStyle_,Align align_, float scale_) {
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(m_tag);

    CCLabelBMFont *_label = CCLabelHelper::createBMFont(buttonStyle_, kFontSizeMedium, true);

    if (_label && tempFrame)
    {
        _label->setPosition(ccp(tempFrame->getContentSize().width*newPosition_.x,tempFrame->getPositionY()+tempFrame->getContentSize().height*newPosition_.y));

        if ( align_ == RIGHT )
            _label->setAnchorPoint(ccp(1.0,0.5));
        else if (align_==LEFT)
            _label->setAnchorPoint(ccp(0.0,0.5));
        else
            _label->setAnchorPoint(ccp(0.5,0.5));
        m_scrollView->addChild(_label);
        return _label;
    }

    return NULL;
}

CCLabelBMFont* ScrollDialog::addLabel(const CCPoint& pos, int tag, const char* text, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color, Align align)
{
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(m_tag);
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(text, fontSize, hasShadow, color);

    if (_label && tempFrame)
    {
        _label->setPosition(ccp(tempFrame->getContentSize().width*pos.x,tempFrame->getPositionY()+tempFrame->getContentSize().height*pos.y));

        if (align == RIGHT )
            _label->setAnchorPoint(ccp(1.0,0.5));
        else if (align == LEFT)
            _label->setAnchorPoint(ccp(0.0,0.5));
        else
            _label->setAnchorPoint(ccp(0.5,0.5));

        m_scrollView->addChild(_label);

        return _label;
    }

    return NULL;
}

CCLabelBMFont*ScrollDialog::addLabelScrollManual(const CCPoint &newPosition_, int tag_, char const *buttonStyle_,Align align_, float scale_)
{
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(buttonStyle_, kFontSizeMedium, true);

    _label->setPosition(ccp(newPosition_.x,newPosition_.y));

    if ( align_ == RIGHT ) {
        
        _label->setAnchorPoint(ccp(1.0,0.5));
        _label->setAlignment(kCCTextAlignmentRight);
    }
    else if (align_==LEFT) {
        
        _label->setAnchorPoint(ccp(0.0,0.5));
        _label->setAlignment(kCCTextAlignmentLeft);
    }
    else {
        
        _label->setAnchorPoint(ccp(0.5,0.5));
        _label->setAlignment(kCCTextAlignmentCenter);
    }

    m_scrollView->addChild(_label);
    return _label;
}

CCSprite*ScrollDialog::addSpriteScroll(const CCPoint &newPosition_, int tag_, char const *buttonStyle_, float scale_) {
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(tag_);

    m_sprite = CCSprite::createWithSpriteFrameName(buttonStyle_);

    if (m_sprite && tempFrame)
    {
        m_sprite->setPosition(ccp(tempFrame->getContentSize().width*newPosition_.x, tempFrame->getPositionY()+tempFrame->getContentSize().height*newPosition_.y));
        m_sprite->setScale(scale_);
        m_scrollView->addChild(m_sprite);
        return m_sprite;
    }

    return NULL;
}

CCSprite*ScrollDialog::addSpriteScrollManual(const CCPoint &newPosition_, int tag_, char const *buttonStyle_, float scale_) {
    m_sprite = CCSprite::createWithSpriteFrameName(buttonStyle_);
    m_sprite->setPosition(newPosition_);
    m_sprite->setScale(scale_);
    m_scrollView->addChild(m_sprite);
    return m_sprite;
}
