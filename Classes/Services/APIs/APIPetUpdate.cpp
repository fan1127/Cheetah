//
//  APIPetUpdate.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIPetUpdate.h"
using namespace std;

APIPetUpdate *APIPetUpdate::create(Pet* pet,
                                   void *sender,
                                   void (*callback)(void *, std::string)) {
    
    APIPetUpdate *p = new APIPetUpdate();
    p->m_pet = pet;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIPetUpdate::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&puid=" + m_pet->ppVo.puid;
    
    return _params;
    
}