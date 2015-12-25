//
//  GachaButton.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/28/2557 BE.
//
//

#ifndef __Cheetah__GachaButton__
#define __Cheetah__GachaButton__


#include "../Extensions/CCNikButton.h"


class GachaButton : public CCNikButton {
    
    CCSprite *m_badgeSprite;
    CCLabelBMFont *m_badgeLabel;
    
    CCSprite *m_rotationSprite;
    
    int m_badge;
    
public:
    
    static GachaButton* create();
    virtual bool init();
    
    void setBadge(int badge);
    void runRorateAction();
    void stopRotateAction();
};


#endif /* defined(__Cheetah__GachaButton__) */
