//
//  PlaceHolder.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#ifndef __Cheetah__PlaceHolder__
#define __Cheetah__PlaceHolder__

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>

#include "../VOs/PlayerBuildingVO.h"
#include "../VOs/PlayerTrackVO.h"

#define GRID_HOLDER_MAX BUILDING_SPACE_COL_MAX*BUILDING_SPACE_ROW_MAX
#define PLACEHOLDER_MENU_MOVING_SPEED           0.5f
#define PLACEHOLDER_MENU_EASING_SPEED           0.5f
#define PLACEHOLDER_MENU_DELAY                  0.05f

#include "Building.h"
#include "Track.h"
#include "Decoration.h"
#include "../Extensions/CCClippingLayer.h"
#include "../UIs/Elements/ItemTrack.h"

#include "../UIs/Panels/HUDPanel.h"
#include "../Extensions/CCNikScrollView.h"
#include "../Extensions/CCPhatTouchableSprite.h"

enum PlaceHolderType {
    PHT_NEW_BUILDING,
    PHT_EDT_BUILDING,
    PHT_NEW_TRACK,
    PHT_EDT_TRACK,
    PHT_NEW_OBSTACLE,
    PHT_EDT_OBSTACLE,
    PHT_NEW_DECORATION,
    PHT_EDT_DECORATION
};

enum PlaceHolderLaunchInstantDialogType {
    PHT_LIDT_NONE,
    PHT_LIDT_BUILDERBUSY,
    PHT_LIDT_NOTENOUGHCOINS,
    PHT_LIDT_NOTENOUGHCOINS_DEC,
    PHT_LIDT_HABITAT
};

class PlaceHolder: public cocos2d::CCNode, public cocos2d::CCTouchDelegate {
    
    void                                        *m_sender;
    cocos2d::CCLayer                            *m_gridLayer;
    
    cocos2d::extension::CCSkeletonAnimation     *m_spineBuilding;
    
    cocos2d::CCSprite                           *m_track;
    cocos2d::CCSprite                           *m_trackStart;
    cocos2d::CCSprite                           *m_trackDirection;
    
    cocos2d::extension::CCSkeletonAnimation     *m_spineDecoration;
    
    CCClippingLayer*                            m_clippingLayer;
    CCNikScrollView*                            m_scrollView;
    std::vector<ItemTrack*>                     m_item_tracks;
    
    cocos2d::CCSprite*                          m_arrowSprites[4];
    
    void _composeGrid(int col_, int row_);
    
    cocos2d::CCPoint                            _oldPos;
    
    void _touchEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    
protected:
    
    
    cocos2d::CCSprite*          m_grid[GRID_HOLDER_MAX];
    
    PlaceHolderType             m_type;
    cocos2d::CCSize             _gridSize;
    cocos2d::CCPoint            _tileSize;
    cocos2d::CCPoint            _shift;
    bool                        _active;
    
    //@june declare diff offset
    cocos2d::CCPoint            _diffOffset;

    cocos2d::CCMenuItemSprite*  menuItemSwitch;
    cocos2d::CCMenu*            menuSwitch;
    cocos2d::CCAction*          menuSwitchMoveAction;
    cocos2d::CCMenuItemSprite*  menuItemNo;
    cocos2d::CCMenu*            menuNo;
    cocos2d::CCAction*          menuNoMoveAction;
//    cocos2d::CCMenuItemSprite*  menuItemYes;
    
    cocos2d::CCAction*          menuYesMoveAction;
    cocos2d::CCMenuItemSprite*  menuItemSet;
    cocos2d::CCMenu*            menuSet;
    cocos2d::CCAction*          menuSetMoveAction;
    
    PlayerBuildingVO            pbVo_recover;
    PlayerTrackVO               ptVo_recover;
    PlayerDecorationVO          pdVo_recover;
    
public:
    
    int                         life;
    int                         state;
    cocos2d::CCMenuItemSprite*  menuItemYes;
    
    Building                    *building = NULL;
    Track                       *track = NULL;
    Decoration                  *decoration = NULL;
    
    cocos2d::CCLayer            *parent;
    bool                        camLock;
    bool                        shopOpen = false;
    bool                        didBtnPressed;
    bool                        trackStart_recover;
    CCPhatTouchableSprite*      bgMenuSprite;
    
    cocos2d::CCMenu*            menuYes;
    
    static PlaceHolder* createWithBuilding(Building *building_, HUDPanel *panel_, PlaceHolderType type_);
    static PlaceHolder* createWithTrack(Track *track_, HUDPanel *panel_, PlaceHolderType type_);
    static PlaceHolder* createWithTrack(Track *track_, HUDPanel *panel_, PlaceHolderType type_, bool finishbtn_);
    static PlaceHolder* createWithTrackAndObstacle(Track *track_, Track *obstacle_, HUDPanel *panel_, PlaceHolderType type_, bool finishbtn_);
    static PlaceHolder* createWithDecoration(Decoration *decoration_, HUDPanel *panel_, PlaceHolderType type_);
    
    bool initWithBuilding(Building* building_, HUDPanel* panel_, PlaceHolderType type_);
    bool initWithTrack(Track *track_, HUDPanel *panel_, PlaceHolderType type_, bool inishbtn_);
    bool initWithDecoration(Decoration *decoration_, HUDPanel *panel_, PlaceHolderType type_);
    
    virtual ~PlaceHolder();
    virtual void onEnter();
    virtual void onExit();
    
    bool composeMenu(HUDPanel *panel_, bool finish_);
    void updateMenu(HUDPanel *panel_, bool ignorechange_); // param: ignorechange = skip changing button menu
    bool addMoreTrack(Track *track_, HUDPanel *panel_);
    
    virtual void update(float delta_);
    virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesCancelled(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void done();
    
    PlaceHolderType type(){ return m_type; };
    
    void updatePlaceHolder(HUDPanel *panel_);
    
    void hideButton();
    void tellParentToReleaseThis(CCNode *sender_);
    
    void setHolder(cocos2d::CCPoint tilesize_, cocos2d::CCPoint shift_);
    
    bool isOnTrack();
    bool canPlace();
    bool isActive();
    
    bool revert();

    void snapWithGrid(cocos2d::CCPoint location_);
    void snapWithQuadGrid(cocos2d::CCPoint location_);
    void setMeAGoodPosition();
    void verifyMap(void *ptr_isomap_);
    
    PlaceHolderLaunchInstantDialogType getLaunchInstantDialogType(Building *builder);

    void place(cocos2d::CCObject *sender_);
    void cancel(cocos2d::CCObject *sender_);
    void remove(cocos2d::CCObject *sender_);
    void flip(cocos2d::CCObject *sender_);
    
    bool placeBuilding(BuildingBuildType buildType);
    bool placeTrack();
    bool placeDecoration(DecorationBuyType buyType);
    
    void rotateObject();
    void switchTrack(bool force_);
    void setStartOrChangeDirection(cocos2d::CCObject *sender_);
    
    void moveBy(cocos2d::CCPoint coord_);
    PlaceHolderType getType();
    cocos2d::CCSize getGridSize();
    
    PlayerBuildingVO getBuildingRecoverVO();
    PlayerTrackVO getTrackRecoverVO();
    PlayerDecorationVO getDecorationRecoverVO();
    
    void setSender(void *sender_);
    void (*onBegin)(void *sender_);
    void (*onDone)(void *sender_);
    
    static void onHit(void* inst_, void* sender_);
    
    //select track
    void selectTrack(CCObject *sender);
    
};

#endif /* defined(__Cheetah__PlaceHolder__) */
