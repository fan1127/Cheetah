//
//  PlayerWorldMapVillageVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/25/2557 BE.
//
//

#ifndef __Cheetah__PlayerWorldMapVillageVO__
#define __Cheetah__PlayerWorldMapVillageVO__

#include <iostream>

#define kPWMVStatus_Empty   "empty"
#define kPWMVStatus_Other   "other"
#define kPWMVStatus_Own     "own"

struct PlayerWorldMapVillageVO {
    
    unsigned int        village_id;
    std::string         uuid;
    std::string         name;
    std::string         photo;
    unsigned int        horse_power;
    std::string         status;
    unsigned int        timestamp;
    unsigned int        wait;
    unsigned int        bot;
    std::string         host_type;
    
    PlayerWorldMapVillageVO();
    
};

#endif /* defined(__Cheetah__PlayerWorldMapVillageVO__) */
