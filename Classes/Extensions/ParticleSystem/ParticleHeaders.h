//
//  ParticleSystem.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 3/21/2557 BE.
//
//

#ifndef __Cheetah__ParticleSystem__
#define __Cheetah__ParticleSystem__

#include "cocos2d.h"

#if !defined(CG_INLINE)
# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  define CG_INLINE static inline
# elif defined(__MWERKS__) || defined(__cplusplus)
#  define CG_INLINE static inline
# elif defined(__GNUC__)
#  define CG_INLINE static __inline__
# else
#  define CG_INLINE static
# endif
#endif /* !defined(CG_INLINE) */


#define kEaseLinear 0.0f
#define kEaseOut 0.9f
#define kEaseIn 1.1f

#define kMinimumScale 0.01f

#define kAmountOfTracksToVisitorsRatio 0.5f

#define kParticleTypeRunSmoke1Amount 200

class ParticleCache;
class Particle;
class ParticleEmitter;

typedef enum {
    ParticleTypeNothing,
    ParticleTypeRunningSmokeUp,
    ParticleTypeRunningSmokeDown,
    ParticleTypeRunSmoke1,
    ParticleTypeRunSmoke2
} ParticleType;


//PARTICLESYSTEM.H ============================================


class ParticleSystem: public cocos2d::CCNode {
    //ParticleCache *starExplosionCache_, *starExplosionGravityCache_;
    //ParticleCache *starDustCache_, *triangleCache_;
    
    
public:
    cocos2d::CCSpriteBatchNode *batch_;
    cocos2d::CCDictionary *cacheStore_;
    
    
    /** returns a shared instance of the director */
    static ParticleSystem* sharedSystem();
    bool init();
    ~ParticleSystem();
    
    
    void setInitialValues();
    void pauseAllParticles();
    void resumeAllParticles();
    void removeAllParticles();
    
    void addParticleCache(ParticleType particleType, int particleAmount);
    ParticleCache* getParticleCache(ParticleType particleType);
    
    //virtual void setBl
    
    
    //-(ParticleCache*)getParticleCache:(ParticleType)type;
    
    //-(void)setBlendFunc:(ccBlendFunc)blendFunc;
    //-(CCSpriteBatchNode*)batch;
};


//PARTICLECACHE.H ============================================

class ParticleCache: public cocos2d::CCArray {
    
public:
    ParticleType particleType_;
    int nextInactiveParticle_;
    
    static ParticleCache* create(ParticleType particleType, int amount);
    void addParticles(ParticleType particleType, int amount);
    void removeParticles(int amount);
    Particle* getParticle();
};


//PARTICLEEMITTER.H ============================================

#define FIRST_EMISSION -1.0f

typedef enum {
    EmitterTypeSelfEmitting,
    EmitterTypeEmitOnce
} EmitterType;

class ParticleEmitter: public cocos2d::CCNode {
    
public:
    float emissionRate_;
    float emissionRateRandomRange_;
    float emissionAge_;
    ParticleType particleType_;
    ParticleCache *cache_;
    int particleAmount_;
    EmitterType emitterType_;
    
    bool paused_;
    
    static ParticleEmitter* create(EmitterType type, ParticleCache* cache, float emissionRate, float emissionRateRandomRange, int particleAmount);
    void start();
    void stop();
    void emitOnce();
    void emitsOneParticle();
    
    virtual void update(float delta);
};



//PARTICLE.H ============================================


typedef struct {
    float value;
    float valueVar;
    float rValue;
} PSValue;

typedef struct {
    PSValue x;
    PSValue y;
} PSPoint;

typedef struct {
    PSValue r;
    PSValue g;
    PSValue b;
} PSColor;

CG_INLINE PSValue
PSValueMake(float value, float valueVar, float rValue)
{
    PSValue p;
    p.value = value;
    p.valueVar = valueVar;
    p.rValue = rValue;
    return p;
}
#define psv0(__value__,__valueVar__) PSValueMake(__value__,__valueVar__,0.0f)
#define psv00(__value__) PSValueMake(__value__,0.0f,0.0f)
#define psv(__value__,__valueVar__,__rValue__) PSValueMake(__value__,__valueVar__,__rValue__)

#define GET_AND_SET_RVALUE_FROM_PSVALUE(__rValue__, __psValue__) __rValue__ = this->getRValueFromPSValue(__psValue__);

typedef struct {
    PSPoint start;
    PSPoint end;
    bool animate;
    bool autoAngle;
    float age;
    float ageSpeed;
    float speed;
    float ease;
} PSPointAnimation;

typedef struct {
    PSColor start;
    PSColor end;
    bool animate;
    float age;
    float ageSpeed;
    float speed;
    float ease;
} PSColorAnimation;

typedef struct {
    PSValue start;
    PSValue end;
    bool animate;
    float speed;
    float age;
    float ageSpeed;
    float ease;
} PSValueAnimation;

typedef struct {
    PSPoint linearVelocity;
    PSPoint maxLinearVelocity;
    PSPoint minLinearVelocity;
    PSPoint linearAcceleration;
    PSValue angularVelocity;
    PSValue angularAcceleration;
    //PSValue force;
    //PSValue impulse;
    float mass;
    bool enable;
} PSPhysicsAnimation;


class Particle: public cocos2d::CCSprite {
    
public:
    ParticleType particleType_;
    ParticleType particleTypeNext_;
    int stage_;
    float age_;
    float lifeLeft_;
    PSValue life_;
    
    PSColorAnimation color__;
    PSPointAnimation position__;
    PSValueAnimation scale__, opacity__, rotation__;
    PSPhysicsAnimation physics_;
    
    static Particle* create(ParticleType particleType);
    void setInitialValues(ParticleType particleType);
    void setParticleType(ParticleType particleType);
    void resetValues();
    void start();
    void stop();
    float getRValueFromPSValue(PSValue psValue);
    float getNewValueFromPSValueAnimation(PSValueAnimation anim);
    cocos2d::CCPoint getNewCCPointFromPSPointAnimation(PSPointAnimation anim);
    cocos2d::ccColor3B getNewColorFromPSColorAnimation(PSColorAnimation anim);
    void remove();
    void destroy();
    
    virtual void update(float delta);
};

class ParticleClone: public cocos2d::CCSprite {
    
    
public:
    Particle *cloneTarget_;
    cocos2d::CCPoint shadowOffset_;
    float shadowOpacityMult_;
    
    
    static ParticleClone* create(Particle *particle, cocos2d::CCPoint shadowOffset, float shadowOpacity);
    void setInitialValues(Particle *particle, cocos2d::CCPoint shadowOffset, float shadowOpacity);
    
    virtual void update(float delta);
};




#endif /* defined(__Cheetah__ParticleSystem__) */
