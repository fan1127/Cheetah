//
//  APIAppRequestAdd.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIAppRequestAdd__
#define __Cheetah__APIAppRequestAdd__

#include "APIRootClass.h"

class APIAppRequestAdd : public APIRootClass {
    
    std::string m_type;
    std::string m_ids;
    
public:
    
    static APIAppRequestAdd *create(std::string type,
                                      std::string ids,
                                      void *sender,
                                      void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIAppRequestAdd__) */
