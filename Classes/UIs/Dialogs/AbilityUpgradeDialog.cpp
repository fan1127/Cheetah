//
//  AbilityUpgradeDialog.cpp
//  Cheetah
//
//  Created by Teerapat Avalable on 4/7/2557 BE.
//
//

#include "AbilityUpgradeDialog.h"
USING_NS_CC;
using namespace std;

#include <stdio.h>
#include "../../Settings.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCStringHelper.h"

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIAbilityUpgrade.h"
#include "../../Services/APIs/APIAbilityUpdate.h"

#include "../../Entities/PlayerState.h"

#include "../../Entities/Building.h"
#include "../../UIs/Dialogs/InstantDialog.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"

#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;


extern PlayerState *g_playerState;
extern float sizeMultiplier;
extern float margin;

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

AbilityUpgradeDialog *AbilityUpgradeDialog::createWithEntity(GDAbilityVO avo_, GDAbilityVO nxt_avo_) {
    
    AbilityUpgradeDialog *pRet = new AbilityUpgradeDialog();
    pRet->initWithEntity(NULL);
    if (!pRet->compose(avo_, nxt_avo_)) {
        return NULL;
    }
    pRet->autorelease();
    return pRet;
    
}

bool AbilityUpgradeDialog::compose(GDAbilityVO avo_, GDAbilityVO nxt_avo_) {
    
    if (avo_.type == "") {
        return false;
    }
    
    CREATE_CHAR_BUFFER(_buf1, 128);
    CREATE_CHAR_BUFFER(_buf2, 64);
    
    m_nxt_avo = nxt_avo_;
    
    m_bg->setContentSize(CCSizeMake(kMiniDialogSize2.width*sizeMultiplier, kMiniDialogSize2.height*sizeMultiplier*1.05f));
    m_block_sprite->setContentSize(m_bg->getContentSize());
    setContentSize(m_bg->getContentSize());
    
    update(0);
    
    m_entity = (void*)g_playerState->getAbilityForKey(avo_.type);
    Building* _library = g_playerState->getBuilding(kBuildingLibrary, 0);
    
    if (m_nxt_avo.level > 1) {
        snprintf(_buf1, _buf1_size, g_gameContent->Word["upgrade_to_level"].c_str(), m_nxt_avo.level);
    } else {
        snprintf(_buf1, _buf1_size, g_gameContent->Word["locked_ability_title"].c_str(), m_nxt_avo.title.c_str());
    }
    //toUpper
    char const* buffer = UIDisplayHelper::stringToUpperCase(_buf1).c_str();
    snprintf(_buf1, _buf1_size, "%s", buffer);
    
    CCLabelBMFont *_title = CCLabelHelper::createBMFont(_buf1, StyleMiniDialogTitle);
    _title->setPositionY(this->getContentSize().height*0.5f-_title->getContentSize().height*0.85f);
    addChild(_title);
    
    auto pos = nxt_avo_.src.find_first_of("12345");
    CCAssert(pos != string::npos, "Invalid ability icon name!");
    int num = nxt_avo_.src[pos] - '0' - 1;
    UIHelper::UISprites abilitySprite = (UIHelper::UISprites)(UIHelper::TraceBuildingsOptionsPowerUpSmallIcon1 + num);

    CCPoint _tmp_pos = ccp(-getContentSize().width*0.32f, _title->getPositionY() -120*sizeMultiplier);
    snprintf(_buf1, _buf1_size, "%0.1f", avo_.stun_duration);
    snprintf(_buf2, _buf2_size, "+%0.1f", nxt_avo_.stun_duration - avo_.stun_duration);
    addContent(UIHelper::getSpritePath(abilitySprite), _tmp_pos, 1.0f, g_gameContent->Word["stun_duration"].c_str(), _buf1, _buf2);

    _tmp_pos = _tmp_pos + ccp(0.0f, -150*sizeMultiplier);
    snprintf(_buf1, _buf1_size, "%d", g_playerState->player.racing.exp);
    snprintf(_buf2, _buf2_size, "+%d", nxt_avo_.village_exp_gain);
    addContent(UIHelper::getSpritePath(UIHelper::UpgradeRanchIcon), _tmp_pos, g_gameContent->Word["experience_points"].c_str(), _buf1, _buf2);
    
    snprintf(_buf1, _buf1_size, "");
    
    if (_library->pbVo.status != BUILDING_STATUS_DELIVERED)
    {
        snprintf(_buf1, _buf1_size, g_gameContent->Word["library_is_not_complete_cannot_upgrade_ability_msg"].c_str(), _library->bVo.title.c_str());
    }
    else
    {
        if( Ability* _ability= g_playerState->getUpgradingAbility())
        {
            if (_ability->paVo.type == m_nxt_avo.type) {
                if(_ability->paVo.level>0){
                    snprintf(_buf1, _buf1_size, g_gameContent->Word["ability_is_being_upgraded"].c_str(), _ability->aVo.title.c_str(), _ability->paVo.level+1);
                    
                    snprintf(_buf2, _buf2_size, g_gameContent->Word["upgrading_to_level"].c_str(), nxt_avo_.level);
                    char const* buffer = UIDisplayHelper::stringToUpperCase(_buf2).c_str();
                    snprintf(_buf2, _buf2_size, "%s", buffer);
                    _title->setString(_buf2);
                }else{
                    snprintf(_buf1, _buf1_size, g_gameContent->Word["Researhing %s"].c_str(), _ability->aVo.title.c_str());
                }
                
            }else {
                snprintf(_buf1, _buf1_size, "%s", g_gameContent->Word["other_ability_is_being_upgraded_msg"].c_str());
            }
            
        }
    
    }
    
    if (strcmp(_buf1, "") == 0) {
        
        if (_library->pbVo.level < nxt_avo_.unlocks_at_level) {
            
            GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(_library->pbVo.type, nxt_avo_.unlocks_at_level);
            if (_nxt_bvo.type == "") {
                return false;
            }
            
            addRequirementMessage(_library, nxt_avo_);
            
        }else {
            
            UIDisplayHelper::convertToDisplayTime2(nxt_avo_.time, _buf1, _buf1_size);
            addUpgradeButton(nxt_avo_.cost_coins, _buf1);
            
        }
        
    }else {
        
        CCLabelBMFont *_detail = CCLabelHelper::createBMFont(_buf1,
                                                             kFontSizeSmall,
                                                             false,
                                                             kTextColorLightBrown,
                                                             getContentSize().width*0.8f,
                                                             kCCTextAlignmentLeft);
        
        _detail->setPosition(ccp(0, -(getContentSize().height*0.3f)));
        addChild(_detail);
        
    }
    
    return true;
    
}


void AbilityUpgradeDialog::addInstantButton(int value_) {
    
    float _spacing = getContentSize().height*0.15f;
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "%d ", value_);
    
    //Upgrade Button
    CCSprite *_blue = UIHelper::getSprite(UIHelper::UpgradeInstantBuyButton);
    
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(g_gameContent->Word["upgrade_button_label"].c_str(), kFontSizeMedium-5.0f-2.0f, true);
    _label->setPosition(ccp(_blue->getContentSize().width*0.5f, _blue->getContentSize().height*0.75f));
    _blue->addChild(_label);
    CCSprite *_icon = UIHelper::getSprite(UIHelper::NSClaimGemBlue);
    _icon->setAnchorPoint(ccp(1.0f, 0.0f));
    _icon->setPosition(ccp(_blue->getContentSize().width, 0));
    _blue->addChild(_icon);
     snprintf(_buffer, _buffer_size, "%d ",  value_ );
    CCLabelBMFont *_value = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, true);
    _value->setAnchorPoint(ccp(1.0f, _value->getAnchorPoint().y));
    _value->setPosition(ccp(_icon->getPositionX()-_icon->getContentSize().width, _blue->getContentSize().height*0.30f));
    _blue->addChild(_value);

    CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_blue, this, menu_selector(AbilityUpgradeDialog::onInstant));
    CCMenu *_button_upgrade = CCMenu::create(_menuItem, NULL);
    _button_upgrade->setPosition((getContentSize()*-0.5f)+(_blue->getContentSize()*0.5)+ccp(_spacing*0.4f, _spacing*0.4f));
    _button_upgrade->setContentSize(_blue->getContentSize());
    _button_upgrade->setTouchPriority(-2000);
    addChild(_button_upgrade);
    
    CCLabelBMFont *_instant_label = CCLabelHelper::createBMFont(g_gameContent->Word["instant_button_label"].c_str(), kFontSizeSmall, kTextColorLightBrown);
    _instant_label->setAnchorPoint(ccp(0.0f, 0.5f));
    addChild(_instant_label);
    
    CCSprite *_instant_icon = UIHelper::getSprite(UIHelper::UpgradeEnergyIcon);
    _instant_icon->setAnchorPoint(ccp(0.0f, 0.5f));
    addChild(_instant_icon);

    UIDisplayHelper::centerItems(_blue, _instant_icon, _instant_label, 10, 0.3f);
    
    float _label_width = _instant_label->getContentSize().width +_instant_icon->getContentSize().width;
    _instant_label->setPosition(_button_upgrade->getPosition()+
                                ccp(-(_button_upgrade->getContentSize().width*0.5f)+
                                    (_button_upgrade->getContentSize().width-_label_width)*0.5f,
                                    _button_upgrade->getContentSize().height*0.7f));
    _instant_icon->setPosition(ccp(_instant_label->getPositionX()+_instant_label->getContentSize().width, _instant_label->getPositionY()));
}

void AbilityUpgradeDialog::addUpgradeButton(int value_, const char *top_label_)
{
    float _spacing = getContentSize().height*0.15f;
    CREATE_CHAR_BUFFER(_buffer, 64);
//    snprintf(_buffer, _buffer_size, "%d ", value_);
    
    //Upgrade Button
    CCSprite *_button_sprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(g_gameContent->Word["upgrade_button_label"].c_str(), kFontSizeMedium-5.0f-2.0f, true);
    _label->setPosition(ccp(_button_sprite->getContentSize().width*0.5f, _button_sprite->getContentSize().height*0.75f));
    _button_sprite->addChild(_label);
    CCSprite *_icon = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);
    _icon->setAnchorPoint(ccp(1.0f, 0.0f));
    _icon->setPosition(ccp(_button_sprite->getContentSize().width, 0));
    _button_sprite->addChild(_icon);
//    sprintf(buff, "%d ",  value_ );
    CCLabelBMFont *_value = CCLabelHelper::createBMFont("", kFontSizeMedium-5.0f, true);
    _value->setString(CCStringHelper::createWithNumberFormat(value_).c_str());
    _value->setPosition(ccp(_button_sprite->getContentSize().width*0.5f, _button_sprite->getContentSize().height*0.30f));
    _button_sprite->addChild(_value);
    
    CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_button_sprite, this, menu_selector(AbilityUpgradeDialog::onUpgrade));
    CCMenu *_button_upgrade = CCMenu::create(_menuItem, NULL);
//    _button_upgrade->setPosition(ccp(getContentSize().width*0.5f,-getContentSize().height*0.5f)+ccp(-_button_sprite->getContentSize().width*0.5f, _button_sprite->getContentSize().height*0.5f)+ccp(-_spacing*0.4f, _spacing*0.4f));
    _button_upgrade->setPosition(ccp(0.0f,-getContentSize().height*0.5f)+ccp(0.0f, _button_sprite->getContentSize().height*0.5f)+ccp(0.0f, _spacing*0.4f));
    
    _button_upgrade->setContentSize(_button_sprite->getContentSize());
    _button_upgrade->setTouchPriority(-2000);
    addChild(_button_upgrade);
    
    if (g_playerState->player.balance.gold < m_nxt_avo.cost_coins) {
        _value->setColor(kTextColorOrange2);
    }

    UIDisplayHelper::centerItems(_button_sprite, _icon, _value, 10, 0.3f);

    snprintf(_buffer, _buffer_size, "%s", top_label_);
    CCLabelBMFont *_instant_label = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, kTextColorLightBrown);
    _instant_label->setAnchorPoint(ccp(0.0f, 0.5f));
    addChild(_instant_label);

    CCSprite *_instant_icon = UIHelper::getSprite(UIHelper::UpgradeClockIcon);
    _instant_icon->setAnchorPoint(ccp(0.0f, 0.5f));
    addChild(_instant_icon);

    float _label_width = _instant_label->getContentSize().width +_instant_icon->getContentSize().width;
    _instant_icon->setPosition(_button_upgrade->getPosition()+ccp(-(_button_upgrade->getContentSize().width*0.5f)+(_button_upgrade->getContentSize().width-_label_width)*0.5f, _button_upgrade->getContentSize().height*0.7f));
    _instant_label->setPosition(ccp(_instant_icon->getPositionX()+_instant_icon->getContentSize().width + 10 * sizeMultiplier, _instant_icon->getPositionY()));
}

void AbilityUpgradeDialog::addRequirementMessage(Building* building_, GDAbilityVO avo_) {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    if (building_) {
            if(m_nxt_avo.level!=1){
            snprintf(_buffer, _buffer_size, g_gameContent->Word["required_hq_level_to_upgrade"].c_str(), building_->bVo.title.c_str(), avo_.unlocks_at_level);
        }else{
        snprintf(_buffer, _buffer_size, g_gameContent->Word["locked_ability_description"].c_str(), building_->bVo.title.c_str(), avo_.unlocks_at_level);
        }
        
    }else {
        GDBuildingVO _building_vo = g_gameContent->getBuildingVO(kBuildingLibrary, avo_.unlocks_at_level);
        
  if(m_nxt_avo.level!=1){
            snprintf(_buffer, _buffer_size, g_gameContent->Word["required_hq_level_to_upgrade"].c_str(), _building_vo.title.c_str(), avo_.unlocks_at_level);
        }else{
            snprintf(_buffer, _buffer_size, g_gameContent->Word["locked_ability_description"].c_str(), _building_vo.title.c_str(), avo_.unlocks_at_level);
        }
    }
    
    CCLabelBMFont *_detail = CCLabelHelper::createBMFont(_buffer,
                                                         kFontSizeSmall,
                                                         false,
                                                         kTextColorLightBrown,
                                                         getContentSize().width*0.8f,
                                                         kCCTextAlignmentLeft);
    
    _detail->setPosition(ccp(0, -(getContentSize().height*0.3f)));
    addChild(_detail);
    
}

void AbilityUpgradeDialog::onInstant(CCObject *sender_) {
    
    CCLog_("onInstant upgrade ");
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    close();

    if (Building* _library = g_playerState->getBuilding(kBuildingLibrary, 0)) {
        BackendService::getInstance()->send(APIAbilityUpdate::create(_library,
                                                                     g_playerState->Abilities["dash_forward"],
                                                                     true,
                                                                     this,
                                                                     AbilityUpgradeDialog::onUpdated));
    }

}

void AbilityUpgradeDialog::onUpgrade(CCObject *sender_) {
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    SceneVille* _scene = (SceneVille*)g_currentScene;
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    Ability* _currently_upgraded_ability = g_playerState->getUpgradingAbility();
    if(_currently_upgraded_ability) {
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        if(_currently_upgraded_ability->paVo.level==0){
            snprintf(_buffer, _buffer_size, g_gameContent->Word["Researhing %s"].c_str(), _currently_upgraded_ability->aVo.title.c_str());
        }else{
            snprintf(_buffer, _buffer_size, g_gameContent->Word["ability_is_being_upgraded"].c_str(), _currently_upgraded_ability->aVo.title.c_str(), _currently_upgraded_ability->paVo.level+1);
        }
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, _buffer);
        
        return;
    }
    
    if (g_playerState->player.balance.gold < m_nxt_avo.cost_coins) {
        if (_scene->modalController->getParent()) {
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            if (InstantDialog *_dialog = InstantDialog::create(_scene->modalController, m_entity , (void*)_currently_upgraded_ability)) {
                _dialog->setPosition(_screenSize*0.5f);
                _scene->modalController->pushView(_dialog);
                close();
            }
        }
        return;
    }
    
    if (Building* _library = g_playerState->getBuilding(kBuildingLibrary, 0)) {
        
        CCDictionary *params = CCDictionary::create();
        
        CSJson::Value eventParameters;
        eventParameters["abType"] = m_nxt_avo.type; //Current Ability Type
        eventParameters["toAbLevel"] = m_nxt_avo.level+1; //Ability Level to be upgraded to
        eventParameters["abCoin"] = m_nxt_avo.cost_coins;
        
        params->setObject(ccs(AnalyticsClient::getInstance()->write(eventParameters)), "upgradeAbilityInfo");
        
        AnalyticsClient::getInstance()->logEvent(EVENT_UPGRADE_ABILITY, eventParameters);
        
        FlurryCpp::logEvent(EVENT_UPGRADE_ABILITY, params);
        
        FBAppEventsWrapper::logEvent(EVENT_UPGRADE_ABILITY);
        
        BackendService::getInstance()->send(APIAbilityUpgrade::create(_library,
                                                                      m_nxt_avo.type,
                                                                      false,
                                                                      _scene->isoMap,
                                                                      IsoMapBuilding::onCoinShouldBeating));
    }
    
    close();
    
    _scene->modalController->closeAll();
    
}

void AbilityUpgradeDialog::onUpdated(void *sender_, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parseUserRaceData(_root);
    _playerState->parseAbilityData(_root);
    Json_dispose(_root);
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    
    SceneVille *_sceneVille = (SceneVille*)g_currentScene;
    _sceneVille->hudPanel->updateResourceBar(HUDPanelBarCoins);

}
