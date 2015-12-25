//
//  APIAchievementClaim.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/10/2557 BE.
//
//

#include "APIAchievementClaim.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIAchievementClaim *APIAchievementClaim::create(std::string checkkey,
                                                 void *sender,
                                                 void (*callback)(void*,std::string)) {
    
    APIAchievementClaim *pRet = new APIAchievementClaim();
    pRet->m_checkkey = checkkey;
    pRet->setCallback(sender, callback);
    pRet->autorelease();
    return pRet;
    
}

string APIAchievementClaim::name() {
    
    return "achievement_claim";
    
}

string APIAchievementClaim::param() {
    
    return PlayerState::getInstance()->getUserParam() + "&checkkey=" + m_checkkey;
    
}
