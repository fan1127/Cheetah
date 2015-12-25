//
//  MapToScreenUI.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/18/2557 BE.
//
//

#include "MapToScreenUI.h"
USING_NS_CC;

#include "../UIs/Dialogs/MiniUpgradeDialog.h"

bool MapToScreenUI::init() {
    
    if (!CCLayer::init()) {
        return false;
    }
    
//    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
//    
//    MiniDialog *_mini_dialog = MiniDialog::create();
//    _mini_dialog->setPosition(ccp(_screenSize.width*0.3f, _screenSize.height*0.5f));
//    addChild(_mini_dialog);
    
    return true;
    
}

void MapToScreenUI::closeAllUIs() {
    
    if (CCArray *childs = getChildren()) {
        for (int i=0; i<childs->count(); i++) {
//            if (MiniUpgradeDialog *_dialog = (MiniUpgradeDialog*)childs->objectAtIndex(i)) {
//                _dialog->setSender(this);
//                _dialog->onReleaseMe = onReleaseObject;
//                _dialog->close();
//            }
            if (MiniDialog *_dialog = (MiniDialog*)childs->objectAtIndex(i)) {
//                _dialog->setSender(this);
//                _dialog->onReleaseMe = onReleaseObject;
                _dialog->close();
            }
        }
    }
    
}

void MapToScreenUI::onReleaseObject(void *inst_, void *sender_) {
    
//    if (MiniUpgradeDialog *_dialog = (MiniUpgradeDialog*)inst_) {
//        if (typeid(*_dialog) == typeid(MiniUpgradeDialog)) {
//            _dialog->removeFromParentAndCleanup(true);
//            _dialog = NULL;
//        }
//    }
    if (CCNode *_node = (CCNode*)inst_) {
//        if (typeid(*_node) == typeid(CCNode)) {
            _node->removeFromParentAndCleanup(true);
            _node = NULL;
//        }
    }
    
}