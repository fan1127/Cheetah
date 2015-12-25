//
//  ProjectileParticle.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 5/8/2557 BE.
//
//

#include "ProjectileParticle.h"

ProjectileParticle* ProjectileParticle::create(cocos2d::CCSprite *particleSprite)
{
    ProjectileParticle *obj = new ProjectileParticle();
    if (!obj->init(particleSprite)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool ProjectileParticle::init(cocos2d::CCSprite *particleSprite){
 
    if (!CCNode::init()) {
        return false;
    }
    
    m_time = 0;
    
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    
    m_particleSprite = particleSprite;
    addChild(m_particleSprite);
    
    m_defaultScale = particleSprite->getScale();
    
    m_accelerateY = -1500;
    
    return true;
}

ProjectileParticle::~ProjectileParticle()
{
}


void ProjectileParticle::update(float delta)
{
    m_time += delta;
    
//    CCLog_("velocityY : %f", m_velocityY);

//    CCLog_("point : %f , %f", m_currentPoint.x, m_currentPoint.y);
    
    m_currentPoint.x += m_velocityX*delta;
    
    m_currentPoint.y += m_velocityY*delta + 0.5*m_accelerateY*pow(delta, 2);
    
    m_particleSprite->setPosition(m_currentPoint);
    
    m_velocityY = m_velocityY + m_accelerateY*delta;
    
    if (m_time < m_duration*0.2f) {
        m_currentScale += m_velocityScaleUp*delta;
    } else {
        m_currentScale += m_velocityScaleDown*delta;
    }
    
    m_particleSprite->setScale(m_currentScale);
    
    if (m_time > m_duration) {
        this->unscheduleUpdate();
        this->removeFromParent();
    }
}

void ProjectileParticle::animate(cocos2d::CCNode *onNode, cocos2d::CCPoint startPoint, cocos2d::CCPoint endPoint, float duration)
{
    m_time = 0;
    m_duration = duration;
    
    m_startPoint = startPoint;
    m_endPoint = endPoint;
    
    m_currentPoint = startPoint;
    
    m_currentScale = m_defaultScale*0.1f;

    this->setContentSize(onNode->getContentSize());
    this->setPosition(onNode->getContentSize()*0.5f);
    onNode->addChild(this, 100);
    
    m_particleSprite->setPosition(m_startPoint);
    m_particleSprite->setScale(m_currentScale);
    
    m_velocityX = (endPoint.x - startPoint.x)/m_duration;
    m_velocityY = ((endPoint.y - startPoint.y) - 0.5*m_accelerateY*pow(m_duration, 2))/m_duration;
    
    m_velocityScaleUp = (m_defaultScale - m_currentScale)/(m_duration*0.2f);
    m_velocityScaleDown = (m_currentScale - m_defaultScale)/(m_duration*0.8f);

    scheduleUpdate();
}

void ProjectileParticle::animateCollectItem(cocos2d::CCNode *onNode, cocos2d::CCPoint startPoint, cocos2d::CCPoint endPoint, float duration)
{
    m_time = 0;

    m_accelerateY = 500;
    
    m_duration = duration;
    
    m_startPoint = startPoint;
    m_endPoint = endPoint;
    
    m_currentPoint = startPoint;
    
    m_currentScale = m_defaultScale*0.1f;
    
    this->setContentSize(onNode->getContentSize());
    this->setPosition(onNode->getContentSize()*0.5f);
    onNode->addChild(this, 100);
    
    m_particleSprite->setPosition(m_startPoint);
    m_particleSprite->setScale(m_currentScale);
    
    m_velocityX = (endPoint.x - startPoint.x)/m_duration;
    m_velocityY = ((endPoint.y - startPoint.y) - 0.5*m_accelerateY*pow(m_duration, 2))/m_duration;
    
    m_velocityScaleUp = 0;
    m_velocityScaleDown = 0;
    
    scheduleUpdate();
}