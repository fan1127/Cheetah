//
//  PlayerBuildingVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#ifndef __Cheetah__PlayerBuildingVO__
#define __Cheetah__PlayerBuildingVO__

#include "cocos2d.h"
#include "GDBuildingVO.h"

struct PlayerBuildingVO {
    
    std::string         buid;
    std::string         puid; //This one will be used only with habitat type
    std::string         type;
    bool                flip;
    int                 map[BUILDING_SPACE_GRID_NUM];
    int                 level;
    std::string         status;
    unsigned int        timestamp;

    PlayerBuildingVO();
    bool parse(void* json);
    
};


#endif /* defined(__Cheetah__PlayerBuildingVO__) */
