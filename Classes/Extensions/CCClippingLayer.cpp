//
//  CCClippingLayer.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/21/2556 BE.
//
//

#include "CCClippingLayer.h"
USING_NS_CC;

void CCClippingLayer::visit() {
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    CCRect _rect = CCRect(0,
                          0,
                          _screenSize.width,
                          _screenSize.height);
    
    if (preferenceRootParent) {

        CCPoint _pos = this->getPosition();
        CCPoint _scl = ccp(this->getScaleX(), this->getScaleY());
        CCNode *_node = this;
        int _count=0;
        
        while (CCNode *_parent = _node->getParent()) {
            if (_count==0) {
                _pos = _parent->convertToWorldSpace(_pos);
            }
            _count++;
            _scl = ccp(fabsf(_scl.x*_parent->getScaleX()), fabsf(_scl.y*_parent->getScaleY()));
            _node = _parent;
            if (_parent == preferenceRootParent) {
                break;
            }
        }
        
        CCSize _siz = CCSize(getContentSize().width*_scl.x, getContentSize().height*_scl.y);
        CCPoint _ach = getAnchorPoint();
        
        _rect = CCRect(_pos.x-((_siz.width)*_ach.x),
                       _pos.y-((_siz.height)*_ach.y),
                       _siz.width,
                       _siz.height);
        
    }
    
    kmGLPushMatrix();
    glEnable(GL_SCISSOR_TEST);

    cocos2d::CCEGLView::sharedOpenGLView()->setScissorInPoints(_rect.origin.x,
                                                               _rect.origin.y,
                                                               _rect.size.width,
                                                               _rect.size.height);
    
    CCNode::visit();
    glDisable(GL_SCISSOR_TEST);
    kmGLPopMatrix();
    
}