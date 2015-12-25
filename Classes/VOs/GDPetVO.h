//
//  GDPetVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/10/56 BE.
//
//

#ifndef __Cheetah__GDPetVO__
#define __Cheetah__GDPetVO__

#include "cocos2d.h"
#include "CommonStruct.h"

struct GDPetVO {
    
    std::string type;
    std::string title;
    int rarity_stars;
    int max_level;
    int base_top_speed;
    int base_corner_speed;
    int base_jumping;
    int evolution_top_speed;
    int evolution_corner_speed;
    int evolution_jumping;
    std::string track_preference;
    std::vector<FloatKeyValue> next_evolution;
    int exp_multiplier_for_exp_curve;
    std::vector<IntKeyValue> items_needed_for_fusion;
    std::vector<IntKeyValue> items_needed_to_evolve;
    int coins_needed_for_fusion;
    int coins_needed_to_evolve;
    std::vector<std::string> item_drops;
    int sell_value;
    float hunger_rate;
    float mvhm;
    float energy_rate;
    float cprmin;
    float cprmax;
    int mvcmin;
    int mvcmax;
    int unlocks_at_level;
    std::string src;
    std::string sound_src;
    int gacha_distribution;
    
    GDPetVO();
    
};

#endif /* defined(__Cheetah__GDPetVO__) */
