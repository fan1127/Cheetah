//
//  GDGameStartBarVO.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 3/18/2557 BE.
//
//

#ifndef __Cheetah__GDGameStartBarVO__
#define __Cheetah__GDGameStartBarVO__

#include "cocos2d.h"
#include <iostream>

struct GSBRange {
    float from;
    float to;
    
    GSBRange();
};

struct GDGameStartBarVO {
    
    std::string     type;
    std::vector<GSBRange> good_start;
    std::vector<GSBRange> normal_start;
    float           speed;
    unsigned int    min_fans;
    
    GDGameStartBarVO();
    
};

#endif /* defined(__Cheetah__GDGameStartBarVO__) */
