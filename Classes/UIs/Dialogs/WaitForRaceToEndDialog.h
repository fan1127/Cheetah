//
//  WaitForRaceToEndDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/21/2557 BE.
//
//

#ifndef __Cheetah__WaitForRaceToEndDialog__
#define __Cheetah__WaitForRaceToEndDialog__

#include "ModalDialog.h"
#include "../../Settings.h"

class WaitForRaceToEndDialog : public ModalDialog {

    int m_seconds;
    cocos2d::CCLabelBMFont *m_footer;
    
public:
    
    static WaitForRaceToEndDialog* create(CCModalController* parentModalController, int seconds);
    bool init(CCModalController* parentModalController);

    virtual void updateAtSecond(float delta);

};

#endif /* defined(__Cheetah__WaitForRaceToEndDialog__) */
