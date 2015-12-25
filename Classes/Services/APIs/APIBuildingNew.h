//
//  APIBuildingNew.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/20/2557 BE.
//
//

#ifndef __Cheetah__APIBuildingNew__
#define __Cheetah__APIBuildingNew__

#include "APIRootClass.h"

class APIBuildingNew : public APIRootClass {
    
    void *m_ptr_building;
    bool m_instant;
    
public:
    
    static APIBuildingNew *create(void *ptr_building,
                                 bool instant,
                                 void *sender,
                                 void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIBuildingNew__) */
