//
//  ShopItemDecoration.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/28/56 BE.
//
//

#ifndef __Cheetah__ShopItemDecoration__
#define __Cheetah__ShopItemDecoration__

#include <cocos-ext.h>
#include <spine/spine-cocos2dx.h>

#include "../Extensions/CCNikScrollViewButton.h"
#include "../Extensions/CCLabelHelper.h"
#include "../VOs/GDDecorationVO.h"
//#include "ShopPanel.h"
USING_NS_CC;

class ShopItemDecoration: public CCNikScrollViewButton {

    cocos2d::CCLabelBMFont*                     m_labelMax;
    cocos2d::extension::CCSkeletonAnimation*    m_spineDecorationPreview;
    cocos2d::CCSprite*                          m_arrow;
    CCSprite* _priceTag;
    CCLabelBMFont *_labelPrice;
    CCSprite *_iconCoin;
    cocos2d::extension::CCScale9Sprite *_bg;
    CCLabelBMFont *decorationDesc;
    char bufferShortMsgDec[64];
public:

    GDDecorationVO    vo;
    std::string     warning_msg;

    static ShopItemDecoration *createWithDecorationVO(GDDecorationVO vo_, CCSize size_);
    bool initWithDecorationVO(GDDecorationVO vo_, CCSize size_);
    cocos2d::CCPoint getPreviewDecorationPosition();

    void updateLayoutForDynamicItems();
    void updateState();

    static std::string isAnyReasonCannotBuild(GDDecorationVO vo_);

};

#endif /* defined(__Cheetah__ShopItemDecoration__) */
