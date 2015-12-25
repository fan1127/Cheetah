//
//  APIChecklistUpdate.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#include "APIChecklistUpdate.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIChecklistUpdate *APIChecklistUpdate::create(string check_key,
                                               void *sender,
                                               void (*callback)(void *, std::string)) {
    
    APIChecklistUpdate *p = new APIChecklistUpdate();
    p->m_check_key = check_key;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIChecklistUpdate::name() {
    
    return "checklist_update";
    
}

string APIChecklistUpdate::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&checkkey=" + m_check_key;
    
    return _params;
    
}