//
//  APIRaceNew.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIRaceNew.h"
using namespace std;

extern string race_id;

APIRaceNew *APIRaceNew::create(Pet* pet,
                               int garage_level,
                               bool instant,
                               void *sender,
                               void (*callback)(void *, std::string)) {
    
    APIRaceNew *p = new APIRaceNew();
    p->m_pet = pet;
    p->m_garage_level = garage_level;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIRaceNew::param() {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    
    snprintf(_buffer, _buffer_size, "&garage_level=%d", m_garage_level);
    _params += _buffer;
    _params += "&puid="+m_pet->ppVo.puid;
    _params += "&race_id=" + race_id;
    _params += m_instant?"&instant=1":"";
    
    return _params;
    
}