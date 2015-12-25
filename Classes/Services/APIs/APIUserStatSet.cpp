//
//  APIUserStatSet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/6/2557 BE.
//
//

#include "APIUserStatSet.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIUserStatSet *APIUserStatSet::create(std::string stats) {
    
    APIUserStatSet *pRet = new APIUserStatSet();
    pRet->m_stats = stats;
    pRet->m_return_me_purchase_stat = false;
    pRet->autorelease();
    return pRet;
    
}

APIUserStatSet *APIUserStatSet::create(std::string stats,
                                       bool return_me_purchase_stat) {
    
    APIUserStatSet *pRet = new APIUserStatSet();
    pRet->m_stats = stats;
    pRet->m_return_me_purchase_stat = return_me_purchase_stat;
    pRet->autorelease();
    return pRet;
    
}

APIUserStatSet *APIUserStatSet::create(std::string stats,
                                       bool return_me_purchase_stat,
                                       void *sender,
                                       void (*callback)(void*,std::string)) {
    
    APIUserStatSet *pRet = new APIUserStatSet();
    pRet->m_stats = stats;
    pRet->m_return_me_purchase_stat = return_me_purchase_stat;
    pRet->setCallback(sender, callback);
    pRet->autorelease();
    return pRet;
    
}

string APIUserStatSet::name() {
    
    return "user_stat_set";
    
}

string APIUserStatSet::param() {
    
    string param = PlayerState::getInstance()->getUserParam() + "&" + m_stats;
    if (m_return_me_purchase_stat) param += "&return_me_purchase_stat=1";
    return param;
    
}
