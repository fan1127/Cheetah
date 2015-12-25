//
//  ResourceProgressButton.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/3/2557 BE.
//
//

#include "ResourceProgressButton.h"

#include "../../Extensions/CCLabelHelper.h"
#include "../../Helpers/CCStringHelper.h"

#include "../../Settings.h"

#include "../../Services/GameAudioService.h"
#include "../../Helpers/UIHelper.h"

extern GameAudioService* g_gameAudio;

extern float margin;
extern float retinaMultiplier;
extern float sizeMultiplier;

ResourceProgressButton* ResourceProgressButton::create(cocos2d::CCSprite *iconSprite)
{
    ResourceProgressButton *obj = new ResourceProgressButton();
    if (!obj->init(iconSprite))
    {
        return NULL;
    }

    obj->autorelease();
    return obj;
}

bool ResourceProgressButton::init(cocos2d::CCSprite *iconSprite)
{
    if (!CCNikButton::init(UIHelper::getSprite(UIHelper::HUDCounterBig), NULL))
    {
        return false;
    }
    
    m_progressBarEnabled = true;
    
    m_currentAmount = 0;
    m_currentAmountMax = 100;
    m_currentTargetAmount = m_currentAmount;
    m_partialAdditionAmount = abs(m_currentTargetAmount - m_currentAmount)/ResourceProgressButtonChangeStep;
    m_targetAmountQueue.clear();

    CCSpriteFrame *spriteFrame = UIHelper::getSpriteFrame(UIHelper::HUDCounterBigColor);

    CCSize size = spriteFrame->getRect().size;
    CCScale9Sprite *progressBarSprite = CCScale9Sprite::createWithSpriteFrame(spriteFrame, CCRect(size.width*0.4f, size.height*0.45f, size.width*0.4f, size.height*0.1f));
    
    m_progressBar = CCNikProgressBar::create(NULL, progressBarSprite, NULL);
    m_progressBar->setPosition(this->getContentSize()*0.5f);
    m_progressBar->setContentSize(CCSize(this->getContentSize().width*0.97f, size.height*0.9f));
    this->addChild(m_progressBar);

    m_iconSprite = iconSprite;
    m_iconSprite->setPosition(ccp(m_iconSprite->getContentSize().width*0.15f, this->getContentSize().height*0.5f));
    this->addChild(m_iconSprite);
    
    m_label = CCLabelHelper::createBMFont(retinaMultiplier, "0", kFontSizeLarge-5.0f, true);
    m_label->setAnchorPoint(ccp(1, 0.5f));
    m_label->setPosition(ccp(this->getContentSize().width*0.95f, this->getContentSize().height*0.5f));
    this->addChild(m_label);
    
    setMaxLevelLabelShowed(true);
    
    return true;
    
}


void ResourceProgressButton::setMaximumAmount(int maximumAmount) {

    if (m_currentAmountMax == maximumAmount) return;
    m_currentAmountMax = maximumAmount;
    
    if (m_maxLabel) {
        CREATE_CHAR_BUFFER(_buffer, 32);
        snprintf(_buffer, _buffer_size, "max: %s",  CCStringHelper::createWithNumberFormat(m_currentAmountMax).c_str() );
        m_maxLabel->setString(_buffer);
    }
    
    updateProgressBar();
    
}

void ResourceProgressButton::setMaximumAmountAnimated(int maximumAmount) {
    
    if (m_currentAmountMax == maximumAmount)
        return;

    int _tmp_current_amount = m_currentAmount;
    float _ratio = m_currentAmount/m_currentAmountMax;
    int _converted_current_amount = int(maximumAmount*_ratio);
   
    setAmount(_converted_current_amount);
    addTargetAmount(_tmp_current_amount, maximumAmount, AUDIO_NONE);
    
    processTargetAmount();
    
}

void ResourceProgressButton::setAmount(int amount) {
    
    m_currentAmount = amount;
    
    /*fix bug with send to temp string before setstring label*/
    std::string tempString = CCStringHelper::createWithNumberFormat(amount);
    m_label->setString(tempString.c_str());
    
    updateProgressBar();
    
}

void ResourceProgressButton::addUp(int amount, std::string soundFilename) {
    
    addUp(amount, m_currentAmountMax, soundFilename);
    
}

void ResourceProgressButton::addUp(int amount, int amountMax, std::string soundFilename) {
    
    if (m_currentTargetAmount == 0) {
        m_currentTargetAmount = m_currentAmount;
    }
    int _targetAmount = m_currentTargetAmount + amount;

    addTargetAmount(_targetAmount, amountMax, soundFilename);
    processTargetAmount();
    
}

bool ResourceProgressButton::addTargetAmount(int targetAmount) {
    
    return addTargetAmount(targetAmount, m_currentAmountMax, "");
    
}

bool ResourceProgressButton::addTargetAmount(int targetAmount, std::string soundFilename) {
    
    return addTargetAmount(targetAmount, m_currentAmountMax, soundFilename);
    
}

bool ResourceProgressButton::addTargetAmount(int targetAmount, int amountMax, std::string soundFilename) {
    
    int index = (int)m_targetAmountQueue.size()-1;
    if (index >= 0) {
        if (m_targetAmountQueue[index].amount == targetAmount) {
            return false;
        }
    }
    
    if (targetAmount == m_currentAmount &&
        amountMax == m_currentAmountMax) {
        return false;
    }
    
    TargetAmount tmp;
    tmp.amount = targetAmount;
    tmp.amountMax = amountMax;
    tmp.soundFilename = soundFilename;
    m_targetAmountQueue.push_back(tmp);
    
    return true;
}

void ResourceProgressButton::updateProgressBar()
{
    m_progressBar->setPercentage((float)m_currentAmount/(float)m_currentAmountMax*100);
}

void ResourceProgressButton::updateRunningNumber(float delta) {
    
    bool inclease = false; //if false, mean declease
    
    if (m_currentAmount < m_currentTargetAmount) {
        inclease = true;
    }
    
    if (inclease) {
    
        m_currentAmount += m_partialAdditionAmount;
        if (m_currentAmount > m_currentTargetAmount) {
            m_currentAmount = m_currentTargetAmount;
        }
        
    }else { //declease
     
        m_currentAmount -= m_partialAdditionAmount;
        if (m_currentAmount < m_currentTargetAmount) {
            m_currentAmount = m_currentTargetAmount;
        }
        
    }
    
    setAmount(m_currentAmount);
    
    if (m_currentAmount == m_currentTargetAmount) {
        unschedule(schedule_selector(ResourceProgressButton::updateRunningNumber));
    }
    
}

void ResourceProgressButton::animateIconBeating() {
    
    if (!m_iconSprite) return;
    
    m_iconSprite->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.1f),
                                               CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                               NULL));
    
}

void ResourceProgressButton::processTargetAmount() {
    
    if (m_targetAmountQueue.size() > 0) {
        m_currentTargetAmount = m_targetAmountQueue[0].amount;
        setMaximumAmount(m_targetAmountQueue[0].amountMax);
        m_targetAmountQueue.erase(m_targetAmountQueue.begin());
        m_partialAdditionAmount = (int)ceil(abs(m_currentTargetAmount - m_currentAmount)/(ResourceProgressButtonChangeStep*1.0f)); //multiply 1.0f to change number into float
        CCLog_("Process Target Amount: %d/%d", m_targetAmountQueue[0].amount, m_targetAmountQueue[0].amountMax);
        if (m_partialAdditionAmount > 0) {
            if (m_targetAmountQueue[0].soundFilename != "") {
                g_gameAudio->playEffect(m_targetAmountQueue[0].soundFilename.c_str());
            }
            schedule(schedule_selector(ResourceProgressButton::updateRunningNumber), ResourceProgressButtonStepDuration);
            animateIconBeating();
        }
    }
    
}

void ResourceProgressButton::setAnimation(cocos2d::CCArray *filenames, float delay) {
    
    CCAnimation *animation = CCAnimation::create();
    CCObject *obj;
    CCARRAY_FOREACH(filenames, obj) {
        CCString *filename = (CCString *)obj;
        animation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(filename->getCString()));
    }
    animation->setLoops(1);
    animation->setDelayPerUnit(0.05f);
    CCAnimate *animate = CCAnimate::create(animation);
    m_iconSprite->runAction(CCRepeatForever::create(CCSequence::create(animate,
                                                                       CCDelayTime::create(delay),
                                                                       NULL)));
    
}

void ResourceProgressButton::setProgressBarEnable(bool enable) {
    
    m_progressBarEnabled = enable;
    m_progressBar->setVisible(m_progressBarEnabled);

}

void ResourceProgressButton::setLabelOffsetX(float offsetX) {
    
    m_label->setPosition(ccp(this->getContentSize().width*0.95f - offsetX, this->getContentSize().height*0.5f));

}

void ResourceProgressButton::setMaxLevelLabelShowed(bool show_)
{
    if (show_) {
        if (!m_maxLabel) {
            m_maxLabel = CCLabelHelper::createBMFont("0", 30.5f, true);
            m_maxLabel->setAnchorPoint(ccp(1.0f, 0.5f));
            m_maxLabel->setPosition(ccp( getContentSize().width*0.81f, - m_maxLabel->getContentSize().height*0.31f));
            addChild(m_maxLabel);
        }
        CREATE_CHAR_BUFFER(_buffer, 32);
        snprintf(_buffer, _buffer_size, "max: %s",  CCStringHelper::createWithNumberFormat(m_currentAmountMax).c_str());
        m_maxLabel->setString(_buffer);
    }else {
        if (!m_maxLabel) return;
        m_maxLabel->removeFromParent();
        m_maxLabel = NULL;
    }
}
