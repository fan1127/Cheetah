//
//  ResourceProgressButton.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/3/2557 BE.
//
//

#ifndef __Cheetah__ResourceProgressButton__
#define __Cheetah__ResourceProgressButton__

#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCNikProgressBar.h"

#define ResourceProgressButtonChangeStep        50
#define ResourceProgressButtonChangeDuration    0.5f
#define ResourceProgressButtonStepDuration      ResourceProgressButtonChangeDuration/ResourceProgressButtonChangeStep

class ResourceProgressButton : public CCNikButton {
    
    struct TargetAmount {
        int amount;
        int amountMax;
        std::string soundFilename;
    };
    
    CCSprite *m_iconSprite;
    CCLabelBMFont *m_label;
    CCLabelBMFont *m_maxLabel;
    
    CCNikProgressBar *m_progressBar;
    
    bool m_progressBarEnabled;
    
    int m_currentAmount;
    int m_currentAmountMax;
    int m_currentTargetAmount;
    int m_partialAdditionAmount;
    std::vector<TargetAmount> m_targetAmountQueue;

public:
    
    static ResourceProgressButton* create(CCSprite *iconSprite);
    virtual bool init(CCSprite *iconSprite);
    
    void setMaximumAmount(int maximumAmount);
    void setMaximumAmountAnimated(int maximumAmount);
    void setAmount(int amount);
    void addUp(int amount, std::string soundFilename);
    void addUp(int amount, int amountMax, std::string soundFilename);
    
    bool addTargetAmount(int targetAmount);
    bool addTargetAmount(int targetAmount, std::string soundFilename);
    bool addTargetAmount(int targetAmount, int amountMax, std::string soundFilename);
    
    void updateProgressBar();
    void updateRunningNumber(float delta);
    
    CCSprite * getIconSprite() { return m_iconSprite; };
    void animateIconBeating();
    void processTargetAmount();
    
    void setAnimation(cocos2d::CCArray *filenames, float delay);
    
    void setProgressBarEnable(bool enable);
    void setLabelOffsetX(float offsetX);
    
    void setMaxLevelLabelShowed(bool show_);
    
};

#endif /* defined(__Cheetah__ResourceProgressButton__) */
