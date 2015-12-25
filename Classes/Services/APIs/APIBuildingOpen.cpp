//
//  APIBuildingOpen.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/21/2557 BE.
//
//

#include "APIBuildingOpen.h"
using namespace std;

#include "../../Entities/PlayerState.h"
#include "../../Entities/Building.h"

APIBuildingOpen *APIBuildingOpen::create(void *ptr_building,
                                             void *sender,
                                             void (*callback)(void*,std::string)) {
    
    Building* _building = (Building*)ptr_building;
    if (!_building || _building->pbVo.buid == "") return NULL;
    
    APIBuildingOpen *p = new APIBuildingOpen();
    p->m_ptr_building = ptr_building;
    
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIBuildingOpen::name() {
    
    return "building_open";
    
}

string APIBuildingOpen::param() {
    
    Building* _building = (Building*)m_ptr_building;
    CCAssert(_building!=NULL, "Building must not be NULL!");
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&buid=" + _building->pbVo.buid;
    
    return _params;
    
}
