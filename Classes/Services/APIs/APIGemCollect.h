//
//  APIGemCollect.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIGemCollect__
#define __Cheetah__APIGemCollect__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIGemCollect : public APIRootClass {
    
    std::string m_guid;
    
public:
    
    static APIGemCollect *create(std::string guid,
                                 void *sender,
                                 void (*callback)(void*,std::string));
    
    virtual std::string name() { return "gem_collect"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIGemCollect__) */
