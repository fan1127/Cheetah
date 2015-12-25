//
//  APIGemExchange.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIGemExchange__
#define __Cheetah__APIGemExchange__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIGemExchange : public APIRootClass {
    
    std::string m_type;
    int m_amount;
    
public:
    
    static APIGemExchange *create(std::string type,
                                  int amount,
                                  void *sender,
                                  void (*callback)(void*,std::string));
    
    virtual std::string name() { return "gem_exchange"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIGemExchange__) */
