//
//  APIUserLink.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/2/2558 BE.
//
//

#ifndef __Cheetah__APIUserLink__
#define __Cheetah__APIUserLink__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIUserLink : public APIRootClass {
    
    std::string m_key;
    
public:
    
    static APIUserLink *create(std::string key,
                               void *sender,
                               void (*callback)(void*,std::string));
    
    virtual std::string name() { return "user_link"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIUserLink__) */
