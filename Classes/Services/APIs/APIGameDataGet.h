//
//  APIGameDataGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/11/2557 BE.
//
//

#ifndef __Cheetah__APIGameDataGet__
#define __Cheetah__APIGameDataGet__

#include "APIRootClass.h"

class APIGameDataGet : public APIRootClass {
    
    std::string m_type;
    
public:
    
    static APIGameDataGet *create(std::string type);
    static APIGameDataGet *create(std::string type, void *sender, void (*callback)(void*,std::string));
    
    virtual bool init();
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIGameDataGet__) */
