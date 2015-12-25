//
//  ProgressBar.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/26/2557 BE.
//
//

#include "ProgressBar.h"
USING_NS_CC;

#include "../../Settings.h"
#include "../../Entities/Building.h"

#include "../../Helpers/UIDisplayHelper.h"

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

extern float sizeMultiplier;

#include "../../Entities/Ability.h"
#include "../../Helpers/UIHelper.h"

ProgressBar *ProgressBar::create(void* entity_) {
    
    ProgressBar *pRet = new ProgressBar();
    pRet->init(entity_);
    pRet->autorelease();
    return pRet;
    
}

bool ProgressBar::init(void* entity_) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_entity = entity_;
    
    m_icon = NULL;
    m_bar = NULL;
    m_label = NULL;
    
    setTag(1);
    
    return compose();
    
}


#define PROGRESS_BAR_OFFSET cocos2d::CCPoint(7.0f, 8.0f)
#define PROGRESS_BAR_SHIFT cocos2d::CCPoint(1.0f, -0.5f)

bool ProgressBar::compose() {
    
    dispose();
    
    CCScale9Sprite* _bgProgress = NULL;
    CCScale9Sprite* _innerProgress = NULL;
    
    if (Building *_building = (Building*)m_entity) {
        if (typeid(*_building) == typeid(Building)) {
            
            if (_building->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
                m_icon = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsBuildIcon));
                _bgProgress = UIHelper::getScalableSprite(UIHelper::ProgressBarBgHUD);
                _innerProgress = UIHelper::getScalableSprite(UIHelper::ProgressBarColor);
            }else if (_building->pbVo.status == BUILDING_STATUS_UPGRADE) {
                m_icon = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsUpgradingIcon));
                _bgProgress = UIHelper::getScalableSprite(UIHelper::ProgressBarBgHUD);
                _innerProgress = UIHelper::getScalableSprite(UIHelper::ProgressBarColor);
            }
            
            m_bar = CCProgressBar9Sprite::create(_bgProgress,
                                                 _innerProgress,
                                                 kProgressBar4BuildingInMapSize*sizeMultiplier,
                                                 PROGRESS_BAR_OFFSET*sizeMultiplier,
                                                 PROGRESS_BAR_SHIFT*sizeMultiplier);
            addChild(m_bar);
            
            m_label = CCLabelHelper::createBMFont(sizeMultiplier, "", kFontSizeMedium-5.0f, true);
            m_label->setPosition(m_bar->getPosition());
            addChild(m_label);
            
            m_icon->setPositionX(-(m_bar->getContentSize().width + (m_icon->getContentSize().width*1.25f))*0.5f);
            addChild(m_icon);
            
            m_bar->setPercentage(0);
//            schedule(schedule_selector(ProgressBar::update), 1.0f);
            startUpdate();
            
            return true;
            //            float _width = m_icon->getContentSize().width*1.25f + m_bar->getContentSize().width;
            //            m_icon->setPositionX((m_icon->getContentSize().width-_width)*0.5f);
            //            m_bar->setPositionX((_width-m_bar->getContentSize().width)*0.5f);
            
            //            setContentSize(kProgressBar4BuildingInMapSize*retinaMultiplier);
            
        }
    }
    
    if (Pet *_pet = (Pet*)m_entity) {
        if (typeid(*_pet) == typeid(Pet)) {
            
            m_icon = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsSleepIcon));
            _bgProgress = UIHelper::getScalableSprite(UIHelper::ProgressBarBgHUD);
            _innerProgress = UIHelper::getScalableSprite(UIHelper::ProgressBarColor);
            
            m_bar = CCProgressBar9Sprite::create(_bgProgress,
                                                 _innerProgress,
                                                 kProgressBar4BuildingInMapSize*sizeMultiplier,
                                                 PROGRESS_BAR_OFFSET*sizeMultiplier,
                    PROGRESS_BAR_SHIFT*sizeMultiplier);
            addChild(m_bar);
            
            m_label = CCLabelHelper::createBMFont("", kFontSizeMedium-5.0f, true);
            m_label->setPosition(m_bar->getPosition());
            addChild(m_label);
            
            m_icon->setPositionX(-(m_bar->getContentSize().width + (m_icon->getContentSize().width*1.25f))*0.5f);
            addChild(m_icon);
            
            startUpdate();
            
            return true;
        }
    }
    
    if (Ability* _ability = (Ability*)m_entity) {
        if (typeid(*_ability) == typeid(Ability)) {
            
            CREATE_CHAR_BUFFER(_buf_framename, 64);
            
            if (_ability->aVo.src == "_button_powerup_1") {
                strncpy(_buf_framename, UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsPowerUpSmallIcon1)), _buf_framename_size);
            } else if (_ability->aVo.src == "_button_powerup_2") {
                strncpy(_buf_framename, UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsPowerUpSmallIcon2)), _buf_framename_size);
            } else if (_ability->aVo.src == "_button_powerup_3") {
                strncpy(_buf_framename, UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsPowerUpSmallIcon3)), _buf_framename_size);
            } else if (_ability->aVo.src == "_button_powerup_4") {
                strncpy(_buf_framename, UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsPowerUpSmallIcon4)), _buf_framename_size);
            } else if (_ability->aVo.src == "_button_powerup_5") {
                strncpy(_buf_framename, UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsPowerUpSmallIcon5)), _buf_framename_size);
            }
            
            m_icon = CCSprite::createWithSpriteFrameName(_buf_framename);
            _bgProgress = UIHelper::getScalableSprite(UIHelper::ProgressBarBgHUD);
            _innerProgress = UIHelper::getScalableSprite(UIHelper::ProgressBarColor);
            
            m_bar = CCProgressBar9Sprite::create(_bgProgress,
                                                 _innerProgress,
                                                 kProgressBar4BuildingInMapSize*sizeMultiplier,
                                                 PROGRESS_BAR_OFFSET*sizeMultiplier,
                    PROGRESS_BAR_SHIFT*sizeMultiplier);
            addChild(m_bar);
            
            m_label = CCLabelHelper::createBMFont("", kFontSizeMedium-5.0f, true);
            m_label->setPosition(m_bar->getPosition());
            addChild(m_label);
            
            m_icon->setPositionX(-(m_bar->getContentSize().width + (m_icon->getContentSize().width*1.25f))*0.5f);
            addChild(m_icon);
            
            startUpdate();
            
        }
    }

    return false;
    
}

void ProgressBar::startUpdate() {
    
    update();
    unschedule(schedule_selector(ProgressBar::update));
    schedule(schedule_selector(ProgressBar::update), 1.0f);
    
}

void ProgressBar::dispose() {
    
    if (m_icon) {
        m_icon->removeFromParentAndCleanup(true);
        m_icon = NULL;
    }
    
    if (m_bar) {
        m_bar->removeFromParentAndCleanup(true);
        m_bar = NULL;
    }
    
    if (m_label) {
        m_label->removeFromParentAndCleanup(true);
        m_label = NULL;
    }
    
}

void ProgressBar::update() {
    
     CREATE_CHAR_BUFFER(_buffer, 32);
    
    Building *_building = (Building*)m_entity;
    if (_building && typeid(*_building) == typeid(Building)) {
        if (_building->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
            int seconds = _building->getSecondsLeftToFinish();
            UIDisplayHelper::convertToDisplayTime2(seconds, _buffer, _buffer_size);
            m_bar->setPercentage(_building->getPercentToFinish(_building->bVo));
            if (m_label) m_label->setString(_buffer);
        }else if (_building->pbVo.status == BUILDING_STATUS_UPGRADE) {
            UIDisplayHelper::convertToDisplayTime2(_building->getSecondsLeftToFinish(), _buffer, _buffer_size);
            GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(_building->bVo.type, _building->bVo.level+1);
            m_bar->setPercentage(_building->getPercentToFinish(_nxt_bvo));
            if (m_label) m_label->setString(_buffer);
        }
        return;
    }
    
    Pet *_pet = (Pet*)m_entity;
    if (_pet && typeid(*_pet) == typeid(Pet)) {
        UIDisplayHelper::convertToDisplayTime2(_pet->getSecondsLeftToFullEnergy(), _buffer, _buffer_size);
        m_bar->setPercentage(_pet->getCurrentEnergy());
        if (m_label) m_label->setString(_buffer);
        return;
    }
    
    Ability* _ability = (Ability*)m_entity;
    if (_ability && typeid(*_ability) == typeid(Ability)) {
        UIDisplayHelper::convertToDisplayTime2(_ability->getSecondsLeftToFinish(), _buffer, _buffer_size);
        m_bar->setPercentage(_ability->getCurrentProgressRatio());
        if (m_label) m_label->setString(_buffer);
        return;
    }
    
}
