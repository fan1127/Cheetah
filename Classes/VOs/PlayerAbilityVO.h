//
//  PlayerAbilityVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 4/7/2557 BE.
//
//

#ifndef __Cheetah__PlayerAbilityVO__
#define __Cheetah__PlayerAbilityVO__

#include <iostream>
#include "GDAbilityVO.h"

struct PlayerAbilityVO {
    
    std::string     auid;
    std::string     type;
    int             level;
    std::string     status;
    unsigned int    timestamp;
    
    PlayerAbilityVO();
    PlayerAbilityVO(GDAbilityVO vo_);
    
};

#endif /* defined(__Cheetah__PlayerAbilityVO__) */
