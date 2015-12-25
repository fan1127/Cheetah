//
//  GDAbilityVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 4/3/2557 BE.
//
//

#ifndef __Cheetah__GDAbilityVO__
#define __Cheetah__GDAbilityVO__

#include <iostream>

struct GDAbilityVO {
    
    std::string     type;
    int             level;
    unsigned int    cost_coins;
    unsigned int    time;
    unsigned int    starting_cost;
    unsigned int    increasing_cost;
    unsigned int    adding_increasing_cost;
    float           stun_duration;
    std::string     src;
    unsigned int    unlocks_at_level;
    unsigned int    village_exp_gain;
    float           param_1;
    float           param_2;
    float           param_3;
    std::string     title;
    std::string     description;
    
    GDAbilityVO();
    
};

#endif /* defined(__Cheetah__GDAbilityVO__) */
