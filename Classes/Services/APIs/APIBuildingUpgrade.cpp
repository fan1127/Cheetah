//
//  APIBuildingUpgrade.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/21/2557 BE.
//
//

#include "APIBuildingUpgrade.h"
using namespace std;

#include "../../Entities/PlayerState.h"
#include "../../Entities/Building.h"

APIBuildingUpgrade *APIBuildingUpgrade::create(void *ptr_building,
                                               int collect_amount,
                                               int produced_amount,
                                               int instant,
                                               void *sender,
                                               void (*callback)(void *, std::string)) {
    
    Building* _building = (Building*)ptr_building;
    if (!_building || _building->pbVo.buid == "") return NULL;
    
    APIBuildingUpgrade *p = new APIBuildingUpgrade();
    p->m_ptr_building = ptr_building;
    p->m_collect_amount = collect_amount;
    p->m_produced_amount = produced_amount;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIBuildingUpgrade::name() {
    
    return "building_upgrade";
    
}

string APIBuildingUpgrade::param() {
    
    Building* _building = (Building*)m_ptr_building;
    CCAssert(_building!=NULL, "Building must not be NULL!");
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&buid=" + _building->pbVo.buid;
    
    CREATE_CHAR_BUFFER(_buf, 64);
    snprintf(_buf, _buf_size, "&amount=%d", m_collect_amount);
    _params += _buf;
    snprintf(_buf, _buf_size, "&produced_amount=%d", m_produced_amount);
    _params += _buf;
    
    if (m_instant > 0) {
        snprintf(_buf, _buf_size, "&instant=%d", m_instant);
        _params += _buf;
    }

    return _params;
    
}
