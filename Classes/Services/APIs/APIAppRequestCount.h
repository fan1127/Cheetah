//
//  APIAppRequestCount.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIAppRequestCount__
#define __Cheetah__APIAppRequestCount__

#include "APIRootClass.h"

class APIAppRequestCount : public APIRootClass {
    
    std::string m_type;
    
public:
    
    static APIAppRequestCount *create(std::string type,
                                      void *sender,
                                      void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIAppRequestCount__) */
