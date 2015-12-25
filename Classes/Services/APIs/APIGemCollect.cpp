//
//  APIGemCollect.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIGemCollect.h"
using namespace std;

APIGemCollect *APIGemCollect::create(string guid,
                                     void *sender,
                                     void (*callback)(void *, std::string)) {
    
    APIGemCollect *p = new APIGemCollect();
    p->m_guid = guid;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIGemCollect::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&guid="+m_guid;
    
    return _params;
    
}