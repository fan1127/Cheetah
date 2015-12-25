//
//  APIOpponentGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/11/2557 BE.
//
//

#include "APIOpponentGet.h"
using namespace std;

#include "../../Entities/PlayerState.h"

string APIOpponentGet::name() {
    
    return "opponent_get";
    
}

APIOpponentGet *APIOpponentGet::create(std::string oppo_uuid,
                                       void *sender,
                                       void (*callback)(void*,std::string)) {
    
    APIOpponentGet *p = new APIOpponentGet();
    p->m_oppo_uuid = oppo_uuid;
    if (!p->init()) {
        delete p;
        return NULL;
    }
    p->setCallback(sender, callback);
    p->autorelease();
    p->retries = 0;
    return p;
    
}

string APIOpponentGet::param() {
    
    string params = "";
    params.append(PlayerState::getInstance()->getUserParam());
    params.append("&oppo_uuid=");
    params.append(m_oppo_uuid);
    return params;
    
}

