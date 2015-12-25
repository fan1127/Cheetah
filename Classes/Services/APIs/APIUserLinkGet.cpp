//
//  APIUserLinkGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/2/2558 BE.
//
//

#include "APIUserLinkGet.h"
#include "../../Entities/PlayerState.h"
using namespace std;

APIUserLinkGet *APIUserLinkGet::create(void *sender,
                                       void (*callback)(void *, std::string)) {
    
    APIUserLinkGet *p = new APIUserLinkGet();
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIUserLinkGet::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    
    return _params;
    
}