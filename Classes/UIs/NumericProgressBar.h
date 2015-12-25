//
//  NumericProgressBar.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad & Teerapat on 4/3/2557 BE.
//
//

#ifndef __Cheetah__NumericProgressBar__
#define __Cheetah__NumericProgressBar__

#include "../Extensions/CCNikButton.h"
#include "cocos-ext.h"
#include "../Extensions/CCNikProgressBar.h"
#include "../Helpers/CCStringHelper.h"

USING_NS_CC;
USING_NS_CC_EXT;

class NumericProgressBar : public CCNode {
    
    CCScale9Sprite *_bar;
    CCLabelBMFont *progress_label;
    CCLabelBMFont *_label;
    CCLabelBMFont *maximum_label;
    CCScale9Sprite *m_progressBar;
    
    bool m_progressBarEnabled;
    
    int numProgress;
    int numMaximum;
    bool disableText;
    
public:
    static NumericProgressBar* create();
    virtual bool init();
    
    void setWaitProgress();
    void setProgressNumeric(unsigned int progress, unsigned int maximum);
    void updateProgressBar();

    virtual void setContentSize(const CCSize& contentSize);

    enum TextPosition
    {
        Centered, // old ui style
        LeftBottom,
//...
    };

    void setTextPosition(TextPosition position);
    void hideText();
    
    void setProgressBarEnable(bool enable);
    void setLabelOffsetX(float offsetX);
    
};

#endif /* defined(__Cheetah__NumericProgressBar__) */
