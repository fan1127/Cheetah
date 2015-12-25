//
//  APIRaceGet.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIRaceGet__
#define __Cheetah__APIRaceGet__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIRaceGet : public APIRootClass {
    
    std::string m_race_id;
    
public:
    
    static APIRaceGet *create(std::string race_id,
                              void *sender,
                              void (*callback)(void*,std::string));
    
    virtual std::string name() { return "race_get"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIRaceGet__) */
