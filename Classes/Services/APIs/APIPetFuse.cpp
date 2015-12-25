//
//  APIPetFuse.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/1/2557 BE.
//
//

#include "APIPetFuse.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIPetFuse *APIPetFuse::create(GDPetVO pet_vo,
                               void *ptr_building,
                               bool instant,
                               void *sender,
                               void (*callback)(void* sender, std::string resp_str)) {
    
    if (Building* _building = (Building*)ptr_building) {
        if (_building->pbVo.buid == "") return NULL;
    }
    
    APIPetFuse *p = new APIPetFuse();
    p->m_pet_vo = pet_vo;
    p->m_ptr_building = ptr_building;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIPetFuse::name() {
    
    return "pet_fuse";
    
}

string APIPetFuse::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&ptype=" + m_pet_vo.type;
    if (Building* _building = (Building*)m_ptr_building) {
        _params += "&buid=" + _building->pbVo.buid;
    }
    if (m_instant) {
        _params.append("&instant=1");
    }
    
    return _params;
    
}