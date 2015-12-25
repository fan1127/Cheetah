//
//  APIUserNameChange.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#ifndef __Cheetah__APIUserNameChange__
#define __Cheetah__APIUserNameChange__

#include "APIRootClass.h"

class APIUserNameChange : public APIRootClass {
    
    std::string m_name;
    
public:
    
    static APIUserNameChange *create(std::string name,
                                     void *sender,
                                     void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIUserNameChange__) */
