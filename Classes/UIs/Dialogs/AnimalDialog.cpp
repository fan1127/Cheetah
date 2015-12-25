//
//  AnimalDialog.cpp
//  Cheetah
//
//  Created by Kaweerut on 2/24/57 BE.
//
//

#include "AnimalDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Helpers/CreateSpriteHelper.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;
#include "AppMacros.h"
extern TargetDevice targetDevice;
#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIPetHost.h"
#include "../../Services/Apis/APIPetRetrieve.h"
#include "../../Services/APIs/APIPetStore.h"

#include "../../Entities/PlayerState.h"
extern PlayerState *g_playerState;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

extern float sizeMultiplier;

#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCNodeHelper.h"

#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCMenuHelper.h"

#include "../../Extensions/CCPopOverDialog.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/StringHelper.h"


#include "NotEnoughSpaceDialog.h"
#include "InstantDialog.h"
#include "AnimalBarnDialog.h"
#include "SellAnimalDialog.h"
#include "../../Helpers/UIHelper.h"
#include "../NumericProgressBar.h"

AnimalDialog *AnimalDialog::create(CCModalController *parentModalController_, void *entity_, AnimalDialogType t) {
    
    return AnimalDialog::create(parentModalController_, entity_, t, IMV_PLAYABLE);
    
}

AnimalDialog *AnimalDialog::create(CCModalController *parentModalController_, void *entity_, AnimalDialogType t, int viewMode_) {
    
    AnimalDialog *pRet = new AnimalDialog();
    if (pRet) {
        pRet->viewMode = viewMode_;
        if (pRet->init(parentModalController_, entity_, t, cocos2d::CCSize(1300.0f, 950.0f))) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
}

bool AnimalDialog::init(CCModalController *parentModalController_, void *entity_, AnimalDialogType t, CCSize size) {
    
    if (!ModalDialog::init(parentModalController_, size)) {
        return false;
    }
    
    m_entity = entity_;
    m_type = t;
    
    m_sendToBarnButtonEnabled = true;
    
    Building *_building = (Building*)m_entity;
    if (typeid(*_building) != typeid(Building) && t != AnimalDialogType_2) {
        return false;
    }
    
    Pet *_pet = _building->pet;
    if (Pet *__pet = (Pet*)m_entity) {
        if (typeid(*__pet) == typeid(Pet)) {
            _pet = __pet;
            _building = NULL;
        }
    }

    addTitle(_pet->pVo.title.c_str());

    cocos2d::extension::CCSkeletonAnimation *m_petSpine =  g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, _pet->ppVo.type.c_str());
    m_petSpine->setScale(sizeMultiplier * 1.2f);
    m_petSpine->setAnimation("habitat_idle_stand", true);
    m_petSpine->setPosition(ccp(this->getContentSize().width*-0.22f,
                                110.0f*sizeMultiplier));
    this->addChild(m_petSpine);
    
#define FIRST_COLUMN_ALIGHT_LEFT (-502.0f*sizeMultiplier)
#define FIRST_COLUMN_ALIGHT_RIGHT (-110.0f*sizeMultiplier)
    
    CCString *str = CCString::createWithFormat("%d",
                                               ((int)_pet->ppVo.top_speed+
                                                (int)_pet->ppVo.corner_speed+
                                                (int)_pet->ppVo.jumping));
    
    CCSprite *horsePower = CreateSpriteHelper::getHorsePowerSprite(str->getCString(), _pet->pVo.rarity_stars);
    horsePower->setPosition(ccp(m_petSpine->getPositionX(),
                                m_petSpine->getPositionY()
                                -(m_petSpine->getContentSize().height*0.5f)
                                -(80.0f * sizeMultiplier)));
    this->addChild(horsePower);
    
    
    CCLabelBMFont *lblLevel = CCLabelHelper::createBMFont(g_gameContent->Word["level"].c_str(), kFontSizeSmall, true);
    
    lblLevel->setAlignment(kCCTextAlignmentLeft);
    lblLevel->setPosition(ccp(FIRST_COLUMN_ALIGHT_LEFT, -60.0f*sizeMultiplier));
    lblLevel->setAnchorPoint(ccp(0.0f, 0.5f));
    this->addChild(lblLevel);
    
    str = CCString::createWithFormat(g_gameContent->Word["max_level"].c_str(),_pet->ppVo.level , _pet->pVo.max_level);
    CCLabelBMFont *lblMaxLevel = CCLabelHelper::createBMFont(str->getCString(), kFontSizeSmall, true);
    lblMaxLevel->setAlignment(kCCTextAlignmentRight);
    lblMaxLevel->setPosition(ccp(FIRST_COLUMN_ALIGHT_RIGHT, lblLevel->getPositionY()));
    lblMaxLevel->setAnchorPoint(ccp(1.0f, 0.5f));
    this->addChild(lblMaxLevel);

    NumericProgressBar *pb = NumericProgressBar::create();
    pb->setPosition(ccp(-305.f * sizeMultiplier, lblLevel->getPositionY() - (65.0f*sizeMultiplier)));
    pb->setContentSize(CCSizeMake(400*sizeMultiplier, 40*sizeMultiplier));
    pb->setProgressBarEnable(true);
    addChild(pb);
    pb->setWaitProgress();
    pb->hideText();

    if (_pet->ppVo.level >= _pet->pVo.max_level)
    {
        pb->setProgressNumeric(5.0f, 5.0f);
    }
    else
    {
        CCLog_("progress: %f", _pet->getExpRatio()*100.0f);
        pb->setProgressNumeric(_pet->getExpRatio()*100.0f, 100.0f);
    }

#if (!IS_RELEASE_BUILD)
#define ANIMDIALOG_DEBUG_PROGRESS_BAR
#endif
#ifdef ANIMDIALOG_DEBUG_PROGRESS_BAR
    CREATE_CHAR_BUFFER(buffer, 64);
    snprintf(buffer, buffer_size, "debug: %.2f%%", _pet->getExpRatio()*100.0f);
    CCLabelBMFont *debugProgressBar = CCLabelHelper::createBMFont(buffer, 35.0f, true);
    //debugProgressBar->setPosition(pb->getPosition());
    pb->addChild(debugProgressBar);
#endif

    str = CCString::createWithFormat("%s",g_gameContent->Word["coin_rate"].c_str());
    CCLabelBMFont *lblCoinRate = CCLabelHelper::createBMFont(str->getCString(), kFontSizeSmall, true);
    lblCoinRate->setAlignment(kCCTextAlignmentLeft);
    lblCoinRate->setPosition(ccp(FIRST_COLUMN_ALIGHT_LEFT, -190.0f*sizeMultiplier));
    lblCoinRate->setAnchorPoint(ccp(0.0f, 0.5f));
    this->addChild(lblCoinRate);
    
    str = CCString::createWithFormat("%s",g_gameContent->Word["per_min"].c_str());
    
    CCLabelBMFont *lblCoinRatePerMin = CCLabelHelper::createBMFont(str->getCString(), kFontSizeSmall, true);
    lblCoinRatePerMin->setAlignment(kCCTextAlignmentRight);
    lblCoinRatePerMin->setPosition(ccp(FIRST_COLUMN_ALIGHT_RIGHT, lblCoinRate->getPositionY()));
    lblCoinRatePerMin->setAnchorPoint(ccp(1.0f, 0.5f));
    this->addChild(lblCoinRatePerMin);
    
    str = CCString::createWithFormat("%.f",roundf(_pet->getCoinsProducedPerSecond() * 60.0f));
    
    CCLabelBMFont *lblCoinRateVal = CCLabelHelper::createBMFont(str->getCString(), kFontSizeSmall, true, ccc3(186, 255, 0));
    lblCoinRateVal->setAlignment(kCCTextAlignmentRight);
    lblCoinRateVal->setPosition(ccp(FIRST_COLUMN_ALIGHT_RIGHT
                                    - lblCoinRatePerMin->getContentSize().width
                                    + 10.0f*sizeMultiplier,
                                    lblCoinRate->getPositionY()));
    lblCoinRateVal->setAnchorPoint(ccp(1.0f, 0.5f));
    this->addChild(lblCoinRateVal);
    
    CCSprite *coinIcon = UIHelper::getSprite(UIHelper::HUDCoin);
    coinIcon->setPosition(ccp(FIRST_COLUMN_ALIGHT_RIGHT
                              - lblCoinRatePerMin->getContentSize().width
                              - lblCoinRateVal->getContentSize().width
                              - 10.0f*sizeMultiplier,
                              lblCoinRate->getPositionY()));
    coinIcon->setScale(0.5f);
    coinIcon->setAnchorPoint(ccp(1.0f, 0.5f));
    this->addChild(coinIcon);
    
    str = CCString::createWithFormat("%s",g_gameContent->Word["race_entered"].c_str());
    CCLabelBMFont *lblRatio = CCLabelHelper::createBMFont(str->getCString(), kFontSizeSmall, true);
    lblRatio->setAlignment(kCCTextAlignmentLeft);
    lblRatio->setPosition(ccp(FIRST_COLUMN_ALIGHT_LEFT, -250.0f*sizeMultiplier));
    lblRatio->setAnchorPoint(ccp(0.0f, 0.5f));
    this->addChild(lblRatio);
    
    str = CCString::createWithFormat("%d",_pet->ppVo.race_entered);
    
    CCLabelBMFont *lblRatioVal = CCLabelHelper::createBMFont(str->getCString(), kFontSizeSmall, true);
    lblRatioVal->setAlignment(kCCTextAlignmentRight);
    lblRatioVal->setPosition(ccp(FIRST_COLUMN_ALIGHT_RIGHT, lblRatio->getPositionY()));
    lblRatioVal->setAnchorPoint(ccp(1.0f, 0.5f));
    this->addChild(lblRatioVal);
    
    str = CCString::createWithFormat("%s",g_gameContent->Word["wins"].c_str());
    CCLabelBMFont *lblWins = CCLabelHelper::createBMFont(str->getCString(), kFontSizeSmall, true);
    lblWins->setAlignment(kCCTextAlignmentLeft);
    lblWins->setPosition(ccp(FIRST_COLUMN_ALIGHT_LEFT, -304.0f*sizeMultiplier));
    lblWins->setAnchorPoint(ccp(0.0f, 0.5f));
    this->addChild(lblWins);
    
    str = CCString::createWithFormat("%d",_pet->ppVo.wins);
    CCLabelBMFont *lblWinsVal = CCLabelHelper::createBMFont(str->getCString(), kFontSizeSmall, true);
    lblWinsVal->setAlignment(kCCTextAlignmentRight);
    lblWinsVal->setPosition(ccp(FIRST_COLUMN_ALIGHT_RIGHT, lblWins->getPositionY()));
    lblWinsVal->setAnchorPoint(ccp(1.0f, 0.5f));
    this->addChild(lblWinsVal);
    
    str = CCString::createWithFormat("%d",_pet->ppVo.top_speed);
    CCLabelBMFont *lblSpinVal = CCLabelHelper::createBMFont(str->getCString(), kFontSizeMedium, true);
    lblSpinVal->setAlignment(kCCTextAlignmentRight);
    lblSpinVal->setPosition(ccp(180.0f*sizeMultiplier, 196.0f*sizeMultiplier));
    lblSpinVal->setAnchorPoint(ccp(1.0f, 0.5f));
    this->addChild(lblSpinVal);
    
    str = CCString::createWithFormat("%d",_pet->ppVo.corner_speed);
    CCLabelBMFont *lblCornerVal = CCLabelHelper::createBMFont(str->getCString(), kFontSizeMedium, true);
    lblCornerVal->setPosition(ccp(180.0f*sizeMultiplier, 104.0f*sizeMultiplier));
    lblCornerVal->setAlignment(kCCTextAlignmentRight);
    lblCornerVal->setAnchorPoint(ccp(1.0f, 0.5f));
    this->addChild(lblCornerVal);
    
    str = CCString::createWithFormat("%d",_pet->ppVo.jumping);
    CCLabelBMFont *lblJumpVal = CCLabelHelper::createBMFont(str->getCString(), kFontSizeMedium, true);
    lblJumpVal->setPosition(ccp(180.0f*sizeMultiplier, 12.0f*sizeMultiplier));
    lblJumpVal->setAlignment(kCCTextAlignmentRight);
    lblJumpVal->setAnchorPoint(ccp(1.0f, 0.5f));
    this->addChild(lblJumpVal);
    
    //button detail
    CCNikButton *_topSpeedButton = CCNikButton::create(UIHelper::getSprite(UIHelper::AnimalStatsSpeedIcon), NULL);
    _topSpeedButton->setTarget(this, menu_selector(AnimalDialog::onTopSpeed), CCNikButtonEventTouchUpInside);
    _topSpeedButton->setContentSize(CCSizeMake(440.0f*sizeMultiplier, _topSpeedButton->getContentSize().height + 15.0f*sizeMultiplier));
    _topSpeedButton->setPosition(ccp(51.0f*sizeMultiplier, 196.0f*sizeMultiplier));
    _topSpeedButton->setAnchorPoint(ccp(0.5f,0.5f));
    this->addChild(_topSpeedButton, 6);

    CCNikButton *_cornerSpeedButton = CCNikButton::create(UIHelper::getSprite(UIHelper::AnimalStatsMobilityIcon), NULL);
    _cornerSpeedButton->setTarget(this, menu_selector(AnimalDialog::onCornerSpeed), CCNikButtonEventTouchUpInside);
    _cornerSpeedButton->setContentSize(CCSizeMake(440.0f*sizeMultiplier, _cornerSpeedButton->getContentSize().height + 15.0f*sizeMultiplier));
    _cornerSpeedButton->setPosition(ccp(51.0f*sizeMultiplier, 104.0f*sizeMultiplier));
    this->addChild(_cornerSpeedButton, 7);

    CCNikButton *_jumpingButton = CCNikButton::create(UIHelper::getSprite(UIHelper::AnimalStatsEnergyIcon), NULL);
    _jumpingButton->setTarget(this, menu_selector(AnimalDialog::onJumping), CCNikButtonEventTouchUpInside);
    _jumpingButton->setContentSize(CCSizeMake(440.0f*sizeMultiplier, _jumpingButton->getContentSize().height + 15.0f*sizeMultiplier));
    _jumpingButton->setPosition(ccp(51.0f*sizeMultiplier, 12.0f*sizeMultiplier));
    this->addChild(_jumpingButton, 8);

    UIHelper::UISprites trackSprite = UIHelper::AnimalStats_iconTrackAsphalt;
    const char* tracks[] = {"asphalt", "dirt", "grass", "sand", "snow"};
    std::string favTrackName;

    for (int i=0; i<5; i++)
    {
        if (std::strstr(_pet->pVo.track_preference.c_str(), tracks[i]) != NULL)
        {
            trackSprite = UIHelper::UISprites(UIHelper::AnimalStats_iconTrackAsphalt + i);
            favTrackName = StringHelper::capitalize(tracks[i]);
            break;
        }
    }

    CCSprite *preferIcon = UIHelper::getSprite(UIHelper::AnimalStatsLikeIcon);
    CCNikButton *preferValImage = CCNikButton::create(UIHelper::getSprite(trackSprite),NULL);
    preferValImage->setTarget(this, menu_selector(AnimalDialog::onFavoriteType), CCNikButtonEventTouchUpInside);
    preferValImage->setContentSize(preferValImage->getContentSize());
    preferValImage->setPosition(ccp(370.0f*sizeMultiplier, 150.0f*sizeMultiplier));
    this->addChild(preferValImage, 9);

    preferIcon->setPosition(preferValImage->getPosition());
    this->addChild(preferIcon,10);

    CCLabelBMFont *lblFavTrack = CCLabelHelper::createBMFont("Favorite Track:", kFontSizeMedium-10.0f, false, kTextColorBrown);
    lblFavTrack->setAlignment(kCCTextAlignmentRight);
    lblFavTrack->setPosition(ccp(preferIcon->getPositionX(), preferIcon->getPositionY() - 110 * sizeMultiplier));
    this->addChild(lblFavTrack);

    CCLabelBMFont *lblFavTrackName = CCLabelHelper::createBMFont(favTrackName.c_str(), kFontSizeMedium-10.0f, true, kTextColorWhite);
    lblFavTrackName->setAlignment(kCCTextAlignmentRight);
    lblFavTrackName->setPosition(ccp(lblFavTrack->getPositionX(), lblFavTrack->getPositionY() - 50 * sizeMultiplier));
    this->addChild(lblFavTrackName);

    setSellBtnEnabled(true);

    if (viewMode == IMV_PLAYABLE)
    {
        this->addButton();
    }

    if ((m_type == AnimalDialogType_1 && g_gameContent->Word["note_animal_dialog_1"] != "") ||
        (m_type == AnimalDialogType_2 && g_gameContent->Word["note_animal_dialog_1"] != ""))
    {
        if (g_gameContent->Word["note_animal_dialog_1"] != "") {
            addHelpButton(g_gameContent->Word["note_animal_dialog_1"].c_str(),
                          menu_selector(AnimalDialog::onNote));
        } else {
            addHelpButton(g_gameContent->Word["note_animal_dialog_2"].c_str(),
                          menu_selector(AnimalDialog::onNote));
        }
    }
    
    return true;
}

void AnimalDialog::addButton()
{
    auto bgButton = new CCNode();
    bgButton->setContentSize(CCSize(523.0f*sizeMultiplier,
                                    240.0f*sizeMultiplier));
    bgButton->setPosition(ccp(220.0f*sizeMultiplier,
                              -150.0f*sizeMultiplier));
    //this->addChild(bgButton);
    
    CCString *strLbl1 = NULL;
    CCString *strLbl2 = NULL;
    
    CCSprite *button_icon1 = NULL;
    CCSprite *button_icon1_press = NULL;
    
    CCSprite *button_icon2 = NULL;
    CCSprite *button_icon2_press = NULL;

    auto textColor = kTextColorLightBrown;
    
    switch (m_type) {
        case AnimalDialogType_1:
            
            strLbl1 = CCString::createWithFormat("%s",g_gameContent->Word["set_host_runner"].c_str());
            strLbl2 = CCString::createWithFormat("%s",g_gameContent->Word["store"].c_str());
            
            button_icon1 = UIHelper::getSprite(UIHelper::AnimalStatsLeaderIcon);
            button_icon1_press = UIHelper::getSprite(UIHelper::AnimalStatsLeaderIcon);
            
            button_icon2 = UIHelper::getSprite(UIHelper::AnimalStatsInBarnIcon);
            button_icon2_press = UIHelper::getSprite(UIHelper::AnimalStatsInBarnIcon);
            break;
            
        case AnimalDialogType_2:
            
            strLbl1 = CCString::createWithFormat("%s",g_gameContent->Word["sell"].c_str());
            strLbl2 = CCString::createWithFormat("%s",g_gameContent->Word["retrieve"].c_str());
            
            button_icon1 = UIHelper::getSprite(UIHelper::AnimalStatsSellIcon);
            button_icon1_press = UIHelper::getSprite(UIHelper::AnimalStatsSellIcon);

            button_icon2 = UIHelper::getSprite(UIHelper::AnimalStatsFromBarnIcon);
            button_icon2_press = UIHelper::getSprite(UIHelper::AnimalStatsFromBarnIcon);

            textColor = kTextColorRed;
            break;
            
        default:
            break;
    }
    
    float spreadPercentage = 0.23f;
    
    CCLabelBMFont *lblHostRunner = CCLabelHelper::createBMFont(strLbl1->getCString(), kFontSizeSmall, textColor);
    lblHostRunner->setPosition(ccp(bgButton->getPositionX()-(bgButton->getContentSize().width*spreadPercentage),
                                   bgButton->getPositionY()-(bgButton->getContentSize().height*0.6f) ));
    this->addChild(lblHostRunner);
    
    CCLabelBMFont *lblStore = CCLabelHelper::createBMFont(strLbl2->getCString(), kFontSizeSmall, kTextColorLightBrown);
    lblStore->setPosition(ccp(bgButton->getPositionX()+(bgButton->getContentSize().width*spreadPercentage),
                              lblHostRunner->getPositionY() ));
    this->addChild(lblStore);
    
    //Host Runner Button
    button_icon1_press->setScale(0.8f);
    button_icon1_press->setPosition(button_icon1->getContentSize()*0.1f);
    CCMenuItemSprite *_menuItem = CCMenuItemSprite::create(button_icon1, button_icon1_press, this, menu_selector(AnimalDialog::onButton));
    _menuItem->setTag(0);
    
    menuSetLeader = CCMenu::create(_menuItem, NULL);
    menuSetLeader->setPosition(ccp(bgButton->getPositionX()-(bgButton->getContentSize().width*spreadPercentage),
                                   bgButton->getPositionY()-(bgButton->getContentSize().width*0.07f)));
    this->addChild(menuSetLeader);

    //Store Button
    button_icon2_press->setScale(0.8f);
    button_icon2_press->setPosition(button_icon2->getContentSize()*0.1f);
    CCMenuItemSprite *_menuItem2 = CCMenuItemSprite::create(button_icon2, button_icon2_press, this, menu_selector(AnimalDialog::onButton));

    _menuItem2->setTag(1);
    _button_action2 = CCMenu::create(_menuItem2, NULL);
    _button_action2->setPosition(ccp(bgButton->getPositionX()+(bgButton->getContentSize().width*spreadPercentage),
                                     bgButton->getPositionY()-(bgButton->getContentSize().width*0.07f)));
    this->addChild(_button_action2, 2);
}

void AnimalDialog::onButton(CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    CCMenuItemSprite *menuItem = (CCMenuItemSprite*)sender_;
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    Building *_building = (Building*)m_entity;
    Pet *_pet = _building->pet;
    
    switch (m_type) {
            
        case AnimalDialogType_1: {
            
            if (typeid(*_building) != typeid(Building)) {
                return;
            }
            
            switch (menuItem->getTag()) {
                case 0: { //set host
                    
                    CREATE_CHAR_BUFFER(_buffer, 64);
                    
                    for (vector<Building*>::iterator it=g_playerState->Buildings.begin(); it!=g_playerState->Buildings.end(); ++it) {
                        if (Building *__building = (Building*)(*it)) {
                            if (__building->bVo.building_type == BUILDING_TYPE_HABITAT) {
                                __building->setPetAsHost(false);
                            }
                        }
                    }

                    if (_building->m_bIsHost) {
                        
                        snprintf(_buffer, _buffer_size, g_gameContent->Word["info_pet_is_already_host_animal"].c_str()
                                ,_pet->pVo.title.c_str());
                        
                        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
                        _building->setPetAsHost(true);
                        break;
                    } else {
                        snprintf(_buffer, _buffer_size, g_gameContent->Word["info_pet_is_now_host_animal"].c_str()
                                ,_pet->pVo.title.c_str());
                        
                        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
                        _building->setPetAsHost(true);
                    }
                    
                    BackendService::getInstance()->send(APIPetHost::create(_building->pet,
                                                                           this,
                                                                           AnimalDialog::onPetHosted));
                    this->m_modalController->closeAll();
                    
                }break;
                    
                case 1: { // store
                    
                    if (!m_sendToBarnButtonEnabled) return;
                    
                    Building *animalDorm = g_playerState->getBuilding(kBuildingAnimal, 0);
                    
                    if (_building->m_bIsHost) {
                        
                        CREATE_CHAR_BUFFER(_buffer, 64);
                        snprintf(_buffer, _buffer_size, g_gameContent->Word["error_cannot_store_host_animal"].c_str()
                                ,g_gameContent->Word["host_runner"].c_str()
                                ,animalDorm->bVo.title.c_str());
                        
                        g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, _buffer);
                        break;
                    }
                    
                    bool unfinish_animaldorm = false;
                    bool not_enough_space = false;
                    if (!animalDorm) {
                        not_enough_space = true;
                    }else {
                        if (animalDorm->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
                            unfinish_animaldorm = true;
                        }else {
                            int n_homeless_pet = g_playerState->getNHomelessPet();
                            int n_slot = animalDorm->bVo.param_1;
                            if (n_homeless_pet >= n_slot) {
                                not_enough_space = true;
                            }
                        }
                    }
                    
                    if (unfinish_animaldorm) {
                        if (_scene->modalController->getNStackView() == 1) {
                            InstantDialog *_dialog = InstantDialog::create(_scene->modalController, animalDorm);
                            _dialog->setPosition(_screenSize*0.5f);
                            _scene->modalController->pushView(_dialog);
                        }
                    }else {
                        if (not_enough_space) {
                            g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING,
                                                                     g_gameContent->Word["animal_barn_is_full_msg"].c_str());
                        }else {
                            BackendService::getInstance()->send(APIPetStore::create(_building->pet,
                                                                                    NULL,
                                                                                    EntityAction::
                                                                                    onSendToBarnCompleted));
                            _building->removePetFromBuilding();
                            _scene->modalController->closeAll();
                        }
                    }
                
                }break;
                    
                default:
                    break;
            }
            
        }break;
            
        case AnimalDialogType_2: {
            
            if (Pet *__pet = (Pet*)m_entity) {
                if (typeid(*__pet) == typeid(Pet)) {
                    _pet = __pet;
                    _building = NULL;
                }
            }
            
            switch (menuItem->getTag()) {
                    
                case 0: { //sell
                
                    if (!m_sellBtnEnabled) return;

                    SellAnimalDialog *_dialog = SellAnimalDialog::create(_scene->modalController, _pet);
                    _dialog->setPosition(_screenSize*0.5f);
                    _scene->modalController->pushView(_dialog);
                    
                }break;
                    
                case 1: { //_retrieve
                    
                    Building *_available_habitat = g_playerState->getAvailableHabitat();
                    
                    if (_available_habitat) {
                        
                        APIPetRetrieve* _request = APIPetRetrieve::create(_pet,
                                                                          _available_habitat,
                                                                          g_sceneManager,
                                                                          SceneManager::onPetRetrieved);
                        if (_request) {
                            BackendService::getInstance()->send(_request);
                        }else {
                            _available_habitat->markForPetRetrieve(NULL, NULL);
                        }
                        
                        this->m_modalController->closeAll();
                        _scene->isoMap->addPetToBuilding(_pet->ppVo.puid,
                                                         _available_habitat->pbVo.buid);
                        
                    }else {
                        
                        if (_scene->modalController->getNStackView() == 2) {
                            
                            CREATE_CHAR_BUFFER(_buffer, 64);
                            
                            Building *_building_habitat = g_playerState->getBuilding(kBuildingHabitat, 0);
                            GDBuildingVO vo_ = _building_habitat->bVo;
                            
                            Building *_building_hq = g_playerState->getBuilding(kBuildingHQ, 0);
                            
                            int currentHQLevel = _building_hq->pbVo.level;
                            int currentBuildingAmount = g_playerState->getNBuilding(vo_.type);
                            int currentMaxAmount = g_gameContent->getBuildingMax(vo_.type, currentHQLevel);
                            
                            if (currentBuildingAmount >= currentMaxAmount) {
                                
                                int maxHQLevel = g_gameContent->getBuildingMaxLevel(kBuildingHQ);
                                
                                for (int hqLevelInLoop = currentHQLevel; hqLevelInLoop <= maxHQLevel ; hqLevelInLoop++) {
                                    int newMaxAmount = g_gameContent->getBuildingMax(vo_.type, hqLevelInLoop);
                                    if (newMaxAmount > currentBuildingAmount) {
                                        
                                        snprintf(_buffer, _buffer_size, g_gameContent->Word["upgrade_mansion_to_build_more_building"].c_str(),
                                                _building_hq->bVo.title.c_str(),
                                                hqLevelInLoop,
                                                vo_.title.c_str());
                                        
                                        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, _buffer);
                                        return;
                                    }
                                }
                                //if survives the previous for loop
                                strncpy(_buffer, g_gameContent->Word["all_pens_occupied"].c_str(), _buffer_size);
                                
                                g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, _buffer);
                                return;
                            }
                            
                            NotEnoughSpaceDialog *_dialog = NotEnoughSpaceDialog::create(_scene->modalController, m_entity);
                            _dialog->setPosition(_screenSize*0.5f);
                            _scene->modalController->pushView(_dialog);
                        }
                    }
                    
                }break;
                    
                default:
                    break;
            }
            
        }break;
            
        default:
            break;
    }
    
    _scene->hudPanel->updateHUDTrack(_scene->isoMap->isTrackComplete(), true);
}

void AnimalDialog::onTopSpeed(cocos2d::CCObject *sender_)
{
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(g_gameContent->Word["straight_line_speed"].c_str(), kFontSizeMedium+5.0f, kTextColorDark);//"Straight line speed"
    
    CCSize _contentSize = CCNodeHelper::getContentSize(_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog *_dialog = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _label->setPosition(_dialog->getContentSize()*0.5f);
    _dialog->addChild(_label);
    
    _dialog->presentPopover((CCNode *)sender_, true);
    
};

void AnimalDialog::onCornerSpeed(cocos2d::CCObject *sender_) {
    
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(g_gameContent->Word["cornering_speed"].c_str(), kFontSizeMedium+5.0f, kTextColorDark);//"Cornering speed"
    
    CCSize _contentSize = CCNodeHelper::getContentSize(_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog *_dialog = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _label->setPosition(_dialog->getContentSize()*0.5f);
    _dialog->addChild(_label);
    
    _dialog->presentPopover((CCNode *)sender_, true);

};

void AnimalDialog::onJumping(cocos2d::CCObject *sender_) {
    
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(g_gameContent->Word["starting_energy"].c_str(), kFontSizeMedium+5.0f, kTextColorDark);//"Starting energy and energy regeneration"
    
    CCSize _contentSize = CCNodeHelper::getContentSize(_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog *_dialog = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _label->setPosition(_dialog->getContentSize()*0.5f);
    _dialog->addChild(_label);
    
    _dialog->presentPopover((CCNode *)sender_, true);
    
};

void AnimalDialog::onFavoriteType(cocos2d::CCObject *sender_){
    
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(g_gameContent->Word["favorite_track_type"].c_str(), kFontSizeMedium+5.0f, kTextColorDark);//"Favorite track type"
    
    CCSize _contentSize = CCNodeHelper::getContentSize(_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog *_dialog = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _label->setPosition(_dialog->getContentSize()*0.5f);
    _dialog->addChild(_label);
    
    _dialog->presentPopover((CCNode *)sender_, true);
    
};

#pragma mark = callback functions

void AnimalDialog::onPetHosted(void *sender, std::string resp_str) {
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille))
        return;
    
    PlayerState* _playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserRaceData(_root);
    Json_dispose(_root);
    
    _scene->isoMap->isTrackComplete();
    _scene->hudPanel->updateHUD();
    _scene->hudPanel->resourceBars[HUDPanelBarCoins]->addTargetAmount(_playerState->player.balance.gold, _playerState->getTotalCoinStorageCapacity(), AUDIO_NONE);
    _scene->hudPanel->resourceBars[HUDPanelBarCoins]->processTargetAmount();
    _scene->hudPanel->resourceBars[HUDPanelBarGems]->addTargetAmount(_playerState->player.balance.gem);
    _scene->hudPanel->resourceBars[HUDPanelBarGems]->processTargetAmount();
    
}

void AnimalDialog::onNote(cocos2d::CCObject *obj_) {
    
    CREATE_CHAR_BUFFER(_buffer, 512);
    if (m_type == AnimalDialogType_1) {
        if (Pet* _pet = ((Building*)m_entity)->pet) {
            snprintf(_buffer, _buffer_size, g_gameContent->Word["note_animal_dialog_1"].c_str(),
                    _pet->pVo.title.c_str(),
                    _pet->pVo.title.c_str(),
                    _pet->pVo.title.c_str(),
                    g_gameContent->getBuildingVO(kBuildingAnimal, 1).title.c_str(),
                    _pet->pVo.title.c_str());
        }
    }else {
        if (Pet* _pet = (Pet*)m_entity) {
            snprintf(_buffer, _buffer_size, g_gameContent->Word["note_animal_dialog_2"].c_str(),
                    _pet->pVo.title.c_str(),
                    g_gameContent->getBuildingVO(kBuildingAnimal, 1).title.c_str(),
                    _pet->pVo.title.c_str(),
                    g_gameContent->getBuildingVO(kBuildingHabitat, 1).title.c_str());
        }
    }
    
    CCLabelBMFont* _note_label = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, false, kTextColorDark, getContentSize().width*0.4f, kCCTextAlignmentCenter);
    
    CCSize _contentSize = CCNodeHelper::getContentSize(_note_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog* _hintPopOver = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _note_label->setPosition(_hintPopOver->getContentSize()*0.5f);
    _hintPopOver->addChild(_note_label);
    _hintPopOver->presentPopover(m_help, true);
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
}
