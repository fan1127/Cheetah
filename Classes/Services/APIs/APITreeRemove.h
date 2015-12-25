//
//  APITreeRemove.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/19/2557 BE.
//
//

#ifndef __Cheetah__APITreeRemove__
#define __Cheetah__APITreeRemove__

#include "APIRootClass.h"

class APITreeRemove : public APIRootClass {
    
    void *m_ptr_tree;
    bool m_instant;
    
public:
    
    static APITreeRemove *create(void *ptr_tree,
                                 bool instant,
                                 void *sender,
                                 void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APITreeRemove__) */
