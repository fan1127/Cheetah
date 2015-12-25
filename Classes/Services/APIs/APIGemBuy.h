//
//  APIGemBuy.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIGemBuy__
#define __Cheetah__APIGemBuy__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIGemBuy : public APIRootClass {
    
    std::string m_product_id;
    std::string m_receipt;
    
public:
    
    static APIGemBuy *create(std::string product_id,
                             std::string receipt,
                             void *sender,
                             void (*callback)(void*,std::string));
    
    virtual std::string name() { return "gem_buy"; }
    virtual std::string param();
    virtual std::string param2();
    
};

#endif /* defined(__Cheetah__APIGemBuy__) */
