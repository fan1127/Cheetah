//
//  APIBuildingCollect.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/25/2557 BE.
//
//

#ifndef __Cheetah__APIBuildingCollect__
#define __Cheetah__APIBuildingCollect__

#include "APIRootClass.h"

class APIBuildingCollect : public APIRootClass {
    
    void *m_ptr_building;
    int m_collect_amount;
    int m_produced_amount;
    
public:
    
    static APIBuildingCollect *create(void *ptr_building,
                                      int collect_amount,
                                      int produced_amount,
                                      void *sender,
                                      void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIBuildingCollect__) */
