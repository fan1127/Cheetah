//
//  APIBuildingNew.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/20/2557 BE.
//
//

#include "APIBuildingNew.h"
using namespace std;

#include "../../Entities/PlayerState.h"
#include "../../Entities/Building.h"

APIBuildingNew *APIBuildingNew::create(void *ptr_building,
                                       bool instant,
                                       void *sender,
                                       void (*callback)(void*,std::string)) {
    
    APIBuildingNew *p = new APIBuildingNew();
    p->m_ptr_building = ptr_building;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIBuildingNew::name() {
    
    return "building_new";
    
}

string APIBuildingNew::param() {
    
    Building* _building = (Building*)m_ptr_building;
    CCAssert(_building!=NULL, "Building must not be NULL!");

    CREATE_CHAR_BUFFER(_buffer, 16);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&type=" + _building->pbVo.type;
    snprintf(_buffer, _buffer_size, "&flip=%d", (int)_building->pbVo.flip);
    _params.append(_buffer);

    CREATE_CHAR_BUFFER(map_, (BUILDING_SPACE_GRID_NUM*5)+2);
    snprintf(map_, map__size, "[");
    bool added = false;
    for (int i=0; i<BUILDING_SPACE_GRID_NUM; i++) {
        if (_building->pbVo.map[i] >= 0) {
            snprintf(map_, map__size, "%s%d,", map_, _building->pbVo.map[i]);
            added = true;
        }
    }
    string map__;
    map__.clear();
    map__.append(map_);
    if (added && map__.size() > 0) map__.resize(map__.size()-1);
    map__.append("]");
    _params.append("&map=");
    _params.append(map__);
    
    if (m_instant) {
        _params += "&instant=1";
    }
    
    return _params;
    
}
