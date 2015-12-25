//
//  APIHelperPetGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIHelperPetGet__
#define __Cheetah__APIHelperPetGet__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIHelperPetGet : public APIRootClass {
    
    std::string m_fbids;
    std::string m_oppo_uuid;
    
public:
    
    static APIHelperPetGet *create(std::string fbids,
                                   std::string oppo_uuid,
                                   void *sender,
                                   void (*callback)(void*,std::string));
    
    virtual std::string name() { return "helper_pet_get"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIHelperPetGet__) */
