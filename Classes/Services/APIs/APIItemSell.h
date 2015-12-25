//
//  APIItemSell.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIItemSell__
#define __Cheetah__APIItemSell__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIItemSell : public APIRootClass {
    
    std::string m_item_type;
    int m_amount;
    
public:
    
    static APIItemSell *create(std::string item_type,
                               int amount,
                                void *sender,
                               void (*callback)(void*,std::string));
    
    virtual std::string name() { return "item_sell"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIItemSell__) */
