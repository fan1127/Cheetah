//
//  GachaItem.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/24/2557 BE.
//
//

#ifndef __Cheetah__GachaItem__
#define __Cheetah__GachaItem__

#include "cocos2d.h"

#include "../VOs/GDPetVO.h"
#include "../VOs/GDItemVO.h"

typedef enum {
    GachaTypeNumberEven,
    GachaTypeNumberOdd,
}GachaTypeNumber;

typedef enum {
    GachaTypeFree,
    GachaTypePremium,
}GachaType;

class GachaItem: public cocos2d::CCNode {
    
    GachaType m_gachaType;
    
    CCNode *contentNode;
    CCNode *itemContentNode;
    
    int m_index;
    
    int m_randomedRound;
    bool m_didChangeItem;
    
    cocos2d::CCSprite *petSprite;
    
public:
    
    static GachaItem* create(GachaType type, GachaTypeNumber typeNumber, float height, int index);
    virtual bool init(GachaType type, GachaTypeNumber typeNumber, float height, int index);
    
    void updateRotation(float updateRotation, int selectedIndex, GDPetVO selectedPetVO, GDItemVO selectedItemVO, float remainingRotation);
    virtual void setRotation(float fRotation);
    
    void randomPetVO();
    void changePetVO(GDPetVO petVO);
    
    void randomItemVO();
    void changeItemVO(GDItemVO itemVO);

    
};

#endif /* defined(__Cheetah__GachaItem__) */
