//
//  APIPetSell.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIPetSell__
#define __Cheetah__APIPetSell__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIPetSell : public APIRootClass {
    
    Pet* m_pet;
    
public:
    
    static APIPetSell *create(Pet* pet,
                              void *sender,
                              void (*callback)(void*,std::string));
    
    virtual std::string name() { return "pet_sell"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIPetSell__) */
