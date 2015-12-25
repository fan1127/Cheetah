//
//  EllipseParicle.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 5/5/2557 BE.
//
//

#ifndef __Cheetah__EllipseParicle__
#define __Cheetah__EllipseParicle__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

typedef enum {
    EllipseParicleDirectionTypeIncrease,
    EllipseParicleDirectionTypeDecrease,
}EllipseParicleDirectionType;

class EllipseParicle : public CCNode {
    
    EllipseParicleDirectionType m_directionType;
    
    CCSprite *m_paticleSprite;
    
    float m_a;
    float m_b;
    
    float m_originalScale;
    float m_maximumScale;
    float m_minimumScale;
    float m_currentScale;
    float m_coefficientScale;
    
    float m_maximumOpacity;
    float m_minimumOpacity;
    float m_currentOpacity;
    float m_coefficientOpacity;
    
    CCPoint currentPoint;
    float m_coefficientTransition;
    
    float m_time;
    float m_restartDuration;
    
    CCNode *m_onNode;
    
public:
    
    static EllipseParicle *create(CCSprite *paticleSprite);
    virtual bool init(CCSprite *paticleSprite);
    
    void startPaticleRandom(CCNode *onNode);
    void randomPaticleValue();
    
    virtual void update(float delta);
};

#endif /* defined(__Cheetah__EllipseParicle__) */
