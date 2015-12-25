//
//  APIAppRequestCount.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIAppRequestCount.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIAppRequestCount *APIAppRequestCount::create(string type,
                                               void *sender,
                                               void (*callback)(void *, std::string)) {
    
    APIAppRequestCount *p = new APIAppRequestCount();
    p->m_type = type;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIAppRequestCount::name() {
    
    return "apprequest_count";
    
}

string APIAppRequestCount::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&type=" + m_type;
    
    return _params;
    
}