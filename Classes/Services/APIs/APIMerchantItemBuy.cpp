//
//  APIMerchantItemBuy.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIMerchantItemBuy.h"
using namespace std;

APIMerchantItemBuy *APIMerchantItemBuy::create(string item,
                                                   void *sender,
                                                   void (*callback)(void *, std::string)) {
    
    APIMerchantItemBuy *p = new APIMerchantItemBuy();
    p->m_item = item;
    p->m_instant = false;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

APIMerchantItemBuy *APIMerchantItemBuy::create(string item,
                                               bool instant,
                                               void *sender,
                                               void (*callback)(void *, std::string)) {
    
    APIMerchantItemBuy *p = new APIMerchantItemBuy();
    p->m_item = item;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIMerchantItemBuy::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&item="+m_item;
    _params += m_instant?"&instant=1":"";
    
    return _params;
    
}