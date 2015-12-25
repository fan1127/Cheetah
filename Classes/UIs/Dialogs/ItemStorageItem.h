//
//  ItemStorageItem.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#ifndef __Cheetah__ItemStorageItem__
#define __Cheetah__ItemStorageItem__

#include "cocos2d.h"
#include "../../Extensions/CCNikScrollViewButton.h"
#include "../../Extensions/CCLabelHelper.h"

#include "../../VOs/GDItemVO.h"
#include "../../VOs/CommonStruct.h"

class ItemStorageItem : public CCNikScrollViewButton {
    
    
public:
    GDItemVO itvo;
    IntKeyValue *item;
    
    static ItemStorageItem *create(IntKeyValue *item_,GDItemVO itvo_, CCSize size_);
    
    bool init(IntKeyValue *item_,GDItemVO itvo_, CCSize size_);
    
};


#endif /* defined(__Cheetah__ItemStorageItem__) */
