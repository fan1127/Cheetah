//
//  APIRaceNew.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIRaceNew__
#define __Cheetah__APIRaceNew__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIRaceNew : public APIRootClass {
    
    Pet* m_pet;
    int m_garage_level;
    bool m_instant;
    
public:
    
    static APIRaceNew *create(Pet* pet,
                              int garage_level,
                              bool instant,
                              void *sender,
                              void (*callback)(void*,std::string));
    
    virtual std::string name() { return "race_new"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIRaceNew__) */
