//
//  APIChecklistUpdate.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#ifndef __Cheetah__APIChecklistUpdate__
#define __Cheetah__APIChecklistUpdate__

#include "APIRootClass.h"

class APIChecklistUpdate : public APIRootClass {
    
    std::string m_check_key;
    
public:
    
    static APIChecklistUpdate *create(std::string check_key,
                                      void *sender,
                                      void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIChecklistUpdate__) */
