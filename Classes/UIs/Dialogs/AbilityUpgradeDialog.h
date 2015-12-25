                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     //
//  AbilityUpgradeDialog.h
//  Cheetah
//
//  Created by Teerapat Avalable on 4/7/2557 BE.
//
//

#ifndef __Cheetah__AbilityUpgradeDialog__
#define __Cheetah__AbilityUpgradeDialog__

#include "MiniDialog.h"
#include "../../Entities/Building.h"
#include "../../Entities/Ability.h"

class AbilityUpgradeDialog : public MiniDialog {
    
    GDAbilityVO     m_nxt_avo;
public:
    
    static AbilityUpgradeDialog *createWithEntity(GDAbilityVO avo_, GDAbilityVO nxt_avo_);
    bool compose(GDAbilityVO avo_, GDAbilityVO nxt_avo_);
    
    void addInstantButton(int value_);
    void addUpgradeButton(int value_, const char *top_label_);
    void addUpgradeButton(int value_, const char *top_label_, int tag_);
    void addRequirementMessage(Building* building_, GDAbilityVO avo_);
    
    void onInstant(cocos2d::CCObject *sender_);
    void onUpgrade(cocos2d::CCObject *sender_);
    
    static void onUpdated(void *sender_, std::string resp_json_str_);
    
};

#endif /* defined(__Cheetah__MiniUpgradeDialog__) */
