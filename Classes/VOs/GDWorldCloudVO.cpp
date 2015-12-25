//
//  GDWorldCloudVO.h
//  Cheetah
//
//  Created by teerapat on 3/24/57 BE.
//
//

#include "GDWorldCloudVO.h"

GDWorldCloudVO::GDWorldCloudVO() {
    
    group               = -1;
    
    for (int i=0; i<LINKED_CLOUD_MAX; i++) {
        if (i < UNLOCK_VILL_MAX) unlock_vill[i] = -1;
        link_cloud[i] = -1;
    }
    
    active              = false;
    unlock_level        = 99;
    price               = 0;
    child_cloud         = false;

}