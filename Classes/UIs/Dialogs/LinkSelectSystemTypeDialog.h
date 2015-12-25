//
//  LinkSelectSystemtypeDialog.h
//  Cheetah
//
//  Created by Kristian Mitra on 1/14/2015 BE.
//
//

#ifndef __Cheetah__LinkSelectSystemDialog__
#define __Cheetah__LinkSelectSystemDialog__

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Settings.h"
#include "../../Extensions/CCNikButton.h"
#include "ModalDialog.h"


struct LinkDeviceResponseData {

	std::string key;
	std::string timeExpire;
	std::string timeCurrent;
};

class LinkSelectSystemTypeDialog : public ModalDialog {

	cocos2d::CCMenu*		m_sameSystemButton;
	cocos2d::CCMenu*		m_otherSystemButton;

public:

    static LinkSelectSystemTypeDialog *create(CCModalController *parentModalController_);

    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

    void onSelectSameSystem(cocos2d::CCObject *sender_);
    void onSelectAnotherSystem(cocos2d::CCObject *sender_);
    bool parseLinkResponseData(const char *json_str);
    void showCodeDialog();


    static void onGetLinkCodeCallback(void *sender, std::string response_json);
};

#endif /* defined(__Cheetah__LinkSelectSystemDialog__) */
