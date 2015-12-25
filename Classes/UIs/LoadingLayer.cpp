//
//  LoadingLayer.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/8/2557 BE.
//
//

#include "LoadingLayer.h"

#include "../SceneManager.h"
#include "../UIs/Dialogs/ModalDialog.h"

#include "../Extensions/Modal/CCModalController.h"
extern SceneManager *g_sceneManager;
extern float sizeMultiplier;

LoadingLayer* LoadingLayer::create() {
    
    LoadingLayer *obj = new LoadingLayer();
    if (!obj->init()) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}
bool LoadingLayer::init() {
    
    if (!CCLayerColor::initWithColor(ccc4(0, 0, 0, 135))) {
        return false;
    }
    
    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);
    setTouchPriority(-1000000);

    const char *filename;
    int rand = arc4random()%3;
    if (rand == 0) {
        filename = "cow1";
    } else if (rand == 1) {
        filename = "pig1";
    } else {
        filename = "sheep1";
    }
    
    petSpine = g_sceneManager->sharedSkeletonData->getSkeleton("common/pet/", filename);
    petSpine->setScale(sizeMultiplier);
    this->addChild(petSpine);
    petSpine->setPosition(this->getContentSize()*0.5f);
    
    petSpine->setAnimation("habitat_idle_sleepy_to_stand", false);
    petSpine->addAnimation("habitat_sleep", true);
    
    m_loadingLabel = CCLabelHelper::createBMFont("Loading...", kFontSizeExtraLarge, true);
    m_loadingLabel->setAnchorPoint(ccp(0, 0.5f));
    m_loadingLabel->setAlignment(kCCTextAlignmentLeft);
    m_loadingLabel->setPosition(petSpine->getPosition() - ccp(m_loadingLabel->getContentSize().width*0.5f, this->getContentSize().height*0.05f));
    this->addChild(m_loadingLabel);
    
    animateIndex = 0;
    
    animateLoading();
    
    return true;
}

void LoadingLayer::animateLoading()
{
    switch (animateIndex%4) {
        case 0:
            m_loadingLabel->setString("Loading");
            break;
        case 1:
            m_loadingLabel->setString("Loading.");
            break;
        case 2:
            m_loadingLabel->setString("Loading..");
            break;
        case 3:
            m_loadingLabel->setString("Loading...");
            break;
        default:
            break;
    }
    
    animateIndex++;
    
    m_loadingLabel->runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                                 CCCallFunc::create(this, callfunc_selector(LoadingLayer::animateLoading)),
                                                 NULL));
}

void LoadingLayer::show() {
    
    CCScene *runningScene = CCDirector::sharedDirector()->getRunningScene();
    runningScene->addChild(this, 100000000);
    this->setOpacity(0);
    this->runAction(CCFadeTo::create(0.3f, 135));
    
}

void LoadingLayer::hide() {
    
    if (petSpine) {
        petSpine->removeFromParent();
        petSpine = NULL;
    }
    
    if (m_loadingLabel) {
        m_loadingLabel->removeFromParent();
        m_loadingLabel = NULL;
    }
    
    this->runAction(CCSequence::create(CCFadeTo::create(0.3f, 0.0f), //CCFadeOut::create(0.3f),
                                       CCRemoveSelf::create(),
                                       NULL));
    
}

void LoadingLayer::showInDialog(ModalDialog* dialog)
{
    dialog->addChild(this, 100000000);

    setPosition(dialog->getContentSize()*-0.5f);
    setContentSize(dialog->getContentSize());
    petSpine->setPosition(this->getContentSize()*0.5f);
    m_loadingLabel->setPosition(petSpine->getPosition() - ccp(m_loadingLabel->getContentSize().width*0.5f, this->getContentSize().height*0.05f));

    setOpacityModifyRGB(false);
    setColor(ccc3(0, 0, 0));
    this->setOpacity(0);
//    this->runAction(CCFadeTo::create(0.3f, 0));
}

void LoadingLayer::hideInDialog()
{
    hide();
    removeFromParent();
}

bool LoadingLayer::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    CCPoint point = this->convertTouchToNodeSpace(touch);
    CCRect rect = this->boundingBox();
    if (rect.containsPoint(point)) {
        return true;
    }
    return false;
}
