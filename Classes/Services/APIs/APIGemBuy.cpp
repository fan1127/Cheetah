//
//  APIGemBuy.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIGemBuy.h"
using namespace std;

APIGemBuy *APIGemBuy::create(string product_id,
                             string receipt,
                             void *sender,
                             void (*callback)(void *, std::string)) {
    
    APIGemBuy *p = new APIGemBuy();
    p->m_product_id = product_id;
    p->m_receipt = receipt;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIGemBuy::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&product_id="+m_product_id;
    
    return _params;
    
}

string APIGemBuy::param2() {
    
    string _params = "";
    _params += "&receipt="+m_receipt;
    
    return _params;
    
}