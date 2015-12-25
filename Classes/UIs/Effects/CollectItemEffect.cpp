//
//  CollectItemEffect.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/11/2557 BE.
//
//

#include "CollectItemEffect.h"
USING_NS_CC;
using namespace std;

extern float sizeMultiplier;


vector<CollectItemEffect*> s_collectItemEffects;

bool CollectItemEffect::addCollectItemEffect(CollectItemEffect *effect,
                                             cocos2d::CCNode* node) {
    
    if (!effect || !node) return false;
    
    node->addChild(effect, kCollectItemEffectZOrder);
    s_collectItemEffects.push_back(effect);
    
    return true;
    
}

void CollectItemEffect::remCollectItemEffect(CollectItemEffect *effect) {
    
    if (!effect) return;
    
    for (vector<CollectItemEffect*>::iterator it=s_collectItemEffects.begin();
         it!=s_collectItemEffects.end(); ++it) {
        if (*it != effect) continue;
        s_collectItemEffects.erase(it);
        break;
    }
    
}

int CollectItemEffect::countCollectItemEffect() {
    
    return (int)s_collectItemEffects.size();

}

CollectItemEffect* CollectItemEffect::create(const char *spriteFrameName,
                                             const char* effectFileName,
                                             CCPoint initialPosition,
                                             CCPoint destinationPosition,
                                             void* sender,
                                             void (*callback)(void*)) {
    
    CollectItemEffect* p = new CollectItemEffect();
    if (p->init(spriteFrameName,
                effectFileName,
                initialPosition,
                destinationPosition,
                sender,
                callback)) {
        p->autorelease();
        return p;
    }
    CC_SAFE_DELETE(p);
    return NULL;
    
}

bool CollectItemEffect::init(const char *spriteFrameName,
                             const char* effectFileName,
                             CCPoint initialPosition,
                             CCPoint destinationPosition,
                             void* sender,
                             void (*callback)(void*)) {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    this->effectFilename = effectFileName;

    m_onFinished_sender = sender;
    m_onFinished_callback = callback;
    
    float distance = MIN(getContentSize().width, getContentSize().height)*0.2f;
    
    CCSprite *effect = CCSprite::create(effectFileName);
    effect->setPosition(initialPosition);
    effect->setScale(0);
    addChild(effect);
    
    effect->stopAllActions();
    effect->runAction(CCSpawn::create(CCScaleTo::create(0.5f, distance*3.f/effect->getContentSize().width),
                                      CCFadeOut::create(0.5f),
                                      NULL));
    
    
    int startAngle = (10 + arc4random()%16);
    
    for (int i = 0; i < 360; i+= (10 + arc4random()%16)) {
        
        CCSprite *itemSprite = CCSprite::createWithSpriteFrameName(spriteFrameName); //UIHelper::getSprite(UIHelper::HUDGem);
        itemSprite->setPosition(initialPosition);
        itemSprite->setScale(sizeMultiplier);
        addChild(itemSprite);
        
        int angle =  startAngle + i;
        float fangle = angle*M_PI/180;
        
        CCPoint minimunDestinationPoint = itemSprite->getPosition() + ccp(distance*cosf(fangle),
                                                                   distance*sinf(fangle))*0.3f;
        float minimundistance = sqrt(pow(minimunDestinationPoint.x - itemSprite->getPosition().x, 2)
                                     + pow(minimunDestinationPoint.y - itemSprite->getPosition().y, 2));
        
        CCPoint destinationPoint = itemSprite->getPosition() + ccp(distance*cosf(fangle),
                                                            distance*sinf(fangle))*(0.3f + arc4random()%71/100.f);
        float distanceFromCenter = sqrt(pow(destinationPoint.x - itemSprite->getPosition().x, 2)
                                        + pow(destinationPoint.y - itemSprite->getPosition().y, 2));
        
        float time = distanceFromCenter/minimundistance*0.3f;
        
        itemSprite->runAction(CCSequence::create(CCEaseOut::create(CCMoveTo::create(0.3f, destinationPoint), 1 + arc4random()%101/100.f),
                                          CCEaseIn::create(CCMoveTo::create(time, destinationPosition), 2 + arc4random()%101/100.f),
                                          CCCallFuncO::create(this, callfuncO_selector(CollectItemEffect::volatileAnimation), itemSprite),
                                          CCRemoveSelf::create(),
                                          NULL));
    }
    
    runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                 CCCallFunc::create(this, callfunc_selector(CollectItemEffect::finish)),
                                 CCRemoveSelf::create(),
                                 NULL));
    
    return true;
    
}

void CollectItemEffect::volatileAnimation(cocos2d::CCSprite *itemSprite) {
    
    CCSprite *effect = CCSprite::create(effectFilename.c_str());
    effect->setPosition(itemSprite->getPosition());
    effect->setScale(0);
    addChild(effect);
    
    effect->runAction(CCScaleTo::create(0.1f,
                                        itemSprite->getContentSize().width
                                        * itemSprite->getScale()
                                        * 3/effect->getContentSize().width));
    effect->runAction(CCSequence::create(CCFadeOut::create(0.1f),
                                         CCRemoveSelf::create(),
                                         NULL));
    
}

void CollectItemEffect::finish() {
    
    remCollectItemEffect(this);
    
    if (!m_onFinished_callback) return;
    m_onFinished_callback(m_onFinished_sender);

}