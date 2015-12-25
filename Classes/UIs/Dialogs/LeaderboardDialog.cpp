//
//  LeaderboardDialog.cpp
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#include "LeaderboardDialog.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CreateSpriteHelper.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../UIs/NumericProgressBar.h"
USING_NS_CC;

using namespace std;

#include <spine/Json.h>
using namespace cocos2d::extension;

#include "../../Scenes/SceneVille.h"
#include "../../Scenes/SceneVisit.h"
extern CCScene *g_currentScene;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIAppRequestAdd.h"
#include "../../Services/APIs/APIAppRequestCount.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../AppMacros.h"
#include "../../Settings.h"
#include "../../Extensions/CCOpenURL/CCOpenURL.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"
#include "../../Helpers/CustomEventsHelper.h"

#include "../../Helpers/UIHelper.h"
#include "../../Helpers/DataCache.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;

#define kMarginFromFrontBackgroundToBackground (50.0f*sizeMultiplier)
#define kMarginFromBackgroundToClippingLayer (30.0f*sizeMultiplier)
#define FOOTER_Y_POSITION (180.0f*sizeMultiplier)
#define SHIFT_TOP_RATIO (0.05f)
#define SHIFT_FRIENDS_SCROLL (0.15f)

LeaderboardDialog* LeaderboardDialog::create(CCModalController* parentModalController_)
{
    LeaderboardDialog *pRet = new LeaderboardDialog();
    pRet->init(parentModalController_, kDialogLeaderboardSize);
    pRet->autorelease();
    return pRet;
}

bool LeaderboardDialog::init(CCModalController* parentModalController_, CCSize size)
{
    if (!ScrollDialog::init(parentModalController_, size))
    {
        return false;
    }

    m_leaderboard.clear();
    m_friendboard.clear();

    m_modalController->userObject = this;
    _modalController = m_modalController;
    m_modalController->onHitShadow = onHitShadow;
    m_modalController->setHitBGToCloseAllEnabled(false);

    m_background->setZOrder(kLeaderboardDialogBGZOrder);
    m_closeButton->setZOrder(kLeaderboardDialogBGZOrder);
    m_frontBackground->setZOrder(kLeaderboardDialogBGZOrder);
    m_clippingLayer->setZOrder(kLeaderboardDialogBGZOrder);

    createScrollContent(LEADERBOARD_DIALOG_SETTINGS);

    //---------- tab friend -----------
    m_tab_1 = CCNikButton::create(UIHelper::getScalableSpriteAsNormal(UIHelper::TabActive), NULL);
    m_tab_1->setTarget(this, menu_selector(LeaderboardDialog::onTab), CCNikButtonEventTouchUpInside);
    m_tab_1->setPosition(m_background->getPosition() + ccp(-m_tab_1->getContentSize().width*0.5f,
                                                            m_background->getContentSize().height*0.4f + m_tab_1->getContentSize().height*0.45f));
    m_tab_1->setTouchEnabled(false);
    addChild(m_tab_1, kLeaderboardDialogAboveBGZORDER, 100);

    m_tab_1_icon = UIHelper::getSprite(UIHelper::FriendsIcon);
    m_tab_1->addChild(m_tab_1_icon);
    m_tab_1_label = CCLabelHelper::createBMFont(g_gameContent->Word["friend_tab_title"].c_str(), kFontSizeMedium, true);
    m_tab_1->addChild(m_tab_1_label);
    
#define ICON_X_POSITION (70.0f*sizeMultiplier)
    
    m_tab_1_icon->setPosition(ccp(ICON_X_POSITION,
                                  m_tab_1->getContentSize().height*0.5f)
                              );
    m_tab_1_label->setPosition(m_tab_1->getContentSize()*0.5f
                               + ccp(ICON_X_POSITION*0.8f,
                                     0.0f));
    
    //--------- tab leaderboard-----------
    m_tab_2 = CCNikButton::create(UIHelper::getScalableSpriteAsNormal(UIHelper::TabInactive), NULL);
    m_tab_2->setTarget(this, menu_selector(LeaderboardDialog::onTab), CCNikButtonEventTouchUpInside);
    m_tab_2->setPosition(m_background->getPosition() + ccp(m_tab_2->getContentSize().width*0.55f, m_background->getContentSize().height*0.5f + m_tab_2->getContentSize().height*0.35f));
    m_tab_2->setTouchEnabled(true);
    addChild(m_tab_2, kLeaderboardDialogUnderBGZOrder, 101);
    
    m_tab_2_icon = UIHelper::getSprite(UIHelper::WorldIcon);
    m_tab_2->addChild(m_tab_2_icon);
    
    m_tab_2_label = CCLabelHelper::createBMFont(g_gameContent->Word["leaderboard_tab_title"].c_str(), kFontSizeMedium, true);
    m_tab_2->addChild(m_tab_2_label);
    
    m_tab_2_icon->setPosition(ccp(ICON_X_POSITION,
                                  m_tab_2->getContentSize().height*0.5f)
                              );
    m_tab_2_label->setPosition(m_tab_2->getContentSize()*0.5f
                               + ccp(ICON_X_POSITION*0.8f,
                                     0.0f));

    moveBy(0.0f, SHIFT_TOP_RATIO * m_background->getContentSize().height);

    this->setBottomSpaceOnFrontBackground(LEADERBOARD_DIALOG_SETTINGS, 0.0f); //default

    onTab(NULL); //set to leaderboard tab as default

    return true;
}

void LeaderboardDialog::moveBy(float x, float y)
{
    ScrollDialog::moveBy(x, y);
//    _brownBackground->setPosition(CCPointMake(_brownBackground->getPositionX() + x, _brownBackground->getPositionY() - y));
}

void LeaderboardDialog::onExit()
{
    DataCache::Instance().resetCallback();
    CCModalView::onExit();
    BackendService::getInstance()->clearAllRequests();
}

void LeaderboardDialog::startLoading()
{
    //g_sceneManager->showLoading();
    showLoading();
}

void LeaderboardDialog::finishLoading()
{
    //g_sceneManager->hideLoading();
    hideLoading();
}

void LeaderboardDialog::updateContent(int currentTab_)
{
    while(m_scrollView->contentNode->getChildrenCount()>0)
    {
        CCObject* child = m_scrollView->contentNode->getChildren()->objectAtIndex(0);
        CCNode *target = (CCNode*)child;
        
        if(target) {
            target->stopAllActions();
            target->removeFromParent();
        }
    }
    m_scrollView->removeAllChildren();

    CCNode *_last_item = NULL;

    vector<LeaderboardItemVO> _list;
    if (currentTab == kTabFriend) { //friendboar
        _list = m_friendboard;

    }else { // leaderboard
        _list = m_leaderboard;
    }
    
#define kYSpaceBetweenItems (10.0f*sizeMultiplier)

    //change for loop to while loop vector here!
    int frameCount = _list.size(); // <=== *must declare total vector count to this var
    
    int countIncludingInviteItem = 0;
    
    float firstItemHeight = 0.0f;
    InviteItem *inviteItem = NULL;
    if (currentTab_ == kTabFriend && frameCount > 0) {
        inviteItem = InviteItem::create();
        firstItemHeight = inviteItem->getContentSize().height;
        
        countIncludingInviteItem++;
    }
    
    int count = 0;
    
    for (int i = 0; i<frameCount; i++)
    {
        count++;
        countIncludingInviteItem++;
        
        LeaderboardItem *_item = LeaderboardItem::create(_list[i], i, currentTab_);
        _item->setPosition(ccp(m_clippingLayer->getContentSize().width*0.5f,
                               i*(_item->getContentSize().height+kYSpaceBetweenItems)
                               ));
        
        _item->setTarget(this, menu_selector(LeaderboardDialog::onVisit), CCNikButtonEventTouchUpInside);
        _item->setTag(i);
        m_scrollView->addChild(_item);
        _last_item = _item;
    }
    
    if (currentTab_ == kTabFriend && inviteItem != NULL) {
        inviteItem->setTarget(this, menu_selector(LeaderboardDialog::onFacebook), CCNikButtonEventTouchUpInside);
        inviteItem->setPosition(ccp(m_clippingLayer->getContentSize().width*0.5f
                                    -5.0f*sizeMultiplier,
                                    count*(inviteItem->getContentSize().height+kYSpaceBetweenItems)));
        m_scrollView->addChild(inviteItem);
        _last_item = inviteItem;
    }

    if(frameCount > 0 && _last_item != NULL) {
        float lastHeight = _last_item->getPositionY();

        float sizeHeight = MAX(_last_item->getPositionY()
                               + _last_item->getContentSize().height
                               + kUpperOffsetLeaderboard*sizeMultiplier,
                               m_scrollView->getContentSize().height);

        //swap frame from bottom to top
        if(countIncludingInviteItem*(_last_item->getContentSize().height+kYSpaceBetweenItems)
           <
           m_frontBackground->getContentSize().height)
        {
            for (int i = 0; i < frameCount; i++) {
                if (LeaderboardItem *_item = (LeaderboardItem*)m_scrollView->contentNode->getChildByTag(i)) {
                    _item->setPosition(ccp(_item->getPositionX(),
                                           lastHeight
                                           - firstItemHeight
                                           -i*(_item->getContentSize().height+kYSpaceBetweenItems)
                                           
                                           + m_frontBackground->getContentSize().height
                                           - countIncludingInviteItem*(_item->getContentSize().height+kYSpaceBetweenItems)
                                           -kYSpaceBetweenItems
                                           ));
                }
            }
            if (inviteItem != NULL) {
                inviteItem->setPosition(inviteItem->getPosition()
                                        + ccp(0.0f,
                                              m_frontBackground->getContentSize().height
                                              - countIncludingInviteItem*(inviteItem->getContentSize().height+kYSpaceBetweenItems)
                                              -kYSpaceBetweenItems));
            }
        }
        else {
            for (int i = 0; i < frameCount; i++) {
                if (LeaderboardItem *_item = (LeaderboardItem*)m_scrollView->contentNode->getChildByTag(i)) {
                    _item->setPosition(ccp(_item->getPositionX(),
                                           lastHeight
                                           - firstItemHeight
                                           -i*(_item->getContentSize().height+kYSpaceBetweenItems)));
                }
            }
        }
        
        


        m_scrollView->setContentView(CCSizeMake(m_scrollView->getContentSize().width, sizeHeight));
        m_scrollView->setContentOffset(ccp(0, m_scrollView->getContentView().height - m_scrollView->getContentSize().height));
    }
}


void LeaderboardDialog::setDisplay() {
    
    if (m_facebookButton) {
        m_facebookButton->removeFromParent();
        m_facebookButton = NULL;
    }
    
    if (m_fbLoginSubGroup) {
        m_fbLoginSubGroup->removeAllChildrenWithCleanup(true);
        m_fbLoginSubGroup->removeFromParent();
        m_fbLoginSubGroup = NULL;
    }
    if (m_fbLoginGroup) {
        m_fbLoginGroup->removeAllChildrenWithCleanup(true);
        m_fbLoginGroup->removeFromParent();
        m_fbLoginGroup = NULL;
    }
    
    if(currentTab == kTabFriend) { // Tab Friend
        
        m_tab_1->setPositionY(m_background->getPositionY() + m_background->getContentSize().height*0.45f + m_tab_1->getContentSize().height*0.5f);
        CCSpriteFrame *_sprite_frame_1 = UIHelper::getSpriteFrame(UIHelper::TabActive);
        m_tab_1->m_normalSprite->setDisplayFrame(_sprite_frame_1);
        m_tab_1->setZOrder(kLeaderboardDialogAboveBGZORDER);
        m_tab_1->setTouchEnabled(false);
        
        m_tab_2->setPositionY(m_background->getPositionY() + m_background->getContentSize().height*0.5f + m_tab_1->getContentSize().height*0.25f);
        CCSpriteFrame *_sprite_frame_2 = UIHelper::getSpriteFrame(UIHelper::TabInactive);
        m_tab_2->m_normalSprite->setDisplayFrame(_sprite_frame_2);
        m_tab_2->setZOrder(kLeaderboardDialogUnderBGZOrder);
        m_tab_2->setTouchEnabled(true);
        
        //-----------facebook---------
        m_facebookButton = CCNikButton::create(UIHelper::getSprite(UIHelper::FacebookConnectionFacebookButton), NULL);
        m_facebookButton->setAnchorPoint(ccp(0.5f, 0.5f));
        m_facebookButton->setTarget(this, menu_selector(LeaderboardDialog::onFacebook), CCNikButtonEventTouchUpInside);
        m_facebookButton->setPosition(ccp(m_background->getContentSize().width * 0.80f, FOOTER_Y_POSITION));
        m_background->addChild(m_facebookButton, kLeaderboardDialogNameMaxLength);
        
        m_facebookButton->setVisible(false); //TODO: remove facebook button properly
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        strncpy(_buffer, g_gameContent->Word["invite_friends"].c_str(), _buffer_size);
        
        CCLabelBMFont* _facebookLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeVerySmall, true);
        _facebookLabel->setAnchorPoint(CCPointMake(0.0f, 0.5f));
        _facebookLabel->setPosition(m_facebookButton->getContentSize().width*0.35f, m_facebookButton->getContentSize().height*0.5f);
        m_facebookButton->addChild(_facebookLabel);
        
        //-------------------------------------
        m_facebookButton->setScale(1.25);
        
        
        if (!m_fbLoginGroup) {
            m_fbLoginGroup = CCNode::create();
            m_frontBackground->addChild(m_fbLoginGroup);
        }

        m_fbLoginGroup->setPositionX(m_fbLoginGroup->getPositionX());

        if (g_sceneManager->facebook->isLoggedIn()) {
            m_facebookButton->setVisible(true);
            m_facebookButton->setVisible(false); //TODO: remove facebook button properly
            this->setBottomSpaceOnFrontBackground(LEADERBOARD_DIALOG_SETTINGS, 0.0f);

        } else {
            m_facebookButton->setVisible(false);
            
            CCSprite *m_merchantSay = CCSprite::create("common/character/merchant_say.png");
            
            this->setBottomSpaceOnFrontBackground(LEADERBOARD_DIALOG_SETTINGS, 0.0f);
            
            m_merchantSay->setScale(sizeMultiplier);
            
            m_merchantSay->setAnchorPoint(ccp(0.5f, 0.0f));
            m_merchantSay->setPosition(ccp(-m_frontBackground->getContentSize().height*0.5f
                                           -13.0f*sizeMultiplier,
                                           -m_frontBackground->getContentSize().height*0.5f
                                           -20.0f*sizeMultiplier
                                           ));

            m_fbLoginGroup->addChild(m_merchantSay);

            if (!m_fbLoginSubGroup) {
                m_fbLoginSubGroup = CCNode::create();
                m_fbLoginGroup->addChild(m_fbLoginSubGroup);
            }

//#define GEM_REWARD
#ifdef GEM_REWARD
#define DESC_BUTTON_VERTICAL_SPREAD (150.0f*sizeMultiplier)
#define GEM_LABEL_GEM_HORIZONTAL_SPREAD (65.0f*sizeMultiplier)
#else
#define DESC_BUTTON_VERTICAL_SPREAD (110.0f*sizeMultiplier)
#endif
            
#ifdef GEM_REWARD
#else
#endif
            auto arrowBackground = UIHelper::getSprite(UIHelper::FacebookConnectionDialogueBox);
            arrowBackground->setScale(1.3f);
            arrowBackground->setPositionY(arrowBackground->getPositionY() - 20.0f * sizeMultiplier);
            arrowBackground->setPositionX(arrowBackground->getPositionX() + 220.0f * sizeMultiplier);
            m_fbLoginSubGroup->addChild(arrowBackground);
            m_fbLoginSubGroup->setPosition(ccp(m_frontBackground->getContentSize().width*0.0f,
                                               m_frontBackground->getContentSize().height*0.0f));

            CREATE_CHAR_BUFFER(_buffer, 64);
#ifdef GEM_REWARD
            int _gemReward = g_gameContent->GameConfig["FACEBOOK_LOGIN_DIALOG_GEM_REWARD"];
            snprintf(_buffer, _buffer_size, g_gameContent->Word["fblogindialog_desc"].c_str(), _gemReward);
#else
            strncpy(_buffer, g_gameContent->Word["fblogindialog_desc_no_reward"].c_str(), _buffer_size);
#endif
            
            CCLabelBMFont *_desc = CCLabelHelper::createBMFont(_buffer,
                                                               kFontSizeMedium,
                                                               kTextColorDark);
            _desc->setAnchorPoint(ccp(0.5f, 0.5f));
            _desc->setAlignment(kCCTextAlignmentCenter);
            _desc->setWidth(500.0f*sizeMultiplier);
            _desc->setPosition(260.0f*sizeMultiplier,
                               DESC_BUTTON_VERTICAL_SPREAD
                               -10.0f*sizeMultiplier);
            m_fbLoginSubGroup->addChild(_desc);
            
#ifdef GEM_REWARD
            snprintf(_buffer, _buffer_size, "+%d", _gemReward);
            CCLabelBMFont *gemReward_label = CCLabelHelper::createBMFont(_buffer,
                                                                         kFontSizeVeryLarge,
                                                                         true);
            gemReward_label->setPosition(ccp(-GEM_LABEL_GEM_HORIZONTAL_SPREAD,
                                             0.0f));
            m_fbLoginSubGroup->addChild(gemReward_label);
            
            
            CCSprite *gemReward_gem = UIHelper::getSprite(UIHelper::HUDGem);
            gemReward_gem->setPosition(ccp(GEM_LABEL_GEM_HORIZONTAL_SPREAD,
                                           0.0f));
            m_fbLoginSubGroup->addChild(gemReward_gem);
#endif
            
            CCSprite *connectFacebook_sprite = UIHelper::getSprite(UIHelper::FacebookConnectionFacebookButton);
            
            CCMenu *connectFacebook = CCMenuHelper::createMenu(connectFacebook_sprite,
                                                               this,
                                                               menu_selector(LeaderboardDialog::onFacebook));
            
            connectFacebook_sprite->setAnchorPoint(ccp(0.5f, 0.5f));
            connectFacebook_sprite->setPosition(connectFacebook_sprite->getContentSize()*0.5f);
            
            connectFacebook->setAnchorPoint(ccp(0.5f, 0.5f));
            connectFacebook->setPosition(ccp(260.0f*sizeMultiplier,
                                             -DESC_BUTTON_VERTICAL_SPREAD
                                             -10.0f*sizeMultiplier));
            m_fbLoginSubGroup->addChild(connectFacebook);
            
            CCLabelBMFont *connectFacebook_label = CCLabelHelper::createBMFont(g_gameContent->Word["fblogindialog_connect"].c_str(),
                                                                               kFontSizeMedium,
                                                                               true);
            connectFacebook_label->setPosition(ccp(connectFacebook->getContentSize().width*0.62f,
                                                   connectFacebook->getContentSize().height*0.5f
                                                   +3.0f*sizeMultiplier));
            connectFacebook_sprite->addChild(connectFacebook_label);
            
            if (CustomEventsHelper::Instance().shouldAnimateIdle(kIdleAnimationConnectFacebook)) {
                connectFacebook_sprite->runAction(CustomEventsHelper::Instance().getIdleAnimation());
            }
        }
        
    } else { //Tab Leaderboard
        
        this->setBottomSpaceOnFrontBackground(LEADERBOARD_DIALOG_SETTINGS, 0.0f); //default
        m_tab_1->setPositionY(m_background->getPositionY() + m_background->getContentSize().height*0.5f + m_tab_1->getContentSize().height*0.25f);
        CCSpriteFrame *_sprite_frame_1 = UIHelper::getSpriteFrame(UIHelper::TabInactive);
        m_tab_1->m_normalSprite->setDisplayFrame(_sprite_frame_1);
        m_tab_1->setZOrder(kLeaderboardDialogUnderBGZOrder);
        m_tab_1->setTouchEnabled(true);
        
        m_tab_2->setPositionY(m_background->getPositionY() + m_background->getContentSize().height*0.45f + m_tab_1->getContentSize().height*0.5f);
        CCSpriteFrame *_sprite_frame_2 = UIHelper::getSpriteFrame(UIHelper::TabActive);
        m_tab_2->m_normalSprite->setDisplayFrame(_sprite_frame_2);
        m_tab_2->setZOrder(kLeaderboardDialogAboveBGZORDER);
        m_tab_2->setTouchEnabled(false);
    }
    
    updateContent(currentTab);
}

void LeaderboardDialog::onHitShadow(cocos2d::CCPoint& pnt,void *sender_)
{
    // test touch hit for tabs
    LeaderboardDialog* obj = static_cast<LeaderboardDialog*>(static_cast<CCModalController*>(sender_)->userObject);

    auto localPos = obj->convertToNodeSpace(pnt);
    CCRect bb1 = obj->m_tab_1->boundingBox();
    
#define ExtraSpace (60.0*sizeMultiplier)
    bb1 = CCRectMake(bb1.origin.x - ExtraSpace,
                     bb1.origin.y - ExtraSpace,
                     bb1.size.width + ExtraSpace*2,
                     bb1.size.height + ExtraSpace*2);
    CCRect bb2 = obj->m_tab_2->boundingBox();
    bb2 = CCRectMake(bb2.origin.x - ExtraSpace,
                     bb2.origin.y - ExtraSpace,
                     bb2.size.width + ExtraSpace*2,
                     bb2.size.height + ExtraSpace*2);

    if (bb1.containsPoint(localPos) ||
        bb2.containsPoint(localPos)
        )
    {
        CCLog_("Tab was hit!");
    }
    else
    {
        obj->onClose(NULL);
    }
}

void LeaderboardDialog::onTab(cocos2d::CCObject *sender_)
{
    CCLog_("OnTab: %d", (bool)sender_);

    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    
    currentTab = kTabFriend;
    
    if (CCSprite *target = (CCSprite*)sender_) {
        if(target->getTag()==101) {
            currentTab = kTabLeaderboard;
        }
    }

    finishLoading();

    if (currentTab == kTabFriend) {
        if (g_sceneManager->facebook->isLoggedIn()) {
            if (m_friendboard.size() <= 0) {
                startLoading();
                CCLog_("Getting friends with %s",g_playerState->fbFriendsRawData.c_str());
                DataCache::Instance().getDataAsync(DataCache::LeaderboardFriend, this, LeaderboardDialog::onLeaderboardRetrieved);
                //g_gameBackend->leaderboard_get(g_playerState->fbFriendsRawData, this, LeaderboardDialog::onLeaderboardRetrieved);
            }
        }
    }else if (currentTab == kTabLeaderboard) {
        if (m_leaderboard.size() <= 0) {
            startLoading();
            DataCache::Instance().getDataAsync(DataCache::Leaderboard, this, LeaderboardDialog::onLeaderboardRetrieved);
//            g_gameBackend->leaderboard_get("", this, LeaderboardDialog::onLeaderboardRetrieved);
        }
    }
    
    setDisplay();
}

void LeaderboardDialog::onVisit(cocos2d::CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    
    if (CCNikButton *target = (CCNikButton*)sender_)
    {
        m_modalController->closeAll();
        
        if (typeid(*g_currentScene) == typeid(SceneVille)) {
            if (SceneVille* _sceneView = (SceneVille*)g_currentScene) {
                if (currentTab == kTabFriend) {
                    CCLog_("visit at tag:%s", m_friendboard[target->getTag()].uuid.c_str());
                    _sceneView->fadeOutButWaitForOpponent(SCENE_VISIT, m_friendboard[target->getTag()].uuid);
                }else {
                    CCLog_("visit at tag:%s", m_leaderboard[target->getTag()].uuid.c_str());
                    
#if defined(DEBUG_SELECT_UUID_FOR_VISIT) && IS_DEBUG_BUILD
                    _sceneView->fadeOutButWaitForOpponent(SCENE_VISIT, DEBUG_SELECT_UUID_FOR_VISIT);
#else
                    _sceneView->fadeOutButWaitForOpponent(SCENE_VISIT, m_leaderboard[target->getTag()].uuid);
#endif
                }
                return;
            }
        }
        
        if (typeid(*g_currentScene) == typeid(SceneVisit)) {
            if (SceneVisit* _sceneVisit = (SceneVisit*)g_currentScene) {
                SceneList _next_scene = SCENE_VISIT;
                if (g_sceneManager->sceneSelector == SCENE_VISIT) {
                    _next_scene = SCENE_VISIT2;
                }
                if (currentTab == kTabFriend) {
                    CCLog_("visit at tag:%s", m_friendboard[target->getTag()].uuid.c_str());
                    _sceneVisit->fadeOutButWaitForOpponent(_next_scene, m_friendboard[target->getTag()].uuid);
                }else {
                    CCLog_("visit at tag:%s", m_leaderboard[target->getTag()].uuid.c_str());
                    
#if defined(DEBUG_SELECT_UUID_FOR_VISIT) && IS_DEBUG_BUILD
                    _sceneVisit->fadeOutButWaitForOpponent(_next_scene, DEBUG_SELECT_UUID_FOR_VISIT);
#else
                    _sceneVisit->fadeOutButWaitForOpponent(_next_scene, m_leaderboard[target->getTag()].uuid);
#endif
                }
                return;
            }
        }
    }
}

void LeaderboardDialog::setBottomSpaceOnFrontBackground(ScrollDialog::BounceOptions bounce, float leftMargin, float rightMargin, float topMargin, float bottomMargin, float contentXShift, float contentYShift, float bottomSpaceHeight) {

    bottomMargin += bottomSpaceHeight;

    m_frontBackground->setContentSize(CCSizeMake(m_background->getContentSize().width *  (1.0f - (leftMargin+rightMargin)),
                                                 m_background->getContentSize().height * (1.0f - (topMargin + bottomMargin))));

    m_frontBackground->setPosition(CCPointMake(-m_background->getContentSize().width * 0.5f +  m_frontBackground->getContentSize().width * 0.5f + m_background->getContentSize().width * leftMargin,
                                               -m_background->getContentSize().height * 0.5f + m_frontBackground->getContentSize().height * 0.5f + m_background->getContentSize().height * bottomMargin));


    m_clippingLayer->setContentSize(CCSizeMake(m_frontBackground->getContentSize().width - contentXShift * m_background->getContentSize().height,
            m_frontBackground->getContentSize().height));// - contentYShift * m_background->getContentSize().width));

    m_clippingLayer->setPosition(CCPointMake(-m_frontBackground->getContentSize().width * 0.5f + m_frontBackground->getPositionX() + contentXShift * m_background->getContentSize().height,
                                             -m_frontBackground->getContentSize().height * 0.5f + m_frontBackground->getPositionY() - contentYShift * m_background->getContentSize().width));

    m_scrollView->setContentSize(m_clippingLayer->getContentSize());
}

void LeaderboardDialog::onFacebook(cocos2d::CCObject *sender_)
{
    //    g_sceneManager->facebook->logout(NULL);
    
    if(g_sceneManager->facebook->isLoggedIn()) {
        
        CCDictionary *params = CCDictionary::create();
        params->setObject(ccs("LeaderboardDialog"), "Source");
        FlurryCpp::logEvent(EVENT_INVITE_BUTTON_TAPPED, params);
        
        CSJson::Value eventParameters;
        eventParameters["source"] = "LeaderboardDialog";
        AnalyticsClient::getInstance()->logEvent(EVENT_INVITE_BUTTON_TAPPED, eventParameters);
        
        FBAppEventsWrapper::logEvent(EVENT_INVITE_BUTTON_TAPPED);
        
        g_sceneManager->facebook->inviteFriends("Invite Friends", "Play Ranch Run with your friends!", this, LeaderboardDialog::onInvitationSent);
    } else {
        
        CustomEventsHelper::Instance().checkAndIncreaseTimesUsedFor(kIdleAnimationConnectFacebook);
        
        CCDictionary *params = CCDictionary::create();
        params->setObject(ccs("LeaderboardDialog"), "Source");
        FlurryCpp::logEvent(EVENT_FBLOGIN_BUTTON_TAPPED, params);
        
        CSJson::Value eventParameters;
        eventParameters["source"] = "LeaderboardDialog";
        AnalyticsClient::getInstance()->logEvent(EVENT_FBLOGIN_BUTTON_TAPPED, eventParameters);
        
        FBAppEventsWrapper::logEvent(EVENT_FBLOGIN_BUTTON_TAPPED);
        
        g_sceneManager->facebook->login(this, LeaderboardDialog::onFacebookLoggedIn);
        //        g_sceneManager->facebook->login(g_sceneManager, SceneManager::onFBLoggedIn);
        //        this->m_modalController->closeAll();
    }
    
}

void LeaderboardDialog::onFacebookLoggedIn(void* sender_, bool success_)
{
    
    if(success_) {
        
        CCDictionary *params = CCDictionary::create();
        params->setObject(ccs("LeaderboardDialog"), "Source");
        FlurryCpp::logEvent(EVENT_FBLOGIN_SUCCESS, params);
        
        CSJson::Value eventParameters;
        eventParameters["source"] = "LeaderboardDialog";
        AnalyticsClient::getInstance()->logEvent(EVENT_FBLOGIN_SUCCESS, eventParameters);
        
        FBAppEventsWrapper::logEvent(EVENT_FBLOGIN_SUCCESS);
        
        g_sceneManager->onFBLoggedIn(g_sceneManager, sender_, true, LeaderboardDialog::onFBInstalledFriendRetrieved);
        
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, g_gameContent->Word["facebook_login_success_message"]);
        
        if (LeaderboardDialog* _this = (LeaderboardDialog*)sender_) {

            if (_this->m_facebookButton) {
                _this->m_facebookButton->removeFromParent();
                _this->m_facebookButton = NULL;
            }

            if (_this->m_fbLoginSubGroup) {
                _this->m_fbLoginSubGroup->removeAllChildrenWithCleanup(true);
                _this->m_fbLoginSubGroup->removeFromParent();
                _this->m_fbLoginSubGroup = NULL;
            }
            if (_this->m_fbLoginGroup) {
                _this->m_fbLoginGroup->removeAllChildrenWithCleanup(true);
                _this->m_fbLoginGroup->removeFromParent();
                _this->m_fbLoginGroup = NULL;
            }

            _this->startLoading();
        }
    }
}

void LeaderboardDialog::onFBInstalledFriendRetrieved(void* sender_, bool success_) {
    
    g_sceneManager->onFBInstalledFriendRetrieved(sender_, success_);

    DataCache::Instance().getDataAsync(DataCache::LeaderboardFriend, sender_, LeaderboardDialog::onLeaderboardRetrieved);
//    g_gameBackend->leaderboard_get(g_playerState->fbFriendsRawData, sender_, LeaderboardDialog::onLeaderboardRetrieved);
}

void LeaderboardDialog::onClose(CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    if (this->numberOfRunningActions() > 0) return;
    this->m_modalController->popView();
    m_modalController->setHitBGToCloseAllEnabled(true);
    m_modalController->onHitShadow = NULL;
}

#pragma mark - Leaderboard Callback

void LeaderboardDialog::onLeaderboardRetrieved(void *sender_, std::string resp_json_str_) {

    if (typeid(*g_currentScene) != typeid(SceneVille) &&
        typeid(*g_currentScene) != typeid(SceneVisit)) return;

    CCLog("LeaderboardDialog::onLeaderboardRetrieved 0:  %s", resp_json_str_.c_str());

    if (LeaderboardDialog* _this = static_cast<LeaderboardDialog*>(sender_)) {

        //JsonAllocator allocator;
        Json *_root = Json_create(resp_json_str_.c_str());
        if (!_root) {
            return;
        }

        CCLog("LeaderboardDialog::onLeaderboardRetrieved 1");

        if (Json *_items = Json_getItem(_root, "leaderboard")) {
            
            _this->m_leaderboard.clear();
            int _nitem = Json_getSize(_items);
            for (int i=0; i<_nitem; i++) {
                
                if (Json *_item = Json_getItemAt(_items, i)) {
                    
                    LeaderboardItemVO _vo = LeaderboardItemVO();
                    
                    _vo.uuid = Json_getString(_item, "uuid", "");
                    _vo.name = Json_getString(_item, "name", "");
                    _vo.photo = Json_getString(_item, "photo", "");
                    _vo.highest_win_streak = Json_getInt(_item, "highest_win_streak", 0);
                    _vo.total_wins = Json_getInt(_item, "total_wins", 0);
                    _vo.score = Json_getInt(_item, "score", 0);
                    _vo.host_type = Json_getString(_item, "host_type", "");
                    _vo.level = Json_getInt(_item, "level", 0);
                    
                    _this->m_leaderboard.push_back(_vo);
                    
                }
                
            }
            
        }

        CCLog("LeaderboardDialog::onLeaderboardRetrieved 2");

        if (Json *_items = Json_getItem(_root, "friendboard")) {

            CCLog("LeaderboardDialog::onLeaderboardRetrieved 3");

            bool _add_myself = true;
            
            _this->m_friendboard.clear();
            int _nitem = Json_getSize(_items);
            for (int i=0; i<_nitem; i++) {
                
                if (Json *_item = Json_getItemAt(_items, i)) {
                    
                    LeaderboardItemVO _vo = LeaderboardItemVO();
                    
                    _vo.uuid = Json_getString(_item, "uuid", "");
                    _vo.name = Json_getString(_item, "name", "");
                    _vo.photo = Json_getString(_item, "photo", "");
                    _vo.highest_win_streak = Json_getInt(_item, "highest_win_streak", 0);
                    _vo.total_wins = Json_getInt(_item, "total_wins", 0);
                    _vo.score = Json_getInt(_item, "score", 0);
                    _vo.host_type = Json_getString(_item, "host_type", "");
                    _vo.level = Json_getInt(_item, "level", 0);
                    
                    _this->m_friendboard.push_back(_vo);
                    
                    if (_vo.uuid == g_playerState->player.uuid) {
                        _add_myself = false;
                    }
                }
            }
            
            if (_add_myself) {

                CCLog("LeaderboardDialog::onLeaderboardRetrieved 4");

                LeaderboardItemVO _my_vo;
                _my_vo.uuid = g_playerState->player.uuid;
                _my_vo.name = g_playerState->player.info.name;
                _my_vo.photo = g_playerState->player.info.photo;
                _my_vo.highest_win_streak = g_playerState->player.info.highest_win_streak;
                _my_vo.total_wins = g_playerState->player.info.total_wins;
                _my_vo.score = g_playerState->player.balance.fan;
                if (Pet* _host = g_playerState->getPet(g_playerState->player.racing.host)) {
                    _my_vo.host_type = _host->pVo.type;
                }
                _my_vo.level = g_playerState->player.racing.level;
                _this->m_friendboard.push_back(_my_vo);
            }
            
            //sort
            int n = _this->m_friendboard.size();
            if (n > 0) {

                CCLog("LeaderboardDialog::onLeaderboardRetrieved 5");

                map<int, vector<LeaderboardItemVO> > tmp;
                tmp.clear();
                while (_this->m_friendboard.size() > 0) {
                    LeaderboardItemVO _vo = _this->m_friendboard[_this->m_friendboard.size()-1];
                    vector<LeaderboardItemVO> stack = tmp[_vo.score];
                    stack.push_back(_vo);
                    tmp[_vo.score] = stack;
                    _this->m_friendboard.pop_back();
                }
                _this->m_friendboard.clear();
                for (map<int, vector<LeaderboardItemVO> >::reverse_iterator it=tmp.rbegin(); it!=tmp.rend(); ++it) {
                    vector<LeaderboardItemVO> stack = (*it).second;
                    for (vector<LeaderboardItemVO>::iterator it2=stack.begin(); it2!=stack.end(); ++it2) {
                        LeaderboardItemVO _vo = (*it2);
                        _this->m_friendboard.push_back(_vo);
                    }
                }
                tmp.clear();
            }
            
        }
        
        Json_dispose(_root);


        if ((_this->currentTab == kTabFriend && !_this->m_friendboard.empty()) ||
            (_this->currentTab == kTabLeaderboard && !_this->m_leaderboard.empty()))
        {
            _this->finishLoading();
        }

        _this->setDisplay();
    }
}

void LeaderboardDialog::onInvitationSent(void* sender_, int numberOfInvites, std::string fbids_) {
    
    if (numberOfInvites == 0) return;
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(CCString::createWithFormat("%d",numberOfInvites), "Number of Invites");
    params->setObject(ccs("LeaderboardDialog"), "Source");
    FlurryCpp::logEvent(EVENT_INVITE_FRIEND_SUCCESS, params);
    
    CSJson::Value eventParameters;
    eventParameters["source"] = "LeaderboardDialog";
    eventParameters["numberOfInvites"] = numberOfInvites;
    AnalyticsClient::getInstance()->logEvent(EVENT_INVITE_FRIEND_SUCCESS, eventParameters);
    
    FBAppEventsWrapper::logEvent(EVENT_INVITE_FRIEND_SUCCESS);
    
    AppsFlyerWrapper::trackEvent(EVENT_INVITE_FRIEND_SUCCESS);
    
    if (LeaderboardDialog* _this = (LeaderboardDialog*)sender_) {
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, g_gameContent->Word["facebook_invites_sent_message"].c_str(), numberOfInvites);
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
        
        CCLog_("%s", fbids_.c_str());
        BackendService::getInstance()->send(APIAppRequestAdd::create("facebook",
                                                                     fbids_,
                                                                     _this,
                                                                     LeaderboardDialog::onApprequestAdd));
        
    }
    
}

void LeaderboardDialog::onApprequestAdd(void *sender_, std::string resp_json_str_) {
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    
    LeaderboardDialog* _this = (LeaderboardDialog*)sender_;
    if (_this) {

        //JsonAllocator allocator;
        Json *_root = Json_create(resp_json_str_.c_str());
        if (!_root) {
            return;
        }
        
        if (Json_getItem(_root, "number_of_request")) {
            
            int number_of_request = Json_getInt(_root, "number_of_request", 0);
            
            CCUserDefault::sharedUserDefault()->setIntegerForKey("number_of_request", number_of_request);
            CCLog_("Number Of App Request = %d", number_of_request);
            
        }
        
        Json_dispose(_root);
        
    }
    
}

#pragma mark - LeaderboardItem

LeaderboardItem *LeaderboardItem::create(LeaderboardItemVO vo_, int index_, int currentTab_) {
    
    if (LeaderboardItem *pRet = new LeaderboardItem()) {
        if (pRet->initWithVO(vo_, index_, currentTab_)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool LeaderboardItem::initWithVO(LeaderboardItemVO vo_, int index_, int currentTab_) {

    if (!CCScale9Sprite::initWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::AchievementBG))) {
        return false;
    }
    
    setContentSize(kFrameLeaderboardSize*sizeMultiplier);
    setAnchorPoint(ccp(0.5, 0));

    // Rank Number
    m_rank_frame = NULL;
    
    // Pet
    m_pet_filename = "";
    m_pet_sprite = NULL;
    
    // Profile Picture
    m_picture_filename = "";
    m_picture_sprite = NULL;
    m_picture_frame = NULL;
    
    // Name
    m_name_label = NULL;
    
    // Level
    m_level_frame = NULL;
    m_level_icon = NULL;
    m_level_label = NULL;
    
    // Won
    m_won_label = NULL;
    
    // Visit Button
    visitButton = NULL;
    
    // Fan
    m_fan_label = NULL;
    m_fan_sprite = NULL;
    
    updateContent(vo_, index_, currentTab_);
    
    return true;
    
}

void LeaderboardItem::updateContent(LeaderboardItemVO vo_, int index_, int currentTab_)
{

    CREATE_CHAR_BUFFER(_buffer, 128);
    bool unknown_pet = false;
    CCSize sizeDefaultAvatar = ccp(102, 102) * sizeMultiplier;
    float _padding = 20.0f * sizeMultiplier;
    float _spacing = 10.0f * sizeMultiplier;

    if (!m_rank_label)
    {
        m_rank_label = CCLabelHelper::createBMFont("", kFontSizeMedium, false, kTextColorDark);
        m_rank_label->setPosition(60.0f*sizeMultiplier,
                                  getContentSize().height*0.5f);
        addChild(m_rank_label);
    }

    snprintf(_buffer, _buffer_size, "%d.", index_ + 1);
    m_rank_label->setString(_buffer);

    // Pet
    snprintf(_buffer, _buffer_size, "%s.png", vo_.host_type.c_str());
    CCSpriteFrame *_petFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(_buffer);

    if (_petFrame == NULL) {
        unknown_pet = true;
    }
    
#define PetScale 0.65f

    if (m_pet_filename != _buffer) {
        if (m_pet_sprite) {
            m_pet_sprite->removeFromParent();
        }
        
        if (unknown_pet) {
            m_pet_sprite = CCSprite::createWithSpriteFrameName("cow1.png");
            m_pet_sprite->setColor(ccBLACK);
            CCLabelBMFont *_mark_sprite = CCLabelHelper::createBMFont("?", kFontSizeExtraLarge, true, kTextColorWhite);
            _mark_sprite->setScaleX(-1.0f);
            _mark_sprite->setScaleY(1.0f);
            m_pet_sprite->setScale(PetScale);
            m_pet_sprite->addChild(_mark_sprite);
            _mark_sprite->setPosition(m_pet_sprite->getContentSize() * 0.5f
                                      + ccp(0.0f,
                                            10.0f*sizeMultiplier));
        } else {
            m_pet_sprite = CCSprite::createWithSpriteFrameName(_buffer);
        }
        m_pet_sprite->setPosition(ccp(getContentSize().width * 0.75f,
                                      getContentSize().height * 0.5f));
        m_pet_sprite->setScale(PetScale);
        m_pet_sprite->setScaleX(-1.0f*PetScale);
        m_pet_sprite->setFlipX(true);
        addChild(m_pet_sprite);
    }

    // Profile Picture
    if (m_picture_filename != vo_.photo || !m_picture_sprite)
    {
        if (m_picture_sprite)
        {
            m_picture_sprite->removeFromParent();
        }
        
        if (currentTab_ == kTabFriend) {
            m_picture_sprite = CCLoadSpriteURL::create((char *) UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::AvatarDefault)),
                                                       sizeDefaultAvatar,
                                                       (char *) vo_.photo.c_str(),
                                                       (char *) vo_.photo.c_str());
            m_picture_sprite->setPosition(ccp(170.0f*sizeMultiplier,
                                              getContentSize().height * 0.5f));
            m_picture_sprite->setScale(0.65f);
            addChild(m_picture_sprite);
            if (!m_picture_frame) {
                m_picture_frame = UIHelper::getSprite(UIHelper::UISprites(UIHelper::AvatarBorder));
                m_picture_frame->setPosition(m_picture_sprite->getPosition());
                m_picture_frame->setAnchorPoint(ccp(0.51, 0.5));
                m_picture_frame->setScale(m_picture_sprite->getScale()+kExtraAvatarFrameScale);
                addChild(m_picture_frame);
            }
        }
    }

    // Name
    if (!m_name_label) {
        m_name_label = CCLabelHelper::createBMFont(vo_.name.c_str(), kFontSizeLarge, true);
        addChild(m_name_label);
    } else {
        m_name_label->setString(vo_.name.c_str());
    }

    m_name_label->setAnchorPoint(CCPointMake(0.0f, 0.5f));
    m_name_label->setAlignment(kCCTextAlignmentLeft);
    
    snprintf(_buffer, _buffer_size, " %d", vo_.level);
    if (!m_level_label) {
        m_level_label = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, true);
        m_level_icon = UIHelper::getSprite(UIHelper::InboxLevelIcon);
        addChild(m_level_icon);
        m_level_icon->addChild(m_level_label);
    } else {
        m_level_label->setString(_buffer);
    }

    // Won
    snprintf(_buffer, _buffer_size, g_gameContent->Word["races_won"].c_str(), vo_.total_wins);
    if (!m_won_label) {
        m_won_label = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall-5.0f, false, kTextColorDark);
        addChild(m_won_label);
    } else
    {
        m_won_label->setString(_buffer);
    }

    m_won_label->setAnchorPoint(CCPointMake(0.0f, 0.5f));
    
    if (m_picture_sprite && currentTab_ == kTabFriend) {
        m_level_icon->setPosition(ccp(m_picture_sprite->getPositionX()
                                      + m_picture_sprite->getContentSize().width * 0.5f
                                      * m_picture_sprite->getScaleX()
                                      - _spacing
                                      + m_picture_sprite->getContentSize().width * 0.5f,
                                      getContentSize().height*0.5f));
    } else {
        m_level_icon->setPosition(ccp(160.0f*sizeMultiplier,
                                      getContentSize().height*0.5f));
    }
    
    m_level_label->setPosition(m_level_icon->getContentSize()*0.5f
                               + ccp(-2.0f*sizeMultiplier,
                                     0.0f));
    
    m_name_label->setPosition(ccp(m_level_icon->getPositionX()
                                  + m_level_icon->getContentSize().width*0.5f
                                  + _spacing*2,
                                  getContentSize().height*0.5f
                                  + 20.0f*sizeMultiplier));
    m_won_label->setPosition(ccp(m_level_icon->getPositionX()
                                 + m_level_icon->getContentSize().width*0.5f
                                 + _spacing*2,
                                 getContentSize().height*0.5f
                                  - 35.0f*sizeMultiplier));

    // Visit Button
    //ถ้าตรง uuid ของเราไม่ให้ visit
    if (vo_.uuid != g_playerState->player.uuid &&
        vo_.uuid != g_opponentState->player.uuid)
    {
        if (!visitButton)
        {
            visitButton = CCNikButton::create(UIHelper::getSprite(UIHelper::RankingVisitButton), NULL);
            CCLabelBMFont *_visitLabel = CCLabelHelper::createBMFont(g_gameContent->Word["map_ville_button_visit"].c_str(), kFontSizeLarge - 3.0f, true);
            _visitLabel->setPosition(visitButton->getContentSize() * 0.5f
                                     + ccp(0.0f, 5.0f*sizeMultiplier));
            visitButton->addChild(_visitLabel);
        }
        visitButton->setPosition(ccp(getContentSize().width
                                     - _padding
                                     - visitButton->getContentSize().width * 0.5f,
                                     getContentSize().height * 0.5f));
        visitButton->setTag(index_);
        addChild(visitButton);
    }

    // fan sprite
    if (!m_fan_sprite) {
        m_fan_sprite = UIHelper::getSprite(UIHelper::RankingScoreCounter);
        addChild(m_fan_sprite);
    }
//    m_fan_sprite->setPosition(0.0f, 0.0f);

    // fan label
    auto fanPositionX = getContentSize().width * 0.57f;
    
    snprintf(_buffer, _buffer_size, "%d", vo_.score);
    if (!m_fan_label) {
        m_fan_label = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, true, kTextColorWhite);
        addChild(m_fan_label);
    } else {
        m_fan_label->setString(_buffer);
    }
    
    m_fan_label->setPosition(ccp(fanPositionX,
                                 getContentSize().height * 0.5f));

    m_fan_sprite->setPositionX(fanPositionX);
    m_fan_sprite->setPositionY(m_fan_label->getPositionY());
    m_fan_label->setPositionX(fanPositionX + _spacing*3);

    // hilight player
    if (vo_.uuid == g_playerState->player.uuid) {
        setColor(ccc3(178, 216, 79));
    }
}


void LeaderboardItem::setTarget(cocos2d::CCObject *rec, SEL_MenuHandler selector, CCNikButtonEvent state) {
    
    if (visitButton) {
        visitButton->setTarget(rec, selector, state);
    }
    
}


#pragma mark - LeaderboardItem

InviteItem *InviteItem::create() {
    
    if (InviteItem *pRet = new InviteItem()) {
        if (pRet->initWithInfo()) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

bool InviteItem::initWithInfo() {
    
    if (!CCNode::init()) {
        return false;
    }
    
    setContentSize(kFrameLeaderboardSize*sizeMultiplier);
    setAnchorPoint(ccp(0.5, 0));
    
    returnNumInviteReady = false;
    
    updateContent();
    
    return true;
    
}

unsigned int InviteItem::checkCurrentInviteProgress() {
    
    return MAX(0, CCUserDefault::sharedUserDefault()->getIntegerForKey("number_of_request"));
}

void InviteItem::onAppRequestCountReceived(void *sender_, string resp_json_str_) {
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    
    if (InviteItem* _this = (InviteItem*)sender_) {

        //JsonAllocator allocator;
        Json *_root = Json_create(resp_json_str_.c_str());
        if (!_root) {
            return;
        }
        
        if (Json_getItem(_root, "number_of_request")) {
            
            unsigned int number_of_request = Json_getInt(_root, "number_of_request", 0);
            CCUserDefault::sharedUserDefault()->setIntegerForKey("number_of_request", number_of_request);
            CCLog_("Number Of App Request = %d", number_of_request);
        }
        
        Json_dispose(_root);
        
        _this->returnNumInviteReady = true;
        _this->updateInviteAchievement(sender_);
        
    }
    
}

void InviteItem::updateInviteAchievement(void *sender_)
{
    InviteItem* _this = (InviteItem*)sender_;
    if (!_this)
        return;
    
    _this->updateInviteInfo();
}

void InviteItem::updateInviteInfo() {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    if (returnNumInviteReady) 
        return;

    int currentProgress = checkCurrentInviteProgress();
    int friendsNeeded = 0;
    int gemRewards = 0;

    int target = 0;

    bool rewardFirstTime = false;

    int achievementAmount = 3;

    for (int i = 1; i <= achievementAmount; ++i)
    {
        GDAchievementsVO avo = getInviteAchievementVO(i);
        target = avo.param_1;

        if (currentProgress < target)
        {
            friendsNeeded = target - currentProgress;
            gemRewards = avo.gems_reward;

            CCLog_("achievement num:%d",i);
            CCLog_("friendsNeeded:%d",friendsNeeded);
            CCLog_("gemRewards:%d",gemRewards);

            if (i == 1) {
                snprintf(_buffer, _buffer_size, "Invite %d more friends for achievement rewards!", friendsNeeded);
            } else {
                snprintf(_buffer, _buffer_size, "Invite %d more friends for the next achievement reward!", friendsNeeded);
            }
            break;
        } else {
            if (i == achievementAmount) //no more achievement rewards to earn
            {
                strncpy(_buffer, "The more friends you have, the more helpers you get!", _buffer_size);
            }
        }
    }
    titleLabel->setString(_buffer);

    if (gemRewards != 0)
    {
        snprintf(_buffer, _buffer_size, "%d", gemRewards);
        gemNum->setString(_buffer);
        gemNum->setVisible(true);
        gemPic->setVisible(true);
    }
}

GDAchievementsVO InviteItem::getInviteAchievementVO(int num)
{
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "social_%d", num);

    GDAchievementsVO _avo;
    for (vector<GDAchievementsVO>::iterator it=g_gameContent->Achievement.begin(); it!=g_gameContent->Achievement.end(); ++it) {
       _avo = (GDAchievementsVO)(*it);

       if (_avo.type.compare(_buffer)==0)
            return _avo;
    }
    
    CCAssert(true, "searched avo is not found");
    return _avo;
}

void InviteItem::updateContent() {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    strncpy(_buffer, "Invite friends to earn achievement rewards", _buffer_size);
    
    if (!returnNumInviteReady) {
        BackendService::getInstance()->send(APIAppRequestCount::create("facebook",
                                                                       this,
                                                                       InviteItem::onAppRequestCountReceived));
    }
    
    titleLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-3.0f, kTextColorDark);
    titleLabel->setWidth(getContentSize().width*0.5f);
    titleLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    titleLabel->setPosition(ccp(25.0f*sizeMultiplier,
                                getContentSize().height*0.5f
                                +15.0f*sizeMultiplier
                                ));
    addChild(titleLabel);
    
    //-----------facebook---------
    facebookButton = CCNikButton::create(UIHelper::getSprite(UIHelper::FacebookConnectionFacebookButton), NULL);
    facebookButton->setAnchorPoint(ccp(0.5f, 0.5f));
    facebookButton->setScale(1.1f);
    facebookButton->setPosition(ccp(getContentSize().width
                                    - 25.0f*sizeMultiplier
                                    - facebookButton->getContentSize().width*facebookButton->getScale()*0.5f,
                                    getContentSize().height*0.5f
                                    +15.0f*sizeMultiplier));
    addChild(facebookButton, kLeaderboardDialogNameMaxLength);
    
    CCLabelBMFont* _facebookLabel = CCLabelHelper::createBMFont("Invite", kFontSizeMedium, true);
    _facebookLabel->setAnchorPoint(CCPointMake(0.5f, 0.5f));
    _facebookLabel->setPosition(facebookButton->getContentSize().width*0.62f,
                                facebookButton->getContentSize().height*0.5f
                                +5*sizeMultiplier);
    facebookButton->addChild(_facebookLabel);
    
    gemPic = UIHelper::getSprite(UIHelper::AchievementsRewardGem);
    gemPic->setPosition(ccp(facebookButton->getPositionX()
                            - facebookButton->getContentSize().width*facebookButton->getScale()*0.5f
                            - 210.0f*sizeMultiplier,
                            getContentSize().height*0.5f
                            +15.0f*sizeMultiplier));
    addChild(gemPic);
    
    gemNum = CCLabelHelper::createBMFont("", kFontSizeLarge, true);
    gemNum->setAnchorPoint(ccp(0.0f, 0.5f));
    gemNum->setAlignment(kCCTextAlignmentLeft);
    gemNum->setPosition(ccp(gemPic->getPositionX()
                            + gemPic->getContentSize().width*0.5f
                            + 30.0f*sizeMultiplier,
                            getContentSize().height*0.5f
                            +15.0f*sizeMultiplier));
    gemNum->setAnchorPoint(ccp(0,0.5));
    addChild(gemNum);

    gemPic->setVisible(false);
    gemNum->setVisible(false);
    
    updateInviteInfo();
}

void InviteItem::setTarget(cocos2d::CCObject *rec, SEL_MenuHandler selector, CCNikButtonEvent state) {
    
    if (facebookButton) {
        facebookButton->setTarget(rec, selector, state);
    }
    
}
