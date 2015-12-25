//
//  APIUserNameChange.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#include "APIUserNameChange.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIUserNameChange *APIUserNameChange::create(string name,
                                             void *sender,
                                             void (*callback)(void*,std::string)) {
    
    APIUserNameChange *p = new APIUserNameChange();
    p->m_name = name;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIUserNameChange::name() {
    
    return "user_name_change";
    
}

string APIUserNameChange::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&name=" + m_name;
    
    return _params;
    
}