//
//  LinkShowCodeDialog.h
//  Cheetah
//
//  Created by Kristian Mitra on 1/14/2015 BE.
//
//

#ifndef __Cheetah__LinkShowCodeDialog__
#define __Cheetah__LinkShowCodeDialog__

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Settings.h"
#include "../../Extensions/CCNikButton.h"
#include "ModalDialog.h"
#include "LinkSelectSystemTypeDialog.h"

class LinkShowCodeDialog : public ModalDialog {

	void initTimer();
	float codeTimer;
	CCLabelBMFont *_labelValidTime;

public:

    static LinkShowCodeDialog *create(CCModalController *parentModalController_, LinkDeviceResponseData responseData);

    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize, LinkDeviceResponseData responseData);
    void updateCodeValidityTimer();
    void onCodeValidityEnded();
    virtual void onClose(cocos2d::CCObject *sender_);
};

#endif /* defined(__Cheetah__LinkShowCodeDialog__) */
