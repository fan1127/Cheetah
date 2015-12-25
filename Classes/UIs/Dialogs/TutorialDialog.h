//
//  TutorialDialog.h
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#ifndef __Cheetah__TutorialDialog__
#define __Cheetah__TutorialDialog__


#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCClippingLayer.h"
#include "../../Settings.h"
#include "ScrollDialog.h"

#define kFirstSpaceToTheLeft 0.0f
#define kSpacesBetweenItems 1.125f //1.0f = no spaces
#define kLeftOffset 4.0f
#define kUpperOffset 28.0f

class TutorialDialog : public ModalDialog
{
public:
    static TutorialDialog *create(CCModalController *parentModalController_);
    bool init(CCModalController* parentModalController_, CCSize size);
    
    virtual void onExit();
    void updateContent();
};

#endif /* defined(__Cheetah__TutorialDialog__) */
