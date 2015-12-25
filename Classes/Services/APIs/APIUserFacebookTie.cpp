//
//  APIUserFacebookTie.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#include "APIUserFacebookTie.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIUserFacebookTie *APIUserFacebookTie::create(string fbid,
                                               string email,
                                               string gender,
                                               string age_range_min,
                                               bool reward_gems,
                                               void *sender,
                                               void (*callback)(void *, std::string)) {
    
    APIUserFacebookTie *p = new APIUserFacebookTie();
    p->m_fbid = fbid;
    p->m_email = email;
    p->m_gender = gender;
    p->m_age_range_min = age_range_min;
    p->m_reward_gems = reward_gems;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIUserFacebookTie::name() {
    
    return "user_facebook_tie";
    
}

string APIUserFacebookTie::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&fbid=" + m_fbid;
    _params += "&email=" + m_email;
    _params += "&gender=" + m_gender;
    _params += "&age_range_min=" + m_age_range_min;
    if (m_reward_gems) {
        _params += "&reward_gems=1";
    }
    
    return _params;
    
}