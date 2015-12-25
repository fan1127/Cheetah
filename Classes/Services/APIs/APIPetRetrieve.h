//
//  APIPetRetrieve.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/1/2557 BE.
//
//

#ifndef __Cheetah__APIPetRetrieve__
#define __Cheetah__APIPetRetrieve__

#include "APIRootClass.h"

class APIPetRetrieve : public APIRootClass {
    
    void *m_ptr_pet;
    void *m_ptr_building;
    
public:
    
    static APIPetRetrieve *create(void *ptr_pet,
                                  void *ptr_building,
                                  void *sender,
                                  void (*callback)(void *, std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIPetRetrieve__) */
