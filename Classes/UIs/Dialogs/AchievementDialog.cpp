//
//  AchievementDialog.cpp
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#include "AchievementDialog.h"
#include "../../Helpers/CCStringHelper.h"

#include "../../Extensions/Modal/CCModalController.h"
#include "../../Extensions/CCOpenURL/CCOpenURL.h"

#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CreateSpriteHelper.h"
#include "../../UIs/NumericProgressBar.h"

USING_NS_CC;

using namespace std;
#include <spine/Json.h>
using namespace cocos2d::extension;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIAppRequestCount.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../AppMacros.h"
#include "../../Settings.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/UIHelper.h"

#include "../../Helpers/SocialPlatform/SocialPlatform.h"
#include "../../Services/APIs/APIAchievementClaim.h"

#include "../Effects/CollectItemEffect.h"

#include "../../Helpers/FlurryCpp.h"
#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"

extern TargetDevice targetDevice;
//extern CCPoint screenFactor;
extern float sizeMultiplier;
extern float retinaMultiplier;

AchievementDialog* AchievementDialog::create(CCModalController* parentModalController_) {
    AchievementDialog *pRet = new AchievementDialog();
    pRet->init(parentModalController_, kDialogAchievementSize);
    pRet->autorelease();
    return pRet;
}

bool AchievementDialog::init(CCModalController* parentModalController_, CCSize size)
{
    if (!ScrollDialog::init(parentModalController_, size))
    {
        return false;
    }

    m_modalController->userObject = this;
    _modalController = m_modalController;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CCNikButton *gameCenterButton = CCNikButton::create(UIHelper::getSprite(UIHelper::AchievementsGameCenter), NULL);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCNikButton *gameCenterButton = CCNikButton::create(UIHelper::getSprite(UIHelper::GooglePlayIcon), NULL);
#endif

    gameCenterButton->setAnchorPoint(ccp(0.5f, 0.5f));
    gameCenterButton->setTarget(this, menu_selector(AchievementDialog::onAchievement), CCNikButtonEventTouchUpInside);
    gameCenterButton->setPosition(ccp( - m_background->getContentSize().width*0.5f + gameCenterButton->getContentSize().width*0.92, m_background->getContentSize().height*0.5f - gameCenterButton->getContentSize().height*0.6));
    this->addChild(gameCenterButton, 5);

    addTitle(g_gameContent->Word["achievement_dialog_title"].c_str());

//    canTouch = false;
    
    m_number_of_apprequest = 0;
//    g_gameBackend->apprequest_count("facebook", this, AchievementDialog::onAppRequestCountReceived);
    
    createScrollContent(ACHIEVEMENT_DIALOG_SETTINGS);

    updateContent();
    //animation waiting
//    showWaiting();
    
    //update Achievement Gamecenter
    updateAchievementsGameCenter();
    
    return true;
}

void AchievementDialog::showWaiting()
{
    fxB1 = CCSprite::create("common/vfx/fx_gacha_to_barn_b.png");
    fxB1->setScale(sizeMultiplier*0.5f);
    fxB1->setPosition(m_clippingLayer->getContentSize()*0.5f);
    m_clippingLayer->addChild(fxB1);
    
    fxB2 = CCSprite::create("common/vfx/fx_gacha_to_barn_b.png");
    fxB2->setScale(sizeMultiplier*0.5f);
    fxB2->setPosition(m_clippingLayer->getContentSize()*0.5f);
    m_clippingLayer->addChild(fxB2);
    
    fxA = CCSprite::create("common/vfx/fx_gacha_to_barn_a.png");
    fxA->setScale(sizeMultiplier*0.5f);
    fxA->setPosition(m_clippingLayer->getContentSize()*0.5f);
    m_clippingLayer->addChild(fxA);
    
    fxB1->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, 360)));
    fxB2->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, -360)));
    fxA->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, 360)));
}

void AchievementDialog::clearWaiting()
{
    if(fxB2) {
        fxB2->stopAllActions();
        fxB2->setVisible(false);
    }
    
    if(fxB1) {
        fxB1->stopAllActions();
        fxB1->setVisible(false);
    }
    
    if(fxA) {
        fxA->stopAllActions();
        fxA->setVisible(false);
    }

}

void AchievementDialog::updateAchievementsGameCenter()
{
    //loop update all achievements
    for (vector<GDAchievementsVO>::iterator it=g_gameContent->Achievement.begin(); it!=g_gameContent->Achievement.end(); ++it) {
        GDAchievementsVO _avo = (GDAchievementsVO)(*it);
    
        unsigned int currentNumProgress = AchievementDialog::checkCurrentProgress(_avo);
        int percentAchievement = (int)floor(currentNumProgress*100/_avo.param_1);
        
        SocialPlatform::getInstance()->postAchievement(_avo.type.c_str(), percentAchievement);
    }
}

void AchievementDialog::onExit()
{
    CCModalView::onExit();
}

unsigned int AchievementDialog::checkCurrentProgress(GDAchievementsVO _avo)
{
    PlayerState *_playerState = PlayerState::getPlayer();
    
    //check group type
    if(_avo.group.compare("social")==0) {
        return MAX(0, CCUserDefault::sharedUserDefault()->getIntegerForKey("number_of_request"));
    }
    else if(_avo.group.compare("fan")==0) {
        return MAX(0, _playerState->player.balance.fan);
    }
    else if(_avo.group.compare("victory")==0) {
        return MAX(0, _playerState->player.info.total_wins);
    }
    else if(_avo.group.compare("streak")==0) {
        return MAX(0, _playerState->player.info.highest_win_streak);
    }
    else if(_avo.group.compare("pet_maximum")==0) {
        return MAX(0, _playerState->getNPet());
    }
    else if(_avo.group.compare("cuttree")==0) {
        CCLog_("cuttree:%d",MIN(0, (kMaxTrees-_playerState->getNTree())));
        return MAX(0, (kMaxTrees-_playerState->getNTree()));
    }
    else if(_avo.group.compare("facebook")==0) {
        if(_avo.type.compare("facebook_1")==0) {
            //login facebook in game
            return g_sceneManager->facebook->isLoggedIn();
        }
    }
    else if(_avo.group.compare("map_open")==0) {
        
        Building* _building = _playerState->getBuilding(kBuildingGarage, 0);
        if (_building) {
            
            //for first level, also check if contruction is completed only, otherwise shouldn't be 1
            if (_building->pbVo.level == 1 &&
                _building->pbVo.status != BUILDING_STATUS_UPGRADE &&
                _building->pbVo.status != BUILDING_STATUS_DELIVERED)
            {
                return 0;
            }
            
            return _building->pbVo.level;
        }
    }
    else if(_avo.group.compare("bank")==0) {
        
        Building* _building = _playerState->getBuilding(kBuildingBank, 0);
        if (_building) {
            
            //for first level, also check if contruction is completed only, otherwise shouldn't be 1
            if (_building->pbVo.level == 1 &&
                _building->pbVo.status != BUILDING_STATUS_UPGRADE &&
                _building->pbVo.status != BUILDING_STATUS_DELIVERED)
            {
                return 0;
            }
            
            return _building->pbVo.level;
        }
    }
    else if(_avo.group.compare("silo")==0) {
        
        Building* _building = _playerState->getBuilding(kBuildingFoodStorage, 0);
        if (_building) {
            
            //for first level, also check if contruction is completed only, otherwise shouldn't be 1
            if (_building->pbVo.level == 1 &&
                _building->pbVo.status != BUILDING_STATUS_UPGRADE &&
                _building->pbVo.status != BUILDING_STATUS_DELIVERED)
            {
                return 0;
            }
            
            return _building->pbVo.level;
        }
    }
    else if(_avo.group.compare("mansion")==0) {
        
        Building* _building = _playerState->getBuilding(kBuildingHQ, 0);
        if (_building) {
            return _building->pbVo.level;
        }
    }
    else if(_avo.group.compare("library")==0) {
        
        Building* _building = _playerState->getBuilding(kBuildingLibrary, 0);
        if (_building) {
            
            //for first level, also check if contruction is completed only, otherwise shouldn't be 1
            if (_building->pbVo.level == 1 &&
                _building->pbVo.status != BUILDING_STATUS_UPGRADE &&
                _building->pbVo.status != BUILDING_STATUS_DELIVERED)
            {
                return 0;
            }
            
            return _building->pbVo.level;
        }
    }
    else if(_avo.group.compare("rare_pet")==0) {
        
        int targetRarityStars = RARE_PET_3_STARS;
        
        if(_avo.type.compare("rare_pet_1")==0) {
            
            targetRarityStars = RARE_PET_1_STARS;
        } else if(_avo.type.compare("rare_pet_2")==0) {
            
            targetRarityStars = RARE_PET_2_STARS;
        } else if(_avo.type.compare("rare_pet_3")==0) {
            
            targetRarityStars = RARE_PET_3_STARS;
        }
        
        int highestRarityStars = 0;
        
        for (vector<Pet*>::iterator it=_playerState->Pets.begin(); it!=_playerState->Pets.end(); ++it) {
            if (Pet *_pet = (Pet*)(*it)) {
                
                highestRarityStars = MAX(highestRarityStars, _pet->pVo.rarity_stars);
            }
        }
        
        return (highestRarityStars >= targetRarityStars);
    }
    
    CCAssert(true, "Achievement mismatch. Logic should not reach this point");
    
    return 0;
}

unsigned int AchievementDialog::checkAchievementPass()
{
    PlayerState *_playerState = PlayerState::getPlayer();
    int count = 0;
    
    for (vector<GDAchievementsVO>::iterator it=g_gameContent->Achievement.begin(); it!=g_gameContent->Achievement.end(); ++it) {
        GDAchievementsVO _avo = (GDAchievementsVO)(*it);
        
        int currentNumProgress = AchievementDialog::checkCurrentProgress(_avo);
        
        if(currentNumProgress >= _avo.param_1 &&
           !_playerState->Checklist[_avo.type.c_str()]) {
            CCLog_("_avo.type.c_str():%s", _avo.type.c_str());
            CCLog_("g_playerState->Checklist[_avo.type.c_str()]:%d", _playerState->Checklist[_avo.type.c_str()]);
            CCLog_("currentNumProgress:%d", currentNumProgress);
            CCLog_("_avo.param_1:%d", _avo.param_1);
            count++;
        }
    }
    
    CCLog_("count:%d", count);
    return count;
}

void AchievementDialog::updateContent()
{
    PlayerState *_playerState = PlayerState::getPlayer();
    CCScale9Sprite *_frameAchievement;
    std::string backup_group = "";
 
    int countAchievement = 0;
    int maxAchievement = kMaxAchievement;
    int iii = 0;

    entryToScrollTo = NULL;

    for (vector<GDAchievementsVO>::iterator it=g_gameContent->Achievement.begin(); it!=g_gameContent->Achievement.end(); ++it) {
        
        GDAchievementsVO _avo = (GDAchievementsVO)(*it);
        
//        CCLog_("i:%d  ------",iii);
//        CCLog_("_avo.type:%s",_avo.type.c_str());
//        CCLog_("_avo.group:%s",_avo.group.c_str());
//        CCLog_("_avo.gems_reward:%d",_avo.gems_reward);
//        CCLog_("_avo.param_1:%d",_avo.param_1);
//        CCLog_("_avo.param_2:%d",_avo.param_2);
//        CCLog_("_avo.title:%s",_avo.title.c_str());
//        CCLog_("_avo.description:%s",_avo.description.c_str());
        string achievement_rank = _avo.type.substr(_avo.type.length()-1,1);
        
        CCLog("%s=%d", _avo.type.c_str(), (int)_playerState->Checklist[_avo.type.c_str()]);
        //*** disable achievement with set achievement param_2 = 1 in game data
        if(backup_group == _avo.group || _avo.param_2 == 1) {

//            CCLog_("Before Achievement %s not pass",_avo.type.c_str());
            
        }
        //ยังไม่เคยผ่าน achievement นี้
        else if(!_playerState->Checklist[_avo.type.c_str()]) {
            
            backup_group = _avo.group;
            
            if(countAchievement<maxAchievement) {
                countAchievement++;
                
                _frameAchievement = UIHelper::getScalableSprite(UIHelper::AchievementBG);
                _frameAchievement->setContentSize(kFrameAchievementSize*sizeMultiplier);
                _frameAchievement->setAnchorPoint(ccp(0.5, 0));
                _frameAchievement->setPosition(ccp(m_clippingLayer->getContentSize().width*0.5 - kLeftOffset*retinaMultiplier,
                                                   kUpperOffset*retinaMultiplier + iii*_frameAchievement->getContentSize().height*1.1));
                m_scrollView->addChild(_frameAchievement);
                _frameAchievement->setTag(iii);
                
                if(_avo.group.compare("social")==0) {
                    if(returnNumInviteReady) {
                        setDescriptionTxt(_avo, iii);
                        updateAchievement(_avo, iii, _frameAchievement);
                    }
                    else {
                        BackendService::getInstance()->send(APIAppRequestCount::create("facebook",
                                                                                       this,
                                                                                       AchievementDialog::onAppRequestCountReceived));
                        indexFrameInvite = iii;
                        _inviteFacebook_vo = _avo;
                        setWaiting(_avo, iii);
                    }
                } else {
                    setDescriptionTxt(_avo, iii);
                    updateAchievement(_avo, iii, _frameAchievement);
                }
                
                iii++;
            }
            
        } else if(StringHelper::toInt(achievement_rank.c_str()) >= 3) {
            
            if(countAchievement<maxAchievement) {
                countAchievement++;
                
                _frameAchievement = UIHelper::getScalableSprite(UIHelper::AchievementBG);
                _frameAchievement->setContentSize(kFrameAchievementSize*sizeMultiplier);
                _frameAchievement->setAnchorPoint(ccp(0.5, 0));
                _frameAchievement->setPosition(ccp(m_clippingLayer->getContentSize().width*0.5 - kLeftOffset*retinaMultiplier,
                                                   kUpperOffset*retinaMultiplier + iii*_frameAchievement->getContentSize().height*1.1 ));
                m_scrollView->addChild(_frameAchievement);
                _frameAchievement->setTag(iii);
                
                setDescriptionTxt(_avo, iii);
                setCompleted(_avo, iii);
                
                iii++;
            }
        } else if(_avo.type.compare("facebook_1")==0) {
            if(countAchievement<maxAchievement) {
                countAchievement++;
                
                _frameAchievement = UIHelper::getScalableSprite(UIHelper::AchievementBG);
                _frameAchievement->setContentSize(kFrameAchievementSize*sizeMultiplier);
                _frameAchievement->setAnchorPoint(ccp(0.5, 0));
                _frameAchievement->setPosition(ccp(m_clippingLayer->getContentSize().width*0.5 - kLeftOffset*retinaMultiplier,
                                                   kUpperOffset*retinaMultiplier + iii*_frameAchievement->getContentSize().height*1.1 ));
                m_scrollView->addChild(_frameAchievement);
                _frameAchievement->setTag(iii);
                
                setDescriptionTxt(_avo, iii);
                setCompleted(_avo, iii);
                
                iii++;
            }
        }
    }
    
    int frameCount = iii;
    
//    CCLog_("Achievement Count = %d",i);

    if(frameCount>0) {
        
        float lastHeight = _frameAchievement->getPositionY();
        float sizeHeight = MAX(_frameAchievement->getPositionY()
                               + _frameAchievement->getContentSize().height
                               + kUpperOffset*retinaMultiplier,
                               m_scrollView->getContentSize().height);
        
        //swap frame from bottom to top
        if(frameCount<5) {
            for (int i = 0; i < frameCount; i++) {
                CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(i);
                if(tempFrame) {
                    tempFrame->setPosition(ccp(tempFrame->getPositionX(),
                                               sizeHeight - (i+1)*tempFrame->getContentSize().height*1.1));
                }
            }
        }
        else {
            for (int i = 0; i < frameCount; i++) {
                CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(i);
                if(tempFrame) {
                    tempFrame->setPosition(ccp(tempFrame->getPositionX(),
                                               lastHeight-i*tempFrame->getContentSize().height*1.1));
                }
            }
        }
        
        m_scrollView->setContentView(CCSizeMake(m_scrollView->getContentSize().width, sizeHeight));
        m_scrollView->setContentOffset(ccp(0, m_scrollView->getContentView().height - m_scrollView->getContentSize().height));
    
    }
    
    canTouch = true;
    
    //remove Waiting Sprite
    clearWaiting();

    if(entryToScrollTo)
        m_scrollView->zoomOnPosition(entryToScrollTo->getPosition(), 1.0f, 1.0f);
}


void AchievementDialog::setWaiting(GDAchievementsVO _avo, int tag)
{
    setDescriptionTxt(_avo, tag);
    
    CREATE_CHAR_BUFFER(_buffer, 256);
    
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(tag);
    if(tempFrame) {
        
        CCLabelBMFont *_rewardTxtAchievement = CCLabelHelper::createBMFont("Reward", kFontSizeLarge, true);
        _rewardTxtAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.825,tempFrame->getContentSize().height*0.74));
        _rewardTxtAchievement->setAnchorPoint(ccp(1,0.5));
        tempFrame->addChild(_rewardTxtAchievement);
        _rewardTxtAchievement->setTag(99);
        
        CCSprite *_gemPicAchievement = UIHelper::getSprite(UIHelper::AchievementsRewardGem);
        _gemPicAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.87,tempFrame->getContentSize().height*0.72));
        tempFrame->addChild(_gemPicAchievement);
        _gemPicAchievement->setTag(99);
        
        snprintf(_buffer, _buffer_size, "%u", _avo.gems_reward);
        CCLabelBMFont *_numGemAchievement = CCLabelHelper::createBMFont(_buffer, kFontSizeLarge, true);
        _numGemAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.915,tempFrame->getContentSize().height*0.72));
        _numGemAchievement->setAnchorPoint(ccp(0,0.5));
        tempFrame->addChild(_numGemAchievement);
        _numGemAchievement->setTag(99);
        
        NumericProgressBar *_progressAchievement = NumericProgressBar::create();
        _progressAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.83,tempFrame->getContentSize().height*0.4));
        _progressAchievement->setProgressBarEnable(true);
        tempFrame->addChild(_progressAchievement);
        _progressAchievement->setTag(99);
        
        _progressAchievement->setWaitProgress();
    }
}

void AchievementDialog::updateAchievement(GDAchievementsVO _avo, int tag, CCScale9Sprite* entry)
{
    string achievement_rank = _avo.type.substr(_avo.type.length()-1,1);
    
    unsigned int currentNumProgress = AchievementDialog::checkCurrentProgress(_avo);
//    CCLog_("Current Progress = %d",currentNumProgress);
    
    bool claimMode = false;

    if(currentNumProgress>=_avo.param_1) {
        claimMode = true;
//        currentNumProgress = _avo.param_1;
    }
    
    //---- Achievement not completed ---
    if(claimMode) {
        if(entryToScrollTo == NULL && entry != NULL)
            entryToScrollTo = entry;
        setClaim(_avo, tag);
    }
    else {
        setProgress(_avo, tag);
    }
}

void AchievementDialog::createStars(CCNode* parent, int status)
{
    for (auto& star : m_stars)
    {
        CC_SAFE_DELETE(star);
    }
    m_stars.clear();

    const float xPos[] = {106.0f*sizeMultiplier, 208.0f*sizeMultiplier, 300.0f*sizeMultiplier};
    const float yPos[] = {102.0f*sizeMultiplier, 140.0f*sizeMultiplier, 102.0f*sizeMultiplier};
    const float rot[]  = {-15.0f, 0.0f, 15.0f};
    const int zOrder[]  = {1, 2, 1};

    for (int i=0 ;i<3; i++)
    {
        // 0 ... 3 empty stars
        // 1 ... 1 full, 2 empty
        // 2 ... 2 full, 1 empty
        // 3 ... 3 full stars

        UIHelper::UISprites sprite = UIHelper::AchievementsStarEmpty;

        if (status > i)
        {
            sprite = UIHelper::AchievementsStar;
        }

        auto star = UIHelper::getSprite(sprite);
        star->setPosition(ccp(xPos[i], yPos[i]));
        star->setRotation(rot[i]);
        parent->addChild(star, zOrder[i]);
    }
}

void AchievementDialog::setDescriptionTxt(GDAchievementsVO _avo, int tag)
{
    CREATE_CHAR_BUFFER(_buffer, 256);
    CREATE_CHAR_BUFFER(_buffer2, 256);
    
    string achievement_rank = _avo.type.substr(_avo.type.length()-1,1);
    CCLog_("Achievement %s", achievement_rank.c_str());
    
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(tag);
    if(tempFrame) {
        
        createStars(tempFrame, StringHelper::toInt(achievement_rank.c_str())-1);

        snprintf(_buffer, _buffer_size, "%s", _avo.title.c_str());
        CCLabelBMFont *_titleAchievement = CCLabelHelper::createBMFont(_buffer, kFontSizeLarge, true);
        _titleAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.3,tempFrame->getContentSize().height*0.55));
        _titleAchievement->setAnchorPoint(ccp(0,0));
        tempFrame->addChild(_titleAchievement);
        
        snprintf(_buffer, _buffer_size, "%s", _avo.type.c_str());
        CCLabelBMFont *_tempStr = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, false);
        _tempStr->setVisible(false);
        tempFrame->addChild(_tempStr);
        _tempStr->setTag(10000+tag);
        
        snprintf(_buffer, _buffer_size, "%s", _avo.description.c_str());
        snprintf(_buffer2, _buffer2_size, _buffer, (int)_avo.param_1);
        
        CCLabelBMFont* _detailAchievement = CCLabelHelper::createBMFont( _buffer2, kFontSizeMedium , false, kTextColorDarkBrown);
        _detailAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.3,tempFrame->getContentSize().height*0.35));
        _detailAchievement->setWidth(tempFrame->getContentSize().width*0.32);
        _detailAchievement->setAlignment(kCCTextAlignmentLeft);
        _detailAchievement->setAnchorPoint(ccp(0,0.5));
        tempFrame->addChild(_detailAchievement);
        
    }
}

void AchievementDialog::setProgress(GDAchievementsVO _avo, int tag)
{
    CREATE_CHAR_BUFFER(_buffer, 256);
    
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(tag);
    if(tempFrame) {
        
        unsigned int currentNumProgress = AchievementDialog::checkCurrentProgress(_avo);
        
        CCLabelBMFont *_rewardTxtAchievement = CCLabelHelper::createBMFont("Reward", kFontSizeLarge, true);
        _rewardTxtAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.825,tempFrame->getContentSize().height*0.74));
        _rewardTxtAchievement->setAnchorPoint(ccp(1,0.5));
        tempFrame->addChild(_rewardTxtAchievement);
        
        CCSprite *_gemPicAchievement = UIHelper::getSprite(UIHelper::AchievementsRewardGem);
        _gemPicAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.87,tempFrame->getContentSize().height*0.72));
        tempFrame->addChild(_gemPicAchievement);
        
        snprintf(_buffer, _buffer_size, "%u", _avo.gems_reward);
        CCLabelBMFont *_numGemAchievement = CCLabelHelper::createBMFont(_buffer, kFontSizeLarge, true);
        _numGemAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.915,tempFrame->getContentSize().height*0.72));
        _numGemAchievement->setAnchorPoint(ccp(0,0.5));
        tempFrame->addChild(_numGemAchievement);
        
        NumericProgressBar *_progressAchievement = NumericProgressBar::create();
        _progressAchievement->setPosition(ccp(tempFrame->getContentSize().width*0.83,tempFrame->getContentSize().height*0.35));
        _progressAchievement->setProgressBarEnable(true);
        tempFrame->addChild(_progressAchievement);
        _progressAchievement->setTextPosition(NumericProgressBar::LeftBottom);

        _progressAchievement->setProgressNumeric(currentNumProgress,_avo.param_1);
    }
}

void AchievementDialog::setClaim(GDAchievementsVO _avo, int tag)
{
    CREATE_CHAR_BUFFER(_buffer, 256);
    
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(tag);
    if(tempFrame) {
        CCNikScrollViewButton *_Button = CCNikScrollViewButton::create(UIHelper::getSprite(UIHelper::AchievementsClaimButton), NULL);
        _Button->setTarget(this, menu_selector(AchievementDialog::touchClaimButton), CCNikScrollViewButtonStateTouchUp);
        _Button->setPosition(ccp(tempFrame->getContentSize().width*0.83,
                                 tempFrame->getContentSize().height*0.5));
        _Button->setAnchorPoint(ccp(0.5,0.5));
        tempFrame->addChild(_Button);
        _Button->setTag(tag+100000);
        _Button->setDelegate(m_scrollView);
        
        CCLabelBMFont *_rewardTxtAchievement = CCLabelHelper::createBMFont("Claim", kFontSizeLarge, true);
        _rewardTxtAchievement->setPosition(ccp(_Button->getContentSize().width*0.46,_Button->getContentSize().height*0.57));
        _rewardTxtAchievement->setAnchorPoint(ccp(1,0.5));
        _Button->addChild(_rewardTxtAchievement);
        
        CCSprite *_gemPicAchievement = UIHelper::getSprite(UIHelper::AchievementsClaimGem);
        _gemPicAchievement->setPosition(ccp(_Button->getContentSize().width*0.6,_Button->getContentSize().height*0.52));
        //_gemPicAchievement->setScale(1.0/_Button->getScale());
        _gemPicAchievement->setTag(10000);
        _Button->addChild(_gemPicAchievement);
        
        snprintf(_buffer, _buffer_size, "%u", _avo.gems_reward);
        CCLabelBMFont *_numGemAchievement = CCLabelHelper::createBMFont(_buffer, kFontSizeLarge, true);
        _numGemAchievement->setPosition(ccp(_Button->getContentSize().width*0.74,_Button->getContentSize().height*0.55));
        _numGemAchievement->setAnchorPoint(ccp(0,0.5));
        _Button->addChild(_numGemAchievement);
    }
}

void AchievementDialog::setCompleted(GDAchievementsVO _avo, int tag)
{
//    CCLog_("set Complete");
    
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(tag);
    if(tempFrame) {
        createStars(tempFrame, 4);
        
        CCNikScrollViewButton *_Button = (CCNikScrollViewButton*)tempFrame->getChildByTag(tag+100000);
        if(_Button) _Button->removeFromParent();
        
        auto ribbon = UIHelper::getScalableSprite(UIHelper::RibbonSmall);
        ribbon->setContentSize(CCSizeMake(ribbon->getContentSize().width,
                                          ribbon->getContentSize().height));
        ribbon->setPosition(ccp(tempFrame->getContentSize().width*0.84f,tempFrame->getContentSize().height*0.5f));
        
        tempFrame->addChild(ribbon);

        CCLabelBMFont *_completedAchievement = CCLabelHelper::createBMFont("Completed", kFontSizeLarge-5.0f, true);
        _completedAchievement->setPosition(ccp(ribbon->getPositionX(), ribbon->getPositionY()+11*sizeMultiplier));
        tempFrame->addChild(_completedAchievement);
    }
}

///----------------------------------------------------

void AchievementDialog::onAchievement(CCObject *sender_) {
    
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    bool success = SocialPlatform::getInstance()->openAchievement();
    
    if (!success) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, "Please login to GameCenter!");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, "Please login to Google Play!");
#endif
    }
    
}

void AchievementDialog::touchClaimButton(cocos2d::CCObject *sender) {
    
    if(!canTouch) return;
        
    CCSprite *target = (CCSprite*)sender;
    if(!target) return;
    
    int tag = target->getTag()-100000;
    CCLog_("Touch Claim at tag:%d", tag);
    posTouch = target->getParent()->convertToWorldSpace(target->getPosition());
    
    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(tag);
    if(!tempFrame) return;
        
    CCLabelBMFont *tempLabel = (CCLabelBMFont*)tempFrame->getChildByTag(tag + 10000);
    if(!tempLabel) return;
        
    for (vector<GDAchievementsVO>::iterator it=g_gameContent->Achievement.begin();
         it!=g_gameContent->Achievement.end(); ++it) {
        
        GDAchievementsVO _avo = (GDAchievementsVO)(*it);
        if(_avo.type.compare(tempLabel->getString()) !=0 ) continue;
        
        g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
        g_gameAudio->playEffect(AUDIO_FINISH_NOW);
        
        canTouch = false;
        lastAchievement = _avo.type;
        lastTag = tag;
        
        m_scrollView->setTouchEnabled(false);
        m_scrollView->setScrollEnabled(false);
        
        string achievement_rank = _avo.type.substr(_avo.type.length()-1,1);
        id_achievement = StringHelper::toInt(achievement_rank.c_str());
        
        // preprcess for achievement claim
        if (!PlayerState::getInstance()->prePrecessAchievementClaim(_avo)) break;
            
        CCDictionary *params = CCDictionary::create();
        params->setObject(CCString::createWithFormat("%d",_avo.gems_reward), "gem");
        FlurryCpp::logEvent(EVENT_GEM_EARN_ACHIEVEMENT, params);
        
        CSJson::Value eventParameters;
        eventParameters["gem"] = _avo.gems_reward;
        AnalyticsClient::getInstance()->logEvent(EVENT_GEM_EARN_ACHIEVEMENT, eventParameters);
        
        //send achievement type to server
        g_sceneManager->showLoading();
        BackendService::getInstance()->send(APIAchievementClaim::create(_avo.type.c_str(),
                                                                        this,
                                                                        AchievementDialog::onAchievementClaimed));
        
        break;
        
    }
    
}

void AchievementDialog::clearDetail(int tag) {

    CCScale9Sprite *tempFrame = (CCScale9Sprite*)m_scrollView->contentNode->getChildByTag(tag);
    if(!tempFrame) return;
    
    while(tempFrame->getChildrenCount()>1) {
        CCObject* child = tempFrame->getChildren()->objectAtIndex(tempFrame->getChildrenCount()-1);
        CCSprite *target = (CCSprite*)child;
        if(!target || target->getTag()==tag) continue;
        target->stopAllActions();
        target->removeFromParent();
    }
    
}


void AchievementDialog::updateClaim() {
    
    for (vector<GDAchievementsVO>::iterator it=g_gameContent->Achievement.begin();
         it!=g_gameContent->Achievement.end(); ++it) {
        
        GDAchievementsVO _avo = (GDAchievementsVO)(*it);
        
        if(_avo.type.compare(lastAchievement) != 0) continue;

        if(_avo.type.compare("facebook_1")==0) { //facebook only one achievement
            setCompleted(_avo, lastTag);
        }
        else if(id_achievement<3) {
            clearDetail(lastTag);
            
            ++it;
            GDAchievementsVO _bvo = (GDAchievementsVO)(*it);
            
            setDescriptionTxt(_bvo, lastTag);
            updateAchievement(_bvo, lastTag);
            
        }
        else {
            setCompleted(_avo, lastTag);
        }

    }
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if(_scene) {
        _scene->hudPanel->updateBadgeAchievement();
    }
    
    canTouch = true;
    m_scrollView->setTouchEnabled(true);
    m_scrollView->setScrollEnabled(true);

}

void AchievementDialog::updateInviteAchievement(void *sender) {
    
    AchievementDialog* _this = (AchievementDialog*)sender;
    if (!_this) return;
    
    clearDetail(indexFrameInvite);
    setDescriptionTxt(_inviteFacebook_vo, indexFrameInvite);
    updateAchievement(_inviteFacebook_vo, indexFrameInvite);
    
}

void AchievementDialog::onAppRequestCountReceived(void *sender, string resp_json) {
    
    AchievementDialog* _this = (AchievementDialog*)sender;
    if (!_this || typeid(*_this) != typeid(AchievementDialog)) return;

    //JsonAllocator allocator;
    Json *_root = Json_create(resp_json.c_str());
    if (!_root) {
        return;
    }
    
    if (Json_getItem(_root, "number_of_request")) {
        _this->m_number_of_apprequest = Json_getInt(_root, "number_of_request", 0);
        CCUserDefault::sharedUserDefault()->setIntegerForKey("number_of_request",
                                                             _this->m_number_of_apprequest);
    }
    
    Json_dispose(_root);
    
    _this->returnNumInviteReady = true;
    _this->updateInviteAchievement(sender);
    
}

#pragma mark - CCNikScrollViewDelegate

void AchievementDialog::scrollViewWillBeginDragging(CCNikScrollView *view)
{
    
}

#pragma mark - Callback from Backend

void AchievementDialog::onAchievementClaimed(void *sender_, std::string resp_json_str_) {

    g_sceneManager->hideLoading();
    
    //JsonAllocator allocator;
    Json* root = Json_create(resp_json_str_.c_str());
    PlayerState::getInstance()->parseUserBalance(root);
    PlayerState::getInstance()->parseChecklistData(root);
    Json_dispose(root);
    
    AchievementDialog  *_this = (AchievementDialog*)sender_;
    if (!_this) return;
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille))
        return;
    HUDPanel *hudPanel = _scene->hudPanel;
    CCSprite *gemIconSprite = hudPanel->resourceBars[HUDPanelBarGems]->getIconSprite();
    CCPoint endPoint = gemIconSprite->convertToWorldSpaceAR(CCPointZero);
    CollectItemEffect::addCollectItemEffect(CollectItemEffect::create(UIHelper::getSpritePath(UIHelper::HUDGem),
                                                                      kCollectItemEffectDefaultSprite,
                                                                      _this->posTouch,
                                                                      endPoint,
                                                                      _this,
                                                                      AchievementDialog::onCollectGemsEffectFinished), _scene);
    
}

#pragma mark - callback from animation

void AchievementDialog::onCollectGemsEffectFinished(void *sender) {
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    SceneVille* _scene = (SceneVille*)g_currentScene;
    
    _scene->hudPanel->updateAllResourceBars();
    
    if (!_scene->hudPanel->achievementDialog) return;
    _scene->hudPanel->achievementDialog->updateClaim();
    
}

