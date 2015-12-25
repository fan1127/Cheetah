//
//  Rank.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/24/2557 BE.
//
//

#include "Rank.h"

//extern CCPoint screenFactor;
extern float sizeMultiplier;
extern float retinaMultiplier;

#include "../Entities/Pet.h"
#include "../Extensions/CCNikScrollView.h"

Rank* Rank::create(CCNode *parent)
{
    Rank *obj = new Rank();
    if (!obj->init(parent)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
}

bool Rank::init(CCNode *parent)
{
    CCSize _size = CCSizeMake(125.0f*sizeMultiplier, 114.0f*sizeMultiplier);
    if (!CCPopOverDialog::init(_size, CCPopoverArrowDirectionUp)) {
        return false;
    }
    
    m_parent = parent;
    
    setAutoDismiss(false);
    m_rankSprite = CCSprite::create();
    m_rankSprite->setPosition(this->getContentSize()*0.5f);
    this->addChild(m_rankSprite);
    this->cocos2d::CCNode::setVisible(false);
    
    this->scheduleUpdate();
    
    return true;
}

void Rank::update(float delta)
{
    CCNikScrollView *scrollView = (CCNikScrollView*)m_pet->getParent()->getParent()->getParent();
    
    float scale = scrollView->getScale();
    
    //CCPoint root = m_pet->petSpine->getBonePosition("root");
    //CCPoint bone1_center = m_pet->petSpine->getBonePosition("bone1_center");
    //CCPoint bone_body = m_pet->petSpine->getBonePosition("bone_body");
    //CCPoint bone_head_center = m_pet->petSpine->getBonePosition("bone_head_center");
    //CCPoint bone_head = m_pet->petSpine->getBonePosition("bone_head");
    
//    CCLog_("----------");
//    DEBUG_FLOAT(scale);
//    CCLog_("----------");
//    DEBUG_POINT_XY(root);
//    DEBUG_POINT_XY(bone1_center);
//    DEBUG_POINT_XY(bone_body);
//    DEBUG_POINT_XY(bone_head_center);
//    DEBUG_POINT_XY(bone_head);
//    CCLog_("----------");

    //CCPoint pointOnHead = root + bone1_center + bone_body + bone_head_center + bone_head;
    
    float xMult = HEAD_X_PERCENT;
    if (m_pet->isFlip()) xMult = -HEAD_X_PERCENT;
    
    //this->setPositionAdjustment(ccp(pointOnHead.x * xMult, pointOnHead.y)
                                //* scale * sizeMultiplier
                                //+ ccp(0.0f, RANK_OFFSET_Y)
                                //* scale * sizeMultiplier
                                //);
    
    this->presentPopover(m_parent, m_pet, false);
}


void Rank::changeRank(Pet *pet, RankType type, float duration)
{
    this->setRankType(type);
    
    this->animateRank(pet, type, duration);
}

void Rank::animateRank(Pet *pet, RankType type, float duration)
{
    m_pet = pet;
    
    this->setVisible(true);
    this->stopAllActions();
    this->presentPopover(m_parent, pet, true);
    
    if (type == RankTypeYou) {
        m_rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("oldversion/_icon_you.png"));
    }
    
    this->runAction(CCSequence::create(CCDelayTime::create(duration),
                                       CCCallFunc::create(this, callfunc_selector(Rank::changeRankFinished)),
                                       NULL));
}

void Rank::changeRankFinished()
{
    this->dismissPopover(true, false);
}

void Rank::setRankType(RankType type)
{
    if (m_rankType == type) return;
    m_rankType = type;
    
    switch (type) {
        case RankTypeFirst:{
            m_rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("oldversion/_icon_position1.png"));
        }
            break;
        case RankTypeSecond:{
            m_rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("oldversion/_icon_position2.png"));
        }
            break;
        default:
            break;
    }
}
