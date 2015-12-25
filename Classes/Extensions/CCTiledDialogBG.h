//
//  CCTiledDialogBG.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/7/2557 BE.
//
//

#ifndef __Cheetah__CCTiledDialogBG__
#define __Cheetah__CCTiledDialogBG__

#include "cocos2d.h"

class CCTiledDialogBG : public cocos2d::CCNode {
    
    cocos2d::CCSpriteBatchNode  *m_batchNode    = NULL;
    cocos2d::CCSprite           *m_head         = NULL;
    cocos2d::CCSprite           *m_body         = NULL;
    cocos2d::CCSprite           *m_body_glue[2] = {NULL, NULL};
    cocos2d::CCSprite           *m_foot         = NULL;
    
public:
    
//    static CCTiledDialogBG *createWithSpriteFileNames(const char *sprite_head_filename_, const char *sprite_body_filename_, const char *sprite_foot_filename_);
//    static CCTiledDialogBG *createWithSpriteFrameNames(const char *sprite_head_framename_, const char *sprite_body_framename_, const char *sprite_foot_framename_);
    static CCTiledDialogBG *createWithSpriteFrameNamesInBatch(const char *texture_filename_, const char *sprite_head_framename_, const char *sprite_body_framename_, const char *sprite_foot_framename_);
    
//    bool initWithSpriteFileNames(const char *sprite_head_filename_, const char *sprite_body_filename_, const char *sprite_foot_filename_);
//    bool initWithSpriteFrameNames(const char *sprite_head_framename_, const char *sprite_body_framename_, const char *sprite_foot_framename_);
    bool initWithSpriteFrameNamesInBatch(const char *texture_filename_, const char *sprite_head_framename_, const char *sprite_body_framename_, const char *sprite_foot_framename_);
    
    void setSize(cocos2d::CCSize size_);
    void setHeight(float height_);
    void setWidth(float height_);
    
    
};

#endif /* defined(__Cheetah__CCTiledDialogBG__) */
