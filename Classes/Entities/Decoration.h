//
//  Decoration.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/25/2556 BE.
//
//

#ifndef __Cheetah__Decoration__
#define __Cheetah__Decoration__

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>
#include "../VOs/PlayerDecorationVO.h"

#define kDecorationSrcPath          "common/decoration/"

#define kDecorationTypeGeneral      "general"
#define kDecorationTypeFlag         "flag"
#define kDecorationTypeUnwanted     "unwanted"

#define kDecorationStatusUsed       "used"
#define kDecorationStatusKept       "kept"

enum DecorationBuyType {
    DecorationBuyTypeNormal,
    DecorationBuyTypeInstant
};

class Decoration: public cocos2d::CCNode {

    cocos2d::CCLayer*                           m_spineLayer;
//    cocos2d::CCAction*                          m_actionTouch;
    
    float decorationScale;
    cocos2d::CCPoint decorationPoint;
    cocos2d::CCNode *decorationParent;
    int decorationOrder;
    
public:
    
    void*                                       preferenceRootParent;
    cocos2d::extension::CCSkeletonAnimation*    spineDecoration;
    bool                                        didMove;
    
    GDDecorationVO                              dVo;
    PlayerDecorationVO                          pdVo;
    
    static Decoration *createWithDecorationVO(GDDecorationVO dvo_);
    static Decoration *createWithDecorationVO(GDDecorationVO dvo_, PlayerDecorationVO pdvo_);
    
    bool initWithDecorationVO(GDDecorationVO vo, PlayerDecorationVO pbvo);
    bool createDecoration();
    bool createDecoration(bool bumping_);
    
    void updateFlip();
    
    std::string getMoveJsonRendered();
    void keep();
    void onKept(cocos2d::CCObject* object_);
    
    void removeMe();
    void removeMeAnimated(bool animated_);
    
    void removeMeAnimateSmoke();
    void removeMeAnimateLeaf();
    void animateLeaf(cocos2d::extension::CCSkeletonAnimation *spine);
    
};


#endif /* defined(__Cheetah__Decoration__) */
