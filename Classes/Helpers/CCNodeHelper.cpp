//
//  CCNodeHelper.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 7/25/2557 BE.
//
//

#include "CCNodeHelper.h"
USING_NS_CC;

CCSize CCNodeHelper::getContentSize(CCNode *node_) {
    
    if (node_) {
        return CCSize(node_->getContentSize().width * node_->getScaleX(),
                      node_->getContentSize().height * node_->getScaleY());
    }
    return CCSizeZero;
    
}