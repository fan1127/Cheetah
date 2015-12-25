//
//  ShopItemBuilding.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/28/56 BE.
//
//

#include "ShopItemBuilding.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include "../Settings.h"
#include "../Helpers/UIDisplayHelper.h"
#include "../Entities/PlayerState.h"

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../Extensions/CCPopOverDialog.h"
#include "../Helpers/CCStringHelper.h"

#include "../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;

CREATE_CHAR_BUFFER(bufferShortMsgDec, 64);

ShopItemBuilding *ShopItemBuilding::createWithBuildingVO(GDBuildingVO vo_, cocos2d::CCSize size_)
{
    ShopItemBuilding *pRet = new ShopItemBuilding();
    if (!pRet->initWithBuildingVO(vo_, size_))
    {
        delete pRet;
        return NULL;
    }
    pRet->autorelease();
    return pRet;
}

bool ShopItemBuilding::initWithBuildingVO(GDBuildingVO vo_, cocos2d::CCSize size_)
{
    if (!CCNikScrollViewButton::init(NULL, NULL))
    {
        return NULL;
    }

    this->setContentSize(size_);

    m_arrow = NULL;

    this->vo = vo_;

#define ITEM_MARGIN_W (30.0f*sizeMultiplier)
#define TEXT_MARGIN_W (20.0f*sizeMultiplier)
#define ITEM_MARGIN_H (40.0f*sizeMultiplier)

    CCLabelBMFont *_labelTitle = CCLabelHelper::createBMFont(vo.title.c_str(), kFontSizeLarge, true);
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
    _bg->setPosition(ccp(30.0f*sizeMultiplier,
                         0.0f));

    buildingDesc = CCLabelHelper::createBMFont(vo.description.c_str(), kFontSizeSmall, kTextColorLightBrown);
    buildingDesc->setWidth(_bg->getContentSize().width-40.0f*sizeMultiplier);
    buildingDesc->setAlignment(kCCTextAlignmentLeft);
    buildingDesc->setAnchorPoint(ccp(0.0f, 1.0f));
    buildingDesc->setPosition(ccp(_bg->getPositionX() + 20*sizeMultiplier,
                                  _bg->getPositionY() + _bg->getContentSize().height - 20*sizeMultiplier));

    _labelTitle->setPosition(ccp(_bg->getPositionX() + 20*sizeMultiplier,
                                 _bg->getPositionY() + _bg->getContentSize().height + 70*sizeMultiplier));

    m_spineBuildingPreview = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, vo.src.c_str());
    m_spineBuildingPreview->setScale(0.5f*sizeMultiplier);
    m_spineBuildingPreview->setAnchorPoint(ccp(0.5f, 0.5f));
    m_spineBuildingPreview->setPosition(ccp(this->getContentSize().width
                                            -160.0f*sizeMultiplier,
                                            this->getContentSize().height*0.4f));

    CREATE_CHAR_BUFFER(bufferTime, 64)
    UIDisplayHelper::convertToDisplayTime2(vo.time, bufferTime, bufferTime_size);
    _labelTime = CCLabelHelper::createBMFont(bufferTime, kFontSizeSmall, kTextColorLightBrown);
    _iconTime = UIHelper::getSprite(UIHelper::UpgradeClockIcon);
    m_labelMax = CCLabelHelper::createBMFont("", kFontSizeSmall, kTextColorLightBrown);
    
    if (vo.building_type == BUILDING_TYPE_HABITAT && BUILD_HABITAT_WITH_GEMS) {
        _iconCoin = UIHelper::getSprite(UIHelper::NSBuyGemIcon);
    }else {
        _iconCoin = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);
    }
    
    m_arrow = UIHelper::getSprite(UIHelper::BuildArrow);
    m_arrow->setPosition(ccp(this->getContentSize().width
                             +20.0f*sizeMultiplier,
                             m_spineBuildingPreview->getPositionY() - 20*sizeMultiplier));

    this->addChild(_bg);
    this->addChild(_iconCoin);
    this->addChild(_labelTime);
    this->addChild(_iconTime);
    this->addChild(m_spineBuildingPreview);
    this->addChild(buildingDesc);
    this->addChild(_labelTitle);
    this->addChild(m_labelMax);
    this->addChild(_labelPrice);
    this->addChild(m_arrow);

    updateState();

    return true;
}

CCPoint ShopItemBuilding::getPreviewBuildingPosition()
{
    if (m_spineBuildingPreview)
    {
        return m_spineBuildingPreview->getPosition();
    }

    return CCPointZero;
}

string ShopItemBuilding::getPriceAsString() {
    
    string _out = "";
    CREATE_CHAR_BUFFER(_buf, 16);
    
    if (BUILD_HABITAT_WITH_GEMS && vo.building_type == BUILDING_TYPE_HABITAT) {
        snprintf(_buf, _buf_size, "%d",int(vo.param_2));
    }else {
        snprintf(_buf, _buf_size, "%d", vo.cost_coins);
    }
    _out = _buf;
    
    return _out;
    
}

void ShopItemBuilding::updateLayoutForDynamicItems()
{
    auto textYOffset = _bg->getContentSize().height*0.5f - 30*sizeMultiplier;

    if (warning_msg != "")
    {
        _labelPrice->setAnchorPoint(ccp(0.5f, 0.5f));
        _labelPrice->setAlignment(kCCTextAlignmentCenter);
        _labelPrice->setPosition(m_spineBuildingPreview->getPositionX(), m_spineBuildingPreview->getPositionY() - textYOffset);

        _iconCoin->setVisible(false);
        _labelTime->setVisible(false);
        _iconTime->setVisible(false);
    }
    else
    {
        _labelPrice->setAnchorPoint(ccp(0.0f, 0.5f));
        _labelPrice->setAlignment(kCCTextAlignmentLeft);

        _labelPrice->setString(getPriceAsString().c_str());

        _iconCoin->setVisible(true);

        _labelTime->setAnchorPoint(ccp(1.0f, 0.5f));
        _labelTime->setPosition(ccp(_bg->getContentSize().width
                                    -20.0f*sizeMultiplier,
                                    _labelPrice->getPositionY()));
        _labelTime->setVisible(true);

        _iconTime->setAnchorPoint(ccp(1.0f, 0.5f));
        _iconTime->setPosition(ccp(_labelTime->getPositionX()
                                   -_labelTime->getContentSize().width
                                   -10.0f*sizeMultiplier,
                                   _labelPrice->getPositionY()));
        _iconTime->setScale(0.9f);
        _iconTime->setVisible(true);
        
        UIHelper::setTwoNodesToMiddlePosition(_iconCoin, _labelPrice, 10.0f*sizeMultiplier, ccp(m_spineBuildingPreview->getPositionX() - 20.0f*sizeMultiplier, m_spineBuildingPreview->getPositionY() - textYOffset));
    }

    m_labelMax->setAnchorPoint(ccp(0.0f, 0.5f));
    m_labelMax->setPosition(ccp(buildingDesc->getPositionX(),
                                _labelPrice->getPositionY()));
}

void ShopItemBuilding::updateState() {
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    if (BUILD_HABITAT_WITH_GEMS
        && vo.building_type == BUILDING_TYPE_HABITAT) {
        vo = g_gameContent->getBuildingVO(vo.type, _playerState->getNBuilding(vo.type)+1);
    }
    
    CREATE_CHAR_BUFFER(_buffer, 16);
    Building *_building_hq = _playerState->getBuilding(kBuildingHQ, 0);
    
    int number_of_building_i_have = _playerState->getNBuilding(vo.type);
    int number_of_building_i_can_have = g_gameContent->getBuildingMax(vo.type, _building_hq->pbVo.level);
    if (number_of_building_i_can_have == 0) {
        number_of_building_i_can_have = 1;
    }
    
    snprintf(_buffer, _buffer_size, "%d/%d",
             number_of_building_i_have,
             number_of_building_i_can_have);
    m_labelMax->setString(_buffer);
    
    warning_msg = isAnyReasonCannotBuild(vo);
    
//    bool reachMaximum = (number_of_building_i_have >= number_of_building_i_can_have);
    
    updateLayoutForDynamicItems();

    if (warning_msg.compare("") == 0) {
        m_spineBuildingPreview->setColor(kTintUnlock);
        m_arrow->setVisible(true);
        
        if (vo.building_type == BUILDING_TYPE_HABITAT && BUILD_HABITAT_WITH_GEMS)
        {
            bool notEnoughGems = (_playerState->player.balance.gem < vo.param_2);
            
            if (notEnoughGems)
            {
                _labelPrice->setColor(kTextColorOrange2);
            }
            else
            {
                _labelPrice->setColor(kTextColorWhite);
            }
        }
        else
        {
            bool notEnoughCoins = (_playerState->player.balance.gold < vo.cost_coins);
            
            if (notEnoughCoins)
            {
                _labelPrice->setColor(kTextColorOrange2);
            }
            else
            {
                _labelPrice->setColor(kTextColorWhite);
            }

        }
        
        snprintf(_buffer, _buffer_size, "%s", getPriceAsString().c_str());
        snprintf(_buffer, _buffer_size, "%s", CCStringHelper::createWithNumberFormat(_buffer).c_str());
        _labelPrice->setString(_buffer);
        
    }else {
        
        _labelPrice->setString(bufferShortMsgDec);
        _labelPrice->setWidth(250.0f*sizeMultiplier);
        
        m_spineBuildingPreview->setColor(kTintLock);
        m_arrow->setVisible(false);
    }
    
//    if (reachMaximum || !warning_msg.empty()) {
//        _labelPrice->setColor(kTextColorOrange2);
//    }
    
}

string ShopItemBuilding::isAnyReasonCannotBuild(GDBuildingVO vo_) {
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    string msg = "";
    
    Building *_building_hq = _playerState->getBuilding(kBuildingHQ, 0);
    if (_building_hq->pbVo.level < vo_.unlocks_at_level) {
        snprintf(_buffer, _buffer_size, g_gameContent->Word["unlocks_at_level"].c_str(), vo_.title.c_str(), _building_hq->bVo.title.c_str(), vo_.unlocks_at_level);
        msg = _buffer;
        snprintf(bufferShortMsgDec, bufferShortMsgDec_size, g_gameContent->Word["need_building_lv"].c_str(), _building_hq->bVo.title.c_str(), vo_.unlocks_at_level);
        return msg;
    }
    
    int currentHQLevel = _building_hq->pbVo.level;
    int currentBuildingAmount = _playerState->getNBuilding(vo_.type);
    int currentMaxAmount = g_gameContent->getBuildingMax(vo_.type, currentHQLevel);
    CCLog_("%s %d/%d", vo_.title.c_str(), currentBuildingAmount, currentMaxAmount);
    if (msg == "" && currentBuildingAmount >= currentMaxAmount) {
        
        int maxHQLevel = g_gameContent->getBuildingMaxLevel(kBuildingHQ);
        
        for (int hqLevelInLoop = currentHQLevel; hqLevelInLoop <= maxHQLevel ; hqLevelInLoop++) {
            int newMaxAmount = g_gameContent->getBuildingMax(vo_.type, hqLevelInLoop);
            if (newMaxAmount > currentBuildingAmount) {
                
                snprintf(_buffer, _buffer_size, g_gameContent->Word["upgrade_mansion_to_build_more"].c_str(), _building_hq->bVo.title.c_str(), hqLevelInLoop);
                snprintf(bufferShortMsgDec, bufferShortMsgDec_size, g_gameContent->Word["need_building_lv"].c_str(), _building_hq->bVo.title.c_str(), hqLevelInLoop);
                
                msg = _buffer;
                
                break;
            }
        }
        
        //msg is still ""
        if (msg == "") {
            msg = g_gameContent->Word["maximum_amount_built"];
            strncpy(bufferShortMsgDec, g_gameContent->Word["maximum_reached"].c_str(), bufferShortMsgDec_size);
            
            
        }
    }
    
    return msg;
    
}
