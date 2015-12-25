//
//  APIPetStore.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIPetStore__
#define __Cheetah__APIPetStore__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIPetStore : public APIRootClass {
    
    Pet* m_pet;
    
public:
    
    static APIPetStore *create(Pet* pet,
                               void *sender,
                               void (*callback)(void*,std::string));
    
    virtual std::string name() { return "pet_store"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIPetStore__) */
