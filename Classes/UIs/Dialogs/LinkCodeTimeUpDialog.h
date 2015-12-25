/*
 * LinkSameSystemDialog.h
 *
 *  Created on: Jan 14, 2015
 *      Author: patrick.pamplona
 */

#ifndef __Cheetah__LinkCodeTimeUpDialog__
#define __Cheetah__LinkCodeTimeUpDialog__

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "ModalDialog.h"

class LinkCodeTimeUpDialog : public ModalDialog
{
	public:
		static LinkCodeTimeUpDialog *create(CCModalController *parentModalController_);
		virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

		void onOkaySelected();

};


#endif /* __Cheetah__LinkCodeTimeUpDialog__ */
