/*
 * LinkInputCodeDialog.h
 *
 *  Created on: Jan 20, 2015
 *      Author: patrick.pamplona
 */

#ifndef __Cheetah__LinkInputCodeDialog__
#define __Cheetah__LinkInputCodeDialog__

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "ModalDialog.h"
#include "../../../../extensions/GUI/CCEditBox/CCEditBox.h"

#define kCodeDialogSize         CCSize(1200.0f, 500.0f)
#define kCodeDialogPosRatio     ccp(0.5f, 0.75f)
#define kCodeDialogLengthMin    1
#define kCodeDialogCharacters   "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 .:_!@#$()+-="

class LinkInputCodeDialog : public ModalDialog, public cocos2d::extension::CCEditBoxDelegate
{
	cocos2d::extension::CCEditBox*          m_codeBox;
	void editBoxShouldOpen(cocos2d::CCObject* object_);

	public:
		static LinkInputCodeDialog *create(CCModalController *parentModalController_);
		virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

		void onOkaySelected();

		virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);
		virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);

		bool parseLinkResponseData(const char *json_str);

		static void onSendCodeResponse(void *sender, std::string response_json);
};

#endif /* __Cheetah__LinkInputCodeDialog__ */
