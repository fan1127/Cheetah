//
//  RaceResultPanel.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/2/2557 BE.
//
//

#ifndef __Cheetah__RaceResultPanel__
#define __Cheetah__RaceResultPanel__

#include "cocos2d.h"

#include "../Entities/Pet.h"
#include "../Extensions/CCNikButton.h"

USING_NS_CC;
USING_NS_CC_EXT;


#define PET_SCALE_RACERESULT (sizeMultiplier/0.585)


class RaceResultPanel : public cocos2d::CCLayer {
   
public:
    
    enum RaceResultPanelState {
        NONE,
        WIN,
        LOSE
    };
    
private:
    
    Pet *m_pet;
    
    CCSprite *m_spinningGlowSprite;
    
    CCSprite *m_placeCircleSprite;
    CCScale9Sprite *m_ribbonSprite;
    CCLabelBMFont *m_placeLabel;
    
    CCNikButton *m_fanSprite;
    CCLabelBMFont *m_fanLabel;
    CCNikButton *m_itemSprite;
    CCLabelBMFont *m_itemLabel;
    CCNikButton *m_coinSprite;
    CCLabelBMFont *m_coinLabel;
    
    CCNikButton *m_continueButton;
    bool m_buttonEnabled;
    bool m_startEnabled;
    
    cocos2d::extension::CCSkeletonAnimation *m_pet_spine;
    
    bool lockTutorial;
    void playSoundEffectWin();
    bool canTouch;
    
    
public:
    
    bool m_tutSecondRace;
    
    /** this coins_reward could be set publicly, this is just for displaying */
    int coins_reward;
    int fans_won;
    
    RaceResultPanelState raceResultState;
    
    static RaceResultPanel* create(Pet *pet);
    virtual bool init(Pet *pet);
    
    void start();
    
    void createWinResult();
    void createLoseResult();
    
    void animateWinResult();
    void animateLoseResult();
    
    void playStarPaticle();
    
    void skipWinResultAnimation();
    void skipLoseResultAnimation();
    
    void petActionWin();
    void petActionLose();
    
    void setButtonEnabled(bool enabled_) { m_buttonEnabled = enabled_; }
    void setStartEnabled(bool enabled_) { m_startEnabled = enabled_; }
    void continueButton(CCObject *sender);
    
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    
    void onFan(cocos2d::CCObject* object_);
    void onItem(cocos2d::CCObject* object_);
    void onCoin(cocos2d::CCObject* object_);
    
};

#endif /* defined(__Cheetah__RaceResultPanel__) */
