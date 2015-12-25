//
//  PopupItem.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 1/22/2557 BE.
//
//

#ifndef __Cheetah__PopupItem__
#define __Cheetah__PopupItem__

#include "cocos2d.h"

USING_NS_CC;

class PopupItem: public cocos2d::CCNode {
    
    int             m_requiredAmount    = 0;
    int             m_itemAmount        = 0;

public:
    
    static PopupItem *create();
    bool init();
    
};
#endif /* defined(__Cheetah__PopupItem__) */
