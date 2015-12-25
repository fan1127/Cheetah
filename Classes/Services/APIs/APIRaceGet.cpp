//
//  APIRaceGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIRaceGet.h"
using namespace std;

APIRaceGet *APIRaceGet::create(std::string race_id,
                               void *sender,
                               void (*callback)(void *, std::string)) {
    
    APIRaceGet *p = new APIRaceGet();
    p->m_race_id = race_id;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIRaceGet::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&race_id=" + m_race_id;
    
    return _params;
    
}