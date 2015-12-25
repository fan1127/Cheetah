//
//  APIItemSell.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIItemSell.h"
using namespace std;

APIItemSell *APIItemSell::create(string item_type,
                                 int amount,
                                 void *sender,
                                 void (*callback)(void *, std::string)) {
    
    APIItemSell *p = new APIItemSell();
    p->m_item_type = item_type;
    p->m_amount = amount;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIItemSell::param() {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&item=" + m_item_type;
    snprintf(_buffer, _buffer_size, "&amount=%d", m_amount);
    _params += _buffer;
    
    return _params;
    
}