//
//  CCModalController.cpp
//  Cheetah
//
//  Created by Kaweerut, Plus Pingya on 10/29/56 BE.
//
//

#include "CCModalController.h"
#include "CCModalView.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "../../Helpers/CCSpriteHelper.h"
#include "../CCProgressBar9Sprite.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"

#include "../../Settings.h"

//std::vector<cocos2d::CCTouchDelegate*> mSavedTouches;


////////////////////////////////////////////////////////
// core method
////////////////////////////////////////////////////////


CCModalController* CCModalController::create() {
    
    CCModalController* pRet = new CCModalController();
    if (pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return pRet;
    
}

bool CCModalController::init() {

    if (!CCLayer::init()) {
        return false;
    }
    
    m_sender = NULL;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();

    CCSprite *_solid_black = blankSpriteWithSize(_screenSize);
    m_fade_sprite = new CCPhatTouchableSprite();
    m_fade_sprite->autorelease();
    m_fade_sprite->initWithTexture(_solid_black->getTexture());
    m_fade_sprite->setAnchorPoint(CCPointZero);
    m_fade_sprite->setColor(ccBLACK);
    m_fade_sprite->setOpacity(100);
    m_fade_sprite->setSender(this);
    m_fade_sprite->onHit = hitShadow;
    addChild(m_fade_sprite);
    
    m_hitBgToCloseAllEnabled = true;
    
    userObject = NULL;

    return true;
}


////////////////////////////////////////////////////////
// Control method
////////////////////////////////////////////////////////

void CCModalController::pushView(CCModalView *v)
{
    int curInd = this->m_views.size();
    
    // Callback willPushViewAtIndex
    if (willPushViewAtIndex) {
        willPushViewAtIndex(curInd);
    }

    v->retain();
    v->m_currentIndex = curInd;
    this->m_views.push_back(v);
    
//    for (int i=(m_views.size()-1); i>0; i--)
//    {
    int i = m_views.size()-1;
    if (i>0) {
        CCModalView *vi = m_views.at(i);
        CCModalView *prev_vi = m_views.at(i-1);
        vi->m_prevModal = prev_vi;
        _hideView(prev_vi);
    }
    
    _showView(v);
    
    if (m_fade_sprite) {
        m_fade_sprite->runAction(CCFadeTo::create(kCCModalViewTransitionDuration, 120));
    }
    
    if (onPushView) {
        onPushView(m_sender);
    }
    
    // Callback didPushViewAtIndex
    if (didPushViewAtIndex) {
        didPushViewAtIndex(curInd);
    }
    
}

void CCModalController::popView(bool trig_callback_) {
    
    int curInd = this->m_views.size()-1;
    
    if (onWillPopView) {
        onWillPopView(m_sender, m_views.at(curInd));
    }
    
    // Callback willPopViewAtIndex
    if (willPopViewAtIndex) {
        willPopViewAtIndex(curInd);
    }
    
    if (curInd==0) {
        this->closeAll();
    } else {
        // get previous dialog, and display it
        CCModalView *prev_vi = m_views.at(curInd-1);
        _showView(prev_vi);

        // then get the current dialog and remove it from m_views list
        CCModalView *v = m_views.at(curInd);
        m_views.pop_back();
        
        //run the hiding-view animation
        _hideViewRelease(v);
    }
    
    // Callback didPopViewAtIndex
    if (didPopViewAtIndex) {
        didPopViewAtIndex(curInd);
    }
    
    if (onPopView && trig_callback_) {
        onPopView(m_sender);
    }
    
}

void CCModalController::popView() {
    popView(true);
}

void CCModalController::closeAll() {
    
    if (m_views.size()>0) {
        
        int curInd = m_views.size()-1;

        _hideAllViews(m_views.at(curInd));
        
        if (m_fade_sprite) {
            m_fade_sprite->runAction(CCFadeTo::create(kCCModalViewTransitionDuration, 0));
        }
        
    }
 
}

void CCModalController::setSender(void *sender_) {
    
    m_sender = sender_;
    
}

// private method
void CCModalController::clearAllViews() {
    
    for (int i=0; i<m_views.size(); i++) {
        if (CCModalView *vi = m_views.at(i)) {
//            vi->removeAllChildrenWithCleanup(true);
//            vi->release();
            vi->removeFromParentAndCleanup(true);
            vi = NULL;
        }
    }
    m_views.clear();
    
    if (onCloseAll) {
        onCloseAll(m_sender);
    }

    this->removeFromParentAndCleanup(false);
    
}

void CCModalController::removeModalView(cocos2d::CCNode *node_, void* sender_) {
    
    if (CCModalView *view = (CCModalView*)sender_) {
        view->removeFromParentAndCleanup(false);
    }
    
}

void CCModalController::removeLastModalView(cocos2d::CCNode *node_, void* sender_) {
    
    if (CCModalView *view = (CCModalView*)sender_) {
        
//        if (view == m_views[m_views.size()-1]) {
//            m_views.pop_back();
//        }
//        view->release();
        view->removeFromParentAndCleanup(true);
        view = NULL;
    }
}

int CCModalController::getNStackView() {
    
    return m_views.size();
    
}

void CCModalController::hitShadow(void *inst_, void *sender_) {

    if (CCModalController *_this = (CCModalController*)sender_) {
        
        if (_this->m_hitBgToCloseAllEnabled)
        {
            if (CCModalView* _view = _this->m_views[_this->m_views.size()-1]) {
                if (_view->m_close_locked) {
                    return;
                }

                if (_view->m_tap_blackbg_to_popview) {
                    _this->popView();
                } else {
                    _this->closeAll();
                }
            }
        }

        CCLog_("hit shadow!");

        if (_this->onHitShadow)
        {
            _this->onHitShadow(((CCPhatTouchableSprite*)inst_)->getLastTouchPos(), _this);
        }
    }
}

void CCModalController::_showView(CCModalView *view_)
{
    this->addChild(view_, 2);
    view_->setTouchPriority(2);
    view_->setScale(0.5f);
    view_->runAction(CCEaseElasticOut::create(CCScaleTo::create(kCCModalViewTransitionDuration*5, view_->m_scaleFactor), kCCModalViewTransitionDuration*5));
    showAllChildrensAnimated(view_, kCCModalViewTransitionDuration);
}

void CCModalController::_hideView(CCModalView *view_)
{
    view_->setScale(view_->m_scaleFactor);
    view_->runAction(CCSequence::create(CCScaleTo::create(kCCModalViewTransitionDuration, kCCModalViewMinimizeScale),
                                        CCCallFuncND::create(this, callfuncND_selector(CCModalController::removeModalView), view_),
                                        NULL));
    hideAllChildrensAnimated(view_, kCCModalViewTransitionDuration);
}

void CCModalController::_hideViewRelease(CCModalView *view_)
{
    view_->setScale(view_->m_scaleFactor);
    view_->runAction(CCSequence::create(CCScaleTo::create(kCCModalViewTransitionDuration, kCCModalViewMinimizeScale),
                                        CCCallFuncND::create(this, callfuncND_selector(CCModalController::removeLastModalView), view_),
                                        NULL));
    hideAllChildrensAnimated(view_, kCCModalViewTransitionDuration);
}

void CCModalController::_hideAllViews(CCModalView *view_)
{
    view_->setScale(view_->m_scaleFactor);
    view_->runAction(CCSequence::create(CCScaleTo::create(kCCModalViewTransitionDuration, kCCModalViewMinimizeScale),
                                        CCCallFunc::create(this, callfunc_selector(CCModalController::clearAllViews)),
                                        NULL));
    hideAllChildrensAnimated(view_, kCCModalViewTransitionDuration);
}

void CCModalController::showAllChildrensAnimated(CCNode* root, float duration) {
    
    showAllChildrensAnimated(root, duration, NULL);
    
}

void CCModalController::showAllChildrensAnimated(CCNode* root, float duration, void* except) {

    if (!root) return;
    
//    for (int i=0; i<root->getChildrenCount(); i++) {
//        CCNodeRGBA *_node = (CCNodeRGBA*)root->getChildren()->objectAtIndex(i);
//        if (!_node || _node == except) continue;
//        _node->setOpacity(0);
//        _node->runAction(CCFadeTo::create(duration, 255));
//        showAllChildrensAnimated(_node, duration, except);
//    }
    
    if (CCArray *_childs = root->getChildren()) {
        for (int i=0; i<_childs->count(); i++) {
            if (CCSprite *_node = (CCSprite*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCSprite) && _node!=except) {
                    _node->setOpacity(0);
                    _node->runAction(CCFadeTo::create(duration, 255));
                    showAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCLabelTTF *_node = (CCLabelTTF*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCLabelTTF) && _node!=except) {
                    _node->setOpacity(0);
                    _node->runAction(CCFadeTo::create(duration, 255));
                    showAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCLabelBMFont *_node = (CCLabelBMFont*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCLabelBMFont) && _node!=except) {
                    _node->setOpacity(0);
                    _node->runAction(CCFadeTo::create(duration, 255));
                    showAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCMenu *_node = (CCMenu*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCMenu) && _node!=except) {
                    _node->setOpacity(0);
                    _node->runAction(CCFadeTo::create(duration, 255));
                    if (CCArray *_sub_childs = _node->getChildren()) {
                        for (int j=0; j<_sub_childs->count(); j++) {
                            if (CCMenuItemSprite *_item_sprite = (CCMenuItemSprite*)_sub_childs->objectAtIndex(j)) {
                                if (typeid(*_item_sprite) == typeid(CCMenuItemSprite)) {
                                    showAllChildrensAnimated(_item_sprite, duration, except);
                                    continue;
                                }
                            }
                            if (CCMenuItemScaleSprite *_item_sprite = (CCMenuItemScaleSprite*)_sub_childs->objectAtIndex(j)) {
                                if (typeid(*_item_sprite) == typeid(CCMenuItemScaleSprite)) {
                                    showAllChildrensAnimated(_item_sprite, duration, except);
                                    continue;
                                }
                            }
                        }
                    }
                    continue;
                }
            }
            if (CCSkeletonAnimation *_node = (CCSkeletonAnimation*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCSkeletonAnimation) && _node!=except) {
                    _node->setOpacity(0);
                    _node->runAction(CCFadeTo::create(duration, 255));
                    showAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCScale9Sprite *_node = (CCScale9Sprite*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCScale9Sprite) && _node!=except) {
                    _node->setOpacity(0);
                    _node->runAction(CCFadeTo::create(duration, 255));
                    showAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCProgressBar9Sprite *_node = (CCProgressBar9Sprite*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCProgressBar9Sprite) && _node!=except) {
                    showAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCNode *_node = (CCNode*)_childs->objectAtIndex(i)) {
                if (_node!=except) {
                    showAllChildrensAnimated(_node, duration);
                }
            }
        }
    }
    
}

void CCModalController::hideAllChildrensAnimated(cocos2d::CCNode *root, float duration) {
    
    hideAllChildrensAnimated(root, duration, NULL);

}

void CCModalController::hideAllChildrensAnimated(cocos2d::CCNode *root, float duration, void* except) {
    
    if (!root) return;
    
//    for (int i=0; i<root->getChildrenCount(); i++) {
//        CCNodeRGBA *_node = (CCNodeRGBA*)root->getChildren()->objectAtIndex(i);
//        if (!_node || _node == except) continue;
//        _node->setOpacity(255);
//        _node->runAction(CCFadeTo::create(duration, 0));
//        hideAllChildrensAnimated(_node, duration, except);
//    }

    if (CCArray *_childs = root->getChildren()) {
        for (int i=0; i<_childs->count(); i++) {
            if (CCSprite *_node = (CCSprite*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCSprite) && _node != except) {
                    _node->setOpacity(255);
                    _node->runAction(CCFadeTo::create(duration, 0));
                    hideAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCLabelTTF *_node = (CCLabelTTF*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCLabelTTF) && _node != except) {
                    _node->setOpacity(255);
                    _node->runAction(CCFadeTo::create(duration, 0));
                    hideAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCLabelBMFont *_node = (CCLabelBMFont*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCLabelBMFont) && _node != except) {
                    _node->setOpacity(255);
                    _node->runAction(CCFadeTo::create(duration, 0));
                    hideAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCMenu *_node = (CCMenu*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCMenu) && _node != except) {
                    _node->setOpacity(255);
                    _node->runAction(CCFadeTo::create(duration, 0));
                    if (CCArray *_sub_childs = _node->getChildren()) {
                        for (int j=0; j<_sub_childs->count(); j++) {
                            if (CCMenuItemSprite *_item_sprite = (CCMenuItemSprite*)_sub_childs->objectAtIndex(j)) {
                                if (typeid(*_item_sprite) == typeid(CCMenuItemSprite)) {
                                    hideAllChildrensAnimated(_item_sprite, duration, except);
                                    continue;
                                }
                            }
                            if (CCMenuItemScaleSprite *_item_sprite = (CCMenuItemScaleSprite*)_sub_childs->objectAtIndex(j)) {
                                if (typeid(*_item_sprite) == typeid(CCMenuItemScaleSprite)) {
                                    hideAllChildrensAnimated(_item_sprite, duration, except);
                                    continue;
                                }
                            }
                        }
                        continue;
                    }
                    continue;
                }
            }
            if (CCSkeletonAnimation *_node = (CCSkeletonAnimation*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCSkeletonAnimation) && _node != except) {
                    _node->setOpacity(255);
                    _node->runAction(CCFadeTo::create(duration, 0));
                    showAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCScale9Sprite *_node = (CCScale9Sprite*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCScale9Sprite) && _node != except) {
                    _node->setOpacity(255);
                    _node->runAction(CCFadeTo::create(duration, 0));
                    hideAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCProgressBar9Sprite *_node = (CCProgressBar9Sprite*)_childs->objectAtIndex(i)) {
                if (typeid(*_node) == typeid(CCProgressBar9Sprite) && _node != except) {
                    hideAllChildrensAnimated(_node, duration, except);
                    continue;
                }
            }
            if (CCNode *_node = (CCNode*)_childs->objectAtIndex(i)) {
                if (_node != except) {
                    hideAllChildrensAnimated(_node, duration, except);
                }
            }
        }
    }
    
}






