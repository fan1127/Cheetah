//
//  CreateSpriteHelper.h
//  Cheetah
//
//  Created by Kaweerut on 1/15/57 BE.
//
//

#ifndef __Cheetah__CreateSpriteHelper__
#define __Cheetah__CreateSpriteHelper__

#include "../Settings.h"
#include "cocos2d.h"
#include "../Extensions/CCLabelHelper.h"
USING_NS_CC;

#define LABEL_AVERAGE_TAG 1

enum HorsePowerCOLOR {
    HorsePowerCOLOR_BLUE,
    HorsePowerCOLOR_RED
};


class CreateSpriteHelper {

public:
   
    static CCSprite* getHorsePowerSprite(const char *power, int stars, HorsePowerCOLOR color=HorsePowerCOLOR_BLUE);
};


#endif /* defined(__Cheetah__CreateSpriteHelper__) */
