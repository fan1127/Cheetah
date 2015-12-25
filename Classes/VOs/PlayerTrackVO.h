//
//  PlayerTrackVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/6/56 BE.
//
//

#ifndef __Cheetah__PlayerTrackVO__
#define __Cheetah__PlayerTrackVO__

#include "cocos2d.h"
#include "GDTrackVO.h"

#define PT_OBJECT_TRACK     "track"
#define PT_OBJECT_OBSTACLE  "obstacle"

struct PlayerTrackVO {
    
    std::string         type;
    int                 no;
    int                 index;
    
    PlayerTrackVO();
    
};

#endif /* defined(__Cheetah__PlayerTrackVO__) */
