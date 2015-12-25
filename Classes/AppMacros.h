//
//  AppMacros.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#ifndef Cheetah_AppMacros_h
#define Cheetah_AppMacros_h

#include "cocos2d.h"

enum TargetDevice {
    TD_IPHONE,
    TD_IPHONE_RETINA,
    TD_IPAD,
    TD_IPAD_RETINA
};

typedef struct tagResource {
    cocos2d::CCSize size;
    char directory[128];
}Resource;

static Resource smallResource   = {cocos2d::CCSize(480, 320),   "small" };
static Resource mediumResource  = {cocos2d::CCSize(960, 640),   "medium"};
static Resource largeResource   = {cocos2d::CCSize(1024, 768),  "large" };
static Resource xlargeResource  = {cocos2d::CCSize(2048, 1536), "xlarge"};
static Resource defaultResource = {xlargeResource.size, "common"};

#endif /* Cheetah_AppMacros_h */
