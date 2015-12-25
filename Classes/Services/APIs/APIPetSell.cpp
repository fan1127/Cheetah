//
//  APIPetSell.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIPetSell.h"
using namespace std;

APIPetSell *APIPetSell::create(Pet* pet,
                               void *sender,
                               void (*callback)(void *, std::string)) {
    
    APIPetSell *p = new APIPetSell();
    p->m_pet = pet;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIPetSell::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&puid=" + m_pet->ppVo.puid;
    
    return _params;
    
}