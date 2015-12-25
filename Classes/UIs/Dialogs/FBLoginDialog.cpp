//
//  FBLoginDialog.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 9/4/2557 BE.
//
//

#include "FBLoginDialog.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include "../../AppMacros.h"
#include "../../Settings.h"

#include "../../Extensions/Modal/CCModalController.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/CCSpriteHelper.h"

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Entities/PlayerState.h"
#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIAchievementClaim.h"

#include "../Effects/CollectItemEffect.h"

extern float sizeMultiplier;
extern TargetDevice targetDevice;

#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

#define kFBLoginDialog_x_pos                (113.0f*sizeMultiplier)
#define kFBLoginDialog_x_pos_CLAIM_DIALOG   (243.0f*sizeMultiplier)

FBLoginDialog* FBLoginDialog::create(CCModalController *parentModalController, bool lastChance) {
    
    FBLoginDialog *pRet = new FBLoginDialog();
    pRet->init(parentModalController, lastChance, cocos2d::CCSize(1536.0f, 800.0f));
    pRet->autorelease();
    return pRet;
    
}

bool FBLoginDialog::init(CCModalController *parentModalController, bool lastChance, CCSize size) {
    
    if (!ModalDialog::init(parentModalController, size)) {
        return false;
    }
    
    CCSprite *m_merchantSay = CCSprite::create("common/character/merchant_say.png");
    m_merchantSay->setScale(sizeMultiplier);
    m_merchantSay->setAnchorPoint(ccp(0.5f, 0.0f));
    m_merchantSay->setPosition(ccp(-448.0f * sizeMultiplier,
                                   m_background->getContentSize().height*0.5f*-1.0f
                                   +75.0f*sizeMultiplier));
    this->addChild(m_merchantSay);
    
    m_nodesGroup = CCNode::create();
    this->addChild(m_nodesGroup);

    int _gemReward = g_gameContent->GameConfig["FACEBOOK_LOGIN_DIALOG_GEM_REWARD"];
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    if (lastChance) {
        snprintf(_buffer, _buffer_size, g_gameContent->Word["fblogindialog_desc_lastchance"].c_str(), _gemReward);
    } else {
        snprintf(_buffer, _buffer_size, g_gameContent->Word["fblogindialog_desc"].c_str(), _gemReward);
    }
    
    CCLabelBMFont *_desc = CCLabelHelper::createBMFont(_buffer,
                                                       kFontSizeMedium,
                                                       kTextColorDark);
    _desc->setAnchorPoint(ccp(0.5f, 0.5f));
    _desc->setAlignment(kCCTextAlignmentCenter);
    _desc->setWidth(500.0f*sizeMultiplier);
    _desc->setPosition(kFBLoginDialog_x_pos,
                       163.0f*sizeMultiplier);
    m_nodesGroup->addChild(_desc);
    
    CCSprite *connectFacebook_sprite =
    UIHelper::getSprite(UIHelper::UISprites(UIHelper::FacebookConnectionFacebookButton));
    
    CCMenu *connectFacebook =
    CCMenuHelper::createMenu(connectFacebook_sprite,
                             this,
                             menu_selector(FBLoginDialog::onConnectFacebook));
    connectFacebook->setAnchorPoint(ccp(0.5f, 0.5f));
    connectFacebook->setPosition(ccp(kFBLoginDialog_x_pos,
                                     -10.0f*sizeMultiplier));
    m_nodesGroup->addChild(connectFacebook);
    
    CCLabelBMFont *connectFacebook_label =
    CCLabelHelper::createBMFont(g_gameContent->Word["fblogindialog_connect"].c_str(),
                                kFontSizeLarge-5.0f,
                                true);
    connectFacebook_label->setPosition(ccp(connectFacebook->getContentSize().width*0.62f,
                                           connectFacebook->getContentSize().height*0.55f));
    connectFacebook_sprite->addChild(connectFacebook_label);
    
    CCSprite *noThanks_sprite =
    UIHelper::getSprite(UIHelper::UISprites(UIHelper::FacebookConnectionNoThanksButton));
    
    CCMenu *noThanks = CCMenuHelper::createMenu(noThanks_sprite,
                                                this,
                                                menu_selector(FBLoginDialog::onClose));
    noThanks->setAnchorPoint(ccp(0.5f, 0.5f));
    noThanks->setPosition(ccp(kFBLoginDialog_x_pos,
                              -154.0f*sizeMultiplier));
    m_nodesGroup->addChild(noThanks);
    
    CCLabelBMFont *noThanks_label =
    CCLabelHelper::createBMFont(g_gameContent->Word["fblogindialog_nothanks"].c_str(),
                                kFontSizeLarge-5.0f,
                                true);
    noThanks_label->setPosition(ccp(noThanks->getContentSize().width*0.5f,
                                    noThanks->getContentSize().height*0.55f));
    noThanks_sprite->addChild(noThanks_label);
    
    
    snprintf(_buffer, _buffer_size, "+%d", _gemReward);
    
    CCLabelBMFont *gemReward_label = CCLabelHelper::createBMFont(_buffer,
                                                                 kFontSizeLarge,
                                                                 true);
    gemReward_label->setPosition(ccp(connectFacebook->getPosition().x
                                     + connectFacebook->getContentSize().width*0.5f
                                     + 90.0f*sizeMultiplier,
                                     connectFacebook->getPosition().y*0.5f));
    m_nodesGroup->addChild(gemReward_label);
    
    
    CCSprite *gemReward_gem = UIHelper::getSprite(UIHelper::HUDGem);
    gemReward_gem->setScale(0.75f);
    gemReward_gem->setPosition(ccp(gemReward_label->getPosition().x
                                     + 120.0f*sizeMultiplier,
                                     connectFacebook->getPosition().y*0.5f));
    m_nodesGroup->addChild(gemReward_gem);
    
    
    //close Dialog
    if (SceneVille *_scene = (SceneVille*)g_currentScene) {
        if (_scene->mapToScreenUI) {
            _scene->mapToScreenUI->closeAllUIs();
        }
        if (IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap) {
            if (_isoMapBuilding->entityAction) {
                _isoMapBuilding->entityAction->finish();
            }
        }
        
    }
    
#if IS_DEBUG_BUILD
//    composeClaimReward();
#endif
    
    return true;
}


void FBLoginDialog::composeClaimReward() {
    
    GDAchievementsVO vo = g_gameContent->getAchievementVO(kFBLoginDialogAchievementType);
    int _gemReward = vo.gems_reward;
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    m_nodesGroup->removeAllChildrenWithCleanup(true);
    
    
    snprintf(_buffer, _buffer_size,
            g_gameContent->Word["fblogindialog_desc_connectedtofacebook"].c_str(),
            _gemReward);
    
    CCLabelBMFont *_desc = CCLabelHelper::createBMFont(_buffer,
                                                       kFontSizeMedium,
                                                       kTextColorDark);
    _desc->setAnchorPoint(ccp(0.5f, 0.5f));
    _desc->setAlignment(kCCTextAlignmentCenter);
    _desc->setWidth(636.0f*sizeMultiplier);
    _desc->setPosition(kFBLoginDialog_x_pos_CLAIM_DIALOG,
                       143.0f*sizeMultiplier);
    m_nodesGroup->addChild(_desc);
    
    
    CCSprite *claim_sprite =
    UIHelper::getSprite(UIHelper::UISprites(UIHelper::FacebookConnectionClaimGemsButton));
    
    CCMenu *claim = CCMenuHelper::createMenu(claim_sprite,
                                             this,
                                             menu_selector(FBLoginDialog::onClaim));
    claim->setAnchorPoint(ccp(0.5f, 0.5f));
    claim->setPosition(ccp(kFBLoginDialog_x_pos_CLAIM_DIALOG,
                           -164.0f*sizeMultiplier));
    
    m_nodesGroup->addChild(claim);
    
    CCLabelBMFont *claim_label =
    CCLabelHelper::createBMFont(g_gameContent->Word["fblogindialog_claim"].c_str(),
                                kFontSizeLarge-5.0f,
                                true);
    claim_label->setPosition(ccp(claim->getContentSize().width*0.5f,
                                    claim->getContentSize().height*0.55f));
    claim_sprite->addChild(claim_label);
    
    
    snprintf(_buffer, _buffer_size, "+%d", _gemReward);
    
    CCLabelBMFont *gemReward_label = CCLabelHelper::createBMFont(_buffer,
                                                                 kFontSizeLarge,
                                                                 true);
    gemReward_label->setPosition(ccp(kFBLoginDialog_x_pos_CLAIM_DIALOG
                                     - 54.0f*sizeMultiplier,
                                     -24.0f*sizeMultiplier));
    m_nodesGroup->addChild(gemReward_label);
    
    
    CCSprite *gemReward_gem = UIHelper::getSprite(UIHelper::HUDGem);
    gemReward_gem->setScale(0.75f);
    gemReward_gem->setPosition(ccp(kFBLoginDialog_x_pos_CLAIM_DIALOG
                                   + 54.0f*sizeMultiplier,
                                   -24.0f*sizeMultiplier));
    m_nodesGroup->addChild(gemReward_gem);
    
}


void FBLoginDialog::onClaim(CCObject *sender) {
    
    posTouch = ((CCNode*)sender)->convertToWorldSpaceAR(CCPointZero);
    GDAchievementsVO _avo = g_gameContent->getAchievementVO(kFBLoginDialogAchievementType);
    

    if (!PlayerState::getInstance()->prePrecessAchievementClaim(_avo)) return;

    g_sceneManager->showLoading();
    BackendService::getInstance()->send(APIAchievementClaim::create(_avo.type.c_str(),
                                                                    this,
                                                                    FBLoginDialog::onAchievementClaimed));
    
}

void FBLoginDialog::onAchievementClaimed(void *sender, std::string resp_str) {

    g_sceneManager->hideLoading();
    
    //JsonAllocator allocator;
    Json*root = Json_create(resp_str.c_str());
    PlayerState::getInstance()->parseUserBalance(root);
    PlayerState::getInstance()->parseChecklistData(root);
    Json_dispose(root);
    
    FBLoginDialog  *_this = (FBLoginDialog*)sender;
    if (!IS_INSTANCE_OK(_this, FBLoginDialog))
        return;
    _this->m_modalController->closeAll();
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!IS_INSTANCE_OK(_scene, SceneVille))
        return;
    HUDPanel *hudPanel = _scene->hudPanel;
    CCSprite *gemIconSprite = hudPanel->resourceBars[HUDPanelBarGems]->getIconSprite();
    CCPoint endPoint = gemIconSprite->convertToWorldSpaceAR(CCPointZero);
    CollectItemEffect::addCollectItemEffect(CollectItemEffect::create(UIHelper::getSpritePath(UIHelper::HUDGem),
                                                                      kCollectItemEffectDefaultSprite,
                                                                      _this->posTouch,
                                                                      endPoint,
                                                                      _this,
                                                                      FBLoginDialog::onCollectGemsEffectFinished), _scene);
    
}



void FBLoginDialog::onConnectFacebook(cocos2d::CCObject* sender) {
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(ccs("FBLoginDialog"), "Source");
    FlurryCpp::logEvent(EVENT_FBLOGIN_BUTTON_TAPPED, params);
    
    CSJson::Value eventParameters;
    eventParameters["source"] = "FBLoginDialog";
    AnalyticsClient::getInstance()->logEvent(EVENT_FBLOGIN_BUTTON_TAPPED, eventParameters);
    
    FBAppEventsWrapper::logEvent(EVENT_FBLOGIN_BUTTON_TAPPED);
    
    g_sceneManager->facebook->login(this, FBLoginDialog::onFacebookLoggedIn);

}


void FBLoginDialog::onFacebookLoggedIn(void* sender, bool success) {
    
    if(g_sceneManager->facebook->isLoggedIn()) {
        
        CCLog_("======FBLoginDialog::onFacebookLoggedIn=====");
        
        CCDictionary *params = CCDictionary::create();
        params->setObject(ccs("FBLoginDialog"), "Source");
        FlurryCpp::logEvent(EVENT_FBLOGIN_SUCCESS, params);
        
        CSJson::Value eventParameters;
        eventParameters["source"] = "FBLoginDialog";
        AnalyticsClient::getInstance()->logEvent(EVENT_FBLOGIN_SUCCESS, eventParameters);
        
        FBAppEventsWrapper::logEvent(EVENT_FBLOGIN_SUCCESS);
        
        FBLoginDialog *_this = (FBLoginDialog*)sender;
        
        _this->composeClaimReward();
        
        CCUserDefault::sharedUserDefault()->setBoolForKey("fblogindialog_hasloggedin", true);
        
        g_sceneManager->onFBLoggedIn(g_sceneManager, true, true);
        
    }
}

void FBLoginDialog::onCollectGemsEffectFinished(void* sender) {
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!IS_INSTANCE_OK(_scene, SceneVille))
        return;
    _scene->hudPanel->updateAllResourceBars();
    
}
