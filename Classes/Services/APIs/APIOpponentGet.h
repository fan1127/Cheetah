//
//  APIOpponentGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/11/2557 BE.
//
//

#ifndef __Cheetah__APIOpponentGet__
#define __Cheetah__APIOpponentGet__

#include "APIRootClass.h"

class APIOpponentGet : public APIRootClass {
    
    std::string m_oppo_uuid;
    
public:

    static APIOpponentGet *create(std::string oppo_uuid, void *sender, void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIOpponentGet__) */
