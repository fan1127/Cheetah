//
//  APIPetFeed.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIPetFeed.h"
using namespace std;

APIPetFeed *APIPetFeed::create(Pet* pet,
                               int feed_amount,
                               void *sender,
                               void (*callback)(void *, std::string)) {
    
    APIPetFeed *p = new APIPetFeed();
    p->m_pet = pet;
    p->m_feed_amount = feed_amount;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIPetFeed::param() {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&puid=" + m_pet->ppVo.puid;
    snprintf(_buffer, _buffer_size, "&amount=%d", m_feed_amount);
    _params += _buffer;
    
    return _params;
    
}