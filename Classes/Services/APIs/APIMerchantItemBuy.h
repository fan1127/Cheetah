//
//  APIMerchantItemBuy.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIMerchantItemBuy__
#define __Cheetah__APIMerchantItemBuy__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIMerchantItemBuy : public APIRootClass {
    
    std::string m_item;
    bool m_instant;
    
public:
    
    static APIMerchantItemBuy *create(std::string item,
                                      void *sender,
                                      void (*callback)(void*,std::string));
    
    static APIMerchantItemBuy *create(std::string item,
                                      bool instant,
                                      void *sender,
                                      void (*callback)(void*,std::string));
    
    virtual std::string name() { return "merchant_item_buy"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIMerchantItemBuy__) */
