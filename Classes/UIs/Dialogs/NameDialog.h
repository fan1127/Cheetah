//
//  NameDialog.h
//  Cheetah
//
//  Created by Teerapat on 4/11/2557 BE.
//
//

#ifndef __Cheetah__NameDialog__
#define __Cheetah__NameDialog__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "../../../../extensions/GUI/CCEditBox/CCEditBox.h"

#define kNameDialogSize         CCSize(1200.0f, 500.0f)
#define kNameDialogPosRatio     ccp(0.5f, 0.75f)
#define kNameDialogLengthMin    1
#define kNameDialogCharacters   "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890 .:_!@#$()+-="

class NameDialog: public cocos2d::CCNode, public cocos2d::extension::CCEditBoxDelegate {
    
    cocos2d::extension::CCEditBox*          m_nameBox;
    cocos2d::CCLabelBMFont*                 m_warnTxt;
    std::string                             m_backupInput;
    
    void onSaveNameButton(cocos2d::CCObject* sender_);
    void editBoxShouldOpen(cocos2d::CCObject* object_);
    
public:
    
    static NameDialog *create(void* sender_, void (*callback_)(void* sender_, std::string resp_tr_));
    virtual bool init();
    
    virtual void editBoxReturn(cocos2d::extension::CCEditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text);

    void* sender;
    void (*callback)(void* sender_, std::string resp_json_str_);
    
};

#endif /* defined(__Cheetah__NameDialog__) */