//
//  CTutFocusOnBuilding.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutFocusOnBuilding.h"
#include "CTutUnit.h"

void CTutFocusOnBuilding::perform(void* tutUnit_) {
    
    CTutUnit* _tutUnit = (CTutUnit*)tutUnit_;
    if (!_tutUnit) return;
    
    tutUnit = tutUnit_;
    
    _tutUnit->removeAllUIs();
    
}

void CTutFocusOnBuilding::update(float delta_) {
    
    if (scrollView && isoMapBuilding && building && !restore_delegate) {
        ((CTutUnit*)tutUnit)->currentTut = this;
        restore_delegate = scrollView->getDelegate();
        scrollView->setDelegate(this);
        scrollView->zoomOnPosition(building->getPosition() - isoMapBuilding->getContentSize()/4.f, 1.0f, 1.0f);// + ccp(0, dat_->building->bVo.src_height*sizeMultiplier*0.5f)
    }
}

void CTutFocusOnBuilding::scrollViewDidEndZoomingAnimation(CCNikScrollView *scrollView_)
{
    if (restore_delegate)
    {
        // fix zooming when the focus is out of building
        auto inPos = scrollView_->isInPosition(building->getPosition() - isoMapBuilding->getContentSize()/4.0f, 1.0f, 1.0f);
        if (!inPos)
        {
            scrollView->zoomOnPosition(building->getPosition() - isoMapBuilding->getContentSize()/4.f, 1.0f, 0.0f);
        }
        
        CCLog_("CTutFocusOnBuilding Finshied! %s %d", building->bVo.title.c_str(), inPos);
        
        scrollView->setDelegate(restore_delegate);
        restore_delegate = NULL;
        if (callback)
        {
            callback(sender, "");
        }
    }
}
