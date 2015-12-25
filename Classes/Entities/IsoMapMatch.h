//
//  IsoMapMatch.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/9/56 BE.
//
//

#ifndef __Cheetah__IsoMapMatch__
#define __Cheetah__IsoMapMatch__

#include "IsoMap.h"
#include "EntityAction.h"
#include "MapToScreenUI.h"

class IsoMapMatch: public IsoMap, public CCNikScrollViewDelegate {
    
    bool                m_hide_entity_action;
    bool                m_hide_entity_dialog;
    
    cocos2d::CCPoint    m_drag_begin;
    bool                m_ui_is_closing;
    
    void _touchEneded(CCSet *pTouches, CCEvent *pEvent);
    
public:
    
    EntityAction*       entityAction;
    MapToScreenUI*      mapToScreenUI;
    
    virtual bool init();
    CREATE_FUNC(IsoMapMatch);
    
    void closeAllUIs();
    
    virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);
    
    virtual void scrollViewDidScroll(CCNikScrollView* view);
    virtual void scrollViewWillBeginDragging(CCNikScrollView* view);
    
};


#endif /* defined(__Cheetah__IsoMapMatch__) */
