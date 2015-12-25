//
//  AnimalMarketItem.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/26/2557 BE.
//
//

#ifndef __Cheetah__AnimalMarketItem__
#define __Cheetah__AnimalMarketItem__

#include "cocos2d.h"
#include "../../Extensions/CCNikScrollViewButton.h"

#include "../../VOs/GDPetVO.h"
#include "../../Entities/Pet.h"
#include "../../Entities/PlayerState.h"

#define ANIMAL_IS_AT_MAX_LEVEL (pet_->ppVo.level >= pet_->pVo.max_level)
#define NEXT_EVO_IS_UNKNOWN (pet_->pVo.next_evolution.size() > 1)

#define PET_NODE_TAG 12

class AnimalMarketItem : public CCNikScrollViewButton {

    PlayerState*    m_playerState;
    bool            m_questionMark;
    CCLabelBMFont *needMarketLvLabel;
    CCSprite *horsePower;
    CCSprite *spriteGroup;
    
    CCNode          *m_group;
    
public:
    
    GDPetVO         vo;
    Pet             *pet;
    
    bool            canTrade;
    
    static AnimalMarketItem *create(PlayerState* playerState_, GDPetVO vo_, Pet *pet_, bool isSpine_);
    static AnimalMarketItem *create(PlayerState* playerState_, GDPetVO vo_, Pet *pet_, bool mask_, bool isSpine_);
    static AnimalMarketItem *create(PlayerState* playerState_, GDPetVO vo_, Pet *pet_, bool mask_, bool isSpine_, bool questionMark_);

    bool init(PlayerState* playerState_, GDPetVO vo_, Pet *pet_, bool mask_, bool isSpine_, bool questionMark_);
    
    virtual void update(float delta_);
    
    void animateIdleWithDelay(float delay_);
    void animateIdle();
};

#endif /* defined(__Cheetah__AnimalMarketItem__) */
