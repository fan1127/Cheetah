//
//  MatchPanel.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/9/56 BE.
//
//

#include "MatchPanel.h"
USING_NS_CC;
using namespace std;

#include <spine/Json.h>

#include "../../AppMacros.h"
extern float margin;
extern float sizeMultiplier;
extern float retinaMultiplier;
extern float isIPhoneMultiplier;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Scenes/SceneMatch.h"
extern CCScene* g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIPetFeed.h"
#include "../../Services/APIs/APIRaceNew.h"

#include "../../Helpers/FlurryCpp.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCNodeHelper.h"

#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCLoadSpriteURL.h"
#include "../../Extensions/CCLabelHelper.h"

#include "../../UIs/AnimalButton.h"
#include "../../UIs/Dialogs/IAPDialog.h"

#include "../Dialogs/InstantRaceDialog.h"
#include "../Dialogs/ChooseHelperDialog.h"
#include "../../DialogSettings.h"

#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"

#include "../../Helpers/CustomEventsHelper.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

MatchPanel *MatchPanel::create(HUDPanel *hudPanel_) {
    
    if (MatchPanel *pRet = new MatchPanel()) {
        if (pRet->init(hudPanel_)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
}

bool MatchPanel::init(HUDPanel *hudPanel_)
{
    if (!CCLayer::init())
    {
        return false;
    }
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();

    m_centerPanelLayer = CCLayer::create();
    this->addChild(m_centerPanelLayer,3);
    
    m_menuRace = NULL;
    

#ifndef DEBUG_RACE_PLAYERS_EVEN_IF_ONLINE
    if (!g_opponentState->isOnline() || g_opponentState->isBot()) {
#endif

        // Race Button
        raceStatus = RACE_STATUS_READY;
        
        t_btnRace = UIHelper::getSprite(UIHelper::RaceElementsStartButton);
        GDExperienceLevelVO _exp_vo = g_gameContent->getNextExperienceLevelVO(g_playerState->player.racing.level-1);
        m_raceFee = _exp_vo.race_fee;

        raceLabel = CCLabelHelper::createBMFont(g_gameContent->Word["match_panel_race_button_label"].c_str(), kFontSizeVeryLarge, true);
        raceLabel->setPosition(RACE_LABEL_HAS_RACE_FEE_POSITION);
        t_btnRace->addChild(raceLabel);

        raceFeeLabel = CCLabelHelper::createBMFont("", kFontSizeMedium, true);
        raceFeeLabel->setString(CCStringHelper::createWithNumberFormat(m_raceFee).c_str());
        raceFeeLabel->setAnchorPoint(ccp(0.0f, 0.5f));
        raceFeeLabel->setAlignment(kCCTextAlignmentRight);
        raceFeeLabel->setPosition(ccp(t_btnRace->getContentSize().width*0.55f,
                                      t_btnRace->getContentSize().height*0.15f));
        t_btnRace->addChild(raceFeeLabel);
        
        t_btnRaceCoin = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);
        t_btnRaceCoin->setPosition(ccp(t_btnRace->getContentSize().width*0.45f,
                                       raceFeeLabel->getPositionY()+
                                       (t_btnRaceCoin->getContentSize().height*0.1f)));
        t_btnRace->addChild(t_btnRaceCoin);
        
        CCMenuItemSprite* t_menuItemRace = CCMenuItemScaleSprite::create(t_btnRace, this, menu_selector(MatchPanel::onRaceButton));
        m_menuRace = CCMenu::create(t_menuItemRace, NULL);
        m_menuRace->setContentSize(t_btnRace->getContentSize());
        m_menuRaceDefaultPosition = ccp(_screenSize.width
                                        -((t_btnRace->getContentSize().width*0.5f)
                                          +(margin*sizeMultiplier))
                                        ,
                                        (t_btnRace->getContentSize().height*0.5f)
                                        +(margin*sizeMultiplier));
        m_menuRace->setTouchPriority(2);
        m_menuRace->setPosition(m_menuRaceDefaultPosition);
        
        //hack to make the button have center anchor points for animations
        t_btnRace->setAnchorPoint(ccp(0.5f, 0.5f));
        t_btnRace->setPosition(t_btnRace->getContentSize()*0.5f);
        
        if (m_raceFee <= 0) {
            raceFeeLabel->setVisible(false);
            t_btnRaceCoin->setVisible(false);
        }
        
        m_menuRace->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);
        m_centerPanelLayer->addChild(m_menuRace);
        
#ifndef DEBUG_RACE_PLAYERS_EVEN_IF_ONLINE
    }
#endif
    
    // Exit Button
    CCSprite *t_btnExit = UIHelper::getSprite(UIHelper::RaceElementsBackButton);
    CCLabelBMFont *exitLabel = CCLabelHelper::createBMFont(g_gameContent->Word["match_panel_exit_button_label"].c_str(), kFontSizeVeryLarge, true);
    exitLabel->setPosition(ccp(t_btnExit->getContentSize().width*0.5f,
                               t_btnExit->getContentSize().height*0.5f
                               +(6.0f*sizeMultiplier)));
    t_btnExit->addChild(exitLabel);
    CCMenuItemSprite* t_menuItemExit = CCMenuItemScaleSprite::create(t_btnExit, this, menu_selector(MatchPanel::backToMapScene));
    m_menuExit = CCMenu::create(t_menuItemExit, NULL);
    m_menuExit->setContentSize(t_btnExit->getContentSize());
    //position on the left of the race button
    if (m_menuRace) { // regards race button
        m_menuExit->setPosition(ccp(_screenSize.width
                                    -((t_btnExit->getContentSize().width*0.5f)
                                      +m_menuRace->getContentSize().width
                                      +(margin*sizeMultiplier))
                                    ,
                                    (t_btnExit->getContentSize().height*0.5f)
                                    +(margin*sizeMultiplier)));
    }else { //without race button
        m_menuExit->setPosition(ccp(_screenSize.width
                                    -t_btnExit->getContentSize().width*0.5f
                                    -margin*sizeMultiplier,
                                    t_btnExit->getContentSize().height*0.5f
                                    +margin*sizeMultiplier));
    }
    m_menuExit->setTouchPriority(2);
    m_menuExit->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);
    m_centerPanelLayer->addChild(m_menuExit, 2);
    
    if (!m_menuRace) {//
        CCLabelBMFont *_onlineLabel = CCLabelHelper::createBMFont(g_gameContent->Word["match_panel_cannot_race"].c_str(), kFontSizeMedium, true);
        _onlineLabel->setPosition(_screenSize.width*0.5f, _screenSize.height*0.75f);
        _onlineLabel->setWidth(_screenSize.width*0.33f);
        _onlineLabel->setAlignment(kCCTextAlignmentCenter);
        _onlineLabel->setColor(ccYELLOW);
        _onlineLabel->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(2.0f, 0), CCFadeTo::create(2.0f, 255), NULL)));
        m_centerPanelLayer->addChild(_onlineLabel, 2);
    }

    _compose_player_view();
    scheduleUpdate();
    
    showOpponentDialog = false;

    canTouch = true;
    
    
    CREATE_CHAR_BUFFER(_buf, 128);
    
    //----------- show Coin Rewards --------
    
    float ySpaceBetweenItems = 75.0f*sizeMultiplier;
    float startingHeight =
    _screenSize.height
    - 200.0*sizeMultiplier;
    
    
    int coins_reward = g_opponentState->coinsReward;
    
    strncpy(_buf, "1st Place Reward:", _buf_size);
    CCLabelBMFont *_1stPlaceRewardLabel = CCLabelHelper::createBMFont(_buf, kFontSizeLarge, true);
    _1stPlaceRewardLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    _1stPlaceRewardLabel->setAlignment(kCCTextAlignmentLeft);
    _1stPlaceRewardLabel->setPosition(ccp(margin,
                                          startingHeight));
    addChild(_1stPlaceRewardLabel);
    
    CCSprite *coinIcon = UIHelper::getSprite(UIHelper::HUDCoin);
    coinIcon->setScale(0.75f);
    coinIcon->setAnchorPoint(ccp(0.0f, 0.5f));
    coinIcon->setPosition(ccp(margin,
                              _1stPlaceRewardLabel->getPositionY()
                              - ySpaceBetweenItems));
    this->addChild(coinIcon);
    
    snprintf(_buf, _buf_size, "%s",  CCStringHelper::createWithNumberFormat(coins_reward).c_str() ); //coins reward without winstreak
    CCLabelBMFont *_coinsRewardLabel = CCLabelHelper::createBMFont(_buf, kFontSizeLarge, true);
    _coinsRewardLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    _coinsRewardLabel->setAlignment(kCCTextAlignmentLeft);
    _coinsRewardLabel->setPosition(ccp(margin
                                       + (coinIcon->getContentSize().width * coinIcon->getScale())
                                       + margin,
                                       _1stPlaceRewardLabel->getPositionY()
                                       - ySpaceBetweenItems));
    addChild(_coinsRewardLabel);
    
    int total_coins_reward = 0;
    if (Building* _garage = g_playerState->getBuilding(kBuildingGarage, 0)) {
        total_coins_reward = g_gameContent->cal_coins_reward(_garage->pbVo.level, g_opponentState->coinsReward, g_playerState->player.info.win_streak + 1);
    } else {
        total_coins_reward = g_gameContent->cal_coins_reward(0, g_opponentState->coinsReward, g_playerState->player.info.win_streak + 1);
    }
    int streak_reward = total_coins_reward - coins_reward;
    
    if (streak_reward > 0) {
        
//        snprintf(_buf, _buf_size, "+%d", streak_reward); //winstreak without coins reward
        snprintf(_buf, _buf_size, "+%s",  CCStringHelper::createWithNumberFormat(streak_reward).c_str() ); //winstreak without coins reward
        
        CCLabelBMFont *_streakRewardLabel = CCLabelHelper::createBMFont(_buf, kFontSizeLarge, true, ccc3(255, 214, 54));
        _streakRewardLabel->setAnchorPoint(ccp(0.0f, 0.5f));
        _streakRewardLabel->setAlignment(kCCTextAlignmentLeft);
        _streakRewardLabel->setPosition(ccp(margin
                                            + 300.0f*sizeMultiplier,
                                            _coinsRewardLabel->getPositionY()));
        addChild(_streakRewardLabel);
    }
    
    //----------- show Items Drop --------
//    CCLabelBMFont *chanceLabel = CCLabelHelper::createBMFont(g_gameContent->Word["match_panel_possible_rewards"].c_str(),
//                                                             kFontSizeLarge,
//                                                             true);
    CCLabelBMFont *chanceLabel = CCLabelHelper::createBMFont("Chance to get:",
                                                             kFontSizeLarge,
                                                             true);
    chanceLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    chanceLabel->setAlignment(kCCTextAlignmentLeft);
    chanceLabel->setPosition(ccp(margin,
                                 _coinsRewardLabel->getPositionY() -
                                 ySpaceBetweenItems
                                 - 50.0f*sizeMultiplier));
    addChild(chanceLabel);
    
    snprintf(_buf, _buf_size, "%s", g_opponentState->itemDrops.c_str());
    
    listItemsDrop.push_back(_buf);
    
    Pet *opponent_pet = g_opponentState->getPet(g_opponentState->player.racing.host);
    int totalItems = opponent_pet->pVo.item_drops.size();
    
    //convert opponent uuid to seed
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "%s", g_opponentState->player.uuid.c_str());
    
    unsigned int sum = 0;
    for (unsigned int i=0; i<strlen(_buffer); i++) {
        sum += (unsigned int)_buffer[i];
    }
    
    //Another Item show = 2
    int amount = 2;
    if(opponent_pet->pVo.item_drops.size()<amount+1) {
        amount = opponent_pet->pVo.item_drops.size()-1;
    }
    
    /* initialize random seed: */
    srand (sum);
    for (int i=0; i<amount; ++i) {
        while(true) {
            int randomIndex = rand() % totalItems;
            if(!checkDupItems(opponent_pet->pVo.item_drops[randomIndex])) {
                snprintf(_buf, _buf_size, "%s", opponent_pet->pVo.item_drops[randomIndex].c_str());
                listItemsDrop.push_back(_buf);
                break;
            }
        }
    }

    //show item drop
    int randomFirst = sum % listItemsDrop.size();
    CCNode *_some_item = NULL;
    for (int j=0; j<listItemsDrop.size(); ++j)
    {
        randomFirst++;
        randomFirst%=listItemsDrop.size();

        CCSprite *frame = UIHelper::getScalableSpriteAsNormal(UIHelper::AchievementBG);
        CCNikButton* _btnItem = CCNikButton::create(frame, NULL);
        _btnItem->setTarget(this, menu_selector(MatchPanel::onItem), CCNikButtonEventTouchDown);
        _btnItem->setScale(1.5f);
        _btnItem->setPosition((margin + _btnItem->getContentSize().width*0.5f*_btnItem->getScaleX() +
                               (margin + _btnItem->getContentSize().width*_btnItem->getScaleX())*j),
                              chanceLabel->getPositionY()
                              - ySpaceBetweenItems
                              - 30.0f*sizeMultiplier
//                              - _btnItem->getContentSize().height*0.5f*_btnItem->getScaleY()
                              );
        _btnItem->setTag(randomFirst); //set index of selected item into tag, use tag to find what we selected here again in callback
        addChild(_btnItem);

        snprintf(_buf, _buf_size, "%s.png", listItemsDrop[randomFirst].c_str());
        CCSprite *_item = CCSprite::createWithSpriteFrameName(_buf);
        _item->setScale(frame->getContentSize().width*0.75/_item->getContentSize().width);
        _item->setPosition(_btnItem->getContentSize()*0.5);
        _btnItem->addChild(_item);
        
        if (!_some_item) _some_item = _item;
    }
    
    m_chooseHelperEnabled = true;
    onWillLaunchChooseHelper = NULL;
    onWillLaunchChooseHelperSender = NULL;
    
    scheduleOnce(schedule_selector(MatchPanel::checkItemCapacity), 1.0f);
    
    return true;
    
}

bool MatchPanel::checkDupItems(std::string newItem) {
    
    bool result = false;
    
    for (int i=0; i<listItemsDrop.size(); ++i) {
        if(newItem.compare(listItemsDrop[i])==0) {
            return true;
        }
    }
    
    return result;
    
}

void MatchPanel::checkItemCapacity() {
    
    g_playerState->checkAndPrintIfItemBarnIsFull();
    
}

void MatchPanel::update(float delta_) {
    
    if (m_raceSceneLaunching) return;
    
    if(!showOpponentDialog) {
        if(g_opponentState->HostHabitat) {
            SceneMatch *_scene = (SceneMatch*)g_currentScene;
            if(_scene) {
                if(!_scene->scrollView->getAnimated()) {
                    showOpponentDialog = true;
                    _compose_opponent_view();
                }
            }
        }
    }
    
    if (!m_menuRace) return;
    
    if (m_menuRace->numberOfRunningActions() == 0) {
        if (t_btnRace->numberOfRunningActions() == 0) {
            
            if (CustomEventsHelper::Instance().shouldAnimateIdle(kIdleAnimationRaceButton)) {
                t_btnRace->runAction(CustomEventsHelper::Instance().getIdleAnimation());
            }
            
        }
    }
    
    if (g_playerState->PetRunner) {// && showPopupAvatar) {
        g_playerState->PetRunner->updatePet(delta_);
        //SET STATUS PET WANT ENERGY
        if (g_playerState->PetRunner->isSleeping()) {
            
            if (m_petSpine) {
                if (m_petSpine->getTag() != 100) {
                    m_petSpine->setTag(100);
                    m_petSpine->setAnimation("habitat_sleep", true);
                    showHungry(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsSleepyEmotion)), 1.0f);
                }
            }
            
            int required_gems =  g_gameContent->get_gemcal_for_pet_wakeup(g_playerState->PetRunner->getSecondsLeftToFullEnergy());
            
            CREATE_CHAR_BUFFER(_buffer, 16);
            snprintf(_buffer, _buffer_size, "%d", required_gems);
            raceFeeLabel->setString(_buffer);
            raceFeeLabel->setColor(kTextColorWhite);
            
            raceFeeLabel->setVisible(true);
            t_btnRaceCoin->setVisible(true);
            raceLabel->setPosition(RACE_LABEL_FEED_POSITION);
            
            if (raceStatus != RACE_STATUS_WAKEUP) {
                raceStatus = RACE_STATUS_WAKEUP;
                
                raceLabel->setString(g_gameContent->Word["match_panel_race_button_label_wakeup"].c_str());
                
                // wake up
                raceLabel->setPosition(ccp(t_btnRace->getContentSize().width*0.5f, t_btnRace->getContentSize().height*1.0f));
                t_btnRace->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::RaceElementsWakeUpButton));
                t_btnRaceCoin->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::NSClaimGemBlue));
            }
        }
        //SET STATUS PET WANT FOOD
        else if (g_playerState->PetRunner->isHungry()) {
            
            if (m_petSpine) {
                if (m_petSpine->getTag() != 101) {
                    m_petSpine->setTag(101);
                    m_petSpine->setAnimation("habitat_hungry_sleep", true);
                    showHungry(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsHungryEmotion)), 1.0f);
                }
            }
            
            raceFeeLabel->setString(CCStringHelper::createWithNumberFormat((int)g_playerState->PetRunner->getHungerFoodToFull()).c_str());
            raceFeeLabel->setColor(kTextColorWhite);
            
            raceFeeLabel->setVisible(true);
            t_btnRaceCoin->setVisible(true);
            raceLabel->setPosition(RACE_LABEL_FEED_POSITION);

            if (raceStatus != RACE_STATUS_FEED)
            {
                raceStatus = RACE_STATUS_FEED;
                raceLabel->setString(g_gameContent->Word["match_panel_race_button_label_feedanimal"].c_str());

                // feed
                raceLabel->setPosition(RACE_LABEL_FEED_POSITION);
                t_btnRace->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::RaceElementsFeedButton));
                t_btnRaceCoin->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::RaceElementsFoodIcon));
            }
        }
        else
        {
            if (m_petSpine)
            {
                if (m_petSpine->getTag() != 102) {
                    m_petSpine->setTag(102);
                    m_petSpine->setAnimation("track_run", true);
                    hideHungry();
                }
            }

            if (m_raceFee > 0)
            {
                if (g_playerState->player.balance.gold < m_raceFee) {
                    raceFeeLabel->setColor(kTextColorOrange2);
                }
                
                raceFeeLabel->setString(CCStringHelper::createWithNumberFormat(m_raceFee).c_str());
                raceFeeLabel->setVisible(true);
                t_btnRaceCoin->setVisible(true);
                
                raceLabel->setPosition(RACE_LABEL_HAS_RACE_FEE_POSITION);
            }
            else
            {
                raceFeeLabel->setVisible(false);
                t_btnRaceCoin->setVisible(false);
                
                raceLabel->setPosition(RACE_LABEL_NO_RACE_FEE_POSITION);
            }
            
            if (raceStatus != RACE_STATUS_READY) {
                raceStatus = RACE_STATUS_READY;
                
                raceLabel->setString(g_gameContent->Word["match_panel_race_button_label"].c_str());

                t_btnRace->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::RaceElementsStartButton));
                t_btnRaceCoin->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::NSBuyCoinIcon));
            }
        }
    }
}

void MatchPanel::onRaceButton(cocos2d::CCNode* sender) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    if (m_raceSceneLaunching) return;
    if (!((IsoMap*)isoMap)->isTrackComplete()) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, g_gameContent->Word["incompleted_track"].c_str());
        return;
    }
    
    if (g_playerState->PetRunner->ppVo.energy < 100.0f) {
        if (SceneMatch* _scene = (SceneMatch*)g_currentScene) {
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            if (!_scene->modalController->getParent()){
                if (InstantRaceDialog *_dialog = InstantRaceDialog::create(_scene->modalController, IT_INSTANT_ENERGY_FOR_RACE, g_playerState)) {
                    _dialog->setPosition(_screenSize*0.5f);
                    _scene->addChild(_scene->modalController, 10000);
                    _scene->modalController->pushView(_dialog);
                }
            }
        }
        return;
    }
    
    if (g_playerState->PetRunner->isHungry()) {
        
        int _feed_amount = (int)g_playerState->PetRunner->getHungerFoodToFull();
        
        if (g_playerState->player.balance.food >= _feed_amount) {
            
            if (_feed_amount > 0.0f) {
                
                //play effect feed animal
                g_gameAudio->playEffect(AUDIO_FEED_ANIMAL);
                
                g_playerState->player.balance.food -= _feed_amount;
            }
            
            g_sceneManager->showLoading();
            BackendService::getInstance()->send(APIPetFeed::create(g_playerState->PetRunner,
                                                                   _feed_amount,
                                                                   this,
                                                                   MatchPanel::onPetFed));
            
        } else {
            g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, g_gameContent->Word["no_more_food"].c_str());
        }
        return;
    }

    CustomEventsHelper::Instance().checkAndIncreaseTimesUsedFor(kIdleAnimationRaceButton);

    if (g_playerState->player.balance.gold >= m_raceFee) {
        
#ifdef SKIP_CHOOSE_HELPER_WHEN_0
        if (g_playerState->HelperPets.size() == 1 ||
            g_playerState->player.racing.level < (int)g_gameContent->GameConfig["PET_HELPER_UNLOCKS_AT_LEVEL"]) {
            scheduleOnce(schedule_selector(MatchPanel::launchRaceScene), 0.1f);
        }else {
            scheduleOnce(schedule_selector(MatchPanel::launchChooseHelper), 0.1f);
        }
#else
        if (g_playerState->player.racing.level < (int)g_gameContent->GameConfig["PET_HELPER_UNLOCKS_AT_LEVEL"]) {
            scheduleOnce(schedule_selector(MatchPanel::launchRaceScene), 0.1f);
        }else {
            scheduleOnce(schedule_selector(MatchPanel::launchChooseHelper), 0.1f);
        }
#endif
        
    } else {
        
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        SceneMatch* _sceneMatch = (SceneMatch*)g_currentScene;
        if (_sceneMatch->modalController->getNStackView() == 0) {
            InstantRaceDialog *_dialog = InstantRaceDialog::create(_sceneMatch->modalController, IT_INSTANT_COINS, g_playerState);
            _dialog->setPosition(_screenSize*0.5f);
            _sceneMatch->addChild(_sceneMatch->modalController, 10000);
            _sceneMatch->modalController->pushView(_dialog);
        }
        
    }
    
}



void MatchPanel::deductGemsBeforeLaunch(int deducted_gems_) {
    
    g_playerState->player.balance.gem -= deducted_gems_;
    
    if (g_playerState->Checklist[TutChooseHelper::key()]) {
        
        runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                     CCCallFunc::create(this, callfunc_selector(MatchPanel::gemIconBeating)),
                                     CCDelayTime::create(0.5f),
                                     CCCallFunc::create(this, callfunc_selector(MatchPanel::launchChooseHelper)),
                                     NULL));
    } else {
        
        runAction(CCSequence::create(CCDelayTime::create(0.5f),
                                     CCCallFunc::create(this, callfunc_selector(MatchPanel::gemIconBeating)),
                                     CCDelayTime::create(0.5f),
                                     CCCallFunc::create(this, callfunc_selector(MatchPanel::launchRaceScene)),
                                     NULL));
    }
    
}

void MatchPanel::updateHorsePowers() {
    
    for (int i = 0; i<g_playerState->Pets.size(); i++) {
        Pet *_temp_pet = g_playerState->Pets.at(i);
        if (_temp_pet->ppVo.located_in != "") {
            
            CCString *str = CCString::createWithFormat("%d",((int)_temp_pet->ppVo.top_speed+
                                                             (int)_temp_pet->ppVo.corner_speed+
                                                             (int)_temp_pet->ppVo.jumping));\
            
            CCObject *object = NULL;
            CCARRAY_FOREACH(m_selectPetGroup->getChildren(), object)
            {
                AnimalButton *_animalButton = (AnimalButton *)object;
                
                if (_animalButton->getTag() == i) {
                    _animalButton->setHorsePowerLabelString(str->getCString());
                }
                
            }
        }
    }
    
    if (Pet *_pet = g_playerState->PetRunner) {
        CCString *str = CCString::createWithFormat("%d",((int)_pet->ppVo.top_speed+
                                                         (int)_pet->ppVo.corner_speed+
                                                         (int)_pet->ppVo.jumping));
        
        CCLabelBMFont *label = (CCLabelBMFont*)m_horsePowerSprite->getChildByTag(LABEL_AVERAGE_TAG);
        label->setString(str->getCString());
    }
}

void MatchPanel::gemIconBeating(cocos2d::CCObject* obj_) {
    
    SceneMatch *_scene = (SceneMatch*)g_currentScene;
    if (_scene->hudPanel->resourceBars[HUDPanelBarGems]->addTargetAmount(g_playerState->player.balance.gem)) {
        _scene->hudPanel->resourceBars[HUDPanelBarGems]->processTargetAmount();
    }
    
}

void MatchPanel::launchChooseHelper() {
    
    if (g_sceneManager->sceneSelector != SCENE_MATCH &&
        g_sceneManager->sceneSelector != SCENE_MATCH2) {
        return;
    }
    
    if (onWillLaunchChooseHelper) {
        onWillLaunchChooseHelper(onWillLaunchChooseHelperSender);
    }
    
    if (!m_chooseHelperEnabled) return;
    
    if (SceneMatch* _sceneMatch = (SceneMatch*)g_currentScene) {
        if (_sceneMatch->modalController->getNStackView() == 0) {
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            ChooseHelperDialog* _dialog = ChooseHelperDialog::create(_sceneMatch->modalController);
            _dialog->setPosition(_screenSize*0.5f+kDialogFriendHelpPosition);
            _sceneMatch->addChild(_sceneMatch->modalController, 100);
            _sceneMatch->modalController->pushView(_dialog);
            _sceneMatch->modalController->setHitBGToCloseAllEnabled(false);
//            _sceneMatch->modalController->setHitBGToCloseAllEnabled(false);
        }
    }
    
}

void MatchPanel::launchRaceScene() {
    
    if (g_sceneManager->sceneSelector != SCENE_MATCH &&
        g_sceneManager->sceneSelector != SCENE_MATCH2) {
        return;
    }
    
    if(canTouch) {
        if (CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(m_menuRace) == 0) {
            
            if (SceneMatch* _sceneMatch = (SceneMatch*)g_currentScene) {
                
                if (Pet *_pet = g_playerState->PetRunner) { //g_playerState->getPet(g_playerState->player.racing.host)) {
                    
                    canTouch = false;
                    
                    //                CCLog_("%d", g_opponentState->opponentPet.top_speed);
                    //                CCLog_("%d", g_opponentState->Pets[0]->ppVo.top_speed);
                    
                    Building *garage = g_playerState->getBuilding(kBuildingGarage, 0);
                    int garage_level = garage ? garage->pbVo.level : -1; //1 will mean no garage
                    
                    BackendService::getInstance()->send(APIRaceNew::create(_pet,
                                                                           garage_level,
                                                                           instant, this, MatchPanel::onRaceNewCompleted));
                    
                    g_opponentState->opponentInfo = g_opponentState->player.info;
                    
                    if (Pet* host_pet = g_opponentState->getPet(g_opponentState->player.racing.host)) {
                        g_opponentState->opponentPet = host_pet->ppVo;
                    }else {
                        //** warning can not race and return
                        g_opponentState->opponentPet = g_opponentState->Pets[0]->ppVo;
                    }
                    g_opponentState->opponentTime = 0.0f;
                    
                    g_opponentState->playerInfo =g_playerState->player.info;
                    g_opponentState->playerPet = _pet->ppVo;
                    g_opponentState->playerTime = 0.0f;
                    
                    g_opponentState->replayGuestRaw = "";
                    g_opponentState->replayHostRaw = "";
                    
                    if(showPopup) {
                        dialogOpponent->dismissPopover(false);
                        showPopup = false;
                    }
                    
                    m_raceSceneLaunching = true;
                    
                    _sceneMatch->fadeOutThisSceneAndChangeTo((int)SCENE_RACE);
                    
                }
            }
        }
    }

}

void MatchPanel::onRaceNewCompleted(void* sender, std::string resp_str) {

    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parsePetData(_root);
    Json_dispose(_root);
    
}

void MatchPanel::backToMapScene(CCNode* sender) {
    
    if (m_raceSceneLaunching) return;
    
    m_raceSceneLaunching = false;
    
    if(canTouch) {

        if (CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(m_menuExit) == 0) {
            
            if (SceneMatch* _scene = (SceneMatch*)g_currentScene) {
                
                canTouch = false;
                
                //play next music
                g_gameAudio->playEffect(AUDIO_UI_CLICK);
                g_gameAudio->clearEffectFadeOut(3.0f);
                
                if(showPopup) {
                    dialogOpponent->dismissPopover(false);
                    showPopup = false;
                }
                
                _scene->fadeOutThisSceneAndChangeTo(SCENE_WORLDMAP);
            }
        }
    }
}

#pragma mark - Show Emotion while in Habitat

void MatchPanel::showHungry(const char* typeNeed, float scaleItem) {
    
    if (emotionHungry) {
        emotionHungry->release();
        emotionHungry->removeFromParentAndCleanup(true);
        emotionHungry = NULL;
    }
    
    emotionHungry = CCSprite::createWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsBubbleEmotion)));
    emotionHungry->retain();
    CCSprite* _pet_need_icon = CCSprite::createWithSpriteFrameName(typeNeed);
    _pet_need_icon->setAnchorPoint(ccp(0.5f, 0.5f));
    _pet_need_icon->setScale(scaleItem);
    _pet_need_icon->setPosition(ccp(emotionHungry->getContentSize().width*0.5f,emotionHungry->getContentSize().height*0.5f));
    emotionHungry->setAnchorPoint(ccp(0.5f,0.5f));
    emotionHungry->addChild(_pet_need_icon);
    emotionHungry->setPosition(ccp(m_petSpine->getPositionX()+(103.0f*retinaMultiplier),
                                   m_petSpine->getPositionY()+(171.0f*retinaMultiplier)));
    emotionHungry->setOpacity(0);
    addChild(emotionHungry,10);
    
    if (emotionHungry->numberOfRunningActions() == 0 && emotionHungry->getOpacity() <= 0.0f) {
        emotionHungry->runAction(CCFadeTo::create(0.1f, 255));
        emotionHungry->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(1.0f, ccp(0.0f, 10.0f*sizeMultiplier)),
                                                                            CCMoveBy::create(1.0f, ccp(0.0f,-10.0f*sizeMultiplier)),
                                                                            NULL)));
        CCModalController::showAllChildrensAnimated(emotionHungry, 0.1f);
    }
}

void MatchPanel::hideHungry() {
    
    if (emotionHungry) {
        if (emotionHungry->getOpacity() > 0.0f) {
            emotionHungry->runAction(CCSequence::create(CCFadeTo::create(0.1f, 0), CCCallFunc::create(this, callfunc_selector(MatchPanel::delHungry)), NULL));//
            CCModalController::hideAllChildrensAnimated(emotionHungry, 0.1f);
            
        }
    }
}

void MatchPanel::delHungry(cocos2d::CCObject* sender_) {
    
    if (emotionHungry) {
        emotionHungry->release();
        emotionHungry->removeFromParentAndCleanup(true);
        emotionHungry = NULL;
    }
    
}

#pragma mark - Select Pet UI

void MatchPanel::updatePetPlayer(Pet *pet, CCPoint position) {
    
    if (!pet) {
        return;
    }
    
    //    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    if (pet->ppVo.type != "") {
        
        g_playerState->PetRunner = pet;
        
        //CLEANING
        if (m_petSpine != NULL) m_petSpine->removeFromParentAndCleanup(true);
        m_petSpine = NULL;
        
        if (m_horsePowerSprite != NULL) m_horsePowerSprite->removeFromParentAndCleanup(true);
        m_horsePowerSprite = NULL;
        
        //RECREATING
        m_petSpine =  g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, pet->ppVo.type.c_str());
        
        m_petSpine->setScale(sizeMultiplier);
        
        m_petSpine->setPosition(position);
        m_leftPanelLayer->addChild(m_petSpine);
        
        m_petSpine->setAnimation("track_run", true);
        
        CCString *str = CCString::createWithFormat("%d",
                                                   ((int)pet->ppVo.top_speed+
                                                    (int)pet->ppVo.corner_speed+
                                                    (int)pet->ppVo.jumping));
        m_horsePowerSprite = CreateSpriteHelper::getHorsePowerSprite(str->getCString(), pet->pVo.rarity_stars);
        m_horsePowerSprite->setScale(1.0f);
        m_horsePowerSprite->setPosition(ccp(m_petSpine->getPositionX()+(25.0f * sizeMultiplier),
                                            m_petSpine->getPositionY()+(-74.0f * sizeMultiplier)));
        m_leftPanelLayer->addChild(m_horsePowerSprite);
        
    }
    
}

void MatchPanel::buttonPressed(CCObject *sender) {
    
    if (m_raceSceneLaunching) return;
    
    if (m_menuRace) { //this doesn't exist when opponent is online
        m_menuRace->stopAllActions();
        m_menuRace->setPosition(m_menuRaceDefaultPosition - RACE_BTN_START_ANIM_VALUE);
        m_menuRace->runAction(CCEaseBackOut::create(CCMoveTo::create(RACE_BTN_ANIM_TIME, m_menuRaceDefaultPosition)));
    }
    
    CCMenuItemSprite *_menuItem = (CCMenuItemSprite*)sender;
    AnimalButton *_selectedAnimalButton = (AnimalButton*)_menuItem->getParent();
        
    CCObject *object = NULL;
    CCARRAY_FOREACH(m_selectPetGroup->getChildren(), object) {
        AnimalButton *_eachAnimalButton = (AnimalButton *)object;
        if (_selectedAnimalButton == _eachAnimalButton) {
            _eachAnimalButton->toggleButtonActive(true);
        } else {
            _eachAnimalButton->toggleButtonActive(false);
        }
    }
    
    Pet *_pet_player = g_playerState->Pets.at(_selectedAnimalButton->getTag());
    updatePetPlayer(_pet_player, _selectedAnimalButton->getPosition());
    
    //play effect pet
    if(_pet_player->pVo.sound_src.compare("") == 0) {
        g_gameAudio->playEffect(AUDIO_PIG_GRUNT);
    } else {
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, "sfx/animalx/%s%s", _pet_player->pVo.sound_src.c_str(), AUDIO_EXT);
        g_gameAudio->playEffect(_buffer);
    }
}


void MatchPanel::_compose_player_view()
{
    _dispose_player_view();
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    m_leftPanelLayer = CCLayer::create();
    this->addChild(m_leftPanelLayer,2);

    //m_selectPetGroup
    m_selectPetGroup = CCNode::create();
    m_leftPanelLayer->addChild(m_selectPetGroup);
    
    int _countRacePets = 0;
    
    Pet *_pet_player = g_playerState->PetRunner;
    if (!_pet_player || _pet_player->ppVo.located_in == "") {
        _pet_player = g_playerState->getPet(g_playerState->player.racing.host);
    }
    
    int petIndex = 0;
    
    float _itemWidth = 0.0f;
    float _itemHeight = 0.0f;
    m_menuPets.clear();
    
    for (int i = 0; i<g_playerState->Pets.size(); i++) {
        Pet *_temp_pet = g_playerState->Pets.at(i);
        if (_temp_pet->ppVo.located_in != "") {
            _countRacePets++;
            
            AnimalButton *_animalButton = AnimalButton::createWithPet(_temp_pet, this, menu_selector(MatchPanel::buttonPressed));
            _animalButton->setTag(i);
            m_selectPetGroup->addChild(_animalButton);
            
            _itemWidth = _animalButton->itemSize.width;
            _itemHeight = _animalButton->itemSize.height;
            
            if (_temp_pet == _pet_player) {
                petIndex = i;
                _animalButton->toggleButtonActive(true);
            }
        }
    }
    
    float _availableSpaceWidth = _screenSize.width - (920.0f * sizeMultiplier); //was 920, if margin is negative, calculation seems to be wrong
    float _marginBetweenItems = -6.0f * sizeMultiplier;
    
    float _leftMostMargin = 14.0f * sizeMultiplier;
    
    float _firstLineWidth = _leftMostMargin;
    float _countItemsPerLine = 0;
    while ((_firstLineWidth < _availableSpaceWidth) && (_countItemsPerLine < _countRacePets))
    {
        _firstLineWidth += (_itemWidth + (_marginBetweenItems * _countItemsPerLine));
        _countItemsPerLine++;
    }
    
    int _maxItemsPerLine = MIN(_countItemsPerLine, 4);
    int _layoutDoneCount = 0;
    int _lineDoneCount = 0;
    
    int _yMarginBetweenLine = -6.0f * sizeMultiplier;
    
    //setting positions
    for (int i = 0; i<g_playerState->Pets.size(); i++) {
        Pet *_temp_pet = g_playerState->Pets.at(i);
        if (_temp_pet->ppVo.located_in != "") {
            
            
            //set layout
            float yAxis = (121.0f*sizeMultiplier) + ((_yMarginBetweenLine + _itemHeight) * _lineDoneCount);
            
            m_selectPetGroup->getChildByTag(i)->setPosition(ccp(_leftMostMargin
                                                                +
                                                                (_itemWidth*0.5f) //push to align to the left side
                                                                +
                                                                (_itemWidth*_layoutDoneCount) //item space
                                                                +
                                                                (_marginBetweenItems*_layoutDoneCount) //margi
                                                                ,
                                                                yAxis));
            
            _layoutDoneCount++;
            
            if (_layoutDoneCount == _maxItemsPerLine) {
                _layoutDoneCount = 0;
                _lineDoneCount++;
            }
        }
    }
    
    
    if (_pet_player->ppVo.type != "") {
        m_petSpine =  g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, _pet_player->ppVo.type.c_str());
        m_petSpine->setScale(sizeMultiplier);
        m_petSpine->setAnimation("track_run", true);
        m_petSpine->setVisible(false);
        m_leftPanelLayer->addChild(m_petSpine);
    }
    
    this->updatePetPlayer(_pet_player, m_selectPetGroup->getChildByTag(petIndex)->getPosition());
    m_petSpine->setVisible(true);
    
}

void MatchPanel::_dispose_player_view() {
    if (m_horsePowerSprite) {
        m_horsePowerSprite->removeFromParentAndCleanup(true);
        m_horsePowerSprite = NULL;
    }
    
    if (m_leftPanelLayer) {
        m_leftPanelLayer->removeFromParentAndCleanup(true);
        m_leftPanelLayer = NULL;
    }
    
    if (m_selectPetGroup) {
        m_selectPetGroup->removeFromParentAndCleanup(true);
        m_selectPetGroup = NULL;
    }
}


#pragma mark
void MatchPanel::_compose_opponent_view() {
    
    if (g_opponentState->HostHabitat) {
        dialogOpponent = CCPopOverDialog::create(CCSizeMake(397, 230)*sizeMultiplier, CCPopoverArrowDirectionUp);
        dialogOpponent->setAutoDismiss(false);
        dialogOpponent->setAutoBoundCalcuration(false);

        dialogOpponent->setPositionAdjustment(ccp(0.0f, g_gameContent->tileSize.height*2.0f));
        dialogOpponent->presentPopover(g_opponentState->HostHabitat, g_opponentState->HostHabitat, true);
        
        CCLabelBMFont *opponentLabel = CCLabelHelper::createBMFont(g_gameContent->Word["match_panel_host_runner_title"].c_str(), kFontSizeMedium, kTextColorDark);
        opponentLabel->setPosition(ccp(dialogOpponent->getContentSize().width*0.5f,dialogOpponent->getContentSize().height*0.74));
        dialogOpponent->addChild(opponentLabel);
        
        Pet *_pet_opponent = g_opponentState->getPet(g_opponentState->player.racing.host);
        CCString *str = CCString::createWithFormat("%d",((int)_pet_opponent->ppVo.top_speed+
                                                         (int)_pet_opponent->ppVo.corner_speed+
                                                         (int)_pet_opponent->ppVo.jumping));
        m_opponentPetLabel = CreateSpriteHelper::getHorsePowerSprite(str->getCString(),
                                                                     _pet_opponent->pVo.rarity_stars,
                                                                     HorsePowerCOLOR_RED);
        m_opponentPetLabel->setPosition(ccp(dialogOpponent->getContentSize().width*0.55f,
                                            dialogOpponent->getContentSize().height*0.32f));
        dialogOpponent->addChild(m_opponentPetLabel);
        
        showPopup = true;
    }
    
}


void MatchPanel::goldPress(cocos2d::CCNode *sender)
{
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    
    openIAPStore(RESOURCE_TYPE_COIN);
}

void MatchPanel::gemsPress()
{
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    
    openIAPStore(RESOURCE_TYPE_GEM);
}

void MatchPanel::foodPress(cocos2d::CCNode *sender)
{
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    
    openIAPStore(RESOURCE_TYPE_FOOD);
}

void MatchPanel::openIAPStore(RESOURCE_TYPE resourceType)
{
    if(SceneMatch *_scene = (SceneMatch*)g_currentScene) {
        if (_scene->modalController->getNStackView() == 0) {
            
            CCDictionary *ccDict = CCDictionary::create();
            
            std::string source = "";
            switch (resourceType) {
                case RESOURCE_TYPE_GEM:
                {
                    source = "GemsTouch at MatchPanel";
                }
                    break;
                    
                case RESOURCE_TYPE_COIN:
                {
                    source = "CoinTouch at MatchPanel";
                }
                    break;
                    
                case RESOURCE_TYPE_FOOD:
                {
                    source = "FoodTouch at MatchPanel";
                }
                    break;
                    
                default:
                    break;
            }
            ccDict->setObject(CCString::create(source), "Source");
            
            FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, ccDict);
            
            CSJson::Value eventParameters;
            eventParameters["source"] = source;
            AnalyticsClient::getInstance()->logEvent(EVENT_FBLOGIN_SUCCESS, eventParameters);
            
            FBAppEventsWrapper::logEvent(EVENT_USER_OPEN_IAP_STORE);
            
            AppsFlyerWrapper::trackEvent(EVENT_USER_OPEN_IAP_STORE);
            
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            IAPDialog *_dialog = IAPDialog::create(_scene->modalController, resourceType);
            _dialog->setPosition(_screenSize*0.5f + kDialogIAPPosition);
            _scene->addChild(_scene->modalController, 10000);
            _scene->modalController->pushView(_dialog);
        }
    }
}

void MatchPanel::onPetWokeUp(void *sender_, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parsePetData(_root);
    Json_dispose(_root);
    
    g_sceneManager->hideLoading();
    
    SceneMatch* _scene = (SceneMatch*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneMatch)) return;
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    if (Json *_root = Json_create(resp_str.c_str())) {
        if (Json *_pets = Json_getItem(_root, "pet")) {
            if (Json *_pet = Json_getItemAt(_pets, 0)) {
                string type = Json_getString(_pet, "type", "");
                if (type != "") {
                    GDPetVO _pet_vo = g_gameContent->getPetVO(type);
                    snprintf(_buffer, _buffer_size, g_gameContent->Word["match_panel_pet_woke_up"].c_str(), _pet_vo.title.c_str());
                    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
                }
            }
        }
        Json_dispose(_root);
    }

    _scene->hudPanel->resourceBars[HUDPanelBarGems]->addTargetAmount(_playerState->player.balance.gem);
    _scene->hudPanel->resourceBars[HUDPanelBarGems]->processTargetAmount();
 
}

void MatchPanel::onPetFed(void *sender_, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseChecklistData(_root);
    _playerState->parsePetData(_root);
    _playerState->parseUserBalance(_root);
    Json_dispose(_root);
    
    g_sceneManager->hideLoading();
    
    SceneMatch *_scene = (SceneMatch*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneMatch)) return;
    
    _scene->matchPanel->updateHorsePowers();
    if (_scene->hudPanel->resourceBars[HUDPanelBarFoods] &&
        _scene->hudPanel->resourceBars[HUDPanelBarFoods]->addTargetAmount(_playerState->player.balance.food,
                                                                          _playerState->getTotalFoodStorageCapacity(),
                                                                          AUDIO_NONE)) {
        _scene->hudPanel->resourceBars[HUDPanelBarFoods]->processTargetAmount();
    }

}

void MatchPanel::onItem(cocos2d::CCObject *object) {
    
    CCNode* _node = (CCNode*)object;
    if (!_node) return;
    
    string _item_type = listItemsDrop[_node->getTag()];
    GDItemVO _item_vo = g_gameContent->getItemVO(_item_type);
    if (_item_vo.type == "") return;
        
    CCSize _popoverSize = CCSizeZero;
    
    // 1. create all elements and sum the content size
    CCLabelBMFont *_titleLabel = CCLabelHelper::createBMFont(_item_vo.title.c_str(), kFontSizeMedium, true);
    CCSize _contentSize = CCNodeHelper::getContentSize(_titleLabel);
    _popoverSize.height += _contentSize.height + kPopOverDialogSpacing*sizeMultiplier;
    if (_popoverSize.width < _contentSize.width) {
        _popoverSize.width = _contentSize.width;
    }
    
    CCLabelBMFont *_descriptionLabel = CCLabelHelper::createBMFont(_item_vo.description2.c_str(), kFontSizeMedium-5.0f, false, kTextColorDark, kPopOverDialogNarmalWidth*sizeMultiplier, kCCTextAlignmentCenter);
    _contentSize = CCNodeHelper::getContentSize(_descriptionLabel);
    _popoverSize.height += _contentSize.height + kPopOverDialogSpacing*sizeMultiplier;
    if (_popoverSize.width < _contentSize.width) {
        _popoverSize.width = _contentSize.width;
    }
    
    _popoverSize.height -= kPopOverDialogSpacing*sizeMultiplier;
    _popoverSize = _popoverSize + kPopOverDialogPadding*sizeMultiplier;
    
    // 2. create a popup dialog with contentSize calculated from all elements
    CCPopOverDialog *popover = CCPopOverDialog::create(_popoverSize, CCPopoverArrowDirectionAny);
    
    // 3. set position of all elements regard contentSize
    _titleLabel->setPosition(ccp(popover->getContentSize().width*0.5f,
                                 popover->getContentSize().height*1.0f - kPopOverDialogPadding.height*sizeMultiplier*0.5f - CCNodeHelper::getContentSize(_titleLabel).height*0.5f));
    popover->addChild(_titleLabel);
    
    _descriptionLabel->setPosition(_titleLabel->getPosition() + ccp(0.0f,
                                                                    - CCNodeHelper::getContentSize(_titleLabel).height*0.5f - kPopOverDialogSpacing*sizeMultiplier - CCNodeHelper::getContentSize(_descriptionLabel).height*0.5f));
    popover->addChild(_descriptionLabel);
    
    popover->presentPopover(_node, true);
    
}
