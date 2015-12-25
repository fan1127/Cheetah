//
//  APIMapOpponentNew.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIMapOpponentNew.h"
using namespace std;

APIMapOpponentNew *APIMapOpponentNew::create(int village_id,
                                             void *sender,
                                             void (*callback)(void *, std::string)) {
    
    APIMapOpponentNew *p = new APIMapOpponentNew();
    p->m_village_id = village_id;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIMapOpponentNew::param() {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    snprintf(_buffer, _buffer_size, "&village_id=%d", m_village_id);
    _params += _buffer;
    
    return _params;
    
}