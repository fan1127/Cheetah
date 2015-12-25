//
//  APIUserGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/6/2557 BE.
//
//

#ifndef __Cheetah__APIUserGet__
#define __Cheetah__APIUserGet__

#include "APIRootClass.h"

class APIUserGet : public APIRootClass {
    
public:
    
    static APIUserGet *create(void *sender,
                              void (*callback)(void*,std::string));

    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIUserGet__) */
