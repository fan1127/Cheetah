//
//  APIPetFeed.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIPetFeed__
#define __Cheetah__APIPetFeed__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIPetFeed : public APIRootClass {
    
    Pet* m_pet;
    int m_feed_amount;
    
public:
    
    static APIPetFeed *create(Pet* pet,
                              int feed_amount,
                              void *sender,
                              void (*callback)(void*,std::string));
    
    virtual std::string name() { return "pet_feed"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIPetFeed__) */
