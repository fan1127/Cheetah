//
//  MiniUpgradeDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/19/2557 BE.
//
//

#ifndef __Cheetah__MiniUpgradeDialog__
#define __Cheetah__MiniUpgradeDialog__

#include "MiniDialog.h"
#include "../../Entities/Building.h"

#define BUTTON_SCALE_DOWN 0.94f

#define MINI_DIALOG_BUTTON_VALUE_OFFSET (8.0f*sizeMultiplier)

class MiniUpgradeDialog : public MiniDialog {
    
public:

    CCMenu* m_upgradeButton;
    CCMenu* m_instantButton;

    static MiniUpgradeDialog *createWithEntity(void *entity_);
    virtual bool compose();

    void updateContentSize(int numberOfElements);

    void addInstantButton(int value_);
    void addUpgradeButton(int value_, const char *top_label_);
    void addRequirementMessage(Building* hq_, GDBuildingVO nxt_hq_bvo_);
    
    void onInstant(cocos2d::CCObject *sender_);
    void onUpgrade(cocos2d::CCObject *sender_);
};

#endif /* defined(__Cheetah__MiniUpgradeDialog__) */
