//
//  APIHelperPetGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIHelperPetGet.h"
using namespace std;

APIHelperPetGet *APIHelperPetGet::create(string fbids,
                                         string oppo_uuid,
                                         void *sender,
                                         void (*callback)(void *, std::string)) {
    
    APIHelperPetGet *p = new APIHelperPetGet();
    p->m_fbids = fbids;
    p->m_oppo_uuid = oppo_uuid;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIHelperPetGet::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += m_fbids!=""?"&fbids="+m_fbids:"";
    _params += "&oppo_uuid="+m_oppo_uuid;
    
    return _params;
    
}