//
//  APIUserUpdate.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/24/2557 BE.
//
//

#include "APIUserUpdate.h"
using namespace std;

#include "../../Entities/PlayerState.h"

string APIUserUpdate::name() {
    
    return "user_update";
    
}

APIUserUpdate *APIUserUpdate::create(string params, //"ios_token=xxxxx"
                                     void *sender,
                                     void (*callback)(void*,std::string)) {
    
    APIUserUpdate *p = new APIUserUpdate();
    p->m_params = params;
    if (!p->init()) {
        delete p;
        return NULL;
    }
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIUserUpdate::param() {
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    string _param = "";
    string _connector = "";
    
    _param += _playerState->getUserParam();
    _param += "&" + m_params;
    
    return _param;
    
}
