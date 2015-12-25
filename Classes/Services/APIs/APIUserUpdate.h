//
//  APIUserUpdate.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/24/2557 BE.
//
//

#ifndef __Cheetah__APIUserUpdate__
#define __Cheetah__APIUserUpdate__

#include "APIRootClass.h"

class APIUserUpdate : public APIRootClass {
    
    std::string m_params;
    
public:
    
    static APIUserUpdate *create(std::string params,
                                 void *sender,
                                 void (*callback)(void*,std::string));

    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIUserUpdate__) */
