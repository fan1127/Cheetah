//
//  PlayerPetVO.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/8/56 BE.
//
//

#include "PlayerPetVO.h"

PlayerPetVO::PlayerPetVO() {
    
    puid                = "";
    type                = "";
    exp                 = 0.0f;
    
    level               = 0;
    top_speed           = 0;
    corner_speed        = 0;
    jumping             = 0;
    race_entered        = 0;
    wins                = 0;
    
    hunger              = 0.0f;
    hunger_timestamp    = 0;
    energy              = 0.0f;
    energy_timestamp    = 0;
    coins               = 0;
    coins_timestamp     = 0;
    
    located_in          = "";
    updated             = 0;

    helper_pet          = "";
}