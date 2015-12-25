/*
 * LinkDeviceDialog.h
 *
 *  Created on: Jan 14, 2015
 *      Author: patrick.pamplona
 */

#ifndef __Cheetah__LinkSelectTypeDialog__
#define __Cheetah__LinkSelectTypeDialog__

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "ModalDialog.h"

class LinkSelectTypeDialog : public ModalDialog
{
	public:
		static LinkSelectTypeDialog *create(CCModalController *parentModalController_);
		virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

		void onOldDeviceSelected();
		void onNewDeviceSelected();

};


#endif /* __Cheetah__LinkSelectTypeDialog__ */
