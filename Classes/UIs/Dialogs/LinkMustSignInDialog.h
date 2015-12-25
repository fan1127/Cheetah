//
//  LinkMustSignInDialog.h
//  Cheetah
//
//  Created by Kristian Mitra on 2/10/15.
//
//

#ifndef Cheetah_LinkMustSignInDialog_h
#define Cheetah_LinkMustSignInDialog_h

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "ModalDialog.h"

class LinkMustSignInDialog : public ModalDialog
{
public:
    static LinkMustSignInDialog *create(CCModalController *parentModalController_);
    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);
    
    void onOkaySelected();
    
};

#endif
