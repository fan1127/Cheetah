//
//  WinStreak.h
//  Cheetah
//
//  Created by Teerapat on 5/30/57 BE.
//
//

#ifndef __Cheetah__WinStreak__
#define __Cheetah__WinStreak__

//#include <iostream>

#include "../../Extensions/CCNikButton.h"

class WinStreak : public CCNikButton {
    
    float bonus;
    void onTouch(cocos2d::CCNode *sender);
    
public:
    static WinStreak* create(bool win, int win_streak);
    virtual bool init(bool win, int win_streak);
    
    
};

#endif /* defined(__Cheetah__WinStreak__) */
