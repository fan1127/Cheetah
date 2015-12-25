//
//  APIBuildingUpgrade.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/21/2557 BE.
//
//

#ifndef __Cheetah__APIBuildingUpgrade__
#define __Cheetah__APIBuildingUpgrade__

#include "APIRootClass.h"

class APIBuildingUpgrade : public APIRootClass {
    
    void *m_ptr_building;
    int m_collect_amount;
    int m_produced_amount;
    int m_instant;
    
public:
    
    static APIBuildingUpgrade *create(void *ptr_building,
                                      int collect_amount,
                                      int produced_amount,
                                      int instant,
                                      void *sender,
                                      void (*callback)(void*,std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIBuildingUpgrade__) */
