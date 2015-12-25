//
//  CTutFocuzOnNode.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutFocuzOnNode.h"
#include "CTutUnit.h"

void CTutFocusOnNode::perform(void* ptr_tutUnit) {
    
    CTutUnit* _tutUnit = (CTutUnit*)ptr_tutUnit;
    if (!_tutUnit) return;
    
    _tutUnit->removeAllUIs();
    
    if (scrollView && isoMapBuilding && node) {
        _tutUnit->currentTut = this;
        restore_delegate = scrollView->getDelegate();
        scrollView->setDelegate(this);
        scrollView->zoomOnPosition(node->getPosition() - isoMapBuilding->getContentSize()/4.f, 1.0f, 1.0f);
    }
    
}

void CTutFocusOnNode::scrollViewDidEndZoomingAnimation(CCNikScrollView *scrollView) {
    
    scrollView->setDelegate(restore_delegate);
    restore_delegate = NULL;
    if (callback) {
        callback(sender, "");
    }
    
}
