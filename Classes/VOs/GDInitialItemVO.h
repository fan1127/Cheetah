//
//  GDInitialItemVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/24/2556 BE.
//
//

#ifndef __Cheetah__GDInitialItemVO__
#define __Cheetah__GDInitialItemVO__

#include <iostream>
#include <vector>
#include "CommonStruct.h"

struct GDInitialItemVO {
    
    std::string                 type;
    std::vector<IntKeyValue>    items;
    
    GDInitialItemVO();
    
};

#endif /* defined(__Cheetah__GDInitialItemVO__) */
