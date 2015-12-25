//
//  EXPProgress.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/18/2557 BE.
//
//

#ifndef __Cheetah__EXPProgress__
#define __Cheetah__EXPProgress__

#include "cocos2d.h"
#include "../Extensions/CCLabelHelper.h"
#include "../Extensions/CCNikButton.h"

class  EXPProgress: public CCNikButton {
    
    void*                       m_inst;
    void*                       m_sender;
    
    cocos2d::CCLabelBMFont*     m_levelLabel;
    cocos2d::CCProgressTimer*   m_timer;
    bool                       m_clean_status=false;
    
public:
    
    static EXPProgress* create();
    virtual bool init();
    
    void setLevel(int level_);
    int  getLevel() const;
    const char* getLevelAsText() const;
    void setPercentage(float percentage_);
    void setPercentage(float fPercentage_, float duration_);
    
    void _shouldReset(cocos2d::CCObject* sender_);
    void _onFinished(cocos2d::CCObject* sender_);
    void  onClickLevel(cocos2d::CCNode *sender);
    void clearUIDialog();
    void setInst(void* inst_);
    void setSender(void* inst_);
    void setClearUI(bool status);
    void (*onFinished)(void *inst_, void* sender_);
};

#endif /* defined(__Cheetah__EXPProgress__) */
