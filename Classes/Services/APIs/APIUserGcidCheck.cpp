//
//  APIUserGcidCheck.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#include "APIUserGcidCheck.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIUserGcidCheck *APIUserGcidCheck::create(string gcid,
                                           void *sender,
                                           void (*callback)(void *, std::string)) {
    
    APIUserGcidCheck *p = new APIUserGcidCheck();
    p->m_gcid = gcid;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIUserGcidCheck::name() {
    
    return "user_gcid_check";
    
}

string APIUserGcidCheck::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&gcid=" + m_gcid;
    
    return _params;
    
}