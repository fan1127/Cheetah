//
//  APIPetStore.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIPetStore.h"
using namespace std;

APIPetStore *APIPetStore::create(Pet* pet,
                                 void *sender,
                                 void (*callback)(void *, std::string)) {
    
    APIPetStore *p = new APIPetStore();
    p->m_pet = pet;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIPetStore::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&puid=" + m_pet->ppVo.puid;
    
    return _params;
    
}