//
//  APIMapOpponentNew.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIMapOpponentNew__
#define __Cheetah__APIMapOpponentNew__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIMapOpponentNew : public APIRootClass {
    
    int m_village_id;
    
public:
    
    static APIMapOpponentNew *create(int village_id,
                                     void *sender,
                                     void (*callback)(void*,std::string));
    
    virtual std::string name() { return "map_opponent_new"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIMapOpponentNew__) */
