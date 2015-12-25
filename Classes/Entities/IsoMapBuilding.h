//
//  IsoMapBuilding.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#ifndef __Cheetah__IsoMapBuilding__
#define __Cheetah__IsoMapBuilding__

#include "IsoMap.h"
#include "Building.h"
#include "Track.h"
#include "Decoration.h"
#include "ChestGems.h"
//#include "Merchant/Merchant.h"
#include "Environments/EnvManager.h"

#include "../UIs/Panels/HUDPanel.h"
#include "../Entities/CollectableItems.h"
#include "../UIs/ShopPanel.h"

enum ButtonLayerList {
    BLL_NONE,
    BLL_MAIN,
    BLL_SHOP,
    BLL_PLACEHOLDER,
    BLL_HABITAT,
    BLL_PRODUCER,
    BLL_ACHIEVEMENT,
    BLL_FRIEND
};

#define kUpdateTrackInBackendInterval   10.0f    //seconds


#define kCollectAnimTotalAnimationTime 1.0f
#define kCollectAnimFadeTime 0.4f
#define kCollectAnimDistanceMoved 120.0f

#define kIsoMapBuildingGrabbingCountMax 10.0f

#include "../Entities/EntityAction.h"
#include "../Entities/EntityHabitatAction.h"
#include "../Entities/MapToScreenUI.h"

class IsoMapBuilding: public IsoMap, public CCNikScrollViewDelegate {
    
    Building    *m_grabBuilding;
    bool        m_grabDidStartCount;
    float       m_grabCounting;
    Track       *m_grabTrack;
    Track       *m_grabObstacle;
    Decoration  *m_decorationHolder;
    
    SearchPath _searchPath1(cocos2d::CCPoint coord, int count, int no);
    SearchPath _searchPath2(cocos2d::CCPoint coord, int count, int no);
    
    bool        m_keepShopOpen;
    
    bool        m_track_changed;
    bool        m_hide_entity_action;
    bool        m_hide_entity_dialog;
    
    CCPoint     m_drag_begin = CCPointZero;
    bool        m_ui_is_closing;
    
    bool        m_grabTrackEnabled;
    bool        m_grabDecorationEnabled;
    bool        m_grabBuildingEnabled;
    
    void        checkTutorial();
    
    bool        m_pending_save_call;
    bool        m_makeFirstTouch;
    
    bool        m_cancelBuildingPlacementEnabled;

    void FadeBuildings(float delta);
    void UnfadeBuildings(float delta);
    bool IsOverlappedWithTrack(CCRect bounds, CCPoint pos) const;
    void FadeInBuilding(Building* building, float delta, GLubyte maxOpacity);
    void FadeOutBuilding(Building* building, float delta, GLubyte minOpacity);

    int         m_touch_total;
    int         m_touch_state;
    std::vector<cocos2d::CCPoint> m_touch_locations;
    void _touchEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    
public:
    
    bool        showFeed;
    
    //==== tutorial public ======
    
    PlaceHolder                     *placeHolder;
    EntityAction                    *entityAction;
    MapToScreenUI                   *mapToScreenUI;

    
    HUDPanel                        *hudPanel;
    CollectableItems                *collectableFoods;
    CollectableItems                *collectableCoins;
    CollectableItems                *collectableGems;
    ShopPanel                       *shopPanel;
    void*                           merchant;
    
    EnvManager                      *envManager;
    
    
    cocos2d::extension::CCSkeletonAnimation *revokeBuilding;

    std::string storageFullMessage;
    
    CREATE_FUNC(IsoMapBuilding);
    virtual bool init();
    virtual void update(float delta);
    
    virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    virtual void ccTouchesCancelled(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    
    
    void saveInBackend(float delta);
    void addDroppedItemToBarn();
    
    bool addBuildingWithPlaceHolder(GDBuildingVO vo_);
    bool addBuildingWithPlaceHolder(GDBuildingVO vo_, bool validate_coin_);
    bool addBuildingWithPlaceHolder(Building *building_);
    void setGrabBuildingEnabled(bool enabled_) { m_grabBuildingEnabled = enabled_; }

    bool addTrackWithPlaceHolder(GDTrackVO vo_);
    bool addTrackWithPlaceHolder(Track *track_);
    bool addTrackWithPlaceHolder(Track *track_, bool finishbtn_);
    bool addMoreTrackWithPlaceHolder(GDTrackVO vo_, PlayerTrackVO pptvo_); //requires Previous Player Track VO to calculate the next appropreate track
    void setGrabTrackEnabled(bool enabled_) { m_grabTrackEnabled = enabled_; }
    void setGrabDecorationEnabled(bool enabled_) { m_grabDecorationEnabled = enabled_; }
    
    bool addDecorationOnPlaceHolder(GDDecorationVO vo_);
    bool addDecorationOnPlaceHolder(GDDecorationVO vo_, bool valid_coin_);
    bool addDecorationOnPlaceHolder(Decoration *decoration_);
    bool addDecorationOnPlaceHolder(Decoration *decoration_, bool finishbtn);
    
    void setCancelBuildingPlacementEnabled(bool enabled_) { m_cancelBuildingPlacementEnabled = enabled_; }
    bool getCancelBuildingPlacementEnabled() { return m_cancelBuildingPlacementEnabled; }
    
    void plotChestGems(bool reset);

    static void onGemsShouldBeating(void* sender_, std::string json_str_);
    static void onCoinShouldBeating(void* sender_, std::string json_str_);

    bool hidingPlaceHolder();
    bool removePlaceHolder();
    bool placingStuffFromPlaceHolder();
    bool placingStuffFromPlaceHolder(bool cancel, int type);
    bool removingStuffInPlaceHolder();
    
    static void onFoodIconHitHUD(void* inst_, void* sender_);
    static void onCoinIconHitHUD(void* inst_, void* sender_);
    bool collectGems(ChestGems* chestgems_);
    static void onGemsIconHitHUD(void* inst_, void* sender_);
    static void onGemsCollected(void* sender_, std::string json_str_);
    
    bool setStartOrChangeDirection();
    bool updateMenu();
    
    void closeAllMapUIs();
    
    //Callback for PlaceHolder, in order to lock or unlock camera
    static void lockCamera(void *sender_);
    static void unlockCamera(void *sender_);
    
    virtual void scrollViewTouchBegan(CCNikScrollView* view);
    virtual void scrollViewDidScroll(CCNikScrollView* view);
    virtual void scrollViewDidZoom(CCNikScrollView* view);
    virtual void scrollViewWillBeginDragging(CCNikScrollView* view);
    virtual void scrollViewDidEndDragging(CCNikScrollView* view, bool decelerate);
    virtual void scrollViewWillBeginDecelerating(CCNikScrollView* view);
    virtual void scrollViewDidEndDecelerating(CCNikScrollView* view);
    virtual void scrollViewWillBeginZooming(CCNikScrollView* view);
    virtual void scrollViewDidEndZooming(CCNikScrollView* view);
    
    //Release Function 
    void releaseDecoration(Decoration* decoration_);
    
};

#endif /* defined(__Cheetah__IsoMapBuilding__) */
