//
//  APIAppRequestAdd.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIAppRequestAdd.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIAppRequestAdd *APIAppRequestAdd::create(string type,
                                           string ids,
                                           void *sender,
                                           void (*callback)(void *, std::string)) {
    
    APIAppRequestAdd *p = new APIAppRequestAdd();
    p->m_type = type;
    p->m_ids = ids;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIAppRequestAdd::name() {
    
    return "apprequest_add";
    
}

string APIAppRequestAdd::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&type=" + m_type;
    _params += "&ids=" + m_ids;
    
    return _params;
    
}