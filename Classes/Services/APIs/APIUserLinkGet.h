//
//  APIUserLinkGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/2/2558 BE.
//
//

#ifndef __Cheetah__APIUserLinkGet__
#define __Cheetah__APIUserLinkGet__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIUserLinkGet : public APIRootClass {
    
public:
    
    static APIUserLinkGet *create(void *sender,
                                  void (*callback)(void*,std::string));
    
    virtual std::string name() { return "user_link_get"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIUserLinkGet__) */
