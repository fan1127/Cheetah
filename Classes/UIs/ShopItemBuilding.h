//
//  ShopItemBuilding.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/28/56 BE.
//
//

#ifndef __Cheetah__ShopItemBuilding__
#define __Cheetah__ShopItemBuilding__

#include <cocos-ext.h>
#include <spine/spine-cocos2dx.h>

#include "../Extensions/CCNikScrollViewButton.h"
#include "../Extensions/CCLabelHelper.h"
#include "../VOs/GDBuildingVO.h"
//#include "ShopPanel.h"
USING_NS_CC;

class ShopItemBuilding: public CCNikScrollViewButton {

    cocos2d::CCLabelBMFont*                     m_labelMax;
    cocos2d::extension::CCSkeletonAnimation*    m_spineBuildingPreview;
    cocos2d::CCSprite*                          m_arrow;
    static char short_msg[64];
    CCSprite* _priceTag;
    CCLabelBMFont *_labelPrice;
    CCSprite *_iconCoin;
    CCLabelBMFont *_labelTime;
    CCSprite *_iconTime;
    cocos2d::extension::CCScale9Sprite *_bg;
    CCLabelBMFont *buildingDesc;
    
    std::string getPriceAsString();
    
public:
    
    GDBuildingVO    vo;
    std::string     warning_msg;
    
    static ShopItemBuilding *createWithBuildingVO(GDBuildingVO vo_, CCSize size_);
    bool initWithBuildingVO(GDBuildingVO vo_, CCSize size_);
    cocos2d::CCPoint getPreviewBuildingPosition();
    
    void updateLayoutForDynamicItems();
    void updateState();
    
    static std::string isAnyReasonCannotBuild(GDBuildingVO vo_);
};

#endif /* defined(__Cheetah__ShopItemBuilding__) */
