//
//  HistoryItemVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/13/2557 BE.
//
//

#ifndef __Cheetah__HistoryItemVO__
#define __Cheetah__HistoryItemVO__

#include <iostream>

struct HistoryUserVO {
    
    std::string uuid;
    int         level;
    std::string name;
    std::string photo;
    std::string pet;
    float       time;
    int         fan;

    HistoryUserVO();
    
};

struct HistoryResultVO {
    
    bool    host_win;
    int     bonus;
    int     coins_reward;
    int     gems_reward;
    bool    claimed;
    
    HistoryResultVO();
    
};

struct HistoryItemVO {
    
    std::string race_id;
    std::string created;
    unsigned int timestamp;
    
    HistoryUserVO   guest;
    HistoryUserVO   host;
    HistoryResultVO result;
    
    HistoryItemVO();
    
};

#endif /* defined(__Cheetah__HistoryItemVO__) */
