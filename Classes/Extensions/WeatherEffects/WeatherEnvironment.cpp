//
//  WeatherEnvironment.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 11/25/2557 BE.
//
//

#include "WeatherEnvironment.h"
#include "WeatherParticle.h"
USING_NS_CC;

extern float sizeMultiplier;

WeatherEnvironment* WeatherEnvironment::create(CCNikScrollView *scrollView_, IsoMap *isoMap_, std::string spriteName_, int amount_)
{
    WeatherEnvironment *pRet = new WeatherEnvironment();
    if (pRet && pRet->init(scrollView_, isoMap_, spriteName_, amount_))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool WeatherEnvironment::init(CCNikScrollView *scrollView_, IsoMap *isoMap_, std::string spriteName_, int amount_) {
    
    if (!CCSpriteBatchNode::initWithFile(spriteName_.c_str(), amount_)) {
        return false;
    }
    
    scrollView = scrollView_;
    isoMap = isoMap_;
    spriteName = spriteName_;
    
    //starting values
    windFlip = 1.0f; //always start going to the right first
    
    windSpeed = 240.0f*sizeMultiplier;
    
    windAcceleration = windSpeed*-0.5f;//0.0f*sizeMultiplier;
    
    windVelocity = 0.0f;
    
    secondsUntilFlippingWind = RESET_SECONDS_UNTIL_FLIPPING_WIND *0.5f; //always start halfway
    
    createRandomParticle(amount_, false);
    
    scheduleUpdate();
    
    return true;
    
}

void WeatherEnvironment::update(float delta) {
    
    windAcceleration = windSpeed * windFlip * delta;
    
    secondsUntilFlippingWind -= delta;
    if (secondsUntilFlippingWind <= 0.0f)
    {
        secondsUntilFlippingWind = RESET_SECONDS_UNTIL_FLIPPING_WIND;
        windFlip *= -1.0f;
    }
    
    windVelocity += windAcceleration;
    
    setPosition(scrollView->contentNode->getPosition());
    setScale(scrollView->getScale());
}

void WeatherEnvironment::createRandomParticle(int amount, bool fromTop)
{
    for (int i = 0; i < amount; i++) {
        WeatherParticle *snowParticle = WeatherParticle::create(spriteName.c_str(), this);
        
        initializeRandomnessToParticle(snowParticle, fromTop);
        
        snowParticle->scheduleUpdate();
        
        addChild(snowParticle);
    }
}


void WeatherEnvironment::initializeRandomnessToParticle(void *particle, bool fromTop)
{
    CCSize mapSize = isoMap->getContentSize()*0.5f;
    
    WeatherParticle *snowParticle = (WeatherParticle*)particle;
    
    snowParticle->setDistanceFromScreen(WeatherEnvironment::randNum(0.05f, 1.0f));
    snowParticle->weight = WeatherEnvironment::randNum(0.15f, 1.0f);
    
    float startingYPos = 1.0f;
    if (!fromTop) {
        startingYPos = WeatherEnvironment::randNum();
    }
    
    snowParticle->setPosition(ccp(WeatherEnvironment::randNum() * mapSize.width,
                                  startingYPos * mapSize.height));
    snowParticle->currOffset = snowParticle->getPosition();
    snowParticle->initialContentNodePos = scrollView->contentNode->getPosition();
    snowParticle->initialContentNodeScale = scrollView->contentNode->getScale();
    
    
}




