//
//  Rank.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/24/2557 BE.
//
//

#ifndef __Cheetah__Rank__
#define __Cheetah__Rank__

#include "cocos2d.h"
#include "../Extensions/CCPopOverDialog.h"

#define RANK_OFFSET_Y (40.0f*sizeMultiplier)//(60)//120.0f
#define HEAD_X_PERCENT 0.6f

class Pet;

typedef enum
{
    RankTypeNone,
    RankTypeFirst,
    RankTypeSecond,
    RankTypeYou,
} RankType;

USING_NS_CC;

class  Rank: public CCPopOverDialog {
    
    RankType m_rankType;
    CCSprite *m_rankSprite;
    
    Pet *m_pet;
    
    CCNode *m_parent;
    
public:
    
    static Rank* create(CCNode *parent);
    virtual bool init(CCNode *parent);
    
    virtual void update(float delta);
    
    RankType getRankType() { return m_rankType; };
    void changeRank(Pet *pet, RankType type, float duration);
    void animateRank(Pet *pet, RankType type, float duration);
    void setRankType(RankType type);
    void changeRankFinished();
};


#endif /* defined(__Cheetah__Rank__) */
