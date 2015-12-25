//
//  PlayerAbilityVO.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 4/7/2557 BE.
//
//

#include "PlayerAbilityVO.h"

PlayerAbilityVO::PlayerAbilityVO() {
    
    auid        = "";
    type        = "";
    level       = 0;
    status      = "";
    timestamp   = 0;
    
}

PlayerAbilityVO::PlayerAbilityVO(GDAbilityVO vo_) {
    
    auid        = "";
    type        = vo_.type;
    level       = vo_.level;
    status      = "";
    timestamp   = 0;
    
}
