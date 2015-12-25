//
// Created by tosssapong on 10/6/14 AD.
//

#include "ScrollDialog.h"
#include "../../Extensions/CCLoadSpriteURL.h"


class InboxDialog : public ScrollDialog {
    
    int m_tag_replay;
    int m_tag_claim;
    int m_tag_index;
    
    CCLoadSpriteURL*        m_picture_sprite;
    CCNikScrollViewButton*  m_pressedButton;
    
public:
    
    static InboxDialog *create(CCModalController *parentModalController,
                               CCSize size);
    
    bool init(CCModalController *parentModalController,
              CCSize size);
    
    virtual void updateContent(int count);
    virtual bool compose(cocos2d::extension::CCScale9Sprite *frame);
    virtual void cleanup();
    
    void onClaimButton(cocos2d::CCObject *sender);
    void onReplayButton(cocos2d::CCObject *sender);

    static void onHistoryListRetrieved(void* sender, std::string resp_str);
    static void onRewardClaimed(void* sender, std::string resp_str);
    static void onCollectGemsEffectFinished(void* sender);
    
};
