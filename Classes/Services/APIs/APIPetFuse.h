//
//  APIPetFuse.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/1/2557 BE.
//
//

#ifndef __Cheetah__APIPetFuse__
#define __Cheetah__APIPetFuse__

#include "APIRootClass.h"

#include "../../VOs/GDPetVO.h"

class APIPetFuse : public APIRootClass {
    
    GDPetVO m_pet_vo;
    void *m_ptr_building;
    bool m_instant;
    
public:
    
    static APIPetFuse *create(GDPetVO pet_vo,
                              void *ptr_building,
                              bool instant,
                              void *sender,
                              void (*callback)(void*, std::string));
    
    virtual std::string name();
    virtual std::string param();
    
};

#endif /* defined(__Cheetah__APIPetFuse__) */
