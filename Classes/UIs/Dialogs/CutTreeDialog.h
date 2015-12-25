//
//  CutTreeDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#ifndef __Cheetah__CutTreeDialog__
#define __Cheetah__CutTreeDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Entities/Decoration.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikButton.h"
#include "SimpleDialog.h"

#define kCTDButtonMax       1

class CutTreeDialog : public SimpleDialog
{
public:

    void             *m_entity;
    Decoration       *unwanted_tree;
    
    static CutTreeDialog *create(CCModalController *parentModalController, Decoration *tree);

    bool init(CCModalController *parentModalController, Decoration *tree, CCSize size);

    virtual void onButton(cocos2d::CCObject *sender);
    
    static void onTreeRemoved(void* sender, std::string resp_str);
};

#endif /* defined(__Cheetah__CutTreeDialog__) */
