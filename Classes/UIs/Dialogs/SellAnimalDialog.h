//
//  SellAnimalDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/5/2557 BE.
//
//

#ifndef __Cheetah__SellAnimalDialog__
#define __Cheetah__SellAnimalDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Entities/Pet.h"
#include "SimpleDialog.h"

#define kSADButtonMax       1

class SellAnimalDialog : public SimpleDialog {
    
    cocos2d::CCMenu*        m_menu[kSADButtonMax];
    int                     m_price;
    
public:
    
    Pet*                    pet;
    
    static SellAnimalDialog *create(CCModalController *parentModalController,
                                    Pet *pet);
    bool init(CCModalController *parentModalController,
              Pet *pet,
              CCSize size);
    
    bool compose();
    virtual void addButton(int index,
                           const char* label,
                           const char *value,
                           const char *icon_filename,
                           float x_pos,
                           const char *button_framename);
    
    virtual void onClose(cocos2d::CCObject *sender);
    virtual void onButton(cocos2d::CCObject *sender);
    
    static void onPetSold(void *sender, std::string resp_json);
    static void onGemsEffectFinished(void* sender);
    
};

#endif /* defined(__Cheetah__SellAnimalDialog__) */
