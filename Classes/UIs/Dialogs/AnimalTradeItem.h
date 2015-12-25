//
//  AnimalTradeItem.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/27/2557 BE.
//
//

#ifndef __Cheetah__AnimalTradeItem__
#define __Cheetah__AnimalTradeItem__

#include "cocos2d.h"
#include "../../VOs/GDItemVO.h"
#include "../../VOs/CommonStruct.h"
#include "../../Extensions/CCNikScrollViewButton.h"
#include "../../Entities/PlayerState.h"

class AnimalTradeItem : public CCNikScrollViewButton  {
    
    bool                m_complete;
    PlayerState*        m_playerState;
    
public:
    
    GDItemVO            itemVO;
    IntKeyValue         item;
    
    static AnimalTradeItem *create(GDItemVO itemVO_, IntKeyValue item_, cocos2d::CCSize size, PlayerState* playerState_);
    
    bool init(GDItemVO itemVO_, IntKeyValue item_, cocos2d::CCSize size, PlayerState* playerState_);
    bool isComplete();
    
};

#endif /* defined(__Cheetah__AnimalTradeItem__) */
