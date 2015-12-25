//
//  PlayerDecorationVO.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/25/2556 BE.
//
//

#include "PlayerDecorationVO.h"
USING_NS_CC;

PlayerDecorationVO::PlayerDecorationVO() {
    
    duid                = "";
    type                = "";
    flip                = false;
    emptyMap();
    status              = "";
    updated             = 0;

}

void PlayerDecorationVO::emptyMap() {
    
    for (int i=0; i<BUILDING_SPACE_GRID_NUM; i++) {
        map[i]          = -1;
    }
    
}