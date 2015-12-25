//
//  WeatherParticle.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 11/25/2557 BE.
//
//

#ifndef __Cheetah__WeatherParticle__
#define __Cheetah__WeatherParticle__

#include "cocos2d.h"
#include "WeatherEnvironment.h"

#define PARTICLE_SCALE 1.2f
#define GRAVITY (-750.0f)

class WeatherParticle: public cocos2d::CCSprite {
    
    float gravity;
    WeatherEnvironment *environment;
    
    float distanceFromScreen;
    
public:
    
    float weight;
    
    CCPoint currOffset;
    CCPoint initialContentNodePos;
    float initialContentNodeScale;
    
    static WeatherParticle* create(const char *pszFileName_, WeatherEnvironment *environment_);
    
    void setDistanceFromScreen(float distanceFromScreen_);
    float getDistanceFromScreen() { return distanceFromScreen; };
    
    virtual bool initWithFile(const char *pszFilename_, WeatherEnvironment *environment_);
    
    virtual void update(float delta);
    
    void remove();
};

#endif /* defined(__Cheetah__WeatherParticle__) */
