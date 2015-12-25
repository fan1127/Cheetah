//
//  LeaderboardDialog.h
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#ifndef __Cheetah__LeaderboardDialog__
#define __Cheetah__LeaderboardDialog__


#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCClippingLayer.h"
#include "../../Settings.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCPopOverDialog.h"
#include "../../Extensions/CCLoadSpriteURL.h"
#include "../../VOs/GDAchievementsVO.h"

//#define kFirstSpaceToTheLeft 0.0f
//#define kSpacesBetweenItems 1.125f //1.0f = no spaces
#define kLeftOffset 4.0f
#define kUpperOffsetLeaderboard 25.0f
#define kLeaderboardDialogTopOffset 4.0f

#define kLeaderboardDialogBGZOrder  3
#define kLeaderboardDialogUnderBGZOrder 2
#define kLeaderboardDialogAboveBGZORDER 4
#define kLeaderboardDialogNameMaxLength 10

#define kTabFriend 0
#define kTabLeaderboard 1

#include "../../VOs/LeaderboardItemVO.h"
#include "ScrollDialog.h"

class CCNikButton;
class LeaderboardItem;

class LeaderboardDialog : public ScrollDialog
{
    CCModalController           *_modalController;
    
//    CCScale9Sprite              *_brownBackground;
    
    CCNikButton                 *m_tab_1;
    CCSprite                    *m_tab_1_icon;
    CCLabelBMFont               *m_tab_1_label;
    CCNikButton                 *m_tab_2;
    CCSprite                    *m_tab_2_icon;
    CCLabelBMFont               *m_tab_2_label;
    
    CCNikButton                 *m_facebookButton;
    
    CCNode                      *m_fbLoginGroup;
    CCNode                      *m_fbLoginSubGroup;
    
//    CCLabelBMFont               *_facebookLabel;
//    CCPopOverDialog             *dialog;
    
    std::vector<LeaderboardItemVO>  m_leaderboard;
    std::vector<LeaderboardItemVO>  m_friendboard;
    
    int currentTab;
    bool facebookState;
    
    void setDisplay();
    
    void onFacebook(cocos2d::CCObject *sender_);
    static void onFacebookLoggedIn(void* sender_, bool success_);
    static void onFBInstalledFriendRetrieved(void* sender_, bool success_);
    void onClose(cocos2d::CCObject *sender_);
    void onTab(cocos2d::CCObject *sender_);
    void onVisit(cocos2d::CCObject *sender_);

    void startLoading();
    void finishLoading();
    
public:
    
    void setBottomSpaceOnFrontBackground(ScrollDialog::BounceOptions bounce, float leftMargin, float rightMargin, float topMargin, float bottomMargin, float contentXShift, float contentYShift, float bottomSpaceHeight);
    static LeaderboardDialog *create(CCModalController *parentModalController_);
    bool init(CCModalController* parentModalController_, CCSize size);

    static void onHitShadow(cocos2d::CCPoint&, void *sender_);

    virtual void updateContent(int currentTab_);

    virtual void moveBy(float x, float y);

    virtual void onExit();

    /** get leaderboard callback */
    static void onLeaderboardRetrieved(void* sender_, std::string resp_json_str_);
    static void onInvitationSent(void* sender_, int numberOfInvites, std::string fbids_);
    static void onApprequestAdd(void* sender_, std::string resp_json_str_);
    
};

#pragma mark - LeaderboardItem

class LeaderboardItem : public cocos2d::extension::CCScale9Sprite {
    
    cocos2d::CCSprite       *m_rank_frame;
    cocos2d::CCLabelBMFont  *m_rank_label;
    std::string             m_pet_filename;
    cocos2d::CCSprite       *m_pet_sprite;
    std::string             m_picture_filename;
    CCLoadSpriteURL         *m_picture_sprite;
    cocos2d::CCSprite       *m_picture_frame;
    CCLabelBMFont           *m_name_label;
    cocos2d::CCSprite       *m_level_frame;
    cocos2d::CCSprite       *m_level_icon;
    CCLabelBMFont           *m_level_label;
    CCLabelBMFont           *m_won_label;
    CCLabelBMFont           *m_fan_label;
    cocos2d::CCSprite       *m_fan_sprite;
    
public:
    
    CCNikButton             *visitButton;
    
    static LeaderboardItem *create(LeaderboardItemVO vo_, int index_, int currentTab_);
    
    bool initWithVO(LeaderboardItemVO vo_, int index_, int currentTab_);
    void updateContent(LeaderboardItemVO vo_, int index_, int currentTab_);
    void setTarget(cocos2d::CCObject *rec, SEL_MenuHandler selector, CCNikButtonEvent state);
    
};

#pragma mark - InviteItem

class InviteItem : public cocos2d::CCNode {
    
    bool                     returnNumInviteReady;
    
public:
    
    CCLabelBMFont           *titleLabel;
    CCLabelBMFont           *gemNum;
    CCSprite                *gemPic;
    CCNikButton             *facebookButton;
    
    static InviteItem *create();
    
    bool initWithInfo();
    unsigned int checkCurrentInviteProgress();
    static void onAppRequestCountReceived(void* sender_, std::string resp_json_str_);
    void updateInviteAchievement(void *sender_);
    void updateInviteInfo();
    GDAchievementsVO getInviteAchievementVO(int num);
    void updateContent();
    void setTarget(cocos2d::CCObject *rec, SEL_MenuHandler selector, CCNikButtonEvent state);
    
};

#endif /* defined(__Cheetah__LeaderboardDialog__) */
