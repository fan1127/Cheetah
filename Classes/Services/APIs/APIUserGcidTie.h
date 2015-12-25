//
//  APIUserGcidTie.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#ifndef __Cheetah__APIUserGcidTie__
#define __Cheetah__APIUserGcidTie__

#include "APIRootClass.h"

class APIUserGcidTie : public APIRootClass {
    
    std::string m_gcid;
    
public:
    
    static APIUserGcidTie *create(std::string gcid,
                                  void *sender,
                                  void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIUserGcidTie__) */
