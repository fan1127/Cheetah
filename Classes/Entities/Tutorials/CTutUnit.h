//
//  CTutUnit.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutUnit__
#define __Cheetah__CTutUnit__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/CCPhatTouchableSprite.h"

#include "CTut.h"
#include "CTutSpeech.h"
#include "CTutFocuzOnNode.h"
#include "CTutFocusOnBuilding.h"
#include "CTutPointAtBuilding.h"
#include "CTutDragFoodToPet.h"
#include "CTutDragBuildingToBuild.h"
#include "CTutPointAtButton.h"
#include "CTutPointAtVillage.h"
#include "CTutPointAtNikButton.h"
#include "CTutPointAtPowerupButtons.h"
#include "CTutPointAtPlaceHolderButton.h"
#include "CTutManualControl.h"

class CTutUnit: public cocos2d::CCLayer {
    
protected:
    
    bool        m_tutActive;
    bool        m_tutFinish;
    std::string m_key;
    
    std::vector<CTut*>      m_tuts;
    int                     m_tutSelector;
    int                     m_tutSelected;
    void _cleanAllTuts();
    
    CCNikButton*    m_bg;
    
public:
    
    cocos2d::extension::CCSkeletonAnimation*    characterSpineUncle;
    cocos2d::extension::CCSkeletonAnimation*    characterSpineSarah;
    cocos2d::extension::CCSkeletonAnimation*    characterSpineMerchant;
    cocos2d::extension::CCSkeletonAnimation*    characterSpineFans;
    
    cocos2d::extension::CCSkeletonAnimation*    m_spineBuildingPreview;
    CTutCharacter                               characterLast;
    std::string                                 characterLastAni;
    CTut*                                       currentTut;
    
    cocos2d::extension::CCScale9Sprite*         messageDialogSprite;
    cocos2d::extension::CCScale9Sprite*         chooseDialogSprite;
    
    CREATE_FUNC(CTutUnit);
    virtual bool init();
    virtual ~CTutUnit();
    virtual void update(float delta_);
    virtual bool trig();
    
    virtual void start() { };
    virtual void finish() { };
    
    void addSpotlightBG(void* sender_, void (*callback_)(void* inst_, void* sender_));
    void removeSpotlightBG();
    void removeChooseDialogSprite();
    void removeAllUIs();
    
    void goToNextTutorial() {
        m_tutSelector++;
    };
    void goToPreviousTutorial() {
        m_tutSelector--;
    };
    void skipThisTutorial() {
        m_tutSelector = m_tuts.size();
    };
    bool isOver() {
        return (m_tutSelector >= m_tuts.size());
    };
    
    bool isActive() { return m_tutActive; }
    bool isFinish() { return m_tutFinish; }
    std::string getKey() { return m_key; }
    
};

#endif /* defined(__Cheetah__CTutUnit__) */
