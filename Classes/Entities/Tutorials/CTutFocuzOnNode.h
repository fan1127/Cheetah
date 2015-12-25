//
//  CTutFocuzOnNode.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutFocuzOnNode__
#define __Cheetah__CTutFocuzOnNode__

#include "CTut.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../IsoMapBuilding.h"

class CTutFocusOnNode: public CTut, CCNikScrollViewDelegate {
    
public:
    
    /** Required instances, in order to perform the tutorial, these instances are required */
    CCNikScrollView*    scrollView;
    IsoMapBuilding*     isoMapBuilding;
    cocos2d::CCNode*    node;
    
    CCNikScrollViewDelegate* restore_delegate;
    
    CTutFocusOnNode(CCNikScrollView* scrollView,
                    IsoMapBuilding* isoMapBuilding,
                    cocos2d::CCNode* node,
                    void* sender,
                    void (*callback)(void*,std::string)) {
        
        CTut();
        id = CT_ID_FOCUS_BUILDING;
        
        this->scrollView = scrollView;
        this->isoMapBuilding = isoMapBuilding;
        this->node = node;
        
        this->sender = sender;
        this->callback = callback;
        
    }
    
    virtual void perform(void* ptr_tutUnit);
    
    //delegate funcitons
    virtual void scrollViewDidEndZoomingAnimation(CCNikScrollView* scrollView);
    
};

#endif /* defined(__Cheetah__CTutFocuzOnNode__) */
