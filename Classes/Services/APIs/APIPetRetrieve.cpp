//
//  APIPetRetrieve.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/1/2557 BE.
//
//

#include "APIPetRetrieve.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIPetRetrieve *APIPetRetrieve::create(void *ptr_pet,
                                       void *ptr_building,
                                       void *sender,
                                       void (*callback)(void *, std::string)) {

    Pet* _pet = (Pet*)ptr_pet;
    if (!_pet || _pet->ppVo.puid == "") return NULL;
    
    Building* _building = (Building*)ptr_building;
    if (!_building || _building->pbVo.buid == "") return NULL;
    
    APIPetRetrieve *p = new APIPetRetrieve();
    p->m_ptr_pet = ptr_pet;
    p->m_ptr_building = ptr_building;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIPetRetrieve::name() {
    
    return "pet_retrieve";
    
}

string APIPetRetrieve::param() {
    
    Pet* _pet = (Pet*)m_ptr_pet;
    CCAssert(_pet!=NULL, "Pet must not be NULL!");
    
    Building* _building = (Building*)m_ptr_building;
    CCAssert(_building!=NULL, "Building must not be NULL!");
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&puid=" + _pet->ppVo.puid;
    _params += "&buid=" + _building->pbVo.buid;
    
    return _params;
    
}