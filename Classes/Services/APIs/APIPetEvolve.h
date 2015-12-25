//
//  APIPetEvolve.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIPetEvolve__
#define __Cheetah__APIPetEvolve__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIPetEvolve : public APIRootClass {
    
    Building* m_building;
    Pet* m_pet;
    bool m_instant;
    
public:
    
    static APIPetEvolve *create(Building* building,
                                Pet* pet,
                                bool instant,
                                void *sender,
                                void (*callback)(void*,std::string));
    
    virtual std::string name() { return "pet_evolve"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIPetEvolve__) */
