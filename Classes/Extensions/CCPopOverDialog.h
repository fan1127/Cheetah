//
//  CCPopOverDialog.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#ifndef __Cheetah__CCPopOverDialog__
#define __Cheetah__CCPopOverDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#define EXTRA_ARROW_OFFSET (1.0f)//*sizeMultiplier

USING_NS_CC;
USING_NS_CC_EXT;
typedef enum {
    CCPopoverArrowDirectionUp = 1UL << 0,
    CCPopoverArrowDirectionDown = 1UL << 1,
    CCPopoverArrowDirectionLeft = 1UL <<2,
    CCPopoverArrowDirectionRight = 1UL << 3,
    CCPopoverArrowDirectionAny = CCPopoverArrowDirectionUp | CCPopoverArrowDirectionDown | CCPopoverArrowDirectionLeft | CCPopoverArrowDirectionRight
}CCPopoverArrowDirection;

#define kCCPopOverDialogZOrder  1000

class CCPopOverDialog :public cocos2d::CCLayerRGBA {
    
protected:
    CCPopoverArrowDirection m_direction;
    
    CCSprite *m_arrowSprite;
    CCScale9Sprite *m_bg;
    
    CCNode *contentNode;
    
    CCSize m_contentSize;
    
    CCPoint m_positionAdjustment;
    bool m_autoDismiss;
    bool m_autoBoundCalcuration;
    
public:
    
    static CCPopOverDialog* create(CCSize contentSize);
    static CCPopOverDialog* create(CCSize contentSize, CCPopoverArrowDirection direction);
    
    
    
    virtual bool init(CCSize contentSize, CCPopoverArrowDirection direction);
    
    virtual void addChild(CCNode * child);
    virtual void addChild(CCNode * child, int zOrder);
    virtual void addChild(CCNode *child, int zOrder, int tag);
    
    CCSize getContentSize() { return contentNode->getContentSize(); };
    
    void setPositionAdjustment(CCPoint offset) { m_positionAdjustment = offset; };
    
    void setAutoDismiss(bool autoDismiss) { m_autoDismiss = autoDismiss; };
    void setAutoBoundCalcuration(bool autoBoundCalcuration) { m_autoBoundCalcuration = autoBoundCalcuration; }
        
    void presentPopover(CCNode *onNode, bool animated);
    void presentPopover(CCNode *parentNode, CCNode *onNode, bool animated);
    void dismissPopover(bool animated);
    void dismissPopover(bool animated, bool removeSelf);
    static void dismissAllPopovers();
    
    CCPopoverArrowDirection findDirection(CCPoint point, CCNode *parentNode, CCNode *onNode, CCPopoverArrowDirection direction);
    CCPopoverArrowDirection findDirection(CCPoint point, CCNode *parentNode, CCNode *onNode, CCPopoverArrowDirection direction, int count);
    CCPoint findPosition(CCPoint point);
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

};


#endif /* defined(__Cheetah__CCPopOverDialog__) */
