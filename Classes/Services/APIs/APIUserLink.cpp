//
//  APIUserLink.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/2/2558 BE.
//
//

#include "APIUserLink.h"
using namespace std;

APIUserLink *APIUserLink::create(string key,
                                 void *sender,
                                 void (*callback)(void *, std::string)) {
    
    APIUserLink *p = new APIUserLink();
    p->m_key = key;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIUserLink::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&key=" + m_key;
    
    return _params;
    
}