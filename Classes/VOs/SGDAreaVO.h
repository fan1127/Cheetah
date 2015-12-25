//
//  SGDWorldMap.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 11/28/2556 BE.
//
//

#ifndef __Cheetah__SGDAreaVO__
#define __Cheetah__SGDAreaVO__

#include "cocos2d.h"

struct NextArea {
    
    unsigned int area_id;
    
    NextArea();
};

struct SGDAreaVO {

    unsigned int area_id;
    std::string type_area;
    bool is_boss;
    std::vector<NextArea> next_area;
    int x;
    int y;
    SGDAreaVO();
    
};

#endif /* defined(__Cheetah__SGDWorldMap__) */
