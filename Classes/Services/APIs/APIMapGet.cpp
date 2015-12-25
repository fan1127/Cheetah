//
//  APIMapGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIMapGet.h"
using namespace std;

APIMapGet *APIMapGet::create(void *sender,
                             void (*callback)(void *, std::string)) {
    
    APIMapGet *p = new APIMapGet();
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIMapGet::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    
    return _params;
    
}