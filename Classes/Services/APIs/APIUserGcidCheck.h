//
//  APIUserGcidCheck.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#ifndef __Cheetah__APIUserGcidCheck__
#define __Cheetah__APIUserGcidCheck__

#include "APIRootClass.h"

class APIUserGcidCheck : public APIRootClass {
    
    std::string m_gcid;
    
public:
    
    static APIUserGcidCheck *create(std::string gcid,
                                    void *sender,
                                    void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIUserGcidCheck__) */
