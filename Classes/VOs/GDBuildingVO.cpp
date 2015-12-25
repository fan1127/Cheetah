//
//  GDBuildingVO.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/12/56 BE.
//
//

#include "GDBuildingVO.h"

GDBuildingVO::GDBuildingVO() {
    
    type                = "";
    level               = 0;
    building_type       = "";
    title               = "";
    description         = "";
    cost_coins          = 0;
    cost_gems           = 0;
    cost_multiplier     = false;
    time                = 0;
    in_shop             = false;
    grid_col            = 0;
    grid_row            = 0;
    movable             = true;
    stack               = kBuildingStackNormal;
    src                 = "";
    src_width           = 0;
    src_height          = 0;
    unlocks_at_level    = 0;
    param_1             = 0.0f;
    param_2             = 0.0f;
    param_3             = 0.0f;
    param_4             = 0.0f;
    param_5             = 0.0f;
    param_6             = 0.0f;
    
}