//
//  PlayerStateVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/11/56 BE.
//
//

#ifndef __Cheetah__PlayerStateVO__
#define __Cheetah__PlayerStateVO__

#include <stdio.h>

#define PLAYER_UUID_LEN     64
#define PLAYER_FBID_LEN     64
#define PLAYER_AVID_LEN     64
#define PLAYER_NAME_LEN     32
#define PLAYER_PHOTOURL_LEN 128

#include "../Entities/IsoMap.h"

struct PlayerInfoVO {
    
    std::string name;
    std::string photo;
    std::string email;
    
    int free_gacha;
    
    unsigned int last_being_attacked;
    
    unsigned int merchant_timestamp;
    std::string merchant_items;
    
    int win_streak;
    int highest_win_streak;
    int total_races;
    int total_wins;

    bool bot;
    
    PlayerInfoVO();
    
};

struct PlayerBalanceVO {
    
    int gem;
    int gold;//coin
    int food;//cheese
    int fan; //popularity
    int _fan_changed;
    
    PlayerBalanceVO();
    
};

struct PlayerRacingVO {
    
    unsigned int         exp;
    unsigned int         level;
    
    bool        status;
    int         start;
    bool        direction;
    std::string host;
    
    int _track_points;
    int _track_points_limit;
    
    //from 0.0 to 1.0 //0.0 == easiest //1.0 == hardest
    float ai_difficulty;
    
    PlayerRacingVO();
    
};

struct PlayerStateVO {
    
    std::string uuid;
    std::string fbid;
    std::string gcid;
    std::string avid;
    std::string token;
    
    PlayerInfoVO info;
    PlayerBalanceVO balance;
    PlayerRacingVO racing;
    
    PlayerStateVO();
    
};

#endif /* defined(__Cheetah__PlayerStateVO__) */
