//
//  APIGachaGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/12/2557 BE.
//
//

#ifndef __Cheetah__APIGachaGet__
#define __Cheetah__APIGachaGet__

#include "APIRootClass.h"

class APIGachaGet : public APIRootClass {
    
    std::string m_type;

    bool m_promotion;

public:
    
    static APIGachaGet *create(std::string type,
                               bool promotion,
                               void *sender,
                               void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIGachaGet__) */
