//
//  TrackGlue.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/16/56 BE.
//
//

#include "TrackGlue.h"
USING_NS_CC;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

TrackGlue* TrackGlue::createWithTracks(Track* track1, Track* track2) {
    
    TrackGlue* pRet = new TrackGlue();
    pRet->initWithTracks(track1, track2);
    pRet->autorelease();
    return pRet;
    
}

bool TrackGlue::initWithTracks(Track* track1, Track* track2) {
    
    if (!CCNode::create()) {
        return false;
    }
    
    track[0] = track1;
    track[1] = track2;
    
    index[0] = index[1] = -1;
    
    return true;
    
}

void TrackGlue::updateTrackGlueSprite() {
    
    if (!g_gameContent) {
        CCLog_("(TrackGlue::updateTrackGlueSprite) Error: Given gameContent is uninitialized!\n");
        return;
    }
    
    if (!track[0] || !track[1]) {
        CCLog_("(TrackGlue::updateTrackGlueSprite) Error: Given track[0] or track[1] is/are uninitialized!\n");
        return;
    }
    
    GDTrackVO vo = g_gameContent->getTrackVO(track[0]->ptVo.type);
    if (vo.type == "") {
        CCLog_("(TrackGlue::updateTrackGlueSprite) Error: Given track=%s doesn't exist!\n", track[0]->ptVo.type.c_str());
        return;
    }
    
    if (m_trackGlueSprite) {
        m_trackGlueSprite->removeFromParentAndCleanup(true);
        m_trackGlueSprite = NULL;
    }
    
    
    CREATE_CHAR_BUFFER(buffer_, 128);
    snprintf(buffer_, buffer__size, "%s_glue.png", vo.src.c_str());
   
    m_trackGlueSprite = CCSprite::createWithSpriteFrameName(buffer_);
    //m_trackGlueSprite->preferenceRootParent = preferenceRootParent;
    this->addChild(m_trackGlueSprite);
   
    if (track[0]->getPosition().x < track[1]->getPosition().x) {
        if (track[0]->getPosition().y < track[1]->getPosition().y) {
            flipX(false);
        }else if (track[0]->getPosition().y > track[1]->getPosition().y) {
            flipX(true);
        }
    }else if (track[0]->getPosition().x > track[1]->getPosition().x) {
        if (track[0]->getPosition().y < track[1]->getPosition().y) {
            flipX(true);
        }else if (track[0]->getPosition().y > track[1]->getPosition().y) {
            flipX(false);
        }
    }
    
    this->setPosition((track[0]->getPosition()+track[1]->getPosition())/2);
    
    for (int i=0; i<2; i++) {
        index[i] = track[i]->ptVo.index;
    }
    
}

void TrackGlue::flipX(bool flip) {
    
    if (m_trackGlueSprite) {
        float scaleX = fabsf(m_trackGlueSprite->getScaleX());
        if (flip) {
            m_trackGlueSprite->setScaleX(-scaleX);
        }else {
            m_trackGlueSprite->setScaleX( scaleX);
        }
    }
    
}
