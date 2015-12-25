//
//  APIPetEvolve.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIPetEvolve.h"
using namespace std;

APIPetEvolve *APIPetEvolve::create(Building* building,
                                   Pet* pet,
                                   bool instant,
                                   void *sender,
                                   void (*callback)(void *, std::string)) {
    
    APIPetEvolve *p = new APIPetEvolve();
    p->m_building = building;
    p->m_pet = pet;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIPetEvolve::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&puid=" + m_pet->ppVo.puid;
    if (m_building) {
        _params += "&buid=" + m_building->pbVo.buid;
    }
    if (m_instant) {
        _params += "&instant=1";
    }
    
    return _params;
    
}