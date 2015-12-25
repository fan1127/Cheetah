//
//  ItemStorageDialog.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#ifndef __Cheetah__ItemStorageDialog__
#define __Cheetah__ItemStorageDialog__

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCProgressBar9Sprite.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCLabelHelper.h"

#include "../../Settings.h"

#include "../../Entities/Pet.h"
#include "../../Entities/PlayerState.h"
#include "ScrollDialog.h"

class ItemStoragePopoverDialog;
class CCClippingLayer;
class ItemStorageItem;

class ItemStorageDialog : public ScrollDialog
{
    ItemStoragePopoverDialog*   m_popOver;
    
#ifdef kDialogFooterTextEnabled
    cocos2d::CCLabelBMFont*     m_label_footer;
#endif
    cocos2d::CCLabelBMFont*     m_label_footer_right;
    
    cocos2d::CCMenu*            m_noteButton;
    
    PlayerState*                m_playerState;

    CCPoint                     m_zoomPos;
    
public:
    
    static ItemStorageDialog *create(CCModalController *parentModalController_, PlayerState* playerState_);

    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

    virtual void updateContent();

    PlayerState* getPlayerState() { return m_playerState; }
    
    void selectedItem(ItemStorageItem *item);
    
    virtual void scrollViewWillBeginDragging(CCNikScrollView *view);

    static void onHitShadow(cocos2d::CCPoint&, void *sender_);
    
    static void onItemListUpdated(void* sender_, std::string resp_json_str_);
    
    void onNote(cocos2d::CCObject* obj_);
    
};

#endif /* defined(__Cheetah__ItemStorageDialog__) */
