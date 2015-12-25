//
//  EllipseParicle.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 5/5/2557 BE.
//
//

#include "EllipseParicle.h"

EllipseParicle* EllipseParicle::create(cocos2d::CCSprite *paticleSprite)
{
    EllipseParicle *obj = new EllipseParicle();
    if (!obj->init(paticleSprite)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}


bool EllipseParicle::init(cocos2d::CCSprite *paticleSprite)
{
    if (!CCNode::init()) {
        return false;
    }
    
    m_time = 0;
    m_restartDuration = (arc4random()%30)/100.f;
    
    m_directionType = EllipseParicleDirectionTypeIncrease;
    
    m_paticleSprite = paticleSprite;
    
//    Scale
    
    m_originalScale = m_paticleSprite->getScale();
    m_maximumScale = 1*m_originalScale;
    m_minimumScale = 0.1f*m_originalScale;
    m_coefficientScale = 1;
    
//    Opacity
    
    m_maximumOpacity = 255;
    m_minimumOpacity = 0.f;
    m_currentOpacity = 1;
    m_coefficientOpacity = 1;

//    Transition
    
    currentPoint = CCPointZero;
    m_coefficientTransition = 1;

    
    this->setContentSize(CCSizeMake(m_a*2, m_b*2));
    m_paticleSprite->setPosition(this->getContentSize()*0.5f);
    this->addChild(m_paticleSprite);
    
    return true;
}

void EllipseParicle::randomPaticleValue()
{
    m_a = 30 + arc4random()%(int)m_onNode->getContentSize().width*0.8f;
    m_b = 30 + arc4random()%(int)m_onNode->getContentSize().height*0.8f;
    
    m_time = 0;
    m_restartDuration = 1 + (arc4random()%100)/100.f;
    
    //    Scale
    
    m_maximumScale = (arc4random()%101)/100.f*m_originalScale;
    m_minimumScale = MIN((arc4random()%101)/100.f*m_originalScale, m_maximumScale);
    m_coefficientScale = arc4random()%2 ? 1 : -1;
    
    //    Opacity
    
    m_maximumOpacity = (arc4random()%256);
    m_minimumOpacity = MIN(arc4random()%256, m_maximumOpacity);
    m_coefficientOpacity = arc4random()%2 ? 1 : -1;
    
    //    Transition
    
    m_coefficientTransition = arc4random()%2 ? 1 : -1;
    
    this->setRotation(arc4random()%360);
    this->setPosition(ccp(arc4random()%(int)m_onNode->getContentSize().width, arc4random()%(int)m_onNode->getContentSize().height));

}

void EllipseParicle::startPaticleRandom(cocos2d::CCNode *onNode)
{
    
    m_onNode = onNode;
    m_onNode->addChild(this);
    
    randomPaticleValue();
    
    scheduleUpdate();
}

void EllipseParicle::update(float delta)
{
    
    m_time += delta;
    
//    Rotate
    
    m_paticleSprite->setRotation(m_paticleSprite->getRotation() + delta*30);
    
//    Scale
    
    m_currentScale += delta*0.1f*m_coefficientScale;
    
    m_paticleSprite->setScale(m_currentScale);
    
    if (m_currentScale > m_maximumScale) {
        m_currentScale = m_maximumScale;
        m_coefficientScale *= -1;
    } else if (m_currentScale < m_minimumScale) {
        m_currentScale = m_minimumScale;
        m_coefficientScale *= -1;
    }
    
    
//    Opacity
    
    m_currentOpacity += delta*20*m_coefficientOpacity;
    
    m_paticleSprite->setOpacity(m_currentOpacity);
    
    if (m_currentOpacity > m_maximumOpacity) {
        m_currentOpacity = m_maximumOpacity;
        m_coefficientOpacity *= -1;
    } else if (m_currentOpacity < m_minimumOpacity) {
        m_currentOpacity = m_minimumOpacity;
        m_coefficientOpacity *= -1;
    }
    
//    Transition

    if (m_directionType == EllipseParicleDirectionTypeIncrease) {
        currentPoint.x += delta*60;
    } else {
        currentPoint.x -= delta*60;
    }
    
    
    if (currentPoint.x >= m_a) {
        currentPoint.x = m_a;
        m_coefficientTransition *= -1;
        m_directionType = EllipseParicleDirectionTypeDecrease;
    } else if (currentPoint.x <= -m_a){
        currentPoint.x = -m_a;
        m_coefficientTransition *= -1;
        m_directionType = EllipseParicleDirectionTypeIncrease;
    }
    
    currentPoint.y = m_coefficientTransition*sqrtf((1 - powf(currentPoint.x/m_a, 2))*powf(m_b, 2));
    
    m_paticleSprite->setPosition(this->getContentSize()*0.5f + currentPoint);
    
    
    if (m_time > m_restartDuration) {
        randomPaticleValue();
    }
}