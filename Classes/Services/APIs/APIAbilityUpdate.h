//
//  APIAbilityUpdate.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIAbilityUpdate__
#define __Cheetah__APIAbilityUpdate__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIAbilityUpdate : public APIRootClass {
    
    Building* m_building;
    Ability* m_ability;
    bool m_finish_now;
    
public:
    
    static APIAbilityUpdate *create(Building* building,
                                    Ability* ability,
                                    bool finish_now,
                                    void *sender,
                                    void (*callback)(void*,std::string));
    
    virtual std::string name() { return "ability_update"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIAbilityUpdate__) */
