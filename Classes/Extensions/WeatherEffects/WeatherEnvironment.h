//
//  WeatherEnvironment.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 11/25/2557 BE.
//
//

#ifndef __Cheetah__WeatherEnvironment__
#define __Cheetah__WeatherEnvironment__

#include "cocos2d.h"
#include "../../Settings.h"
#include "../CCNikScrollView.h"
#include "../../Entities/IsoMap.h"

#define RESET_SECONDS_UNTIL_FLIPPING_WIND (5.0f)

//#define ENABLE_SNOW

#define PARTICLE_TYPE_SNOW "common/particles/snow_dot.png"
#define SNOW_DEFAULT_AMOUNT 75

class WeatherEnvironment: public cocos2d::CCSpriteBatchNode {
    
    std::string spriteName;
    
    float windFlip;
    float windSpeed;
    
    float windAcceleration;
    float windVelocity;
    
    float secondsUntilFlippingWind;
    
public:
    
    CCNikScrollView *scrollView;
    IsoMap *isoMap;
    
    static WeatherEnvironment* create(CCNikScrollView *scrollView_, IsoMap *isoMap_, std::string spriteName_, int amount_);
    
    virtual bool init(CCNikScrollView *scrollView_, IsoMap *isoMap_, std::string spriteName_, int amount_);
    
    virtual void update(float delta);
    
    float getWindVelocity() { return windVelocity; };
    
    void createRandomParticle(int amount, bool fromTop);
    
    void initializeRandomnessToParticle(void *particle, bool fromTop);
    
    
    
    
    
    static float randNum() //from 0.0 - 1.0
    {
        return static_cast<float>((double)arc4random() / ARC4RANDOM_MAX);
    };
    
    static float randNum(float min, float max)
    {
        return static_cast<float>(min + (((double)arc4random() / ARC4RANDOM_MAX) * (max-min)));
    };
    
    static float randNum(double randNum, float min, float max)
    {
        return static_cast<float>(min + ((randNum / ARC4RANDOM_MAX) * (max-min)));
    };
};

#endif /* defined(__Cheetah__WeatherEnvironment__) */
