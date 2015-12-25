//
//  APIPetHost.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIPetHost.h"
using namespace std;

APIPetHost *APIPetHost::create(Pet* pet,
                               void *sender,
                               void (*callback)(void *, std::string)) {
    
    APIPetHost *p = new APIPetHost();
    p->m_pet = pet;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIPetHost::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&puid=" + m_pet->ppVo.puid;
    
    return _params;
    
}