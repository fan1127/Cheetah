//
//  CloudShadow.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 7/24/2557 BE.
//
//

#include "CloudShadow.h"
USING_NS_CC;

#include "../../Helpers/CCSpriteHelper.h"

CloudShadow* CloudShadow::createWithContentSize(cocos2d::CCSize contentSize_) {
    
    if (CloudShadow* pRet = new CloudShadow()) {
        if (pRet->initWithContentSize(contentSize_)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool CloudShadow::initWithContentSize(cocos2d::CCSize contentSize_) {

    if (!CCLayer::init()) {
        return false;
    }
    
    setContentSize(contentSize_);
    
//    CCSprite* _debug_sprite = blankSpriteWithSize(getContentSize());
//    _debug_sprite->setScale(100.0f);
//    addChild(_debug_sprite);
    
    for (int i=0; i<kCloudShadowNum; i++) {
        m_cloudSprites[i] = CCSprite::createWithSpriteFrameName("oldversion/_environment_cloud.png");
        float _scaleY = (getContentSize().height/m_cloudSprites[i]->getContentSize().height)/3;
        m_cloudSprites[i]->setScale(_scaleY);
        if (i%2==0) {
            m_cloudSprites[i]->setScaleX(-m_cloudSprites[i]->getScaleX());
        }
        resetCloud(m_cloudSprites[i], true);
        addChild(m_cloudSprites[i]);
    }
    
    scheduleUpdate();
    
    return true;
    
}

void CloudShadow::update(float delta_) {
    
    for (int i=0; i<kCloudShadowNum; i++) {
        if (m_cloudSprites[i]) {
            CCSize _cloud_size = CCSizeMake(m_cloudSprites[i]->getContentSize().width*m_cloudSprites[i]->getScaleX(),
                                            m_cloudSprites[i]->getContentSize().height*m_cloudSprites[i]->getScaleY());
            CCPoint _cloud_pos = m_cloudSprites[i]->getPosition();
            int _tagAsMovingSpeed = m_cloudSprites[i]->getTag();
            _cloud_pos.x += _tagAsMovingSpeed * kCloudShadowMovingSpeedRate;
            if (_cloud_pos.x > getContentSize().width*0.5f + _cloud_size.width*0.5f) {
                resetCloud(m_cloudSprites[i], false);
            }else {
                m_cloudSprites[i]->setPosition(_cloud_pos);
            }
        }
    }

}

void CloudShadow::resetCloud(CCSprite* cloudSprite_, bool first_time_) {
    
    if (cloudSprite_) {
        CCSize _cloud_size = CCSizeMake(cloudSprite_->getContentSize().width*cloudSprite_->getScaleX(),
                                        cloudSprite_->getContentSize().height*cloudSprite_->getScaleY());
        CCPoint _cloud_pos = cloudSprite_->getPosition();
//        int _tagAsMovingSpeed = cloudSprite_->getTag();
        
        if (first_time_) {
            _cloud_pos.x = - getContentSize().width*0.5f + arc4random()%int(getContentSize().width);
        }else {
            _cloud_pos.x = - getContentSize().width*0.5f - _cloud_size.width*0.5f;
        }
        _cloud_pos.y = - getContentSize().height*0.5f + arc4random()%int(getContentSize().height);
        int _tagAsMovingSpeed = kCloudShadowMovingSpeedMin + arc4random()%(kCloudShadowMovingSpeedMax-kCloudShadowMovingSpeedMin);
        
        cloudSprite_->setPosition(_cloud_pos);
        cloudSprite_->setTag(_tagAsMovingSpeed);
    }
    
}