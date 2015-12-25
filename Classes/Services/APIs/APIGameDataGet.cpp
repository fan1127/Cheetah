//
//  APIGameDataGet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/11/2557 BE.
//
//

#include "APIGameDataGet.h"
using namespace std;

#include "../../Settings.h"
#include "../../Helpers/Versioning.h"
#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

string APIGameDataGet::name() {
    
    return "gamedata_get";
    
}

APIGameDataGet *APIGameDataGet::create(string type) {
    
    APIGameDataGet *p = new APIGameDataGet();
    p->m_type = type;
    if (!p->init()) {
        delete p;
        return NULL;
    }
    p->autorelease();
    return p;
    
}

APIGameDataGet *APIGameDataGet::create(string type,
                                       void *sender,
                                       void (*callback)(void*,std::string)) {
    
    APIGameDataGet *p = new APIGameDataGet();
    p->m_type = type;
    if (!p->init()) {
        delete p;
        return NULL;
    }
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

bool APIGameDataGet::init() {
    
    if (!APIRootClass::init()) {
        return false;
    }
    
    retries = 1;
    
    return true;
    
};

string APIGameDataGet::param() {
    
    string params = "";
    params.append("client=");
    params.append(Versioning::GetBundleVersionShort().c_str());
    params.append("&uid=");
    params.append(g_gameContent->unique_key);
    if (m_type != "" &&
        m_type != "all" &&
        m_type != "*") {
        params.append("&type=");
        params.append(m_type);
    }
    return params;
    
}

