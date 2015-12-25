//
//  AnimalBarnItem.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#ifndef __Cheetah__AnimalBarnItem__
#define __Cheetah__AnimalBarnItem__

#include "cocos2d.h"
#include "../../Extensions/CCNikScrollViewButton.h"

#include "../../VOs/GDPetVO.h"
#include "../../Entities/Pet.h"

class AnimalBarnItem : public CCNikScrollViewButton {
    
    
public:
    Pet             *pet;
    
    static AnimalBarnItem *create(Pet *pet_);
    
    bool init(Pet *pet_);
    
};

#endif /* defined(__Cheetah__AnimalBarnItem__) */
