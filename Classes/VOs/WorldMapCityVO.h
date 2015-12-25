//
//  WorldMapCityVO.h
//  Cheetah
//
//  Created by Kaweerut on 2/12/57 BE.
//
//

#ifndef __Cheetah__WorldMapCityVO__
#define __Cheetah__WorldMapCityVO__

#include "cocos2d.h"
#include <iostream>

struct WorldMapCityVO {
    
    std::string                     type; //home, city, race, bus
    std::string                     name;
    std::string                     vo_id;
    std::string                     parent_id;
    float                           positionX;
    float                           positionY;
    int                             fansDistribution;
    int                             status; // 0=none, 1=win, 2=lost
    long                            expireTime; // timestamp UTC
    
    std::vector<WorldMapCityVO>     childsVO;
    
    WorldMapCityVO();
    
};

#endif /* defined(__Cheetah__WorldMapCityVO__) */
