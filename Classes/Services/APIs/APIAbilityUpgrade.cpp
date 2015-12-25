//
//  APIAbilityUpgrade.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIAbilityUpgrade.h"
using namespace std;

APIAbilityUpgrade *APIAbilityUpgrade::create(Building* building,
                                             string ability_type,
                                             bool instant,
                                             void *sender,
                                             void (*callback)(void *, std::string)) {
    
    APIAbilityUpgrade *p = new APIAbilityUpgrade();
    p->m_building = building;
    p->m_ability_type = ability_type;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIAbilityUpgrade::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&buid=" + m_building->pbVo.buid;
    _params += "&type=" + m_ability_type;
    _params += m_instant?"&instant=1":"";
    
    return _params;
    
}