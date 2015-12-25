//
//  ScrollDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#ifndef __Cheetah__ScrollTest__
#define __Cheetah__ScrollTest__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ModalDialog.h"


#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCClippingLayer.h"


#define kCTDButtonMax       1

class ScrollDialog : public ModalDialog, public CCNikScrollViewDelegate {

public:

    ScrollDialog();
    virtual ~ScrollDialog();

    enum Align {LEFT,RIGHT,CENTER};

    int                         buttonTag;
    int                         m_tag;
    CCNikScrollView*            m_scrollView;
    CCClippingLayer*            m_clippingLayer;
    cocos2d::CCSprite*          m_sprite;
    cocos2d::CCPoint            posTouch;

    enum BounceOptions
    {
        BounceHorizontal,
        BounceVertical,
    };

    static ScrollDialog *create(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

    virtual void createScrollContent(BounceOptions bounceOptions, float leftMargin, float rightMargin, float topMargin, float bottomMargin);

    virtual void createScrollContent(BounceOptions bounceOptions, float topMargin, float bottomMargin, float leftMargin, float rightMargin, float contentXShift, float contentYShift);

    virtual void updateContent();

    virtual void replaceControlsParent(CCNode* parent);

    virtual void moveBy(float x, float y);
    
    
    CCNikScrollViewButton* createButton(char const *buttonStyle_, int tag_);
    
    void addTextToButton(char const *buttonText_, CCNode *button_);

    CCNikScrollViewButton*addButtonScroll(const CCPoint &newPosition_,
            int tag_,
            char const *buttonStyle_,
            char const *buttonText_ = NULL,
            float scale_ = 1.25f);

    CCLabelBMFont* addLabelScroll(const CCPoint& newPosition_,
                                  int tag_,
                                  char const* buttonStyle_,
                                  Align align_ = CENTER,
                                  float scale_ = 1.25f);

    CCLabelBMFont* addLabel(const CCPoint& pos, int tag, const char* text, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color, Align align=CENTER);

    CCLabelBMFont* addLabelScrollManual(const CCPoint& newPosition_,
                                        int tag_,
                                        char const* buttonStyle_,
                                        Align align_ = CENTER,
                                        float scale_ = 1.25f);

    CCSprite* addSpriteScroll(const CCPoint& newPosition_,int tag_,char const* buttonStyle_,float scale_ = 1.25f);
    CCSprite* addSpriteScrollManual(const CCPoint &newPosition_, int tag_, char const *buttonStyle_, float scale_ = 1.25f);

    virtual bool compose();
};

#endif /* defined(__Cheetah__CutTreeDialog__) */
