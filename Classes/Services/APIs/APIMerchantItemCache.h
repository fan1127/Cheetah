//
//  APIMerchantItemCache.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIMerchantItemCache__
#define __Cheetah__APIMerchantItemCache__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIMerchantItemCache : public APIRootClass {
    
    std::string m_items;
    
public:
    
    static APIMerchantItemCache *create(std::string items,
                                     void *sender,
                                     void (*callback)(void*,std::string));
    
    virtual std::string name() { return "merchant_item_cache"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIMerchantItemCache__) */
