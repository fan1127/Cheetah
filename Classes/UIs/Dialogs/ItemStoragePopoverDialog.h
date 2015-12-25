//
//  ItemStoragePopoverDialog.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#ifndef __Cheetah__ItemStoragePopoverDialog__
#define __Cheetah__ItemStoragePopoverDialog__

#include "../../Extensions/CCPopOverDialog.h"

#include "../../VOs/GDItemVO.h"
#include "../../VOs/CommonStruct.h"

class ItemStoragePopoverDialog : public CCPopOverDialog {
    
    void*       m_root;
    
public:

    GDItemVO itvo;
    IntKeyValue *item;
    
    static ItemStoragePopoverDialog *create(IntKeyValue *item_, GDItemVO itvo_, void* root_);
    bool init(IntKeyValue *item_,GDItemVO itvo_, void* root_);
    
    void sell(CCObject *sender);
};


#endif /* defined(__Cheetah__ItemStoragePopoverDialog__) */
