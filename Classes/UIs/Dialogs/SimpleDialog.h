//
//  ModalDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#ifndef __Cheetah__SimpleDialog__
#define __Cheetah__SimpleDialog__


#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/Modal/CCModalController.h"


namespace cocos2d { namespace extension { class CCScale9Sprite; } }
class CCPhatTouchableSprite;
class CCLabelUpdater;

class SimpleDialog : public CCModalView
{
protected:

    CCPhatTouchableSprite*              m_block_sprite;
    cocos2d::extension::CCScale9Sprite* m_background;
    cocos2d::CCLabelBMFont*             m_title;
    cocos2d::CCLabelBMFont*             m_description;
    cocos2d::CCMenu*                    m_closeButton;
    cocos2d::CCNode*                    m_iconLine;
    std::vector<cocos2d::CCMenu*>       m_buttons;

public:

    SimpleDialog();

    virtual ~SimpleDialog();

    static SimpleDialog* create(CCModalController* parent);

    static SimpleDialog *create(CCModalController* parent, cocos2d::CCSize dialogSize);

    virtual bool init(CCModalController* parent, cocos2d::CCSize dialogSize);

    virtual void moveBy(float x, float y);

    enum ButtonAllign
    {
        Left,
        Center,
        Right
    };

    enum ButtonIcon
    {
        Coin,
        Gem,
        Food,
        None,
    };

    virtual void createButton(const char* title, ButtonAllign align, ButtonIcon icon);

    virtual void createButton(CCLabelUpdater* levelUpdater, ButtonAllign align, ButtonIcon icon);

    virtual void createIconLine(const char* label, ButtonIcon icon);

    virtual void createTitle(const char *name);

    virtual void setTitle(const char *name);

    virtual void createDescription(const char *description);

    virtual void setDescription(const char* desc);

    virtual void onClose(cocos2d::CCObject *sender);

    virtual void onButton(cocos2d::CCObject *sender);

    virtual void autoResize(float minWidth, float minHeight);
};

#endif /* defined(__Cheetah__StandardDialog__) */
