//
//  APIDecorationUpdate.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/4/2557 BE.
//
//

#ifndef __Cheetah__APIDecorationUpdate__
#define __Cheetah__APIDecorationUpdate__

#include "APIRootClass.h"

class APIDecorationUpdate : public APIRootClass {
    
    void *m_ptr_decoration;
    
public:
    
    static APIDecorationUpdate *create(void *ptr_decoration,
                                       void *sender,
                                       void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIDecorationUpdate__) */
