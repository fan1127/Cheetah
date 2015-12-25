//
//  APITrackUpdate.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#ifndef __Cheetah__APITrackUpdate__
#define __Cheetah__APITrackUpdate__

#include "APIRootClass.h"

class APITrackUpdate : public APIRootClass {
    
public:
    
    static APITrackUpdate *create(void *sender,
                                  void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    virtual std::string param2();
    
};

#endif /* defined(__Cheetah__APITrackUpdate__) */
