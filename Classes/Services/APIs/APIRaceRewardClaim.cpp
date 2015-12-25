//
//  APIRaceRewardClaim.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIRaceRewardClaim.h"
using namespace std;

APIRaceRewardClaim *APIRaceRewardClaim::create(std::string race_id,
                                               void *sender,
                                               void (*callback)(void *, std::string)) {
    
    APIRaceRewardClaim *p = new APIRaceRewardClaim();
    p->m_race_id = race_id;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIRaceRewardClaim::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&race_id=" + m_race_id;
    
    return _params;
    
}