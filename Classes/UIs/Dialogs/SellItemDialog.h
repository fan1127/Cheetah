//
//  SellItemDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/24/2557 BE.
//
//

#ifndef __Cheetah__SellItemDialog__
#define __Cheetah__SellItemDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikButton.h"
#include "../../VOs/GDItemVO.h"
#include "SimpleDialog.h"

#define kSIDButtonMax       1

class SellItemDialog : public SimpleDialog {
    
    cocos2d::CCMenu*        m_menu[kSIDButtonMax];

    void*                   m_entity;
    int                     m_value;
    int                     m_value_max;
    int                     m_price;
    cocos2d::CCLabelBMFont* m_label_value;
    cocos2d::CCLabelBMFont* m_label_price;
    
public:
    
    GDItemVO                itemVo;
    
    static SellItemDialog *create(CCModalController *parentModalController,
                                  GDItemVO ivo,
                                  void* root);

    bool init(CCModalController *parentModalController,
              GDItemVO ivo,
              void* root,
              CCSize size);
    bool compose();

    virtual void addButton(int index,
                           const char* label,
                           const char *value,
                           const char *icon_filename,
                           float x_pos,
                           const char *button_framename);

    void onValueChanged(cocos2d::CCObject* sender);
    
    virtual void onClose(cocos2d::CCObject *sender);
    virtual void onButton(cocos2d::CCObject *sender);
    
    static void onItemSold(void* sender, std::string resp_json);
    static void onCollectCoinsFinished(void* sender);
    
};

#endif /* defined(__Cheetah__SellItemDialog__) */
