//
//  APIMapGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIMapGet__
#define __Cheetah__APIMapGet__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIMapGet : public APIRootClass {
    
    Building* m_building;
    Ability* m_ability;
    bool m_finish_now;
    
public:
    
    static APIMapGet *create(void *sender,
                             void (*callback)(void*,std::string));
    
    virtual std::string name() { return "map_get"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIMapGet__) */
