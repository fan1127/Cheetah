//
//  GDWorldBlockVO.h
//  Cheetah
//
//  Created by Teerapat on 18/3/57 BE.
//
//

#ifndef __Cheetah__GDWorldBlockVO__
#define __Cheetah__GDWorldBlockVO__

//#include "cocos2d.h"
#include <iostream>

struct GDWorldBlockVO {
    
    int                             blockConnect[8];    //ช่องที่ติดกัน
    bool                            active;             //เปิดหรือปิดอยู่
    
    GDWorldBlockVO();
    
};

#endif /* defined(__Cheetah__GDWorldBlockVO__) */
