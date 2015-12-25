//
// Created by tosssapong on 10/6/14 AD.
//

#include "InboxDialog.h"
#include "time.h"

USING_NS_CC;
USING_NS_CC_EXT;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;
#include "../../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

//#include "../../Commands/SceneVilleCommand.h"
extern CCScene *g_currentScene;
#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIRaceRewardClaim.h"

#include "../../Scenes/SceneVille.h"
#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;

#include "../../Scenes/SceneWorldMap.h"
extern CCScene* g_currentScene;

#include "../../Settings.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../VOs/HistoryItemVO.h"
#include "../../DialogSettings.h"
#include "../../Helpers/DataCache.h"
#include "../Effects/CollectItemEffect.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"

InboxDialog *InboxDialog::create(CCModalController *parentModalController,
                                 CCSize size ){

    InboxDialog *p = new InboxDialog();
    p->init(parentModalController, size);
    p->autorelease();
    return p;
    
}

bool InboxDialog::init(CCModalController *parentModalController,
                       CCSize size) {
    
    if (!ScrollDialog::init(parentModalController, size)) {
        return false;
    }

    createScrollContent(INBOX_DIALOG_SETTINGS);

    m_tag_replay = 100000;
    m_tag_claim = 110000;

    if (g_playerState->HistoryItems.size() > 0) {
        updateContent(g_playerState->HistoryItems.size());
    }else {
        showLoading();
        DataCache::Instance().getDataAsync(DataCache::Inbox,
                                           this,
                                           InboxDialog::onHistoryListRetrieved);
    }

    return true;
    
}

void InboxDialog::cleanup() {

    g_playerState->clearNewHistoryBadgeNumber();

    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (!IS_INSTANCE_OK(_scene, SceneVille))
        return;

    _scene->hudPanel->updateBadgeInbox();

    ScrollDialog::cleanup();
    DataCache::Instance().resetCallback();
    
}

void InboxDialog::updateContent(int count) {
    
    addTitle("Defense Log");

    if(count <= 0)
    {
        addLabelScrollManual(ccp(m_frontBackground->getContentSize().width*0.5f,
                                 m_frontBackground->getContentSize().height*0.5f),
                             0,
                             "No new messages",
                             CENTER,
                             1.5f);
        return;
    }
    
    CCScale9Sprite *_frame;
    
    m_tag = 0;
    m_tag_index = count-1;
    
#define kYSpaceBetweenItems (8.0f*sizeMultiplier)
    
    CCNode *_last_item = NULL;
    
    for (m_tag = 0; m_tag < count; m_tag++)
    {
        m_tag_index = count - 1 - m_tag;

        HistoryItemVO history = g_playerState->HistoryItems[m_tag_index];

        _frame = UIHelper::getScalableSprite(UIHelper::AchievementBG);
        _frame->setContentSize(kFrameInboxSize * sizeMultiplier);
        _frame->setAnchorPoint(ccp(0.5, 0));
        
        _frame->setPosition(ccp(m_clippingLayer->getContentSize().width*0.5f,
                                m_tag*(_frame->getContentSize().height+kYSpaceBetweenItems)
                                ));

        m_scrollView->addChild(_frame);
        _frame->setTag(m_tag);
        
        _last_item = _frame;

        compose(_frame);
    }

    int frameCount = m_tag;

    if (frameCount > 0 && _last_item != NULL)
    {
        float sizeHeight = MAX(_last_item->getPositionY()
                               + _last_item->getContentSize().height
                               + 25.0f*sizeMultiplier
                               ,
                               m_scrollView->getContentSize().height);

        float totalHeight = _last_item->getPositionY() + _last_item->getContentSize().height + 25.0f*sizeMultiplier;
        //count*(_last_item->getContentSize().height+kYSpaceBetweenItems)
        
        if(totalHeight < m_frontBackground->getContentSize().height)
        {
            for (int i = 0; i < frameCount; i++) {
                if (CCNode *_item = m_scrollView->contentNode->getChildByTag(i)) {
                    _item->setPosition(_item->getPosition()
                                       + ccp(0.0f,
                                             m_frontBackground->getContentSize().height - totalHeight
                                             ));
                }
            }
        }
        
        m_scrollView->setContentView(CCSizeMake(m_scrollView->getContentSize().width, sizeHeight));
        m_scrollView->setContentOffset(ccp(0,
                                           m_scrollView->getContentView().height
                                           - m_scrollView->getContentSize().height));
    }
}

bool InboxDialog::compose(cocos2d::extension::CCScale9Sprite *frame) {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    HistoryItemVO history = g_playerState->HistoryItems[m_tag_index];
    
    CCSize frameSize = frame->getContentSize();
//    CCPoint startingPos = ccp(0.0f, frame->getPositionY());

    bool isNew = g_playerState->isThisANewHistoryItem(history.race_id);

    if (isNew)
    {
        auto star = UIHelper::getSprite(UIHelper::InboxNewStar);
        star->setPosition(ccp(frameSize.width*0.4f, frameSize.height*0.5f));
        frame->addChild(star);
        
        auto starLabel = CCLabelHelper::createBMFont("New", kFontSizeMedium, true);
        starLabel->setPosition(star->getPosition());
        frame->addChild(starLabel);
    }
    
    // Pet
    snprintf(_buffer, _buffer_size, "%s.png", history.guest.pet.c_str());
    CCSpriteFrame *_petFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(_buffer);
    
    bool unknown_pet = (_petFrame == NULL);
    
#define PetScale 0.65f
    
    CCSprite *animal = NULL;
    
    if (unknown_pet) {
        animal = UIHelper::getSpriteByName("cow1.png");
        animal->setColor(ccBLACK);
        CCLabelBMFont *_mark_sprite = CCLabelHelper::createBMFont("?", kFontSizeExtraLarge, true, kTextColorWhite);
        _mark_sprite->setScaleX(-1.0f);
        _mark_sprite->setScaleY(1.0f);
        animal->setScale(PetScale);
        _mark_sprite->setPosition(animal->getContentSize() * 0.5f
                                  + ccp(0.0f*sizeMultiplier,
                                        10.0f*sizeMultiplier));
    } else {
        animal = UIHelper::getSpriteByName(_buffer);
    }
    animal->setPosition(ccp(frameSize.width*0.53f, frameSize.height*0.5f));
    frame->addChild(animal);
    animal->setScale(PetScale);
    animal->setScaleX(-1.0f*PetScale);
    animal->setFlipX(true);
    
    

    //name & level
    
    CCSprite *levelIcon = UIHelper::getSprite(UIHelper::InboxLevelIcon);
    levelIcon->setPosition(ccp(100.0f*sizeMultiplier,
                               frameSize.height*0.5f));
    frame->addChild(levelIcon);
    
    if (history.guest.level <= 0)
    {
        snprintf(_buffer, _buffer_size, "?");
    }
    else
    {
        snprintf(_buffer, _buffer_size, "%d", history.guest.level);
    }
    
    CCLabelBMFont* levelLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, true);
    levelIcon->addChild(levelLabel);
    levelLabel->setPosition(levelIcon->getContentSize()*0.5f
                            + ccp(-2.0f*sizeMultiplier,
                                  1.0f*sizeMultiplier));
    
    CCLabelBMFont *_guestName = CCLabelHelper::createBMFont(history.guest.name.c_str(), kFontSizeMedium, true);
    _guestName->setAnchorPoint(ccp(0.0,0.5));
    _guestName->setAlignment(kCCTextAlignmentLeft);
    _guestName->setPosition(  ccp(levelIcon->getPositionX(), 0.0f)
                            + ccp(levelIcon->getContentSize().width*0.5f
                                  + 35.0f*sizeMultiplier,
                                  frameSize.height*0.5f
                                  + 25.0f*sizeMultiplier));
    frame->addChild(_guestName);
    
    CCNikScrollViewButton *buttonReplay = createButton(UIHelper::getSpritePath(UIHelper::RankingVisitButton), m_tag_replay);
    addTextToButton("Replay", buttonReplay);
    buttonReplay->setPosition(ccp(frameSize.width
                                  -buttonReplay->getContentSize().width*0.5f
                                  ,
                                  frameSize.height*0.5f));
    buttonReplay->setTarget(this,  menu_selector(InboxDialog::onReplayButton), CCNikScrollViewButtonStateTouchUp);
    frame->addChild(buttonReplay);

    //button
    bool you_win = history.result.host_win >= 1;
    auto winLabel = 0;
    
    CCNikScrollViewButton *buttonClaim = createButton(UIHelper::getSpritePath(UIHelper::InboxClaimButton), m_tag_claim);
    addTextToButton("Claim", buttonClaim);
    buttonClaim->setVisible(false);
    frame->addChild(buttonClaim);
    
    CCPoint claimButtonPosition = buttonReplay->getPosition() + ccp(-buttonReplay->getContentSize().width*0.5f
                                                                    -buttonClaim->getContentSize().width*0.5f
                                                                    -16.0f*sizeMultiplier,
                                                                    0.0f);
    
    if (you_win)
    {
        
        if (!history.result.claimed)
        {
            buttonClaim->setVisible(true);
            buttonClaim->setPosition(claimButtonPosition);
            buttonClaim->setTarget(this,  menu_selector(InboxDialog::onClaimButton), CCNikScrollViewButtonStateTouchUp);
            winLabel = -1;
        }

        //gems
        snprintf(_buffer, _buffer_size, "+%d", history.result.gems_reward);
        
        CCLabelBMFont *gemsEarnedLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, true);
        gemsEarnedLabel->setPosition(ccp(levelIcon->getPositionX(), 0.0f)
                                     + ccp(levelIcon->getContentSize().width*0.5f
                                           + 35.0f*sizeMultiplier,
                                           frameSize.height*0.5f
                                           - 35.0f*sizeMultiplier));
        gemsEarnedLabel->setAnchorPoint(ccp(0.0f, 0.5f));
        gemsEarnedLabel->setAlignment(kCCTextAlignmentLeft);
        frame->addChild(gemsEarnedLabel);
        
        
        CCSprite *gemsEarnedIcon = UIHelper::getSprite(UIHelper::InboxGem);
        gemsEarnedIcon->setAnchorPoint(ccp(0.5f, 0.5f));
        gemsEarnedIcon->setPosition(ccp(gemsEarnedLabel->getPositionX()
                                        + gemsEarnedLabel->getContentSize().width
                                        + gemsEarnedIcon->getContentSize().width*0.5f
                                        + 15.0f*sizeMultiplier,
                                        gemsEarnedLabel->getPositionY()));
        frame->addChild(gemsEarnedIcon);
    }
    else
    {
        //fan --
        snprintf(_buffer, _buffer_size, "%d", history.host.fan);
        
        CCLabelBMFont *fansLostLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, true);
        fansLostLabel->setPosition(  ccp(levelIcon->getPositionX(), 0.0f)
                                   + ccp(levelIcon->getContentSize().width*0.5f
                                         + 35.0f*sizeMultiplier,
                                         frameSize.height*0.5f
                                         - 35.0f*sizeMultiplier));
        fansLostLabel->setAnchorPoint(ccp(0.0f, 0.5f));
        fansLostLabel->setAlignment(kCCTextAlignmentLeft);
        frame->addChild(fansLostLabel);
        
        CCSprite *fansLostIcon = UIHelper::getSprite(UIHelper::InboxMedalIcon);
        fansLostIcon->setAnchorPoint(ccp(0.0f, 0.5f));
        fansLostIcon->setPosition(ccp(fansLostLabel->getPositionX()
                                      + fansLostLabel->getContentSize().width
                                      + 15.0f*sizeMultiplier,
                                      fansLostLabel->getPositionY()));
        frame->addChild(fansLostIcon);
        
        winLabel = 2;
        
        //coin
        snprintf(_buffer, _buffer_size, "%s", CCStringHelper::createWithNumberFormat(history.result.coins_reward).c_str());
        
        CCLabelBMFont *coinsLostLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, true);
        coinsLostLabel->setAnchorPoint(ccp(0.0f, 0.5f));
        coinsLostLabel->setAlignment(kCCTextAlignmentLeft);
        coinsLostLabel->setPosition(ccp(fansLostIcon->getPositionX()
                                        + fansLostIcon->getContentSize().width
                                        + 40.0f*sizeMultiplier,
                                        fansLostIcon->getPositionY()));
        frame->addChild(coinsLostLabel);
        
        CCSprite *coinsLostIcon = UIHelper::getSprite(UIHelper::InboxCoin);
        coinsLostIcon->setAnchorPoint(ccp(0.0f, 0.5f));
        coinsLostIcon->setPosition(ccp(coinsLostLabel->getPositionX()
                                       + coinsLostLabel->getContentSize().width
                                       + 15.0f*sizeMultiplier,
                                       coinsLostLabel->getPositionY()));
        frame->addChild(coinsLostIcon);
    }

    if (winLabel != -1)
    {
        extern unsigned int currentTime;

        auto seconds = currentTime - history.timestamp;
        
        CREATE_CHAR_BUFFER(_buffer, 32);
        
        UIDisplayHelper::convertToDisplayTime2(seconds, _buffer, _buffer_size);
        snprintf(_buffer, _buffer_size, "%s ago", _buffer);

        auto timeLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, ccc3(3, 50, 42));
        timeLabel->setPosition(claimButtonPosition
                               + ccp(0.0f,
                                     20.0*sizeMultiplier));
        frame->addChild(timeLabel);
        
        CCSprite *clockIcon = UIHelper::getSprite(UIHelper::UpgradeClockIcon);
        clockIcon->setPosition(claimButtonPosition
                               + ccp(-timeLabel->getContentSize().width*0.5f
                                     -clockIcon->getContentSize().width*0.5f
                                     -15.0f*sizeMultiplier,
                                     20.0f*sizeMultiplier));
        frame->addChild(clockIcon);

        CCLabelBMFont *winLoseLabel = NULL;
        if (winLabel == 0)
        {
            winLoseLabel = CCLabelHelper::createBMFont("You won!", kFontSizeSmall, ccc3(3, 50, 42));
        }
        else
        {
            winLoseLabel = CCLabelHelper::createBMFont("You lost", kFontSizeSmall, ccc3(165, 5, 16));
        }
        winLoseLabel->setPosition(claimButtonPosition
                                  + ccp(0.0f,
                                        -35.0f*sizeMultiplier));
        frame->addChild(winLoseLabel);
    }

    return true;
}

void InboxDialog::onClaimButton(cocos2d::CCObject *sender) {
    
    CCNikScrollViewButton *button = (CCNikScrollViewButton*)sender;
    button->setEnabled(false);
    
    m_pressedButton = button;
    int _tag = button->getTag()-m_tag_claim;

    if (g_playerState->HistoryItems.size() > 0) {
        HistoryItemVO history = g_playerState->HistoryItems[g_playerState->HistoryItems.size()-1-_tag];
        history.result.claimed = 1;
        g_sceneManager->showLoading();
        BackendService::getInstance()->send(APIRaceRewardClaim::create(history.race_id, this, InboxDialog::onRewardClaimed));
        
        CSJson::Value eventParameters;
        eventParameters["gem"] = history.result.gems_reward;
        AnalyticsClient::getInstance()->logEvent(EVENT_GEM_EARN_DEFENSE_WON, eventParameters);
    }
    
    g_gameAudio->playEffect(AUDIO_COLLECT);
    
}

void InboxDialog::onReplayButton(cocos2d::CCObject *sender) {

    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille)) return;
  
    CCNikScrollViewButton *button = (CCNikScrollViewButton*)sender;
    
    int _tag = button->getTag()-m_tag_replay;
    _scene->fadeOutThisSceneButWaitForReplay(SCENE_RACE,
        g_playerState->HistoryItems[g_playerState->HistoryItems.size()-1-_tag].race_id);
    
    m_modalController->closeAll();
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
}

#pragma mark - Callback from Backend

void InboxDialog::onHistoryListRetrieved(void* sender,
                                         std::string testString) {
    
    InboxDialog * _this = static_cast<InboxDialog *>(sender);
    if (!_this) return;

    _this->hideLoading();
    _this->updateContent(g_playerState->HistoryItems.size());
    
}

void InboxDialog::onRewardClaimed(void* sender,
                                  std::string resp_str){

    PlayerState* _playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parseHistoryItemData(_root);
    Json_dispose(_root);
    
    g_sceneManager->hideLoading();
    
    InboxDialog* _this = (InboxDialog*)sender;
    if (!IS_INSTANCE_OK(_this, InboxDialog))
        return;
    
    CCNikScrollViewButton *button = _this->m_pressedButton;
    CCNode *frame = _this->m_pressedButton->getParent();
    
    int _tag = button->getTag()-_this->m_tag_claim;
    int _buttonTag = _tag + _this->m_tag_replay;
    CCSprite *button2 = (CCSprite*)frame->getChildByTag(_buttonTag);
    
    _this->posTouch = button->getParent()->convertToWorldSpace(button->getPosition());
    CCLabelBMFont *claimedLabel = CCLabelHelper::createBMFont("Claimed", kFontSizeMedium, true);
    claimedLabel->setPosition(ccp(frame->getContentSize().width
                                  -button->getContentSize().width*0.5f
                                  ,
                                  frame->getContentSize().height*0.5f));
    claimedLabel->setTag(999);
    frame->addChild(claimedLabel);
    button->removeFromParent();
    button2->removeFromParent();

    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (!IS_INSTANCE_OK(_scene, SceneVille)) return;

    HUDPanel *hudPanel = _scene->hudPanel;
    CCSprite *gemIconSprite = hudPanel->resourceBars[HUDPanelBarGems]->getIconSprite();
    CCPoint endPoint = gemIconSprite->convertToWorldSpaceAR(CCPointZero);
    CollectItemEffect::addCollectItemEffect(CollectItemEffect::create(UIHelper::getSpritePath(UIHelper::HUDGem),
                                                                      kCollectItemEffectDefaultSprite,
                                                                      _this->posTouch,
                                                                      endPoint,
                                                                      _this,
                                                                      InboxDialog::onCollectGemsEffectFinished), _scene);
    
}

void InboxDialog::onCollectGemsEffectFinished(void *sender) {
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (!IS_INSTANCE_OK(_scene, SceneVille))
        return;
    
    _scene->hudPanel->updateAllResourceBars();
    
}
