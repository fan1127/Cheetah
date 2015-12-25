/*
 * LinkSameSystemDialog.h
 *
 *  Created on: Jan 14, 2015
 *      Author: patrick.pamplona
 */

#ifndef __Cheetah__LinkSameSystemDialog__
#define __Cheetah__LinkSameSystemDialog__

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "ModalDialog.h"

class LinkSameSystemDialog : public ModalDialog
{
	public:
		static LinkSameSystemDialog *create(CCModalController *parentModalController_);
		virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

		void onOkaySelected();

};


#endif /* __Cheetah__LinkSameSystemDialog__ */
