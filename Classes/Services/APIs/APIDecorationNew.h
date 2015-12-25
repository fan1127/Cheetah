//
//  APIDecorationNew.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/4/2557 BE.
//
//

#ifndef __Cheetah__APIDecorationNew__
#define __Cheetah__APIDecorationNew__

#include "APIRootClass.h"

class APIDecorationNew : public APIRootClass {
    
    void *m_ptr_decoration;
    bool m_instant;
    
public:
    
    static APIDecorationNew *create(void *ptr_decoration,
                                    bool instant,
                                    void *sender,
                                    void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIDecorationNew__) */
