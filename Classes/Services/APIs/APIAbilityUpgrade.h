//
//  APIAbilityUpgrade.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIAbilityUpgrade__
#define __Cheetah__APIAbilityUpgrade__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIAbilityUpgrade : public APIRootClass {
    
    Building* m_building;
    std::string m_ability_type;
    bool m_instant;
    
public:
    
    static APIAbilityUpgrade *create(Building* building,
                                     std::string ability_type,
                                     bool instant,
                                     void *sender,
                                     void (*callback)(void*,std::string));
    
    virtual std::string name() { return "ability_upgrade"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIAbilityUpgrade__) */
