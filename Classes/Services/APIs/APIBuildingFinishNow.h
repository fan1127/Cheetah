//
//  APIBuildingFinishNow.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/25/2557 BE.
//
//

#ifndef __Cheetah__APIBuildingFinishNow__
#define __Cheetah__APIBuildingFinishNow__

#include "APIRootClass.h"

class APIBuildingFinishNow : public APIRootClass {
    
    void *m_ptr_building;
    
public:
    
    static APIBuildingFinishNow *create(void *ptr_building,
                                        void *sender,
                                        void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIBuildingFinishNow__) */
