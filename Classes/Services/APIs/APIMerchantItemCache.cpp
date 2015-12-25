//
//  APIMerchantItemCache.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIMerchantItemCache.h"
using namespace std;

APIMerchantItemCache *APIMerchantItemCache::create(string items,
                                                   void *sender,
                                                   void (*callback)(void *, std::string)) {
    
    APIMerchantItemCache *p = new APIMerchantItemCache();
    p->m_items = items;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIMerchantItemCache::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&items="+m_items;
    
    return _params;
    
}