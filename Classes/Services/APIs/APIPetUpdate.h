//
//  APIPetUpdate.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIPetUpdate__
#define __Cheetah__APIPetUpdate__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIPetUpdate : public APIRootClass {
    
    Pet* m_pet;
    
public:
    
    static APIPetUpdate *create(Pet* pet,
                                void *sender,
                                void (*callback)(void*,std::string));
    
    virtual std::string name() { return "pet_update"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIPetUpdate__) */
