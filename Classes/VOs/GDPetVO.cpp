//
//  GDPetVO.cpp
//  Cheetah
//d
//  Created by Plus Pingya Avalable on 9/10/56 BE.
//
//

#include "GDPetVO.h"

GDPetVO::GDPetVO() {
    
    type                            = "";
    title                           = "";
    rarity_stars                    = 0;
    max_level                       = 0;
    base_top_speed                  = 0;
    base_corner_speed               = 0;
    base_jumping                    = 0;
    evolution_top_speed             = 0;
    evolution_corner_speed          = 0;
    evolution_jumping               = 0;
    track_preference                = "";
    next_evolution.clear();
    exp_multiplier_for_exp_curve    = 0;
    items_needed_for_fusion.clear();
    items_needed_to_evolve.clear();
    coins_needed_for_fusion         = 0;
    coins_needed_to_evolve          = 0;
    item_drops.clear();
    sell_value                      = 0;
    hunger_rate                     = 0.0f;
    mvhm                            = 0.0f;
    energy_rate                     = 0.0f;
    
    cprmin                          = 0.0f;
    cprmax                          = 0.0f;
    mvcmin                          = 0;
    mvcmax                          = 0;
    
    unlocks_at_level                = 0;
    src                             = "";
    sound_src                       = "";
    gacha_distribution              = 0;
    
}