//
//  CTutFocusOnBuilding.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutFocusOnBuilding__
#define __Cheetah__CTutFocusOnBuilding__

#include "CTut.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../IsoMapBuilding.h"
#include "../Building.h"

class CTutFocusOnBuilding: public CTut, CCNikScrollViewDelegate {
    
public:
    
    /** Required instances, in order to perform the tutorial, these instances are required */
    CCNikScrollView*    scrollView;
    IsoMapBuilding*     isoMapBuilding;
    Building*           building;
    
    CCNikScrollViewDelegate* restore_delegate;
    
    CTutFocusOnBuilding() {
        
        CTut();
        id = CT_ID_FOCUS_BUILDING;
        
        scrollView = NULL;
        isoMapBuilding = NULL;
        building = NULL;
        
        restore_delegate = NULL;
    }
    
    CTutFocusOnBuilding(CCNikScrollView* scrollView_, IsoMapBuilding* isoMapBuilding_, Building* building_, void* sender_, void (*callback_)(void*,std::string)) {
        
        CTut();
        id = CT_ID_FOCUS_BUILDING;
        
        scrollView = scrollView_;
        isoMapBuilding = isoMapBuilding_;
        building = building_;
        
        sender = sender_;
        callback = callback_;
        
        restore_delegate = NULL;
    }
    
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
    //delegate funcitons
    virtual void scrollViewDidEndZoomingAnimation(CCNikScrollView* scrollView_);
    
};

#endif /* defined(__Cheetah__CTutFocusOnBuilding__) */
