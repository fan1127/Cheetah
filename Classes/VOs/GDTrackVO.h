//
//  GDTrackVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/12/56 BE.
//
//

#ifndef __Cheetah__GDTrackVO__
#define __Cheetah__GDTrackVO__

#include <iostream>

#define SPACE_TRACK_GRID_NUM    4

#define GD_TRACK_TYPE_LEN   32
#define GD_TRACK_TITLE_LEN  32
#define GD_TRACK_SRC_LEN    32

#define ABM                 0.95f

struct GDTrackVO {
  
    std::string     type;
    std::string     track_type;
    std::string     title;
    std::string     description;
    unsigned int    point_used;
    unsigned int    unlocks_at_level;
    std::string     src;
    float           param_1;
    float           param_2;
    float           param_3;
    float           param_4;
    
    GDTrackVO();
    
};

#endif /* defined(__Cheetah__GDTrackVO__) */
