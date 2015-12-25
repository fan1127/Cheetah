//
//  PowerupButton.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/19/2557 BE.
//
//

#include "PowerupButton.h"

#include "../Scenes/SceneRace.h"
#include "../Extensions/CCLabelHelper.h"
#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

extern CCScene* g_currentScene;

PowerupButton* PowerupButton::create(CCSprite* background, CCSprite* disabledBg, CCSprite *normalSprite, CCSprite *disabledSprite, int startingCost, SKILL skill)
{
    PowerupButton* pRet = new PowerupButton();
    pRet->init(background, disabledBg, normalSprite, disabledSprite, startingCost, skill);
    pRet->autorelease();
    return pRet;
}

bool PowerupButton::init(CCSprite* background, CCSprite *disabledbg, CCSprite *normalSprite, CCSprite *disabledSprite, int startingCost, SKILL skill)
{
    if (!CCNikButton::init(background, NULL))
    {
        return false;
    }

    addChild(normalSprite);
    addChild(disabledbg, -1);
    m_bgNormal = normalSprite;
    m_bgDisabled = disabledbg;
    m_bgDisabled->setVisible(false);
    m_bgDisabled->setPosition(getContentSize()*0.5f);

    normalSprite->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*0.54f));
    disabledSprite->setPosition(ccp(m_bgDisabled->getContentSize().width*0.5f, m_bgDisabled->getContentSize().height*0.54f));

    setDisableSprite(disabledSprite);
    
//    m_enableSpriteFrame = normalSprite->displayFrame();
//    m_disableSpriteFrame = disabledSprite->displayFrame();
    
    m_cost = startingCost;
    
    m_skill = skill;
    
    m_forceEnabledStatus = false;
    m_forceEnabled = false;
    
    m_costLabel = CCLabelHelper::createBMFont(CCString::createWithFormat("%d",m_cost)->getCString(),
                                              kFontSizeLarge,
                                              true,
                                              ccc3(255, 255, 255),
                                              kCCLabelAutomaticWidth,
                                              kCCTextAlignmentRight);
    m_costLabel->setPosition(ccp(this->getContentSize().width*0.48f, this->getContentSize().height *0.25f));
//    m_costLabel->setPosition(ccp(this->getContentSize().width*0.48f, this->getContentSize().height *0.85f));
    m_costLabel->setAnchorPoint(ccp(1, 0.5f));
    this->addChild(m_costLabel);
    
    m_energyIcon = UIHelper::getSprite(UIHelper::RaceElementsPowerIcon);
    m_energyIcon->setPosition(ccp(this->getContentSize().width*0.5f, this->getContentSize().height *0.25f));
//    m_energyIcon->setPosition(ccp(this->getContentSize().width*0.5f, this->getContentSize().height *0.85f));
    m_energyIcon->setAnchorPoint(ccp(0, 0.5f));
    m_energyIcon->setScale(0.6f);
    this->addChild(m_energyIcon);
    
    return true;
}

void PowerupButton::touchDown()
{
    bool disabled = false;
    
    if (m_forceEnabledStatus && !m_forceEnabled) {
        disabled = true;
    }
    
    if (!m_bEnabled)
    {
        disabled = true;
    }
    
    if (disabled)
    {
        SceneRace* _scene = (SceneRace*)g_currentScene;
        if(_scene) {
            _scene->racePanel->noEnergyUIFeedback();
        }
        g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
        
        return;
    }
    
    //g_gameAudio->setEffectsVolume(0.1f);
    g_gameAudio->playEffect(AUDIO_ABILITY_CLICK);
    //g_gameAudio->setEffectsVolume(VOLUME_MAX_SFX);
    
    CCNikButton::touchUpInside();
    
    CCSprite *effect = CCSprite::create("common/vfx/fx_gacha_to_barn_b2.png");
    effect->setPosition(this->getContentSize()*0.5f);
    effect->setScale(0);
    this->addChild(effect);
    
    effect->runAction(CCSequence::create(CCScaleTo::create(0.1f, (this->getContentSize().width/effect->getContentSize().width)*1.4f),
                                         CCFadeOut::create(0.1f),
                                         CCRemoveSelf::create(),
                                         NULL));
    
    CCNikButton::touchDown();
}

void PowerupButton::touchUpInside()
{
    if (!m_bEnabled) return;
}

void PowerupButton::touchUpOutSide()
{
    if (!m_bEnabled) return;
    
    CCNikButton::touchUpOutSide();
}

void PowerupButton::updateCost(int cost)
{
    m_cost = cost;
    m_costLabel->setString(CCString::createWithFormat("%d",cost)->getCString());
}

void PowerupButton::checkCost(int cost)
{
    setEnabled(m_cost <= cost);
}

void PowerupButton::setCostVisible(bool costVisible)
{
    m_costLabel->setVisible(costVisible);
    m_energyIcon->setVisible(costVisible);
}

void PowerupButton::forceEnabled(bool forceEnable)
{
    this->setForceEnabledStatus(true);
    
    m_forceEnabled = forceEnable;
    
    setState();
}

void PowerupButton::setForceEnabledStatus(bool turnOnForceEnable)
{
    m_forceEnabledStatus = turnOnForceEnable;
    
    setState();
}

void PowerupButton::setEnabled(bool enabled)
{
    if (m_forceEnabledStatus) return;
    
    m_bEnabled = enabled;
    
    setState();
}

void PowerupButton::setState()
{
    if (m_forceEnabledStatus) {
        if (m_forceEnabled && m_disabledSprite) {
            m_normalSprite->setVisible(true);
            m_bgNormal->setVisible(true);
            m_disabledSprite->setVisible(false);
            m_bgDisabled->setVisible(false);

        }
        else if (m_disabledSprite) {
            m_normalSprite->setVisible(false);
            m_bgNormal->setVisible(false);
            m_disabledSprite->setVisible(true);
            m_bgDisabled->setVisible(true);
        }
        return;
    }

    if (m_bEnabled && m_disabledSprite) {
        m_normalSprite->setVisible(true);
        m_bgNormal->setVisible(true);
        m_disabledSprite->setVisible(false);
        m_bgDisabled->setVisible(false);

    }
    else if (m_disabledSprite) {
        m_normalSprite->setVisible(false);
        m_bgNormal->setVisible(false);
        m_disabledSprite->setVisible(true);
        m_bgDisabled->setVisible(true);
    }

    m_disabledSprite->setPosition(ccp(m_bgDisabled->getContentSize().width*0.5f, m_bgDisabled->getContentSize().height*0.54f));
}

//void PowerupButton::resetTimeUsed()
//{
//    m_timeUsed = -1;                 //this is so that...
//    increaseTimeUsedAndUpdateCost(); //m_timeUsed will be 0 here when new m_cost is calculated
//}
