//
//  APICurrentTimeGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#ifndef __Cheetah__APICurrentTimeGet__
#define __Cheetah__APICurrentTimeGet__

#include "APIRootClass.h"

class APICurrentTimeGet : public APIRootClass {
    
public:
    
    static APICurrentTimeGet *create(void *sender,
                                       void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APICurrentTimeGet__) */
