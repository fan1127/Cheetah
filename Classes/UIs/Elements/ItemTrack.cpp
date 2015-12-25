//
//  ItemTrack.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/14/2557 BE.
//
//

#include "ItemTrack.h"
USING_NS_CC;
using namespace std;

#include "../../Settings.h"
#include "../../Helpers/StringHelper.h"
#include "../../Helpers/UIHelper.h"

ItemTrack* ItemTrack::create(GDTrackVO vo_)
{
    ItemTrack* pRet = new ItemTrack();
    pRet->init(vo_);
    pRet->autorelease();
    return pRet;
}

bool ItemTrack::init(GDTrackVO vo_)
{
    if (!CCNikScrollViewButton::init(NULL, NULL))
    {
        return false;
    }
    
    vo = vo_;

    m_selectSprite = NULL;
    m_lock = false;

    CCLog_("ItemTrack: %s", vo_.src.c_str());

    UIHelper::UISprites trackSprite = UIHelper::AnimalStats_iconTrackAsphalt;
    const char* tracks[] = {"asphalt", "dirt", "grass", "sand", "snow"};

    for (int i=0; i<5; i++)
    {
        if (std::strstr(vo_.src.c_str(), tracks[i]) != NULL)
        {
            trackSprite = UIHelper::UISprites(UIHelper::AnimalStats_iconTrackAsphalt + i);
            break;
        }
    }

    m_trackSprite = UIHelper::getSprite(trackSprite);
    m_trackSprite->setAnchorPoint(CCPointZero);
    addChild(m_trackSprite);
    
    setContentSize(m_trackSprite->getContentSize());
    setAnchorPoint(ccp(0.5f, 0.5f));
    
    return true;
    
}

void ItemTrack::select(bool select_) {
    
    if (select_) {
        if (!m_selectSprite) {
            m_selectSprite = UIHelper::getSprite(UIHelper::HUDCheckmarkBig);
#ifdef TRACK_POINT_AT_THE_BUTTOM
            m_selectSprite->setPosition(getContentSize()*0.5f);
#else
            m_selectSprite->setPosition(ccp(getContentSize().width*0.1f, 0.0f));
#endif
            m_selectSprite->setAnchorPoint(CCPointZero);
            addChild(m_selectSprite);
        }
    }else {
        if (m_selectSprite) {
            m_selectSprite->removeFromParentAndCleanup(true);
            m_selectSprite = NULL;
        }
    }
}

void ItemTrack::lock(bool lock_, string message_) {
    
    m_lock = lock_;
    if (m_lock) {
        m_lock_message = message_;
        if (m_trackSprite) {
            m_trackSprite->setColor(kTintLock);
        }
        if (m_labelBGSprite) {
            m_labelBGSprite->setColor(kTintLock);
        }
        if (m_label) {
            m_label->setColor(kTintLock);
        }
        
    }else {
        m_lock_message = "";
        if (m_trackSprite) {
            m_trackSprite->setColor(kTintUnlock);
        }
        if (m_labelBGSprite) {
            m_labelBGSprite->setColor(kTintUnlock);
        }
        if (m_label) {
            m_label->setColor(kTintUnlock);
        }
    }
    
}

bool ItemTrack::isLock() {
    
    return m_lock;
    
}

string ItemTrack::getLockMessage() {
    
    return m_lock_message;
    
}