//
//  APIBuildingUpdate.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/21/2557 BE.
//
//

#ifndef __Cheetah__APIBuildingUpdate__
#define __Cheetah__APIBuildingUpdate__

#include "APIRootClass.h"

class APIBuildingUpdate : public APIRootClass {
    
    void *m_ptr_building;
    
public:
    
    static APIBuildingUpdate *create(void *ptr_building,
                                     void *sender,
                                     void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIBuildingUpdate__) */
