//
//  APIHelperPetUse.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIHelperPetUse.h"
using namespace std;

APIHelperPetUse *APIHelperPetUse::create(string helper_pet_uuid,
                                         void *sender,
                                         void (*callback)(void *, std::string)) {
    
    APIHelperPetUse *p = new APIHelperPetUse();
    p->m_helper_pet_uuid = helper_pet_uuid;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIHelperPetUse::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&helper_pet_uuid="+m_helper_pet_uuid;
    
    return _params;
    
}