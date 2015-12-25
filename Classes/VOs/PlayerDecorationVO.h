//
//  PlayerDecorationVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/25/2556 BE.
//
//

#ifndef __Cheetah__PlayerDecorationVO__
#define __Cheetah__PlayerDecorationVO__

#include "cocos2d.h"
#include "GDBuildingVO.h"
#include "GDDecorationVO.h"


struct PlayerDecorationVO {
    
    std::string         duid;
    std::string         type;
    bool                flip;
    int                 map[BUILDING_SPACE_GRID_NUM];
    std::string         status;
    unsigned int        updated; // when use with Fixed Decoration, this one stands for "level"
    
    PlayerDecorationVO();
    void emptyMap();
    
};

#endif /* defined(__Cheetah__PlayerDecorationVO__) */
