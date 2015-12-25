//
//  GDDecorationVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/12/56 BE.
//
//

#ifndef __Cheetah__GDDecorationVO__
#define __Cheetah__GDDecorationVO__

#include <string>

struct GDDecorationVO {
    
    std::string     type;
    std::string     decoration_type; //unwanted = ตัดได้
    std::string     title;
    std::string     description;
    unsigned int    cost_coins;
    unsigned int    cost_gems;
    int             in_shop;
    unsigned int    grid_col;
    unsigned int    grid_row;
    bool            movable;
    std::string     src;
    unsigned int    src_width;
    unsigned int    src_height;
    unsigned int    base_popularity;
    unsigned int    unlocks_at_level;
    int             maximum;
    bool            ground;
    
    GDDecorationVO();
    
};

#endif /* defined(__Cheetah__GDDecorationVO__) */
