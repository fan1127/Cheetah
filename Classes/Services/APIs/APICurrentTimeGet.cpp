//
//  APICurrentTimeGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#include "APICurrentTimeGet.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APICurrentTimeGet *APICurrentTimeGet::create(void *sender,
                                             void (*callback)(void*,std::string)) {
    
    APICurrentTimeGet *p = new APICurrentTimeGet();
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APICurrentTimeGet::name() {
    
    return "currenttime_get";
    
}

string APICurrentTimeGet::param() {

    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    
    return _params;
    
}