//
//  Transition.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/30/56 BE.
//
//

#include "Transition.h"
USING_NS_CC;

#include "../AppMacros.h"
extern TargetDevice targetDevice;

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Services/GameContentService.h"
#include "BufferUtils.h"

extern GameContentService *g_gameContent;

extern float retinaMultiplier;
extern float sizeMultiplier;

TransitionCloud *TransitionCloud::create(void *preferenceRootParent_) {
    
    if (TransitionCloud *pRet = new TransitionCloud()) {
        //pRet->preferenceRootParent = preferenceRootParent_;
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool TransitionCloud::init() {

    CREATE_CHAR_BUFFER(_filename_buffer, 64);
    snprintf(_filename_buffer, _filename_buffer_size, "cloud%d.png", int(rand()%4+1));
    
    if (!CCSprite::initWithSpriteFrameName(_filename_buffer)) {
        return false;
    }
    
    m_targetPosition = this->getPosition();
    m_movingSpeed = FS_SPEED_DEFAULT;
    
    return true;
    
}

void TransitionCloud::updateMoving() {
    
    CCPoint _currPosition = this->getPosition();
    
    float x = m_targetPosition.x - _currPosition.x;
    float y = m_targetPosition.y - _currPosition.y;
    float d = sqrtf(x*x + y*y);
    float s = (d/100.0f)*m_movingSpeed;
    float a = atan2f(y, x);
    _currPosition.x += s * cos(a);
    _currPosition.y += s * sin(a);
    
    this->setPosition(_currPosition);
    
}

void TransitionCloud::setTargetPosition(CCPoint pos) {
    
    m_targetPosition = pos;
    
}

void TransitionCloud::setMovingSpeed(float spd) {
    
    m_movingSpeed = spd;
    
}


bool Transition::init() {
    
    if (!CCNode::init()) {
        return false;
    }
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    m_didAssetLoad = false;
    
    for (int i=0; i<TRANSITION_CLOUD_MAX; i++) {
        m_cloud[i] = NULL;
    }
    
    m_isFinished = false;
    
    layerFade = CCLayerColor::create(ccc4(255, 255, 255, 0));
    addChild(layerFade, 0);
    
    loadingMessage = CCLabelHelper::createBMFont(retinaMultiplier, "now loading ..", 120.0f, true);
    loadingMessage->setPosition(CCPoint(_screenSize.width*0.5f,
                                        _screenSize.height*0.2f));
    addChild(loadingMessage, 2);
    loadingMessage->setVisible(false);
    
    CCSpriteFrame *spriteFrameFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("single_9sprite_bar_white_frame.png");
    CCSize sizeFrame = spriteFrameFrame->getRect().size;
    progressBarFrame =
    CCScale9Sprite::createWithSpriteFrame(spriteFrameFrame,
                                          CCRect(29.0f*sizeMultiplier,
                                                 27.0f*sizeMultiplier,
                                                 60.0f*sizeMultiplier,
                                                 31.0f*sizeMultiplier));
    
    progressBarFrame->setContentSize(CCSizeMake(600.0f*sizeMultiplier,
                                                sizeFrame.height));
    progressBarFrame->setPosition(ccp(_screenSize.width*0.5f,
                                      180.0f*sizeMultiplier));
    addChild(progressBarFrame, 3);
    
    CCSprite *leftSprite = CCSprite::createWithSpriteFrameName("single_9sprite_bar_yellow_loading_bar_left.png");
    CCSprite *rightSprite = CCSprite::createWithSpriteFrameName("single_9sprite_bar_yellow_loading_bar_left.png");
    rightSprite->setFlipX(true);
    
    CCSpriteFrame *spriteFrameMiddle = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("single_9sprite_bar_white_frame_middle.png");
    CCSize sizeMiddle = spriteFrameMiddle->getRect().size;
    
    
    CCScale9Sprite *progressBarMiddle = CCScale9Sprite::createWithSpriteFrameName("single_9sprite_bar_white_frame_middle.png");
    
    progressBarMiddle->setContentSize(CCSizeMake(sizeMiddle.width,
                                                 59.0f*sizeMultiplier));
    
    progressBar = CCNikProgressBar::create(leftSprite, progressBarMiddle, rightSprite);
    progressBar->setPosition(ccp(progressBarFrame->getContentSize().width*0.5f,
                                 progressBarFrame->getContentSize().height*0.5f
                                 + 7.0f*sizeMultiplier));
    
    progressBar->setContentSize(CCSizeMake(progressBarFrame->getContentSize().width
                                           -(34.0f*sizeMultiplier),
                                           leftSprite->getContentSize().height));
    progressBar->setPercentage(0.0f);
    progressBarFrame->addChild(progressBar, 4);
    progressBarFrame->setVisible(false);
    
    OnSceneFadedOut = NULL;
    OnSceneFadedIn = NULL;
    
    fadeSpeed = FS_SPEED_DEFAULT;
    
    return true;
    
}



void Transition::update(float delta) {
    
    for (int i=0; i<TRANSITION_CLOUD_MAX; i++) {
        if (m_cloud[i]) {
            m_cloud[i]->updateMoving();
        }
    }
    
}

bool Transition::loadAsset() {
    
    if (!m_didAssetLoad) {

        for (int i=0; i<TRANSITION_CLOUD_MAX; i++) {
            if(!m_cloud[i]) {
                m_cloud[i] = TransitionCloud::create(this);
                m_cloud[i]->setVisible(false);
                this->addChild(m_cloud[i], 1);
            }
        }
        
        m_didAssetLoad = true;
        
        return true;
    }
    return false;
}

bool Transition::didAssetLoad() {
    
    return m_didAssetLoad;
    
}

void Transition::setProgressBarEnabled(bool enabled_) {
    
    if (progressBarFrame) {
        progressBarFrame->setVisible(enabled_);
    }
    
}

void Transition::setProgressBarPercent(float percent_) {
    
    if (progressBar) {
        progressBar->setPercentage(percent_);
    }
    
}

void Transition::fadeSceneIn(cocos2d::CCObject* sender) {
    
    g_gameAudio->playEffect(AUDIO_CLOUD_OPEN);
    
    loadingMessage->runAction(CCFadeOut::create(fadeSpeed));
    
    layerFade->setVisible(true);
    layerFade->stopAllActions();
    layerFade->runAction(CCSequence::create(CCFadeOut::create(fadeSpeed),
                                            CCCallFuncN::create(this, callfuncN_selector(Transition::_onSceneFadedIn)),
                                            NULL));
    
#if !(IS_DEBUG_BUILD)
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    
    for (int i=0; i<TRANSITION_CLOUD_MAX; i++) {
        if (m_cloud[i]) {
            m_cloud[i]->setMovingSpeed(4.0f+rand()%5);
            if (i < TRANSITION_CLOUD_MAX/2) {
                m_cloud[i]->setTargetPosition(CCPoint(-screenSize.width*1.0f, m_cloud[i]->getPosition().y));
            }else {
                m_cloud[i]->setTargetPosition(CCPoint( screenSize.width*2.0f, m_cloud[i]->getPosition().y));
            }
        }
    }
#endif
    
    m_isFinished = false;
    
}

void Transition::_onSceneFadedIn(CCObject *sender) {
    
    if (OnSceneFadedIn) {
        OnSceneFadedIn(sender);
    }
    
    m_isFinished = true;
    
}

void Transition::fadeSceneOut(cocos2d::CCObject* sender) {
    
    g_gameAudio->playEffect(AUDIO_CLOUD_CLOSE);
    
    if (loadingMessage) {
        loadingMessage->setVisible(true);
        loadingMessage->runAction(CCFadeIn::create(fadeSpeed));
    }
    if (layerFade) {
        layerFade->stopAllActions();
        layerFade->runAction(CCSequence::create(CCFadeIn::create(fadeSpeed*1.2),
                                                CCCallFuncN::create(this, callfuncN_selector(Transition::_onSceneFadedOut)), NULL));
    }

#if !(IS_DEBUG_BUILD)
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    for (int i=0; i<TRANSITION_CLOUD_MAX; i++) {
        if (m_cloud[i]) {
            m_cloud[i]->setVisible(true);
            m_cloud[i]->setScale(((100+(rand()%200))/100.0f)*(1+(int)(targetDevice==TD_IPAD_RETINA)));
            m_cloud[i]->setMovingSpeed(5.0f+rand()%5);
            if (i < TRANSITION_CLOUD_MAX/2) {
                m_cloud[i]->setPosition(CCPoint( screenSize.width*2.0f, rand()%int(screenSize.height)));
            }else {
                m_cloud[i]->setPosition(CCPoint(-screenSize.width*1.0f, rand()%int(screenSize.height)));
            }
            m_cloud[i]->setTargetPosition(CCPoint(rand()%int(screenSize.width), m_cloud[i]->getPosition().y));
        }
    }
#endif
    
    m_isFinished = false;
    
}

void Transition::_onSceneFadedOut(CCObject* sender) {
    
    if (OnSceneFadedOut) {
        if (sceneManager) {
            OnSceneFadedOut(sceneManager);
        }
    }
    
    progressBarFrame->setVisible(false);
    
    m_isFinished = true;
    
}

