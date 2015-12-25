//
//  GachaResult.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/31/2557 BE.
//
//

#ifndef __Cheetah__GachaResult__
#define __Cheetah__GachaResult__

#include "cocos2d.h"

#include "../VOs/GDPetVO.h"
#include "../VOs/GDItemVO.h"

#include "../Entities/Pet.h"
#include "GachaPanel.h"

USING_NS_CC;

#define TIME_TO_AUTO_DISMISS_GACHA_RESULT 4.0f

class GachaResult : public cocos2d::CCLayerColor
{
    GDPetVO m_petVO;
    GDItemVO m_itemVO;
    
    CCSprite *m_glowSprite1;
    CCSprite *m_glowSprite2;

    CCSprite *m_itemSprite;
    
    CCSkeletonAnimation *m_petSpine;

    GachaPanel* m_gachaPanel;
    
public:

    static GachaResult* create(GDPetVO petVO, GachaPanel* gachaPanel);
    static GachaResult* create(GDItemVO itemVO, GachaPanel* gachaPanel);
    virtual bool init(GDPetVO petVO, GachaPanel* gachaPanel);
    virtual bool init(GDItemVO itemVO, GachaPanel* gachaPanel);
    virtual bool init(GachaPanel* gachaPanel);

    void presentResult(CCNode *onNode,bool animated);
    void dismissResult(bool animated);
    void onDismissResultButton();
    void animateGlow();
    void animateGlow2();
    void animatePetOrItem();
    
    void playStarPaticle();
    
    //virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
};

#endif /* defined(__Cheetah__GachaResult__) */
