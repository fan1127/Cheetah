//
//  APIHelperPetUse.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIHelperPetUse__
#define __Cheetah__APIHelperPetUse__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIHelperPetUse : public APIRootClass {
    
    std::string m_helper_pet_uuid;
    
public:
    
    static APIHelperPetUse *create(std::string helper_pet_uuid,
                                   void *sender,
                                   void (*callback)(void*,std::string));
    
    virtual std::string name() { return "helper_pet_use"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIHelperPetUse__) */
