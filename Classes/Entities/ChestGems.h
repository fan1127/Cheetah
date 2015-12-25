//
//  ChestGems.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 5/7/2557 BE.
//
//

#ifndef __Cheetah__ChestGems__
#define __Cheetah__ChestGems__

#include "cocos2d.h"
#include "cocos-ext.h"
#include <spine/spine-cocos2dx.h>

#include "../VOs/PlayerChestGemsVO.h"

#include "TouchableItem.h"

class ChestGems : public cocos2d::CCNode {
    
    cocos2d::extension::CCSkeletonAnimation *m_spineChest;
    TouchableItem                           *m_touchableItem;
    bool                                    m_didOpen;
    
public:
    
    PlayerChestGemsVO cgVo;
    
    virtual bool init();
    CREATE_FUNC(ChestGems);
    
    void reload();
    void showup(bool reset);
    void prepare();
    void openup();
    void collect(CCObject* obj_);
    
    static void onTouched(void* inst_, void* sender_);
    
};

#endif /* defined(__Cheetah__ChestGems__) */
