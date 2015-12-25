//
//  APIUserGcidTie.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#include "APIUserGcidTie.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIUserGcidTie *APIUserGcidTie::create(string gcid,
                                       void *sender,
                                       void (*callback)(void *, std::string)) {
    
    APIUserGcidTie *p = new APIUserGcidTie();
    p->m_gcid = gcid;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIUserGcidTie::name() {
    
    return "user_gcid_tie";
    
}

string APIUserGcidTie::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&gcid=" + m_gcid;
    
    return _params;
    
}