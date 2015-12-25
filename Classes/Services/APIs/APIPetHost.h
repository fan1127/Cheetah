//
//  APIPetHost.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIPetHost__
#define __Cheetah__APIPetHost__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIPetHost : public APIRootClass {
    
    Pet* m_pet;
    
public:
    
    static APIPetHost *create(Pet* pet,
                              void *sender,
                              void (*callback)(void*,std::string));
    
    virtual std::string name() { return "pet_host"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIPetHost__) */
