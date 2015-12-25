//
//  APILiveDataGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/19/2558 BE.
//
//

#include "APILiveDataGet.h"
using namespace std;

APILiveDataGet *APILiveDataGet::create(std::string type,
                                       void *sender,
                                       void (*callback)(void *, std::string)) {
    
    APILiveDataGet *p = new APILiveDataGet();
    p->m_type = type;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APILiveDataGet::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&type=" + m_type;
    
    return _params;
    
}