//
//  GDCloudGroupVO.h
//  Cheetah
//
//  Created by Teerapat on 18/3/57 BE.
//
//

#ifndef __Cheetah__GDCloudGroupVO__
#define __Cheetah__GDCloudGroupVO__

//#include "cocos2d.h"
#include <iostream>

struct GDCloudGroupVO {
    
    int                             group;              //id group
    int                             unlock_vill[5];     //หมู่บ้านที่ถูกเมฆปิด
    int                             link_cloud[16];     //เมฆต่อกันหลายก้อน
    int                             unlock_level;       //เลเวลเรดาร์ที่ใช้ unlock
    int                             price;              //ราคา unlock
    GDCloudGroupVO();
    
};

#endif /* defined(__Cheetah__GDCloudGroupVO__) */
