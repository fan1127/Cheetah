//
//  ShopItemDecoration.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/28/56 BE.
//
//

#include "ShopItemDecoration.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include "../Settings.h"
//#include "../AppMacros.h"
//extern CCPoint screenFactor;
//extern TargetDevice targetDevice;

#include "../Helpers/UIDisplayHelper.h"
#include "../Helpers/CCSpriteHelper.h"
#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

//#include "../Entities/TutorialManager.h"
//extern TutorialManager *g_gameTutorial;

#include "../Extensions/CCPopOverDialog.h"
#include "../Helpers/CCStringHelper.h"
#include "../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;

ShopItemDecoration *ShopItemDecoration::createWithDecorationVO(GDDecorationVO vo_, cocos2d::CCSize size_)
{
    ShopItemDecoration *pRet = new ShopItemDecoration();
    if (!pRet->initWithDecorationVO(vo_, size_))
    {
        delete pRet;
        return NULL;
    }
    pRet->autorelease();
    return pRet;
}

bool ShopItemDecoration::initWithDecorationVO(GDDecorationVO vo_, cocos2d::CCSize size_)
{
    if (!CCNikScrollViewButton::init(NULL, NULL))
    {
        return false;
    }

//    bufferShortMsgDec = {0};

    this->setContentSize(size_);

    m_arrow = NULL;

    this->vo = vo_;

#define ITEM_MARGIN_W (30.0f*sizeMultiplier)
#define TEXT_MARGIN_W (20.0f*sizeMultiplier)
#define ITEM_MARGIN_H (40.0f*sizeMultiplier)

    CCLabelBMFont *_labelTitle = CCLabelHelper::createBMFont(retinaMultiplier, vo.title.c_str(), kFontSizeLarge, true);
    _labelTitle->setAnchorPoint(CCPoint(0.0f, 1.0f));
    _labelTitle->setPosition(ccp(ITEM_MARGIN_W,
            this->getContentSize().height - ITEM_MARGIN_H));

    CREATE_CHAR_BUFFER(buffer_, 16);
    snprintf(buffer_, buffer__size, "%d", vo.cost_coins);

    warning_msg = isAnyReasonCannotBuild(vo);

    _labelPrice = CCLabelHelper::createBMFont(bufferShortMsgDec, kFontSizeSmall, true);

    _bg = CCScale9Sprite::createWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::BuildDescriptionBg));
    _bg->setAnchorPoint(ccp(0.0f, 0.0f));
    _bg->setContentSize(CCSizeMake(300.0f*sizeMultiplier,
            this->getContentSize().height
                    -90.0f*sizeMultiplier));
    _bg->setPosition(ccp(30.0f*sizeMultiplier, 0.0f));

    decorationDesc = CCLabelHelper::createBMFont("", kFontSizeSmall, kTextColorLightBrown);
    decorationDesc->setWidth(_bg->getContentSize().width-40.0f*sizeMultiplier);
    decorationDesc->setAlignment(kCCTextAlignmentLeft);
    decorationDesc->setAnchorPoint(ccp(0.0f, 1.0f));
    decorationDesc->setPosition(ccp(_bg->getPositionX() + 20*sizeMultiplier,
            _bg->getPositionY() + _bg->getContentSize().height - 20*sizeMultiplier));

    _labelTitle->setPosition(ccp(_bg->getPositionX() + 20*sizeMultiplier,
            _bg->getPositionY() + _bg->getContentSize().height + 70*sizeMultiplier));

    
    m_spineDecorationPreview = g_sceneManager->sharedSkeletonData->getSkeleton(kDecorationSrcPath, vo.src.c_str());
    m_spineDecorationPreview->setAnchorPoint(ccp(0.5f, 0.5f));
    CCSize spaceSize = CCSizeMake(200.0f*sizeMultiplier,
                                  200.0f*sizeMultiplier);
    CCPoint spacePos = ccp(this->getContentSize().width
                           -140.0f*sizeMultiplier,
                           this->getContentSize().height*0.4f);
    CCPoint dynamicScale = CCPoint(spaceSize.width/vo.src_width,
                                   spaceSize.height/vo.src_height);
    float scale = MIN(dynamicScale.x, dynamicScale.y);
    m_spineDecorationPreview->setScale(scale);
    m_spineDecorationPreview->setPosition(spacePos);
    
    
    m_labelMax = CCLabelHelper::createBMFont("", kFontSizeSmall-5, kTextColorRed);

    _iconCoin = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);

    m_arrow = UIHelper::getSprite(UIHelper::BuildArrow);
    m_arrow->setPosition(ccp(this->getContentSize().width
            +20.0f*sizeMultiplier,
            m_spineDecorationPreview->getPositionY() - 20*sizeMultiplier));

    this->addChild(_bg);
    this->addChild(_iconCoin);
    this->addChild(m_spineDecorationPreview);
    this->addChild(decorationDesc);
    this->addChild(_labelTitle);
    this->addChild(m_labelMax);
    this->addChild(_labelPrice);
    this->addChild(m_arrow);

    updateState();

    return true;
}

CCPoint ShopItemDecoration::getPreviewDecorationPosition()
{
    if (m_spineDecorationPreview)
    {
        return m_spineDecorationPreview->getPosition();
    }

    return CCPointZero;
}

void ShopItemDecoration::updateLayoutForDynamicItems()
{
    auto textYOffset = _bg->getContentSize().height*0.5f - 30*sizeMultiplier;

    if (warning_msg != "")
    {
        _labelPrice->setAnchorPoint(ccp(0.5f, 0.5f));
        _labelPrice->setAlignment(kCCTextAlignmentCenter);
        _labelPrice->setPosition(m_spineDecorationPreview->getPositionX(), m_spineDecorationPreview->getPositionY() - textYOffset);
    }
    else
    {
        _labelPrice->setAnchorPoint(ccp(0.0f, 0.5f));
        _labelPrice->setAlignment(kCCTextAlignmentLeft);

        CREATE_CHAR_BUFFER(buffer_, 16);
        snprintf(buffer_, buffer__size, "%d", vo.cost_coins);
        _labelPrice->setString(buffer_);

        _iconCoin->setVisible(true);
        
        UIHelper::setTwoNodesToMiddlePosition(_iconCoin, _labelPrice, 10.0f*sizeMultiplier, ccp(m_spineDecorationPreview->getPositionX() - 20.0f*sizeMultiplier, m_spineDecorationPreview->getPositionY() - textYOffset));
    }

    m_labelMax->setAnchorPoint(ccp(0.0f, 0.5f));
    m_labelMax->setPosition(ccp(decorationDesc->getPositionX(), _labelPrice->getPositionY()));
}

void ShopItemDecoration::updateState()
{
    CREATE_CHAR_BUFFER(_buffer, 16);

    warning_msg = isAnyReasonCannotBuild(vo);

    updateLayoutForDynamicItems();

    if (warning_msg == "")
    {
        m_spineDecorationPreview->setColor(kTintUnlock);
        m_arrow->setVisible(true);

        if (g_playerState->player.balance.gold < vo.cost_coins)
        {
            _labelPrice->setColor(kTextColorOrange2);
        }
        else
        {
            _labelPrice->setColor(kTextColorWhite);
        }

        snprintf(_buffer, _buffer_size, "%d", vo.cost_coins);
        snprintf(_buffer, _buffer_size, "%s", CCStringHelper::createWithNumberFormat(_buffer).c_str());
        _labelPrice->setString(_buffer);
        _iconCoin->setVisible(true);
    }
    else
    {
        _labelPrice->setString(bufferShortMsgDec);
        m_spineDecorationPreview->setColor(kTintLock);
        m_arrow->setVisible(false);
        m_labelMax->setString(warning_msg.c_str());
        _iconCoin->setVisible(false);
    }

    int _badgeNumber = g_playerState->getNKeptDecoration(vo.type);
    bool specialDecoration = vo.in_shop == 2;

    if (_badgeNumber > 0)
    {
        _iconCoin->setVisible(false);
        _labelPrice->setString("");
    }

    m_labelMax->setVisible(true);

    if (warning_msg == g_gameContent->Word["maximum_reached"])
    {
        if (_badgeNumber > 0)
        {
            m_labelMax->setVisible(false);
        }
    }

    int keptDecoration = g_playerState->getNKeptDecoration(vo.type);
    int builtDecoration = g_playerState->getNDecoration(vo.type) - keptDecoration;
    int maxDecoration = vo.maximum;

    std::stringstream ss;
    ss << builtDecoration << "/" << maxDecoration << " in the farm\n" << keptDecoration << " in the storage" << "\n";
    decorationDesc->setString(ss.str().c_str());
}

string ShopItemDecoration::isAnyReasonCannotBuild(GDDecorationVO vo_)
{
    CREATE_CHAR_BUFFER(_buffer, 64);
    std::string warning_msg = "";

    Building *_building_hq = g_playerState->getBuilding(kBuildingHQ, 0);
    if (_building_hq->pbVo.level < vo_.unlocks_at_level) {
        snprintf(_buffer, _buffer_size, g_gameContent->Word["need_building_lv"].c_str(), _building_hq->bVo.title.c_str(), vo_.unlocks_at_level);
        warning_msg = _buffer;
    }

    int _badgeNumber = g_playerState->getNKeptDecoration(vo_.type);
    bool maximum_reached = g_playerState->isDecorationMaximumReached(vo_.type);
    if (warning_msg == "" && maximum_reached && _badgeNumber <= 0) {
        warning_msg = g_gameContent->Word["maximum_reached"];
    }

    return warning_msg;
}
