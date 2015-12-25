//
//  APIUserGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/6/2557 BE.
//
//

#include "APIUserGet.h"
using namespace std;

#include "../../Entities/PlayerState.h"
#include "../../Helpers/Versioning.h"

string APIUserGet::name() {
    
    return "user_get";
    
}

APIUserGet *APIUserGet::create(void *sender,
                               void (*callback)(void*,std::string)) {
    
    APIUserGet *p = new APIUserGet();
    if (!p->init()) {
        delete p;
        return NULL;
    }
    p->setCallback(sender, callback);
    p->autorelease();
    p->retries = 0;
    return p;
    
}

string APIUserGet::param() {
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    string _param = "client_version=" + Versioning::GetBundleVersionShort();

    if (_playerState->getUserParam() != "") {
        _param += "&" + _playerState->getUserParam();
    }
    
    if (_playerState->getGCID() != "") {
        _param += "&gcid=" + _playerState->getGCID();
    }

    if (_playerState->getDeviceToken() != "") {
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    	_param += "&ios_token=" + _playerState->getDeviceToken();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    	_param += "&android_token=" + _playerState->getDeviceToken();
#endif
    }
    
    return _param;
}
