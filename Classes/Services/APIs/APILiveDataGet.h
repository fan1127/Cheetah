//
//  APILiveDataGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/19/2558 BE.
//
//

#ifndef __Cheetah__APILiveDataGet__
#define __Cheetah__APILiveDataGet__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APILiveDataGet : public APIRootClass {
    
    std::string m_type;
    
public:
    
    static APILiveDataGet *create(std::string type,
                                  void *sender,
                                  void (*callback)(void*,std::string));
    
    virtual std::string name() { return "livedata_get"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APILiveDataGet__) */
