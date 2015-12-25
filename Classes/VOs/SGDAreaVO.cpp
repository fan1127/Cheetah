//
//  SGDWorldMap.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 11/28/2556 BE.
//
//

#include "SGDAreaVO.h"

NextArea::NextArea() {
    
    area_id = 0;
    
}

SGDAreaVO::SGDAreaVO() {
    
    area_id = 0;
    type_area = "";
    is_boss = 0;
    next_area.clear();
    x = 0;
    y = 0;
}