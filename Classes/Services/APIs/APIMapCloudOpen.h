//
//  APIMapCloudOpen.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#ifndef __Cheetah__APIMapCloudOpen__
#define __Cheetah__APIMapCloudOpen__

#include "APIRootClass.h"
#include "../../Entities/PlayerState.h"

class APIMapCloudOpen : public APIRootClass {
    
    int m_cloud_id;
    bool m_instant;
    
public:
    
    static APIMapCloudOpen *create(int cloud_id,
                                   bool instant,
                                   void *sender,
                                   void (*callback)(void*,std::string));
    
    virtual std::string name() { return "map_cloud_open"; }
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIMapCloudOpen__) */
