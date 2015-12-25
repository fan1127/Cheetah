//
//  ModalDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#ifndef __Cheetah__StandardDialog__
#define __Cheetah__StandardDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Entities/Decoration.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Helpers/UIHelper.h"

USING_NS_CC;
USING_NS_CC_EXT;


#define kCTDButtonMax       1
class LoadingLayer;

class ModalDialog : public CCModalView
{
protected:

    CCPhatTouchableSprite*  m_block_sprite;
    CCScale9Sprite*         m_background;
    CCNode*                 m_frontBackground;
    CCLabelBMFont*          m_title;
    CCScale9Sprite*         m_header;
    CCMenu*                 m_help;
    cocos2d::CCMenu*        m_closeButton;
    cocos2d::CCMenu*        m_menu[kCTDButtonMax];
    void*                   m_entity;
    bool                    m_closeButtonEnabled;
    LoadingLayer*           m_loadingLayer;

public:

    ModalDialog();
    virtual ~ModalDialog();

    static ModalDialog *create(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

    CCNode*     getFrontBG() const { return m_frontBackground; }

    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

    virtual void createContent(float leftMargin, float rightMargin, float topMargin, float bottomMargin);

    virtual void createContent(float topMargin, float bottomMargin, float leftMargin, float rightMargin, float contentXShift, float contentYShift);

    virtual void createFrontBackground(UIHelper::ScalableSprites sprite);

    virtual void update(float delta_) {}

    virtual void updateContent() {}

    virtual void moveBy(float x, float y);

    virtual void addTitle(const char *icon_filename_);

    virtual void showLoading();

    virtual void hideLoading();

    virtual void addHelpButton(const char* noteId, void (CCObject::*SEL_MenuHandler)(CCObject*));

    virtual void setTitle(const char* name);

    virtual void onClose(cocos2d::CCObject *sender_);

    virtual void onButton(cocos2d::CCObject *sender_);

    void setCloseButtonEnabled(bool enabled_) { m_closeButtonEnabled = enabled_; }

    void *getEntity() { return m_entity; }
};

#endif /* defined(__Cheetah__StandardDialog__) */
