//
//  WeatherParticle.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 11/25/2557 BE.
//
//

#include "WeatherParticle.h"
USING_NS_CC;

extern float sizeMultiplier;

WeatherParticle* WeatherParticle::create(const char *pszFileName_, WeatherEnvironment *environment_)
{
    WeatherParticle *pRet = new WeatherParticle();
    if (pRet && pRet->initWithFile(pszFileName_, environment_))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool WeatherParticle::initWithFile(const char *pszFilename_, WeatherEnvironment *environment_) {
    
    if (!CCSprite::initWithFile(pszFilename_)) {
        return false;
    }
    
    environment = environment_;
    
    gravity = GRAVITY*sizeMultiplier;
    
    return true;
    
}

void WeatherParticle::setDistanceFromScreen(float distanceFromScreen_) {
    
    distanceFromScreen = distanceFromScreen_;
    
    setScale(PARTICLE_SCALE * distanceFromScreen * sizeMultiplier);
}

void WeatherParticle::update(float delta) {
    
    float scale = getScale() * environment->getScale();
    
    if (scale < (PARTICLE_SCALE*0.01f) ||
        scale > (PARTICLE_SCALE*0.35f))
    {
        setVisible(false);
    } else {
        setVisible(true);
    }
    
    CCPoint changes = ccp(environment->getWindVelocity()*delta*distanceFromScreen*weight,
                          gravity*delta*distanceFromScreen);
    
    currOffset = currOffset + changes;
    
    float scaleDiff = (environment->scrollView->contentNode->getScale()
                       -
                       initialContentNodeScale
                       );
    
    //this is sort of a hack that causes problems in some cases
    // 1. pan the camera around
    // 2. zoom
    // 3. notice that snow particles change positions
    if (fabsf(scaleDiff) > 0.0f) {
        initialContentNodePos = environment->scrollView->contentNode->getPosition();
        initialContentNodeScale = environment->scrollView->contentNode->getScale();
    }
    
    CCPoint posDiff = (environment->scrollView->contentNode->getPosition()
                       -
                       initialContentNodePos
                       );
    
    setPosition(currOffset
                + posDiff * distanceFromScreen);
    
    CCPoint currPos = getPosition();
    
    if (currPos.y <
        0.0f
//        environment->isoMap->getContentSize().height*0.25f //half of the screen
        )
    {
        environment->initializeRandomnessToParticle(this, true); //reset everyting
    }
    
}

void WeatherParticle::remove()
{
    environment = NULL;
    removeFromParentAndCleanup(true);
}

