//
//  HistoryItemVO.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/13/2557 BE.
//
//

#include "HistoryItemVO.h"
using namespace std;

HistoryUserVO::HistoryUserVO() {
    
    uuid    = "";
    level   = 0;
    name    = "";
    photo   = "";
    pet     = "";
    time    = 0.0f;
    fan     = 0;
    
}

HistoryResultVO::HistoryResultVO() {
    
    host_win            = false;
    bonus               = 0;
    coins_reward        = 0;
    claimed             = false;
    
}

HistoryItemVO::HistoryItemVO() {
    
    race_id     = "";
    created     = "";
    timestamp   = 0;
    
    guest       = HistoryUserVO();
    host        = HistoryUserVO();
    
}