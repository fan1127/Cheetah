//
//  APIPetWakeUp.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIPetWakeUp.h"
using namespace std;

APIPetWakeUp *APIPetWakeUp::create(Pet* pet,
                                   void *sender,
                                   void (*callback)(void *, std::string)) {
    
    APIPetWakeUp *p = new APIPetWakeUp();
    p->m_pet = pet;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIPetWakeUp::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&puid=" + m_pet->ppVo.puid;
    
    return _params;
    
}