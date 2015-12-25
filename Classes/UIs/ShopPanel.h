//
//  TrackPanel.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/3/56 BE.
//
//

#ifndef __Cheetah__TrackPanel__
#define __Cheetah__TrackPanel__

#include "cocos2d.h"
#include "../Settings.h"

#include "../Extensions/CCClippingLayer.h"
#include "../Extensions/CCLabelHelper.h"

#include "ShopItemBuilding.h"
#include "ShopItemDecoration.h"
#include "../Extensions/CCNikScrollView.h"
#define KEY_STATUS_DECORATION_NEW  "decoration_new"
enum ShopItemType {
    SIT_BUILDING,
    SIT_TRACK,
    SIT_DECORATION,
};

class CCMenuItemScaleSprite;

class ShopPanel: public cocos2d::CCLayer, public CCNikScrollViewDelegate {
    
    void                                *m_sender;
    
    
    int m_badge;
    
    bool                                m_menuToggleShopOpen        = true; // must be true, because it trigs close panel, and change to false
    
    cocos2d::CCPoint                    m_touchBeganPos;
    
    bool                                m_press = false;
    ShopItemType                        m_type = SIT_BUILDING;
    
    bool                                m_didShowShopBtn = false;
    
    
    std::vector<ShopItemBuilding*>      m_itemBuildingsUnlock;
    std::vector<ShopItemBuilding*>      m_itemBuildingsLockMansion;
    std::vector<ShopItemBuilding*>      m_itemBuildingsLockMaximum;
    
    std::vector<ShopItemDecoration*>    m_itemDecorations;
    
    std::string                         m_itemTypeCanBeGrabbed;
    bool                                m_dangerButtonsEnabled;
    bool                                m_zoomWhenAddBuildings;
    bool                                m_manuallyCloseShopPanelEnabled;
    
protected:
    
    cocos2d::CCLayer*                   m_panelLayer                = NULL;
    cocos2d::CCAction*                  m_panelLayerSlideAction     = NULL;
    cocos2d::extension::CCScale9Sprite* m_panelBg                   = NULL;
    
    cocos2d::CCPoint                    m_panelOpenPos;
    cocos2d::CCPoint                    m_panelClosePos;

    float                               m_tabScale;
    float                               m_tabNormalIconScale;

    CCClippingLayer                     *m_clippingLayer            = NULL;

    CCMenu *m_townMenu;
    CCMenuItemScaleSprite *m_townMenuItem;
    CCMenu *m_pineMenu;
    CCMenuItemScaleSprite *m_pineMenuItem;
    
    cocos2d::CCMenuItemSprite*          m_buttonBuildingShop = NULL;
    cocos2d::CCMenu*                    m_menuBuildingShop   = NULL;
    cocos2d::CCMenuItemSprite*          m_buttonTrackShop = NULL;
    cocos2d::CCMenu*                    m_menuTrackShop   = NULL;
    cocos2d::CCMenuItemSprite*          m_buttonDecoShop = NULL;
    cocos2d::CCMenu*                    m_menuDecoShop   = NULL;
    
    
    void _touchEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    
protected:
    
public:
    
    cocos2d::CCSize                     m_size610;
    
    CCSprite                            *m_badgeDecoreSprite;
    CCLabelBMFont                       *m_badgeDecoreLabel;
    
    CCNikScrollView                     *m_scrollView;
    std::string     warning_msg; 
    bool                                lockCamera          = false;
    
    bool                                itemPressed         = false;
    void*                               graspItem           = NULL;
    
    std::vector<ShopItemBuilding*>      m_itemBuildings;
    
    void setItemCount();
    void scheduleItemCount(float delta_);
    void setDecorationNew();
    void refreshDecoration();
    
    virtual bool init();
    virtual void update(float delta);
    virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesCancelled(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual ~ShopPanel();
    CREATE_FUNC(ShopPanel);
    
    void removeCCTouchDelegateFromScrollView();
    
    void townShop(CCObject *sender);
    void pineShop(CCObject *sender);
    void pressButton(CCObject *sender);
    void addBuildingToMapAndZoom(CCObject *sender);
    void addBuildingToMap(CCObject *sender, bool zoom);
    void addDecorationToMap(CCObject *sender);
    void dragToOutOfBoundScrollView(CCObject *sender);
    
    void openPanel();
    void closePanel();
    void closePanel(bool callback_);
    void doneClosePanel();
    bool isPanelOpen();
    void setItemTypeCanBeGrabbed(std::string item_type_) { m_itemTypeCanBeGrabbed = item_type_; }
    void setDangerButtonsEnabled(bool enabled_) { m_dangerButtonsEnabled = enabled_; }
    void setZoomWhenAddBuildings(bool enabled_) { m_zoomWhenAddBuildings = enabled_; }
    void setManuallyCloseShopPanelEnabled(bool enabled_) { m_manuallyCloseShopPanelEnabled = enabled_; }
    bool getManuallyCloseShopPanelEnabled() { return m_manuallyCloseShopPanelEnabled; }
    void updatePanelContent(ShopItemType type);
    
    void switchShopToBuilding(cocos2d::CCObject* sender);
    void switchShopToTrack(cocos2d::CCObject* sender);
    void switchShopToDecoration(cocos2d::CCObject* sender);
    
    static void addDecorationToMap(void *inst, void *sender);
    
    
    //Callback Delegate
    void setSender(void *sender_);
    void (*onClose)(void *sender_);
    
    /** this callback is designed for Tutorial Class usage, should not be assigned by something else! */
    void (*onCloseForTut)(void* sender_);
    /** this callback is designed for Tutorial Class usage, should not be assigned by something else! */
    void (*onBuildingAddedForTut)(void* sender_, void* item_);
    /** this sender is designed for Tutorial Class usage, should not be assigned by something else! */
    void* senderForTut;
    
    virtual void scrollViewTouchBegan(CCNikScrollView* view);
    virtual void scrollViewWillBeginDragging(CCNikScrollView* view);
    virtual void scrollViewDidEndDragging(CCNikScrollView* view, bool decelerate);
    
    
    void  reLoadShopPanel();
};

#endif /* defined(__Cheetah__TrackPanel__) */
