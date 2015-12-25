//
//  APIPetWakeUp.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIPetWakeUp__
#define __Cheetah__APIPetWakeUp__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIPetWakeUp : public APIRootClass {
    
    Pet* m_pet;
    
public:
    
    static APIPetWakeUp *create(Pet* pet,
                                void *sender,
                                void (*callback)(void*,std::string));
    
    virtual std::string name() { return "pet_wakeup"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIPetWakeUp__) */
