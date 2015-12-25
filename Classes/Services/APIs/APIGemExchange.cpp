//
//  APIGemExchange.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIGemExchange.h"
using namespace std;

APIGemExchange *APIGemExchange::create(string type,
                                      int amount,
                                      void *sender,
                                      void (*callback)(void *, std::string)) {
    
    APIGemExchange *p = new APIGemExchange();
    p->m_type = type;
    p->m_amount = amount;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIGemExchange::param() {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&type="+m_type;
    snprintf(_buffer, _buffer_size, "&amount=%d", m_amount);
    _params += _buffer;
    
    return _params;
    
}