//
//  AnimalButton.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 9/22/2557 BE.
//
//

#ifndef __Cheetah__AnimalButton__
#define __Cheetah__AnimalButton__

#include <iostream>

#include "cocos2d.h"
#include "../Entities/Pet.h"

class AnimalButton : public cocos2d::CCMenu {
    
    
public:
    
    cocos2d::CCSize itemSize;
    cocos2d::CCSprite *animalPic;
    cocos2d::CCSprite *btnSquare;
    cocos2d::CCSprite *horsePowerSprite;
    
    Pet *pet;
    
    CREATE_FUNC(AnimalButton);
    
    static AnimalButton* createWithPet(Pet *pet_, CCObject* target_, SEL_MenuHandler selector_);
    
    void toggleButtonActive(bool active_);
    void setHorsePowerLabelString(const char *string);
};

#endif /* defined(__Cheetah__AnimalButton__) */
