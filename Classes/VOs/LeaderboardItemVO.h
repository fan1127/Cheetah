//
//  LeaderboardItemVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 5/9/2557 BE.
//
//

#ifndef __Cheetah__LeaderboardItemVO__
#define __Cheetah__LeaderboardItemVO__

#include <iostream>

struct LeaderboardItemVO {
    
    std::string uuid;
    std::string name;
    std::string photo;
    int highest_win_streak;
    int total_wins;
    int score;
    std::string host_type;
    int level;
    
    LeaderboardItemVO();
    
};

#endif /* defined(__Cheetah__LeaderboardItemVO__) */
