//
//  GDWorldMapVillVO.h
//  Cheetah
//
//  Created by Teerapat on 18/3/57 BE.
//
//

#ifndef __Cheetah__GDWorldMapVillVO__
#define __Cheetah__GDWorldMapVillVO__

//#include "cocos2d.h"
#include <iostream>

struct GDWorldMapVillVO {
    
    std::string                     type; //home, enemy, territory
    std::string                     name;
    bool                            active;
    int                             tag;
    int                             cloud_overlay;
    int                             max_overlay;
    
    GDWorldMapVillVO();
    
};

#endif /* defined(__Cheetah__GDWorldMapVillVO__) */
