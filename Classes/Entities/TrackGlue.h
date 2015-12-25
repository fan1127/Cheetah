//
//  TrackGlue.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/16/56 BE.
//
//

#ifndef __Cheetah__TrackGlue__
#define __Cheetah__TrackGlue__

#include "cocos2d.h"

#include "Track.h"

class TrackGlue: public cocos2d::CCNode {
  
    cocos2d::CCSprite*  m_trackGlueSprite;
    
public:
    
    void                *preferenceRootParent = NULL;
    
    Track*              track[2];
    int                 index[2];
    
    static TrackGlue* createWithTracks(Track* track1, Track* track2);
    
    bool initWithTracks(Track* track1, Track* track2);
    void updateTrackGlueSprite();
    void flipX(bool flip);
    
};

#endif /* defined(__Cheetah__TrackGlue__) */
