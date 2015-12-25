//
//  WorldMap.h
//  Cheetah
//
//  Created by Teerapat on 11/3/57 BE.
//
//

#ifndef __Cheetah__WorldMap__
#define __Cheetah__WorldMap__

#include "cocos2d.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCLabelHelper.h"


#include "../../VOs/GDWorldMapVillVO.h"
#include "../../VOs/GDWorldCloudVO.h"
#include "../../VOs/PlayerWorldMapVillageVO.h"

#include "../../Settings.h"
#include "../../Extensions/CCPopOverDialog.h"

#include "../../UIs/Panels/HUDPanel.h"
#include "../../Entities/ChestGems.h"
#include "../../Entities/CollectableItems.h"

#include "../../VOs/HistoryItemVO.h"
#include "../../UIs/BlockingLayer.h"

USING_NS_CC;

#define debugWorld 0

#define MAP_BUTTON_SCALE (1.0f/_scrollView->contentNode->getScale())
#define MAP_ZOOM_LEVEL 1.8f//1.9f <= adjust this value if less can zoom more

#define MAP_MINIMUM_SCALE_FOR_ZOOMING (_scrollView->getMinimumScale()*1.15f)
#define MAP_MAXIMUM_SCALE_FOR_ZOOMING (1.2f)
#define MAP_ZOOM_DURATION (0.8f)//(1.0f)

#define CLOUD_ADDITIONAL_SCALING 1.0f

#define MAX_UNLOCK_VILL_TABLE 155 //the number of max cloud groups in the game

#define DEFAULT_CLOUD_PRICE_SCALE 1.0f
#define ACTIVE_CLOUD_PRICE_SCALE 0.9f

class BlockingLayer;
class WorldMap: public CCLayer, public CCNikScrollViewDelegate {
        
    CCArray                         *_touch_sprites;
    cocos2d::CCSprite*                _selected_village;
    
    std::vector<GDWorldMapVillVO>   _villList;
    std::vector<GDWorldCloudVO>     _cloudList;
    std::vector<cocos2d::CCPoint>   _treasure_pos;
    
    CCLayer                         *unlockLayer;
    CCLayer                         *treasureLayer;

    CCSpriteBatchNode               *_cloudbatchNode;
    CCSpriteBatchNode               *_avatarbatchNode;
    
    //stop animation
    CCSprite                        *logPopOver;

    int*                            unlockVillTable; //maximum village in version 2
    
    int                             levelGarage;
    int                             id_cloudOpen;
    
    double                          homeX;
    double                          homeY;
    bool                            prepareShowPopup; //ป้องกันเรียก popup ขึ้นหลายอัน
    bool                            showPopup;
    
    char                            select_uuid[256];
    bool                            zoomAtVille;
    
    
    CCPoint                         dialogPos;
    CCPoint                         zoomPoint;
    CCPoint                         startTouch;
    
    int                             tagChange;
    
    int                             numLog;
    float                           tickPercent;
    float                           ratioPic;
    
    bool                            showUnlockCloudAfterTutorial;

    void plotChestGems();
    
    void removeNewBalloon();
    void stopChangeVill();
    
    void animateOpenCloud();
    void removeOpenCloud();
    
    void checkVillActivate();
    void openMapWithIndex(int idCloud);
    void openMapWithAnimation(int idCloud);
    void checkUnlockCloud();
    void clearChildrenPopover(cocos2d::CCSprite *sender);

    
    void clearPopup(bool animate);
    void setZOrder(CCNode* node,int value);
    void createTile(CCSprite *_sprite[], float centerX, float centerY, float width, float height, int numtile);
    void removeOverTile(CCSprite *_sprite[], float homeX,float homeY,float width,float height, int numtile);
    void setCloudAnimation(CCSprite *_sprite[], int numtile);
    void showDialogPopup(CCObject* pObj);
    void startZoomMap();
    void startZoomWin();
    
    void gotoMATCH();
    
    void touchHomeButton(cocos2d::CCObject *sender);
    void touchVisitButton(cocos2d::CCObject *sender);
    void touchExploreButton(cocos2d::CCObject *sender);
    void touchChangeButton(cocos2d::CCObject *sender);

    void createCloudFromJson();
    void createCloud();
    void createGrass();
    void clearPrepareTouch();
    
    void checkCloudUnlockTutorial();
    
    // CallBack
    virtual void onEnter();
    virtual void onExit();
    
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    virtual void scrollViewDidScroll(CCNikScrollView* view);
    virtual void scrollViewDidZoom(CCNikScrollView* view);
    
    CCPoint _posCloud;
    
public:
    bool                            canTouch;
    
    cocos2d::CCLayer*               newLayer;
    CCMenu*                         visitButton;
    CCSpriteBatchNode               *_mapbatchNode;
    CCNikScrollView                 *_scrollView;
    HUDPanel                        *hudPanel;
    CCPopOverDialog                 *m_dialog;
    
    CollectableItems                *collectableGems;
    
    static WorldMap* create();
    virtual bool init();
    
    CCSprite                        *_wait;
    CCSprite                        *_bus;
    int indexUnlockCloud;
    void setAnimateWaiting();
    void showPriceUnlockMap();
    
    void gotoVILL();
    
    void createMapFromJson();
    void updateNewVillage(std::string resp_json_str_);
    void updateVillages();
    
    // callback for classes
    static void onGemsIconHitHUD(void* inst_, void* sender_);
    static void onCloudOpenned(void* sender_, std::string resp_json_str_);
    
    void updateCloudPriceColor();
    
    static void onNewOpponent(void* sender_, bool confirm_);
    static void onNewOpponnentReceived(void* sender_, std::string resp_json_str_);
//    static void onGemsCollected(void* sender_, std::string json_str_);
    bool collectGems(ChestGems* chestgems_);
    
    CCArray* getVillages() { return _touch_sprites; }
    cocos2d::CCObject* getSelectedVillage() { return _selected_village; }
    
    bool isAllLegalCloudsOpenned();
    
};

#endif /* defined(__Cheetah__WorldMap__) */
