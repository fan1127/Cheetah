//
//  APIMapCloudOpen.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIMapCloudOpen.h"
using namespace std;

APIMapCloudOpen *APIMapCloudOpen::create(int cloud_id,
                                         bool instant,
                                         void *sender,
                                         void (*callback)(void *, std::string)) {
    
    APIMapCloudOpen *p = new APIMapCloudOpen();
    p->m_cloud_id = cloud_id;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIMapCloudOpen::param() {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    snprintf(_buffer, _buffer_size, "&cloud_id=%d", m_cloud_id);
    _params += _buffer;
    _params += m_instant?"&instant=1":"";
    
    return _params;
    
}