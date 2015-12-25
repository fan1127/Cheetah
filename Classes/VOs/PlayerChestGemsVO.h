//
//  PlayerChestGemsVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 5/7/2557 BE.
//
//

#ifndef __Cheetah__PlayerChestGemsVO__
#define __Cheetah__PlayerChestGemsVO__

#include <iostream>

struct PlayerChestGemsVO {
    
    std::string uuid;
    std::string guid;
    std::string locate; //values are worldmap & ville
    int gems;
    float posx;
    float posy;
    unsigned int timestamp;
    
    PlayerChestGemsVO();
    
};

#endif /* defined(__Cheetah__PlayerChestGemsVO__) */
