//
//  APIBuildingOpen.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/21/2557 BE.
//
//

#ifndef __Cheetah__APIBuildingOpen__
#define __Cheetah__APIBuildingOpen__

#include "APIRootClass.h"

class APIBuildingOpen : public APIRootClass {
    
    void *m_ptr_building;
    
public:
    
    static APIBuildingOpen *create(void *ptr_building,
                                   void *sender,
                                   void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIBuildingOpen__) */
