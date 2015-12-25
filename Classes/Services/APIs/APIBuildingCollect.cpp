//
//  APIBuildingCollect.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/25/2557 BE.
//
//

#include "APIBuildingCollect.h"
using namespace std;

#include "../../Entities/PlayerState.h"
#include "../../Entities/Building.h"

APIBuildingCollect *APIBuildingCollect::create(void *ptr_building,
                                               int collect_amount,
                                               int produced_amount,
                                               void *sender,
                                               void (*callback)(void *, std::string)) {
    
    Building* _building = (Building*)ptr_building;
    if (!_building || _building->pbVo.buid == "") return NULL;
    
    APIBuildingCollect *p = new APIBuildingCollect();
    p->m_ptr_building = ptr_building;
    p->m_collect_amount = collect_amount;
    p->m_produced_amount = produced_amount;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIBuildingCollect::name() {
    
    return "building_collect";
    
}

string APIBuildingCollect::param() {
    
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
#if FEATURE_COINS_FROM_PET
    if (_building->bVo.building_type == BUILDING_TYPE_HABITAT && _building->pet) {
        _params += "&puid=" + _building->pet->ppVo.puid;
    }
#endif
    
    return _params;
    
}
