//
//  APILeaderboardGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APILeaderboardGet.h"
using namespace std;

APILeaderboardGet *APILeaderboardGet::create(string ids,
                                             void *sender,
                                             void (*callback)(void *, std::string)) {
    
    APILeaderboardGet *p = new APILeaderboardGet();
    p->m_ids = ids;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APILeaderboardGet::param() {
    
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += m_ids!=""?"&ids="+m_ids:"";
    
    return _params;
    
}