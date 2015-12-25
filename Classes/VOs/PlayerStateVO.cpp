//
//  PlayerStateVO.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/11/56 BE.
//
//

#include "PlayerStateVO.h"

PlayerInfoVO::PlayerInfoVO() {
    
    name = "";
    photo = "";
    email = "";
    
    free_gacha         = 0;
    
    last_being_attacked = 0;
    
    merchant_timestamp  = 0;
    merchant_items      = "";

    total_races         = 0;
    win_streak          = 0;
    highest_win_streak  = 0;
    total_wins          = 0;
    bot                 = false;
    
}

PlayerBalanceVO::PlayerBalanceVO() {
    
    gem             = 0;
    gold            = 0;
    food            = 0;
    fan             = 0;
    _fan_changed    = 0;
    
}

PlayerRacingVO::PlayerRacingVO() {
    
    exp = 0;
    level = 0;
    
    status = false;
    start = -1;
    direction = false;
    host = "";
    
    _track_points = 0;
    _track_points_limit = 0;
    
    ai_difficulty = 0.0f;
    
}

PlayerStateVO::PlayerStateVO() {

    uuid = "";
    fbid = "";
    avid = "";
    token = "";

    balance = PlayerBalanceVO();
    racing = PlayerRacingVO();

}
