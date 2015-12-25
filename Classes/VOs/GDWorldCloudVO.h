//
//  GDWorldCloudVO.h
//  Cheetah
//
//  Created by Teerapat on 18/3/57 BE.
//
//

#ifndef __Cheetah__GDWorldCloudVO__
#define __Cheetah__GDWorldCloudVO__

//#include "cocos2d.h"
#include <iostream>

#define UNLOCK_VILL_MAX     5
#define LINKED_CLOUD_MAX    16

struct GDWorldCloudVO {
    
    int                             group;
    int                             unlock_vill[UNLOCK_VILL_MAX];     //หลายหมู่บ้านได้
    int                             link_cloud[LINKED_CLOUD_MAX];      //เปิดเมฆพร้อมกันหลายก้อน
    bool                            active;                         //เปิดหรือปิดอยู่
    int                             unlock_level;       //เลเวลเรดาร์ที่ใช้ unlock
    int                             price;              //ราคา unlock
    bool                            child_cloud;
    
    GDWorldCloudVO();
    
};

#endif /* defined(__Cheetah__GDWorldCloudVO__) */
