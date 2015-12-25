//
//  ItemTrack.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/14/2557 BE.
//
//

#ifndef __Cheetah__ItemTrack__
#define __Cheetah__ItemTrack__

#include "../../Settings.h"
#include "../../VOs/GDTrackVO.h"
#include "../../Extensions/CCNikScrollViewButton.h"
#include "../../Extensions/CCLabelHelper.h"

//#define TRACK_POINT_AT_THE_BUTTOM

class ItemTrack: public CCNikScrollViewButton {
    
    cocos2d::CCSprite*      m_trackSprite;
    cocos2d::CCSprite*      m_labelBGSprite;
    cocos2d::CCLabelBMFont* m_label;
    cocos2d::CCSprite*      m_selectSprite;
    bool                    m_lock;
    std::string             m_lock_message;
    
public:
    
    GDTrackVO vo;
    
    static ItemTrack *create(GDTrackVO vo_);
    bool init(GDTrackVO vo_);
    
    void select(bool select_);
    void lock(bool lock_, std::string message_);
    
    bool isLock();
    std::string getLockMessage();
    
};

#endif /* defined(__Cheetah__ItemTrack__) */
