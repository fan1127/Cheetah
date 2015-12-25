//
//  APIRaceHistoryList.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIRaceHistoryList.h"
using namespace std;

APIRaceHistoryList *APIRaceHistoryList::create(void *sender,
                                               void (*callback)(void *, std::string)) {
    
    APIRaceHistoryList *p = new APIRaceHistoryList();
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIRaceHistoryList::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    
    return _params;
    
}