//
//  Merchant.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/20/2557 BE.
//
//

#ifndef __Cheetah__Merchant__
#define __Cheetah__Merchant__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../IsoMap.h"
#include "../Tutorials/CTutManager.h"

#include "../../Extensions/CCPhatTouchableSprite.h"

#include "MerchantItemChoose.h"

#define kMerchantCharacterFilename  "item_merchant"
#define kMerchantStandAdTileIndex   6755
#define kMerchantTouchTileIndexes   {6755, 6756, 6875, 6876, 6996, 6877}
#define kMerchantGohomeTileIndex    2915
#define kMerchantWalkSpeed          20.0f
#define kMerchantAniIdle            "idle_stand"
#define kMerchantAniWalk            "walk"

class Merchant: public cocos2d::CCNode {
    
    bool                                        m_goHome;
    MerchantItemChoose*                         m_itemChoose;
    
public:
    
    cocos2d::extension::CCSkeletonAnimation*    spine;
    CCPhatTouchableSprite*                      bubble;
    
    IsoMap*         isoMap;
    CTutManager*    tutMgr;
    
    static bool isIntroduced();
    static int getTimeSinceLastAppeared();
    static int getTimeMerchantWillAppear();
    static bool check();
    
    static Merchant* createWithIsoMap(IsoMap* isoMap_,  CTutManager* tutMgr_);
    bool initWithIsoMap(IsoMap* isoMap_, CTutManager* tutMgr_);
    void addObjectInBubble();
    void removeObjectInBubble();
    bool isTouched(int tileIndex_);
    bool isHere() { return !m_goHome; }
    void merchantLeave();
    
    static void onBubbleTouched(void* inst_, void* sender_);
    static void onMerchantItemChosen(void* sender_, std::string result_);
    
    static void onMerchantItemBought(void* sender_, std::string resp_json_str_);
    
};

#endif /* defined(__Cheetah__Merchant__) */
