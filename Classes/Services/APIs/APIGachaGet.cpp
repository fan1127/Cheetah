//
//  APIGachaGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/12/2557 BE.
//
//

#include "APIGachaGet.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIGachaGet *APIGachaGet::create(std::string type,
                                 bool promotion,
                                 void *sender,
                                 void (*callback)(void*,std::string)) {
    
    APIGachaGet *p = new APIGachaGet();
    p->m_type = type;
    p->m_promotion = promotion;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
}

string APIGachaGet::name() {
    
    return "gacha_get";
    
}

string APIGachaGet::param()
{
    return PlayerState::getInstance()->getUserParam() + "&type=" + m_type + (m_promotion?"&promotion=1":"");
}
