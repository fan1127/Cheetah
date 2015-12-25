//
//  APIAbilityUpdate.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIAbilityUpdate.h"
using namespace std;

APIAbilityUpdate *APIAbilityUpdate::create(Building* building,
                                           Ability* ability,
                                           bool finish_now,
                                           void *sender,
                                           void (*callback)(void *, std::string)) {
    
    APIAbilityUpdate *p = new APIAbilityUpdate();
    p->m_building = building;
    p->m_ability = ability;
    p->m_finish_now = finish_now;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIAbilityUpdate::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&buid=" + m_building->pbVo.buid;
    _params += "&auid=" + m_ability->paVo.auid;
    _params += m_finish_now?"&finishnow=1":"";
    
    return _params;
    
}