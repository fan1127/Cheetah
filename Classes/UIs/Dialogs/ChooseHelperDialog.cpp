//
//  ChooseHelperDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/30/2557 BE.
//
//

#include "ChooseHelperDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

#include <spine/Json.h>

#include "../../Helpers/CCStringHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/CreateSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"
#include "../../Scenes/SceneMatch.h"
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

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../AppMacros.h"

extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float retinaMultiplier;

#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/UIHelper.h"
#include "../../Helpers/DataCache.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

#pragma mark - ChooseHelperItem class

ChooseHelperItem* ChooseHelperItem::createWithPlayerHelperPetVO(PlayerHelperPetVO vo_) {

    return createWithPlayerHelperPetVO(vo_, false);

}

ChooseHelperItem* ChooseHelperItem::createWithPlayerHelperPetVO(PlayerHelperPetVO vo_, bool isSelected_) {

    if (ChooseHelperItem* pRet = new ChooseHelperItem()) {
        pRet->m_helper_pet_vo = vo_;
        pRet->m_isSelected = isSelected_;
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
}

bool ChooseHelperItem::init() {
    
    CCNode *bgSprite = UIHelper::getSprite(UIHelper::FriendsHelpBg);
    
    if (m_helper_pet_vo.uuid == "" &&
        m_helper_pet_vo.pet_type == "info")
    {
        CCSize size = bgSprite->getContentSize();
        bgSprite = UIHelper::getScalableSprite(UIHelper::AchievementBG);
        bgSprite->setContentSize(size);
    }
    
    if (!CCNikScrollViewButton::init(bgSprite, NULL))
    {
        return false;
    }

    if (m_helper_pet_vo.uuid == "") {
        if (m_helper_pet_vo.pet_type == "info")
        {
            CCLabelBMFont* _text1 = CCLabelHelper::createBMFont("Choose 1 friend to help you", 55, true);
            _text1->setWidth(getContentSize().width*0.85f);
            _text1->setAlignment(kCCTextAlignmentCenter);
            _text1->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*0.65f));
            addChild(_text1);
            
            CCLabelBMFont* _text2 = CCLabelHelper::createBMFont("They can help you once a day", 45, true);
            _text2->setWidth(getContentSize().width*0.85f);
            _text2->setAlignment(kCCTextAlignmentCenter);
            _text2->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*0.23f));
            addChild(_text2);
        }
        else if (m_helper_pet_vo.pet_type == "facebook" ||
                 m_helper_pet_vo.pet_type == "invite")
        {
            CCSprite *fbIcon = UIHelper::getSprite(UIHelper::UISprites(UIHelper::FriendsHelpFacebookIcon));
            fbIcon->setPosition(ccp(getContentSize().width*0.5f,
                                    getContentSize().height*0.5f
                                    + 160.0f*sizeMultiplier));
            addChild(fbIcon);
            
            CCSprite *buttonSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::FriendsHelpFacebookButtonNoIcon));
            buttonSprite->setPosition(ccp(getContentSize().width*0.5f,
                                          getContentSize().height*0.5f
                                          - 52.0f*sizeMultiplier));
            addChild(buttonSprite);
            
            CCLabelBMFont *buttonLabel = NULL;
            if (m_helper_pet_vo.pet_type == "facebook") {
                buttonLabel = CCLabelHelper::createBMFont("Connect", 55, true);
            } else if (m_helper_pet_vo.pet_type == "invite") {
                buttonLabel = CCLabelHelper::createBMFont("Invite", 55, true);
            }
            if (buttonLabel) {
                buttonLabel->setPosition(buttonSprite->getPosition()
                                         + ccp(0.0f, 5.0f*sizeMultiplier));
                addChild(buttonLabel);
            }
            
            CCLabelBMFont* _text2 = NULL;
            
            if (m_helper_pet_vo.pet_type == "facebook") {
                _text2 = CCLabelHelper::createBMFont("Get more friends to help you", kFontSizeSmall, true);
            } else if (m_helper_pet_vo.pet_type == "invite") {
                _text2 = CCLabelHelper::createBMFont("Invite more friends to help you", kFontSizeSmall, true);
            }
            
            if (_text2) {
                _text2->setWidth(getContentSize().width*0.6f);
                _text2->setAlignment(kCCTextAlignmentCenter);
                _text2->setPosition(ccp(getContentSize().width*0.5f,
                                        getContentSize().height*0.5f
                                        - 200.0f*sizeMultiplier));
                addChild(_text2);
            }
        }
    }
    
    m_checkmark = UIHelper::getSprite(UIHelper::UISprites(UIHelper::HUDCheckmarkBig));
    m_checkmark->setVisible(false);
    m_checkmark->setPosition(ccp(getContentSize().width*0.65f, getContentSize().height*0.7f));
    addChild(m_checkmark, 1000);

    m_petSprite = NULL;
    m_petSpine = NULL;
    m_horsepowerSprite = NULL;
    m_stunTimeSprite = NULL;
    m_photoSprite = NULL;
    m_friendSprite = NULL;
    
    updateContent(m_isSelected);
    
    return true;
    
}

void ChooseHelperItem::updateContent(bool isSelected_) {
    
    if (m_helper_pet_vo.uuid != "") {
        
        m_isSelected = isSelected_;
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        GDPetVO _pet_vo = g_gameContent->getPetVO(m_helper_pet_vo.pet_type);
        if (_pet_vo.type != "") {

            auto petPosY = getContentSize().height*0.77f;
        
            if (m_isSelected) { //selected
                
                m_checkmark->setVisible(true);

                CCSpriteFrame *sp = UIHelper::getSpriteFrame(UIHelper::FriendsHelpBgSelected);
                CCSprite *normalSprite = dynamic_cast<CCSprite*>(m_normalSprite);
                if (normalSprite)
                    normalSprite->setDisplayFrame(sp);

                auto highlight = UIHelper::getSprite(UIHelper::PurchaseLight);
                highlight->setAnchorPoint(m_normalSprite->getAnchorPoint());
                highlight->setPosition(m_normalSprite->getPosition());
                highlight->setPositionY(petPosY);
                m_normalSprite->addChild(highlight, 0, 1234);

                if (m_petSprite) {
                    m_petSprite->removeFromParent();
                    m_petSprite = NULL;
                }
                
                if (!m_petSpine) {
                    m_petSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, _pet_vo.src);
                    m_petSpine->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*0.63f));
                    m_petSpine->setScale(sizeMultiplier);
                    m_petSpine->setAnimation("habitat_touch", true);
                    addChild(m_petSpine);
                }
                
            } else {//unselected
                
                m_checkmark->setVisible(false);
            
                CCSpriteFrame *sp = UIHelper::getSpriteFrame(UIHelper::FriendsHelpBg);
                CCSprite *normalSprite = dynamic_cast<CCSprite*>(m_normalSprite);
                if (normalSprite)
                    normalSprite->setDisplayFrame(sp);

                auto highlight = m_normalSprite->getChildByTag(1234);

                if (highlight)
                {
                    highlight->removeFromParent();
                    highlight = NULL;
                }

                if (!m_petSprite) {
                    snprintf(_buffer, _buffer_size, "%s.png", _pet_vo.src.c_str());
                    m_petSprite = CCSprite::createWithSpriteFrameName(_buffer);
                    m_petSprite->setPosition(ccp(getContentSize().width*0.5f, petPosY));
                    addChild(m_petSprite);
                }
                
                if (m_petSpine) {
                    m_petSpine->removeFromParent();
                    m_petSpine = NULL;
                }
                
            }
            
            if (!m_horsepowerSprite) {
                CCString* _str = CCString::createWithFormat("%d", m_helper_pet_vo.pet_horse_power);
                m_horsepowerSprite = CreateSpriteHelper::getHorsePowerSprite(_str->getCString(), _pet_vo.rarity_stars);
                m_horsepowerSprite->setPosition(ccp(getContentSize().width*0.75f, getContentSize().height*0.52f));
//                m_horsepowerSprite->setScale(0.9f);
                addChild(m_horsepowerSprite, 2);
            }
            
            if (!m_stunTimeSprite) {
                
                float timeTotal = 0.0f, times = 0.0f, timeEach = 0.0f;
                g_gameContent->cal_helper_horsepower_to_stun_time(m_helper_pet_vo.pet_horse_power,
                                                                  &timeTotal,
                                                                  &times,
                                                                  &timeEach);
                CCString* _str = CCString::createWithFormat("%.1fs", timeTotal);
                CCLabelBMFont* _text = CCLabelHelper::createBMFont(_str->getCString(), 45, true);
                m_stunTimeSprite = UIHelper::getSprite(UIHelper::FriendsHelpStunIcon);
                m_stunTimeSprite->setPosition(ccp(getContentSize().width*0.25f, getContentSize().height*0.54f));
                addChild(m_stunTimeSprite, 3);
                _text->setPosition(m_stunTimeSprite->getContentSize()*0.4f);
                _text->setPositionX(_text->getPositionX() + 20 * sizeMultiplier);
                m_stunTimeSprite->addChild(_text);
            }
            
            CCPoint photoPosition = ccp(getContentSize().width*0.5f, getContentSize().height*0.3f);
            
            if (m_helper_pet_vo.isFriend && !m_photoSprite) {
                
                m_photoSprite = CCLoadSpriteURL::create((char*)UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::AvatarDefault)), kDefaultAvatarSize*sizeMultiplier, (char*)m_helper_pet_vo.photo.c_str(), (char*)m_helper_pet_vo.photo.c_str());
                m_photoSprite->setPosition(photoPosition);
                m_photoSprite->setScale(0.7f);
                addChild(m_photoSprite);
                
                CCSprite* _frame_avatar = UIHelper::getSprite(UIHelper::UISprites(UIHelper::AvatarBorder));
                _frame_avatar->setPosition(photoPosition);
                _frame_avatar->setScale(0.7f);
                addChild(_frame_avatar);
            }
            
            if (!m_helper_pet_vo.isFriend) {
                
                CCSprite *defaultAvatar = UIHelper::getSprite(UIHelper::UISprites(UIHelper::AvatarDefault));
                defaultAvatar->setPosition(photoPosition);
                defaultAvatar->setScale(0.5f);
                addChild(defaultAvatar);
            }
            
            CCLabelBMFont* _text2 = CCLabelHelper::createBMFont(m_helper_pet_vo.name.c_str(), 45, true);
            _text2->setWidth(getContentSize().width*0.8f);
            _text2->setAlignment(kCCTextAlignmentCenter);
            _text2->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*0.15f));
            addChild(_text2);
            
            
            if (!m_friendText)
            {
                m_friendText = CCLabelHelper::createBMFont("Friend", 40, true);
                m_friendText->setPosition(ccp(getContentSize().width*0.5f,
                                              -28.0f*sizeMultiplier));
                addChild(m_friendText);
                
                if (m_helper_pet_vo.isFriend)
                {
                    m_friendText->setString("Friend");
                }
                else
                {
                    m_friendText->setString("Neighbor");
                }
            }
        }
    }
}

#pragma mark - ChooseHelperDialog class

ChooseHelperDialog* ChooseHelperDialog::create(CCModalController* parentModalController_) {
    
    if (ChooseHelperDialog *pRet = new ChooseHelperDialog()) {
        pRet->m_modalController = parentModalController_;
        if (pRet->init(parentModalController_, kDialogFriendHelpSize)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
}

bool ChooseHelperDialog::init(CCModalController* parentModalController_, CCSize size)
{
    if (!ScrollDialog::init(parentModalController_, size))
    {
        return false;
    }
    
    m_closeButton->setVisible(false);

    createScrollContent(CHOOSEHELPER_DIALOG_SETTINGS);

    addTitle(g_gameContent->Word["choose_helper_title"].c_str());

    m_middleText1 = CCLabelHelper::createBMFont("", kFontSizeMedium, ccc3(66, 66, 66));
    m_middleText1->setPosition(ccp(0.0f,
                                   (m_background->getContentSize().height * -0.5f)
                                   + 150.0f*sizeMultiplier
                                   + 70.0f*sizeMultiplier));

    m_middleText1->setAnchorPoint(ccp(0.5f,0.5f));
    m_middleText1->setVisible(false);
    this->addChild(m_middleText1);
    
    m_middleText2 = CCLabelHelper::createBMFont("*Tap again to unselect", kFontSizeMedium, ccc3(66, 66, 66));
    m_middleText2->setPosition(ccp(0.0f,
                                   (m_background->getContentSize().height * -0.5f) 
                                   + 108.0f*sizeMultiplier 
                                   + 70.0f*sizeMultiplier));

    m_middleText2->setAnchorPoint(ccp(0.5f,0.5f));
    m_middleText2->setVisible(false);
    this->addChild(m_middleText2);
    
    m_middleText3 = CCLabelHelper::createBMFont("Tap to select a helper", kFontSizeMedium, ccc3(66, 66, 66));
    m_middleText3->setPosition(ccp(0.0f,
                                   (m_background->getContentSize().height * -0.5f) 
                                   + 129.0f*sizeMultiplier 
                                   + 70.0f*sizeMultiplier));

    m_middleText3->setAnchorPoint(ccp(0.5f,0.5f));
    m_middleText3->setVisible(false);
    this->addChild(m_middleText3);
    
    if (g_sceneManager->facebook->isLoggedIn())
    {
        //-----------facebook---------
        CCNikButton *_facebookButton = CCNikButton::create(UIHelper::getSprite(UIHelper::FacebookConnectionFacebookButton), NULL);
        _facebookButton->setAnchorPoint(ccp(0.0f, 0.0f));
        _facebookButton->setTarget(this, menu_selector(ChooseHelperDialog::onFacebook), CCNikButtonEventTouchUpInside);
        _facebookButton->setPosition(ccp((m_background->getContentSize().width * -0.5f) + kChooseHelperDialogFirstSpaceToTheLeft*sizeMultiplier*2.0f,
                                         (m_background->getContentSize().height * -0.5f) + kChooseHelperDialogFirstSpaceToTheLeft*sizeMultiplier*2.0f));

        addChild(_facebookButton, 5);

        CCLabelBMFont* _facebookLabel = CCLabelHelper::createBMFont(g_gameContent->Word["invite_friends"].c_str(), kFontSizeVerySmall+5, true);
        _facebookLabel->setAnchorPoint(CCPointMake(0.5f, 0.5f));
        _facebookLabel->setPosition(_facebookButton->getContentSize().width*0.5f
                                    + 45.0f*sizeMultiplier,
                                    _facebookButton->getContentSize().height*0.5f
                                    + 5.0f*sizeMultiplier);
        _facebookButton->addChild(_facebookLabel);
    }

    //-----------go button---------
    CCNikButton *_goButton = CCNikButton::create(UIHelper::getSprite(UIHelper::FriendsHelpStartButton), NULL);
    _goButton->setAnchorPoint(ccp(1.0f, 0.0f));
    _goButton->setTarget(this, menu_selector(ChooseHelperDialog::onGo), CCNikButtonEventTouchUpInside);
    _goButton->setPosition(ccp((m_background->getContentSize().width * 0.5f) - kChooseHelperDialogFirstSpaceToTheLeft*sizeMultiplier*2.0f,
                               (m_background->getContentSize().height * -0.5f) + kChooseHelperDialogFirstSpaceToTheLeft*sizeMultiplier*2.0f));
    addChild(_goButton, 5);
    CCLabelBMFont* _goLabel = CCLabelHelper::createBMFont(g_gameContent->Word["choose_helper_done"].c_str(), kFontSizeLarge-5.0f, true);
    _goLabel->setPosition(_goButton->getContentSize()*0.5f);
    _goLabel->setAnchorPoint(ccp(0.5f,0.43f));
    _goButton->addChild(_goLabel);

    m_label_foot = CCLabelHelper::createBMFont("", StyleDialogTextDark);
    m_label_foot->setAnchorPoint(ccp(0.5f, 0.5f));
    m_label_foot->setPositionY(_goButton->getPositionY() + _goButton->getContentSize().height*0.5f);
    addChild(m_label_foot);

    g_playerState->HelperPetsSelectedIndex = -1;
    
    if (g_playerState->HelperPets.size() == 0)
    {
        startLoading();
        DataCache::Instance().getDataAsync(DataCache::Helper, this, ChooseHelperDialog::onHelperPetRetrieved);
//        g_gameBackend->helper_pet_get(g_playerState->fbFriendsRawData, g_opponentState->player.uuid, this, ChooseHelperDialog::onHelperPetRetrieved);
    }
    else
    {
        updateContent();
    }

    return true;
}

ChooseHelperDialog::~ChooseHelperDialog()
{
    BackendService::getInstance()->clearAllRequests();
    finishLoading();
    
}

void ChooseHelperDialog::startLoading()
{
    showLoading();
    //g_sceneManager->showLoading();
}

void ChooseHelperDialog::finishLoading()
{
    hideLoading();
    //g_sceneManager->hideLoading();
}

void ChooseHelperDialog::updateContent() {
    
    finishLoading();
    
    ChooseHelperItem* _lastButton = NULL;
    int i = 0;
    
    bool helperIsSelected = false;
    
    PlayerState* _playerState = g_playerState;
    CCLog_("%d", (int)_playerState->HelperPets.size());
    
    g_playerState->updateHelperPetsList();
    
    for (vector<PlayerHelperPetVO>::iterator it=g_playerState->HelperPets.begin(); it!=g_playerState->HelperPets.end(); ++it) {
    
        if (i < m_items.size()) {
            ChooseHelperItem* _item = m_items[i];
            bool selected = (i == g_playerState->HelperPetsSelectedIndex);
            _item->updateContent(selected);
            if (selected &&
                (_item->m_helper_pet_vo.pet_type != "info" &&
                 _item->m_helper_pet_vo.pet_type != "facebook" &&
                 _item->m_helper_pet_vo.pet_type != "invite")
                )
            {
                helperIsSelected = true;
                CREATE_CHAR_BUFFER(buffer, 64);
                snprintf(buffer, buffer_size, "Selected %s", _item->m_helper_pet_vo.name.c_str());
                m_middleText1->setString(buffer);
                m_middleText1->setVisible(true);
                m_middleText2->setVisible(true);
                m_middleText3->setVisible(false);
            }
            _lastButton = _item;
            i ++;
            continue;
        }

        if (ChooseHelperItem* _item = ChooseHelperItem::createWithPlayerHelperPetVO(*it,
                                                                                    i == g_playerState->HelperPetsSelectedIndex)) {
            _item->setPosition(ccp(_item->getContentSize().width*i
                                   +(14.0f*sizeMultiplier*i),
                                   m_clippingLayer->getContentSize().height
                                   - _item->getContentSize().height
                                   -10.0f*sizeMultiplier));
            _item->setTag(i);
            _item->setTarget(this, menu_selector(ChooseHelperDialog::onSelect), CCNikScrollViewButtonStateTouchUp);
            _item->setDelegate(m_scrollView);
            m_scrollView->addChild(_item);
            m_items.push_back(_item);
            _lastButton = _item;
        }
        i++;
        
    }

    if (_lastButton) {
        m_scrollView->setContentView(CCSizeMake(MAX(_lastButton->getPositionX() +
                                                    _lastButton->getContentSize().width +
                                                    60*retinaMultiplier,
                                                    m_scrollView->getContentSize().width),
                                                m_scrollView->getContentSize().height));
    }
    
    if (!helperIsSelected) {
        m_middleText1->setVisible(false);
        m_middleText2->setVisible(false);
        m_middleText3->setVisible(true);
    }
    
}

#pragma mark - Callback functions

void ChooseHelperDialog::onSelect(CCObject *sender_) {
    
    ChooseHelperItem* _item = (ChooseHelperItem*)sender_;
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    if (_item->getHelperPetVO().uuid == "") {
        if (_item->getHelperPetVO().pet_type == "info") {
            //Info
        } else if (_item->getHelperPetVO().pet_type == "facebook" ||
                   _item->getHelperPetVO().pet_type == "invite") {
            onFacebook(sender_);
            return;
        }
    }
    
    int _item_index = _item->getTag();
    CCLog_("select %d", _item_index);
    if (g_playerState->HelperPetsSelectedIndex == _item_index) {
        g_playerState->HelperPetsSelectedIndex = -1;
    }else {
        g_playerState->HelperPetsSelectedIndex = _item_index;
    }
    updateContent();

}

void ChooseHelperDialog::onFacebook(CCObject *sender_) {
    
    if(g_sceneManager->facebook->isLoggedIn()) {
        
        CCDictionary *params = CCDictionary::create();
        params->setObject(ccs("ChooseHelperDialog"), "Source");
        FlurryCpp::logEvent(EVENT_INVITE_BUTTON_TAPPED, params);
        
        CSJson::Value eventParameters;
        eventParameters["source"] = "ChooseHelperDialog";
        AnalyticsClient::getInstance()->logEvent(EVENT_INVITE_BUTTON_TAPPED, eventParameters);
        
        FBAppEventsWrapper::logEvent(EVENT_INVITE_BUTTON_TAPPED);
        
        g_sceneManager->facebook->inviteFriends("Invite Friends", "Play Ranch Run with your friends!", this, ChooseHelperDialog::onInvitationSent);
    } else {
        
        CCDictionary *params = CCDictionary::create();
        params->setObject(ccs("ChooseHelperDialog"), "Source");
        FlurryCpp::logEvent(EVENT_FBLOGIN_BUTTON_TAPPED, params);
        
        CSJson::Value eventParameters;
        eventParameters["source"] = "ChooseHelperDialog";
        AnalyticsClient::getInstance()->logEvent(EVENT_FBLOGIN_BUTTON_TAPPED, eventParameters);
        
        FBAppEventsWrapper::logEvent(EVENT_FBLOGIN_BUTTON_TAPPED);
        
        g_sceneManager->facebook->login(this, ChooseHelperDialog::onFacebookLoggedIn);
        g_playerState->HelperPets.clear();
//        this->m_modalController->closeAll();
    }
    
}

void ChooseHelperDialog::onFacebookLoggedIn(void* sender_, bool success_) {
    
    if(success_) {
        
        CCDictionary *params = CCDictionary::create();
        params->setObject(ccs("ChooseHelperDialog"), "Source");
        FlurryCpp::logEvent(EVENT_FBLOGIN_SUCCESS, params);
        
        CSJson::Value eventParameters;
        eventParameters["source"] = "ChooseHelperDialog";
        AnalyticsClient::getInstance()->logEvent(EVENT_FBLOGIN_SUCCESS, eventParameters);
        
        FBAppEventsWrapper::logEvent(EVENT_FBLOGIN_SUCCESS);
        
        g_sceneManager->onFBLoggedIn(g_sceneManager, sender_, true, ChooseHelperDialog::onFBInstalledFriendRetrieved);
        
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, g_gameContent->Word["facebook_login_success_message"]);
        
        if (ChooseHelperDialog* _this = (ChooseHelperDialog*)sender_) {
            
            while (_this->m_items.size() > 0) {
                _this->m_items[_this->m_items.size()-1]->removeFromParent();
                _this->m_items.pop_back();
            }
            _this->m_items.clear();
            
            _this->startLoading();
        }
    }
}

void ChooseHelperDialog::onFBInstalledFriendRetrieved(void* sender_, bool success_) {
    
    g_sceneManager->onFBInstalledFriendRetrieved(sender_, success_);

    DataCache::Instance().getDataAsync(DataCache::Helper, sender_, ChooseHelperDialog::onHelperPetRetrieved);
//    g_gameBackend->helper_pet_get(g_playerState->fbFriendsRawData, g_opponentState->player.uuid, sender_, ChooseHelperDialog::onHelperPetRetrieved);
}

void ChooseHelperDialog::onGo(CCObject *sender_) {
    
    CCLog_("GO with index: %d!", g_playerState->HelperPetsSelectedIndex);
    if (SceneMatch* _sceneMatch = (SceneMatch*)g_currentScene) {
        if (_sceneMatch->matchPanel) {
            
            g_gameAudio->playEffect(AUDIO_CLOUD_OPEN);
            this->m_modalController->setHitBGToCloseAllEnabled(true);
            this->m_modalController->closeAll();
            _sceneMatch->matchPanel->launchRaceScene();
            
        }
    }
    
}

void ChooseHelperDialog::onHelperPetRetrieved(void *sender_, string resp_json_str_) {
    
    if (typeid(*g_currentScene) != typeid(SceneMatch)) return;
            
    CCLog_("onHelperPetRetrieved: facebook->isLoggedIn: %d",g_sceneManager->facebook->isLoggedIn());
    
    if (ChooseHelperDialog* _this = (ChooseHelperDialog*)sender_) {

        //JsonAllocator allocator;
        Json *_root = Json_create(resp_json_str_.c_str());
        if (!_root) {
            return;
        }
        
        if (Json* _helper_pets = Json_getItem(_root, "helper_pet")) {
            
            g_playerState->HelperPets.clear();
            
            int _nhelper_pet = Json_getSize(_helper_pets);

            PlayerHelperPetVO vo = PlayerHelperPetVO();
            vo.pet_type = "info";
            g_playerState->HelperPets.push_back(vo);
            
            if (!g_sceneManager->facebook->isLoggedIn()) {
                PlayerHelperPetVO vo2 = PlayerHelperPetVO();
                vo2.pet_type = "facebook";
                g_playerState->HelperPets.push_back(vo2);
            }

            map<int, vector<PlayerHelperPetVO> > _petsToBeSorted;
            map<int, vector<PlayerHelperPetVO> > _petsToBeSortedIsFriend;
            
            for (int i = 0; i < _nhelper_pet; i++) {
                
                if (Json* _helper_pet = Json_getItemAt(_helper_pets, i)) {
                    
                    if (Json* _pet = Json_getItem(_helper_pet, "pet")) {
                        
                        PlayerHelperPetVO _vo = PlayerHelperPetVO(Json_getString(_helper_pet, "uuid", ""),
                                                                  Json_getString(_helper_pet, "name", "unknown"),
                                                                  Json_getString(_helper_pet, "photo", ""),
                                                                  Json_getString(_pet, "type", ""),
                                                                  Json_getInt(_pet, "top_speed", 0),
                                                                  Json_getInt(_pet, "corner_speed", 0),
                                                                  Json_getInt(_pet, "jumping", 0),
                                                                  (bool)Json_getInt(_helper_pet, "friend", 0));
                        
                        if (Json* _abilites = Json_getItem(_helper_pet, "ability")) {
                            int _nabilities = Json_getSize(_abilites);
                            for (int j = 0; j<_nabilities; j++) {
                                if (Json* _ability = Json_getItemAt(_abilites, j)) {
                                    _vo.abilities[Json_getString(_ability, "type", "undefined")] = Json_getInt(_ability, "level", 0);
                                }
                            }
                        }
                        if (_vo.isFriend) {
                            vector<PlayerHelperPetVO> stack = _petsToBeSortedIsFriend[_vo.pet_horse_power];
                            stack.push_back(_vo);
                            _petsToBeSortedIsFriend[_vo.pet_horse_power] = stack;
                        } else {
                            vector<PlayerHelperPetVO> stack = _petsToBeSorted[_vo.pet_horse_power];
                            stack.push_back(_vo);
                            _petsToBeSorted[_vo.pet_horse_power] = stack;
                        }
                    }
                    
                }
                
            }
            
            if (g_sceneManager->facebook->isLoggedIn()) {
                for (map<int, vector<PlayerHelperPetVO> >::reverse_iterator it=_petsToBeSortedIsFriend.rbegin(); it!=_petsToBeSortedIsFriend.rend(); ++it) {
                    vector<PlayerHelperPetVO> stack = (*it).second;
                    for (vector<PlayerHelperPetVO>::reverse_iterator it2=stack.rbegin(); it2!=stack.rend(); ++it2) {
                        PlayerHelperPetVO _vo = (*it2);
                        CCLog_("hp: %d", _vo.pet_horse_power);
                        g_playerState->HelperPets.push_back(_vo);
                    }
                }
            }
            
            for (map<int, vector<PlayerHelperPetVO> >::reverse_iterator it=_petsToBeSorted.rbegin(); it!=_petsToBeSorted.rend(); ++it) {
                vector<PlayerHelperPetVO> stack = (*it).second;
                for (vector<PlayerHelperPetVO>::reverse_iterator it2=stack.rbegin(); it2!=stack.rend(); ++it2) {
                    PlayerHelperPetVO _vo = (*it2);
                    CCLog_("hp: %d", _vo.pet_horse_power);
                    g_playerState->HelperPets.push_back(_vo);
                }
            }
        
            if (g_sceneManager->facebook->isLoggedIn()) {
                PlayerHelperPetVO vo = PlayerHelperPetVO();
                vo.pet_type = "invite";
                g_playerState->HelperPets.push_back(vo);
            }
        }
        
        Json_dispose(_root);
        _this->updateContent();
        
    }
    
}

void ChooseHelperDialog::onInvitationSent(void *sender_, int numberOfInvites, string fbids_) {
    
    if (numberOfInvites == 0) return;
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(CCString::createWithFormat("%d",numberOfInvites), "Number of Invites");
    params->setObject(ccs("ChooseHelperDialog"), "Source");
    FlurryCpp::logEvent(EVENT_INVITE_FRIEND_SUCCESS, params);
    
    CSJson::Value eventParameters;
    eventParameters["source"] = "ChooseHelperDialog";
    eventParameters["numberOfInvites"] = numberOfInvites;
    AnalyticsClient::getInstance()->logEvent(EVENT_INVITE_FRIEND_SUCCESS, eventParameters);
    
    FBAppEventsWrapper::logEvent(EVENT_INVITE_FRIEND_SUCCESS);
    
    AppsFlyerWrapper::trackEvent(EVENT_INVITE_FRIEND_SUCCESS);
    
    if (ChooseHelperDialog* _this = (ChooseHelperDialog*)sender_) {
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, g_gameContent->Word["facebook_invites_sent_message"].c_str(), numberOfInvites);
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
        
        CCLog_("%s", fbids_.c_str());
        BackendService::getInstance()->send(APIAppRequestAdd::create("facebook",
                                                                     fbids_,
                                                                     _this,
                                                                     ChooseHelperDialog::onApprequestAdd));
        
    }
    
}

void ChooseHelperDialog::onApprequestAdd(void *sender_, string resp_json_str_) {
    
    if (typeid(*g_currentScene) != typeid(SceneMatch)) return;
    
    ChooseHelperDialog* _this = (ChooseHelperDialog*)sender_;
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

void ChooseHelperDialog::onExit()
{
    ScrollDialog::onExit();
    DataCache::Instance().resetCallback();
}
