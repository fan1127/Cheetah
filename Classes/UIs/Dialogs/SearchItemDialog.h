//
//  SearchItemDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/3/2557 BE.
//
//

#ifndef __Cheetah__SearchItemDialog__
#define __Cheetah__SearchItemDialog__

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCClippingLayer.h"
#include "../../Settings.h"

#include "../../VOs/GDItemVO.h"
#include "ScrollDialog.h"

class SearchItemDialog : public ScrollDialog
{
public:
    
    static SearchItemDialog *create(CCModalController *parentModalController_, GDItemVO item_vo_);
    bool init(CCModalController* parentModalController_, GDItemVO item_vo_, CCSize size);
    
    void search(std::string item_type_);
    static void onSearchRetrieved(void* sender_, std::string resp_json_str_);
    
};

#endif /* defined(__Cheetah__SearchItemDialog__) */
