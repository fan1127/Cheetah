//
//  PlayerPetVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/8/56 BE.
//
//

#ifndef __Cheetah__PlayerPetVO__
#define __Cheetah__PlayerPetVO__

#include <iostream>
#include "GDPetVO.h"

struct PlayerPetVO {
    
    std::string puid;
    std::string type;
    float exp;
    unsigned int level;
    unsigned int top_speed;
    unsigned int corner_speed;
    unsigned int jumping;
    unsigned int race_entered;
    unsigned int wins;
    
    float hunger;
    unsigned int hunger_timestamp;
    float energy;
    unsigned int energy_timestamp;
    int coins;
    unsigned int coins_timestamp;
    
    std::string located_in;
    unsigned int updated;

    std::string helper_pet;
    
    PlayerPetVO();
    
};

#endif /* defined(__Cheetah__PlayerPetVO__) */
