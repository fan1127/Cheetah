//
//  InstantDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/21/2557 BE.
//
//

#include "InstantDialog.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "../../Scenes/SceneVille.h"
#include "../../Scenes/SceneWorldMap.h"
#include "../../Scenes/SceneMatch.h"
extern CCScene* g_currentScene;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APITreeRemove.h"
#include "../../Services/APIs/APIPetWakeUp.h"
#include "../../Services/APIs/APIAbilityUpgrade.h"
#include "../../Services/APIs/APIAbilityUpdate.h"
#include "../../Services/APIs/APIMapCloudOpen.h"
#include "../../Services/APIs/APIGemExchange.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern float isIPhoneMultiplier;

#include "../../Helpers/CCStringHelper.h"

#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"

#include "../../Helpers/CCSpriteHelper.h"
#include "MiniDialog.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../UIs/Dialogs/IAPDialog.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../VOs/GDWorldCloudVO.h"
#include "../../UIs/Dialogs/AnimalTradeDialog.h"
#include "../../UIs/Dialogs/CutTreeDialog.h"
#include "../../Helpers/FlurryCpp.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"

#include "../../../extensions/CCArmature/external_tool/Json/lib_json/value.h"
#include "../../../extensions/CCArmature/external_tool/Json/lib_json/writer.h"

#define kInstantDialogSize cocos2d::CCSize(1000.0f, 700.0f)

InstantDialog* InstantDialog::createWithType(CCModalController *parentModalController_, InstantType type_, void *entity_, void* optional_entity_) {

    InstantDialog* pRet = new InstantDialog();
    pRet->m_type = type_;
    if (!pRet->init(parentModalController_, entity_, optional_entity_, kInstantDialogSize)) {
        pRet->release();
        return NULL;
    }
    pRet->autorelease();
    return pRet;
    
}

InstantDialog* InstantDialog::createWithBuildingVO(CCModalController *parentModalController_, GDBuildingVO bvo_) {

    InstantDialog* pRet = new InstantDialog();
    pRet->m_type = IT_UNKNOWN;
    pRet->initWithBuildingVO(parentModalController_, bvo_, kInstantDialogSize);
    pRet->autorelease();
    return pRet;
    
}

InstantDialog* InstantDialog::create(CCModalController* parentModalController_, void* entity_) {
    
    if (InstantDialog *pRet = new InstantDialog()) {
        pRet->m_type = IT_UNKNOWN;
        if (pRet->init(parentModalController_, entity_, NULL, kInstantDialogSize)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
}

InstantDialog* InstantDialog::create(CCModalController* parentModalController_, void* entity_, void* optional_entity_) {
    
    if (InstantDialog *pRet = new InstantDialog()) {
        pRet->m_type = IT_UNKNOWN;
        if (pRet->init(parentModalController_, entity_, optional_entity_, kInstantDialogSize)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
    
}

InstantDialog* InstantDialog::createWithResourceType(CCModalController* parentModalController_, RESOURCE_TYPE type_, int amount_, CCSize size) {
    
    if (InstantDialog *pRet = new InstantDialog()) {
        
        if (type_ == RESOURCE_TYPE_COIN) {
            
            pRet->m_type = IT_BUYCOIN_WITHGEMS;
            
        } else if (type_ == RESOURCE_TYPE_FOOD) {
            
            pRet->m_type = IT_BUYFOOD_WITHGEMS;
        }
        
        if (pRet->initWithResourceType(parentModalController_, type_, amount_, size)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
}

bool InstantDialog::initWithResourceType(CCModalController* parentModalController_, RESOURCE_TYPE type_, int amount_, CCSize size) {
    
    if (!SimpleDialog::init(parentModalController_, size))
    {
        return false;
    }
    
    m_entity = NULL;
    m_entity_optional = NULL;
    
    m_tap_blackbg_to_popview = true;
    
    m_missing_resource_amount = amount_;
    
    return compose();
}

bool InstantDialog::initWithBuildingVO(CCModalController *parentModalController_, GDBuildingVO bvo_, CCSize size) {
    
    if (!SimpleDialog::init(parentModalController_, size))
    {
        return false;
    }
    
    m_entity = NULL;
    m_entity_optional = NULL;
    m_bvo = bvo_;
    
    m_missing_resource_amount = 0;
    
    return compose();
    
}

bool InstantDialog::init(CCModalController* parentModalController_, void* entity_, void* optional_entity_, CCSize size)
{
    if (!SimpleDialog::init(parentModalController_, size))
    {
        return false;
    }
    
    m_entity = entity_;
    m_entity_optional = optional_entity_;
    
    m_missing_resource_amount = 0;

    return compose();
    
}

bool InstantDialog::compose() {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    CREATE_CHAR_BUFFER(_buffer2, 128);

    Building *_building =  NULL;
    Building *_building_optional = NULL;
    
    if (m_type == IT_UNKNOWN) {
        
        if (m_entity == NULL) {
            
            if (m_bvo.type == "") {
                return false;
            }else {
                m_missing_resource_amount = m_bvo.cost_coins - g_playerState->player.balance.gold;
                m_type = IT_BUILD_BUYRESOURCE;
            }
            
        }else {
            CCLog_("----------------------");

            if (typeid(*(Building*)m_entity) == typeid(Building)) { //Building
            
                _building = (Building*)m_entity;
                
                    m_bvo = g_gameContent->getBuildingVO(_building->pbVo.type, _building->pbVo.level+1);
                    m_missing_resource_amount = m_bvo.cost_coins - g_playerState->player.balance.gold;
                    
                    _building_optional = (Building*)m_entity_optional;
                    if (_building_optional) {
                        if (_building_optional->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
                            if (_building->pbVo.buid == "") {
                                if (g_playerState->player.balance.gold < _building->bVo.cost_coins) {
                                    m_type = IT_FINISHCURRENTBUILD_AND_BUILD_BUYRESOURCE;
                                    m_missing_resource_amount = _building->bVo.cost_coins - g_playerState->player.balance.gold;
                                }else {
                                    m_type = IT_FINISHCURRENTBUILD_AND_BUILD;
                                }
                            }else {
                                if (g_playerState->player.balance.gold < m_bvo.cost_coins) {
                                    m_type = IT_FINISHCURRENTBUILD_AND_UPGRADE_BUYRESOURCE;
                                }else {
                                    m_type = IT_FINISHCURRENTBUILD_AND_UPGRADE;
                                }
                            }
                        } else if (_building_optional->pbVo.status == BUILDING_STATUS_UPGRADE) {
                            if (_building->pbVo.buid == "") {
                                if (g_playerState->player.balance.gold < _building->bVo.cost_coins) {
                                    m_type = IT_FINISHCURRENTUPGRADE_AND_BUILD_BUYRESOURCE;
                                    m_missing_resource_amount = _building->bVo.cost_coins - g_playerState->player.balance.gold;
                                }else {
                                    m_type = IT_FINISHCURRENTUPGRADE_AND_BUILD;
                                }
                            }else {
                                if (g_playerState->player.balance.gold < m_bvo.cost_coins) {
                                    m_type = IT_FINISHCURRENTUPGRADE_AND_UPGRADE_BUYRESOURCE;
                                }else {
                                    m_type = IT_FINISHCURRENTUPGRADE_AND_UPGRADE;
                                }
                            }
                        }
                    }else {
                        if (_building->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
                            m_type = IT_FINISH_BUILD;
                        }else if (_building->pbVo.status == BUILDING_STATUS_UPGRADE) {
                            m_type = IT_FINISH_UPGRADE;
                        }else if (_building->pbVo.status == BUILDING_STATUS_DELIVERED) {
                            if (_building->getTag() == 1) {
                                m_type = IT_UPGRADE_BUYRESOURCE;
                            }else {
                                m_type = IT_INSTANT_UPGRADE;
                            }
                        }
                        
                    }
                
            }else if (typeid(*(Ability*)m_entity) == typeid(Ability)) { // Ability
                
                if (Ability *_ability = (Ability*)m_entity) {
                    m_type = IT_UPGRADE_ABILITY_BUYRESOURCE;
                    GDAbilityVO _nxt_avo = g_gameContent->Abilites[_ability->aVo.type][_ability->aVo.level];
                    m_missing_resource_amount = _nxt_avo.cost_coins - g_playerState->player.balance.gold;
                    CCLog_("price :%d , gold :%d",_nxt_avo.cost_coins,g_playerState->player.balance.gold);
                }
                
                
            }
            else if(typeid(*(Decoration*)m_entity) == typeid(Decoration)){
                if (Decoration *decoration = (Decoration*)m_entity) {
                    m_type = IT_CUTTREE_BUYRESOURCE;
                    m_missing_resource_amount = decoration->dVo.cost_coins - g_playerState->player.balance.gold;
                    CCLog_("price :%d , gold :%d",decoration->dVo.cost_coins,g_playerState->player.balance.gold);
                }
                
            }
        }
    }
    
    //set title
    switch (m_type) {
        case IT_INSTANT_UPGRADE:
        case IT_FINISH_BUILD:
        case IT_FINISH_UPGRADE:
        case IT_FINISH_ABILITY: {
            snprintf(_buffer, _buffer_size, "%s?", g_gameContent->Word["finish_now"].c_str());
        }break;
        
        case IT_INSTANT_ENERGY: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["wakeup_now"].c_str());
        }break;
        case IT_FINISHCURRENTBUILD_AND_UPGRADE:
        case IT_FINISHCURRENTBUILD_AND_BUILD:
        case IT_FINISHCURRENTUPGRADE_AND_UPGRADE:
        case IT_FINISHCURRENTUPGRADE_AND_BUILD: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["finish_now_and_build_title"].c_str());
            snprintf(_buffer2, _buffer2_size, _buffer, g_gameContent->Word["uncle_name"].c_str());
            snprintf(_buffer, _buffer_size, "%s", _buffer2);
        }break;
        case IT_FINISHCURRENTBUILD_AND_BUILD_BUYRESOURCE:
        case IT_FINISHCURRENTUPGRADE_AND_BUILD_BUYRESOURCE:
        case IT_FINISHCURRENTBUILD_AND_UPGRADE_BUYRESOURCE:
        case IT_FINISHCURRENTUPGRADE_AND_UPGRADE_BUYRESOURCE: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["finish_now_and_build_with_resources_title"].c_str());
            snprintf(_buffer2, _buffer2_size, _buffer, g_gameContent->Word["uncle_name"].c_str());
            snprintf(_buffer, _buffer_size, "%s", _buffer2);
        }break;
        case IT_CLOUD_BUYRESOURCE: {
            GDWorldCloudVO cloud_vo = (*(GDWorldCloudVO*)m_entity);
            m_missing_resource_amount = cloud_vo.price - g_playerState->player.balance.gold;
            m_param_1 = cloud_vo.group;
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["missing_resources_title"].c_str());
        }break;
        case IT_FUSE_ANIMAL_BUYRESOURCE: {
            m_pet_vo = (*(GDPetVO*)m_entity);
            m_missing_resource_amount = m_pet_vo.coins_needed_for_fusion - g_playerState->player.balance.gold;
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["missing_resources_title"].c_str());
        }break;
        case IT_EVOLVE_ANIMAL_BUYRESOURCE: {
            if (Pet* _pet = (Pet*)m_entity) {
                m_missing_resource_amount = _pet->pVo.coins_needed_to_evolve - g_playerState->player.balance.gold;
                snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["missing_resources_title"].c_str());
            }
        }break;
        case IT_CUTTREE_BUYRESOURCE:
        case IT_UPGRADE_ABILITY_BUYRESOURCE:
        case IT_BUILD_BUYRESOURCE:
        case IT_UPGRADE_BUYRESOURCE: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["missing_resources_title"].c_str());
        }break;
        case IT_DECORATION_BUYRESOURCE: {
            if (Decoration* _decoration = (Decoration*)m_entity) {
                m_missing_resource_amount = _decoration->dVo.cost_coins - g_playerState->player.balance.gold;
                snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["missing_resources_title"].c_str());
            }
        }break;
        case IT_BUYCOIN_WITHGEMS: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["buy_coin_title"].c_str());
        }break;
        case IT_BUYFOOD_WITHGEMS: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["buy_food_title"].c_str());
        }break;
        case IT_BUYHABITAT_WITHGEMS: {
            Building *habitat = (Building*)m_entity;
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["buy_habitat_title"].c_str());
            if (strcmp(_buffer, "") == 0) {
                snprintf(_buffer, _buffer_size, "Buy %s?", habitat->bVo.title.c_str());
            }
        }break;
        case IT_ENTER_GEM_SHOP: {
            snprintf(_buffer, _buffer_size, "%s", "Not Enough Gems");
        }break;
        case IT_UNKNOWN:
        default:
            return false;
            break;
    }

    createTitle(_buffer);

    switch (m_type) {
        case IT_INSTANT_UPGRADE: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["instant_upgrade_msg"].c_str());
            createDescription(_buffer);
        }break;
            
        case IT_INSTANT_ENERGY: {
            createDescription(g_gameContent->Word["instant_energy_msg"].c_str());
        }break;
        case IT_FINISH_BUILD: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["finish_build_now_msg"].c_str());
            snprintf(_buffer2, _buffer2_size, _buffer, _building->bVo.title.c_str());
            snprintf(_buffer, _buffer_size, "%s", _buffer2);
            createDescription(_buffer);
        }break;
        case IT_FINISH_UPGRADE:
        case IT_FINISH_ABILITY: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["finish_upgrade_now_msg"].c_str());
            createDescription(_buffer);
        }break;
        case IT_FINISHCURRENTBUILD_AND_UPGRADE:
        case IT_FINISHCURRENTBUILD_AND_BUILD:
        case IT_FINISHCURRENTUPGRADE_AND_UPGRADE:
        case IT_FINISHCURRENTUPGRADE_AND_BUILD: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["finish_now_and_build_msg"].c_str());
            snprintf(_buffer2, _buffer2_size, _buffer, g_gameContent->Word["uncle_name"].c_str(), g_gameContent->Word["uncle_name"].c_str());
            snprintf(_buffer, _buffer_size, "%s", _buffer2);
            createDescription(_buffer);
        }break;
        case IT_FINISHCURRENTBUILD_AND_BUILD_BUYRESOURCE:
        case IT_FINISHCURRENTUPGRADE_AND_BUILD_BUYRESOURCE:
        case IT_FINISHCURRENTBUILD_AND_UPGRADE_BUYRESOURCE:
        case IT_FINISHCURRENTUPGRADE_AND_UPGRADE_BUYRESOURCE: {
            
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["finish_now_and_build_with_resources_msg"].c_str());
            createDescription(_buffer);
            createIconLine(StringHelper::toString(m_missing_resource_amount).c_str(), SimpleDialog::Coin);

        }break;
        case IT_CLOUD_BUYRESOURCE:
        case IT_FUSE_ANIMAL_BUYRESOURCE:
        case IT_EVOLVE_ANIMAL_BUYRESOURCE:
        case IT_CUTTREE_BUYRESOURCE:
        case IT_UPGRADE_ABILITY_BUYRESOURCE:
        case IT_BUILD_BUYRESOURCE:
        case IT_UPGRADE_BUYRESOURCE:
        case IT_DECORATION_BUYRESOURCE: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["buy_missing_resources_msg"].c_str());
            createDescription(_buffer);
            createIconLine(StringHelper::toString(m_missing_resource_amount).c_str(), SimpleDialog::Coin);
        }break;
        case IT_BUYCOIN_WITHGEMS:
        case IT_BUYFOOD_WITHGEMS: {
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["buy_resources_msg"].c_str());
            createDescription(_buffer);

            auto resType = SimpleDialog::Coin;
            if (m_type == IT_BUYCOIN_WITHGEMS)
            {
            }
            else
            {
                resType = SimpleDialog::Food;
            }

            createIconLine(StringHelper::toString(m_missing_resource_amount).c_str(), resType);
        }
        break;
        case IT_ENTER_GEM_SHOP :{
        snprintf(_buffer, _buffer_size, "%s", "Would you like to get more?");
            createDescription(_buffer);
        }break;
        default:
            break;
    }

    //set button
    switch (m_type) {
        case IT_INSTANT_UPGRADE: {
            addInstantButton(g_gameContent->cal_instant_building_cost(m_bvo));
        }break;
            
        case IT_INSTANT_ENERGY: {
            addInstantButton((Pet*)m_entity);
        }break;
        case IT_FINISH_BUILD:
        case IT_FINISH_UPGRADE: {
            if (_building) {
                addInstantButton(_building);
            }
        }break;
        case IT_FINISH_ABILITY: {
            addInstantButton((Ability*)m_entity);
        }break;
        case IT_FINISHCURRENTBUILD_AND_UPGRADE:
        case IT_FINISHCURRENTBUILD_AND_BUILD:
        case IT_FINISHCURRENTUPGRADE_AND_UPGRADE:
        case IT_FINISHCURRENTUPGRADE_AND_BUILD: {
            if (_building_optional) {
                addInstantButton(_building_optional);
            }
        }break;
        case IT_FINISHCURRENTBUILD_AND_BUILD_BUYRESOURCE:
        case IT_FINISHCURRENTUPGRADE_AND_BUILD_BUYRESOURCE:
        case IT_FINISHCURRENTBUILD_AND_UPGRADE_BUYRESOURCE:
        case IT_FINISHCURRENTUPGRADE_AND_UPGRADE_BUYRESOURCE: {
            if (_building_optional) {
                addInstantButton(g_gameContent->get_gemcal_for_building_finishnow(_building_optional->getSecondsLeftToFinish())+g_gameContent->get_gemcal_for_coins(m_missing_resource_amount));
            }
        }break;
        case IT_CLOUD_BUYRESOURCE:
        case IT_FUSE_ANIMAL_BUYRESOURCE:
        case IT_EVOLVE_ANIMAL_BUYRESOURCE:
        case IT_CUTTREE_BUYRESOURCE:
        case IT_BUILD_BUYRESOURCE:
        case IT_UPGRADE_BUYRESOURCE:
        case IT_UPGRADE_ABILITY_BUYRESOURCE:
        case IT_DECORATION_BUYRESOURCE: {
            addInstantButton(g_gameContent->get_gemcal_for_coins(m_missing_resource_amount));
        }break;
        case IT_BUYCOIN_WITHGEMS: {
            addInstantButton(g_gameContent->get_gemcal_for_coins(m_missing_resource_amount));
        }break;
        case IT_BUYFOOD_WITHGEMS: {
            addInstantButton(g_gameContent->get_gemcal_for_foods(m_missing_resource_amount));
        }break;
        case IT_BUYHABITAT_WITHGEMS: {
            Building *habitat = (Building*)m_entity;
            int cost_gems = int(habitat->bVo.param_2);
            addInstantButton(cost_gems);
        }break;
        case IT_ENTER_GEM_SHOP :{
            addInstantButton("Enter Shop");
        }break;
        default:
            break;
    }

    autoResize(300*sizeMultiplier, 200*sizeMultiplier);

    return true;
}


void InstantDialog::addInstantButton(int value_) {

    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "%d ", value_);
    
    CCLabelBMFont *_label = CCLabelHelper::createBMFont( _buffer, StyleButtonCoinAndGems);
    _label->setAnchorPoint(ccp(0.0f, 0.5f));
    
    CCLabelUpdater *_updater = CCLabelUpdater::create();
    
    _updater->addLabel(_label);
    _updater->labelValue = value_; //set a value here for proper CCLabelUpdater structure
    
    _updater->building = NULL;
    _updater->pet = NULL;
    _updater->ability = NULL;
    
    _updater->updateIntFromBuilding = NULL;
    _updater->updateIntFromPet = NULL;
    _updater->updateIntFromAbility = NULL;
    
    _updater->callbackSender = NULL;
    _updater->callbackOnceWhenZero = NULL;
    
    this->addInstantButton(_updater);
}

void InstantDialog::addInstantButton(Building *building_)
{
    CREATE_CHAR_BUFFER(_buffer, 64);
    int gemval = EntityAction::getGemCost(building_);
    snprintf(_buffer, _buffer_size, "%d", gemval);
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(_buffer, StyleButtonCoinAndGems);
    _label->setAnchorPoint(ccp(0.0f, 0.5f));
    CCLabelUpdater *_updater = CCLabelUpdater::createWithLabel(_label,
                                                               building_,
                                                               EntityAction::getGemCost);
    _updater->callbackSender = this;
    _updater->callbackOnceWhenZero = InstantDialog::closeAllDialogs;
    this->addInstantButton(_updater);
}

void InstantDialog::addInstantButton(Pet *pet_)
{
    CREATE_CHAR_BUFFER(_buffer, 64);
    int gemval = EntityAction::getGemCost(pet_);
    snprintf(_buffer, _buffer_size, "%d", gemval);
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(_buffer, StyleButtonCoinAndGems);
    _label->setAnchorPoint(ccp(0.0f, 0.5f));
    CCLabelUpdater *_updater = CCLabelUpdater::createWithLabel(_label,
                                                               pet_,
                                                               EntityAction::getGemCost);
    _updater->callbackSender = this;
    _updater->callbackOnceWhenZero = InstantDialog::closeAllDialogs;
    this->addInstantButton(_updater);
}

void InstantDialog::addInstantButton(std::string value_)
{
    
    createButton(value_.c_str(), SimpleDialog::Center , SimpleDialog::None);
    
}

void InstantDialog::addInstantButton(Ability *ability_)
{
    CREATE_CHAR_BUFFER(_buffer, 64);
    int gemval = EntityAction::getGemCost(ability_);
    snprintf(_buffer, _buffer_size, "%d", gemval);
    CCLabelBMFont *_label = CCLabelHelper::createBMFont(_buffer, StyleButtonCoinAndGems);
    _label->setAnchorPoint(ccp(0.0f, 0.5f));
    CCLabelUpdater *_updater = CCLabelUpdater::createWithLabel(_label,
                                                               ability_,
                                                               EntityAction::getGemCost);
    _updater->callbackSender = this;
    _updater->callbackOnceWhenZero = InstantDialog::closeAllDialogs;
    this->addInstantButton(_updater);
}

void InstantDialog::addInstantButton(CCLabelUpdater* updater_)
{
    createButton(updater_, SimpleDialog::Center, SimpleDialog::Gem);
}

void InstantDialog::launchGemsStore()
{
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    
    if (InstantDialog *enterGemDialog = InstantDialog::createWithType(m_modalController, IT_ENTER_GEM_SHOP,m_entity,this )) {
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        enterGemDialog->setPosition(_screenSize*0.5f);
        m_modalController->pushView(enterGemDialog);
    }
}
void InstantDialog::enterGemsStore()
{
        g_gameAudio->playEffect(AUDIO_UI_CLICK);
    
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        IAPDialog *_dialog = IAPDialog::create(m_modalController, RESOURCE_TYPE_GEM);
        _dialog->setPosition(_screenSize*0.5f + kDialogIAPPosition);
        m_modalController->pushView(_dialog);
}
void InstantDialog::onButton(CCObject *sender_) {
    
    switch (m_type) {
        case IT_INSTANT_UPGRADE: {
            if (Building *_building = (Building*)m_entity) {
                
                GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(_building->pbVo.type, _building->pbVo.level+1);
                if (_nxt_bvo.type == "") {
                    CCLog_("Warning received empty BuildingVO!");
                    return;
                }
                int _cost_gems = g_gameContent->cal_instant_building_cost(_nxt_bvo);
                if (g_playerState->player.balance.gem < _cost_gems) {
                     
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_INSTANT_UPGRADE"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_INSTANT_UPGRADE";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["buildingType"] = _nxt_bvo.type;
                eventParameters["toBuildingLevel"] = _building->pbVo.level+1;
                eventParameters["coinCost"] = _nxt_bvo.cost_coins;
                eventParameters["buildTimeTotal"] = _nxt_bvo.time;
                eventParameters["gemCost"] = _cost_gems;
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_INSTANT_UPGRADE_BUILDING, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_INSTANT_UPGRADE_BUILDING, eventParameters);
                
                _building->upgradeState(BuildingUpgradeTypeInstant, _cost_gems);
                this->m_modalController->popView();
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            }
        }break;
        
        case IT_INSTANT_ENERGY: {
            
            if (Pet *_pet = (Pet*)m_entity) {
                int _cost_gems = g_gameContent->get_gemcal_for_pet_wakeup((_pet->getSecondsLeftToFullEnergy()));
                
                if (g_playerState->player.balance.gem < _cost_gems) {
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_INSTANT_ENERGY"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_INSTANT_ENERGY";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }
                if (_cost_gems > 0) {

                    
                    BackendService::getInstance()->send(APIPetWakeUp::create((Pet*)m_entity,
                                                                             this,
                                                                             InstantDialog::onDidInstant));
                    
                    CCDictionary *params = CCDictionary::create();
                    
                    CSJson::Value eventParameters;
                    eventParameters["petType"] = _pet->pVo.type;
                    eventParameters["petTime"] = _pet->getSecondsLeftToFullEnergy();
                    eventParameters["gemCost"] = _cost_gems;
                    
                    params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                    
                    FlurryCpp::logEvent(EVENT_GEM_WAKE_UP_PET, params);
                                                                             
                    AnalyticsClient::getInstance()->logEvent(EVENT_GEM_WAKE_UP_PET, eventParameters);
                    
                    this->m_modalController->closeAll();
                    g_gameAudio->playEffect(AUDIO_FINISH_NOW);
                }
            }
            
        }break;
        case IT_FINISH_BUILD:
        case IT_FINISH_UPGRADE: {
            
            if (Building *_building = (Building*)m_entity) {
                int _cost_gems = g_gameContent->get_gemcal_for_building_finishnow(_building->getSecondsLeftToFinish());
                if (g_playerState->player.balance.gem < _cost_gems) {
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_FINISH_BUILD/IT_FINISH_UPGRADE"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_FINISH_BUILD/IT_FINISH_UPGRADE";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }


                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["buildingType"] = _building->pbVo.type;
                eventParameters["toBuildingLevel"] = _building->pbVo.level+1;
                eventParameters["buildTimeLeft"] = _building->getSecondsLeftToFinish();
                eventParameters["gemCost"] = _cost_gems;
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_FINISH_NOW_BUILDING, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_FINISH_NOW_BUILDING, eventParameters);
                
                _building->finishNowState(this, InstantDialog::onDidInstant);
                
                this->m_modalController->closeAll();
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            }
            
        }break;
        case IT_FINISH_ABILITY: {
            
            CCLog_("IT_FINISH_ABILITY");
            if (Ability* _ability = (Ability*)m_entity) {
                int _cost_gems = g_gameContent->get_gemcal_for_ability_finishnow(_ability->getSecondsLeftToFinish());
                if (g_playerState->player.balance.gem < _cost_gems) {
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_FINISH_ABILITY"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_FINISH_ABILITY";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }
                
                BackendService::getInstance()->send(APIAbilityUpdate::create(g_playerState->getBuilding(kBuildingLibrary, 0),
                                                                             _ability,
                                                                             true,
                                                                             this,
                                                                             InstantDialog::onDidInstant));
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["abilityType"] = _ability->aVo.type;
                eventParameters["toAbilityLevel"] = _ability->paVo.level+1;
                eventParameters["abilityTimeLeft"] = _ability->getSecondsLeftToFinish();
                eventParameters["gemCost"] = _cost_gems;
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_FINISH_NOW_ABILITY, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_FINISH_NOW_ABILITY, eventParameters);
                
                this->m_modalController->closeAll();
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            }
            
        }break;
        case IT_FINISHCURRENTBUILD_AND_UPGRADE:
        case IT_FINISHCURRENTUPGRADE_AND_UPGRADE: {
            if (Building *_building = (Building*)m_entity) {
                
                GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(_building->pbVo.type, _building->pbVo.level+1);
                Building *_building_optional = (Building*)m_entity_optional;
                int _cost_gems = g_gameContent->get_gemcal_for_building_finishnow(_building_optional->getSecondsLeftToFinish());
                if (g_playerState->player.balance.gem < _cost_gems) {
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_FINISHCURRENTBUILD/UPGRADE_AND_UPGRADE"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_FINISHCURRENTBUILD/UPGRADE_AND_UPGRADE";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["buildingType"] = _building_optional->pbVo.type; //Current Building Type
                eventParameters["buildingToLevel"] = _building_optional->pbVo.level+1;
                eventParameters["buildingTimeLeft"] = _building_optional->getSecondsLeftToFinish();
                eventParameters["buildingGemCost"] = _cost_gems; //Gem Cost to Finish Current Building Gem Cost
                eventParameters["newBuildingType"] = _building->pbVo.type; //New Building Type
                eventParameters["newBuildingToLevel"] = _building->pbVo.level+1; //New Building Type
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_FINISH_NOW_BUILDING_TO_UPGRADE_ANOTHER, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_FINISH_NOW_BUILDING_TO_UPGRADE_ANOTHER, eventParameters);
                
                _building_optional->finishNowState(this, InstantDialog::onDidInstant);
                _building->upgradeState();
                this->m_modalController->popView();
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            }
        }break;
        case IT_FINISHCURRENTBUILD_AND_BUILD:
        case IT_FINISHCURRENTUPGRADE_AND_BUILD: {
            if (Building *_building = (Building*)m_entity) {
                
                SceneVille *_scene = (SceneVille*)g_currentScene;
                IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
                
                Building *_building_optional = (Building*)m_entity_optional;
                int _cost_gems = g_gameContent->get_gemcal_for_building_finishnow(_building_optional->getSecondsLeftToFinish());
                if (g_playerState->player.balance.gem < _cost_gems) {
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_FINISHCURRENTBUILD/UPGRADE_AND_BUILD"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_FINISHCURRENTBUILD/UPGRADE_AND_BUILD";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["buildingType"] = _building_optional->pbVo.type; //Current Building Type
                eventParameters["buildingToLevel"] = _building_optional->pbVo.level+1;
                eventParameters["buildingTimeLeft"] = _building_optional->getSecondsLeftToFinish();
                eventParameters["buildingGemCost"] = _cost_gems; //Gem Cost to Finish Current Building Gem Cost
                eventParameters["newBuildingType"] = _building->pbVo.type; //New Building Type
                eventParameters["newBuildingToLevel"] = 1; //Building new buildings are always at level 1
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_FINISH_NOW_BUILDING_TO_BUILD_ANOTHER, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_FINISH_NOW_BUILDING_TO_BUILD_ANOTHER, eventParameters);
                
                _building_optional->finishNowState(this, InstantDialog::onDidInstant);
                _isoMapBuilding->placingStuffFromPlaceHolder();
                this->m_modalController->popView();
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            }
        }break;
        case IT_FINISHCURRENTBUILD_AND_BUILD_BUYRESOURCE:
        case IT_FINISHCURRENTUPGRADE_AND_BUILD_BUYRESOURCE:{
            if (Building *_building = (Building*)m_entity) {
                
                SceneVille *_scene = (SceneVille*)g_currentScene;
                IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
                
                Building *_building_optional = (Building*)m_entity_optional;
                int _building_cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
                int _building_optional_cost_gems = g_gameContent->get_gemcal_for_building_finishnow(_building_optional->getSecondsLeftToFinish());
                int _cost_gems = _building_cost_gems + _building_optional_cost_gems;
                if (g_playerState->player.balance.gem < _cost_gems) {
                    
                    CCDictionary *ccDict = CCDictionary::create();
                    ccDict->setObject(CCString::create("IT_FINISHCURRENTBUILD/UPGRADE_AND_BUILD_BUYRESOURCE"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, ccDict);
                    
                    launchGemsStore();
                    return;
                }
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["buildingType"] = _building_optional->pbVo.type; //Current Building Type
                eventParameters["buildingToLevel"] = _building_optional->pbVo.level+1;
                eventParameters["buildingTimeLeft"] = _building_optional->getSecondsLeftToFinish();
                eventParameters["buildingGemCost"] = _building_optional_cost_gems; //Gem Cost to Finish Current Building Gem Cost
                eventParameters["newBuildingType"] = _building->pbVo.type; //New Building Type
                eventParameters["newBuildingToLevel"] = 1; //Building new buildings are always at level 1
                eventParameters["newBuildingCoinCost"] = _building->bVo.cost_coins; //New Building Coins Cost
                eventParameters["newBuildingMissingResources"] = m_missing_resource_amount; //New Building Missing Resources
                eventParameters["newBuildingGemCost"] = _building_cost_gems; //New Building Gem Cost
                eventParameters["totalGemCost"] = _cost_gems;
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_FINISH_NOW_BUILDING_TO_BUY_RESOURCES, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_FINISH_NOW_BUILDING_TO_BUY_RESOURCES, eventParameters);
                
                _building_optional->finishNowState(this, InstantDialog::onDidInstant);
                _isoMapBuilding->placingStuffFromPlaceHolder(false, BuildingBuildTypeBuyMissingResources);
                
                this->m_modalController->popView();
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            }
        }break;
        case IT_FINISHCURRENTBUILD_AND_UPGRADE_BUYRESOURCE:
        case IT_FINISHCURRENTUPGRADE_AND_UPGRADE_BUYRESOURCE: {
            if (Building *_building = (Building*)m_entity) {
                
                GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(_building->pbVo.type, _building->pbVo.level+1);
                Building *_building_optional = (Building*)m_entity_optional;
                int _building_cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
                int _building_optional_cost_gems = g_gameContent->get_gemcal_for_building_finishnow(_building_optional->getSecondsLeftToFinish());
                int _cost_gems = _building_cost_gems + _building_optional_cost_gems;
                if (g_playerState->player.balance.gem < _cost_gems) {
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_FINISHCURRENTBUILD/UPGRADE_AND_UPGRADE_BUYRESOURCE"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_FINISHCURRENTBUILD/UPGRADE_AND_UPGRADE_BUYRESOURCE";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["buildingType"] = _building_optional->pbVo.type; //Current Building Type
                eventParameters["buildingToLevel"] = _building_optional->pbVo.level+1;
                eventParameters["buildingTimeLeft"] = _building_optional->getSecondsLeftToFinish();
                eventParameters["buildingGemCost"] = _building_optional_cost_gems; //Gem Cost to Finish Current Building Gem Cost
                eventParameters["newBuildingType"] = _building->pbVo.type; //New Building Type
                eventParameters["newBuildingToLevel"] = _building->pbVo.level+1; //Building new buildings are always at level 1
                eventParameters["newBuildingCoinCost"] = _building->bVo.cost_coins; //New Building Coins Cost
                eventParameters["newBuildingMissingResources"] = m_missing_resource_amount; //New Building Missing Resources
                eventParameters["newBuildingGemCost"] = _building_cost_gems; //New Building Gem Cost
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_FINISH_NOW_BUILDING_TO_BUY_RESOURCES, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_FINISH_NOW_BUILDING_TO_BUY_RESOURCES, eventParameters);
                
                _building_optional->finishNowState(this, InstantDialog::onDidInstant);
                _building->upgradeState(BuildingUpgradeTypeBuyMissingResources, _cost_gems);
                this->m_modalController->popView();
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            }
        }break;
        case IT_UPGRADE_BUYRESOURCE: {
            if (Building *_building = (Building*)m_entity) {
                
                GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(_building->pbVo.type, _building->pbVo.level+1);
                if (_nxt_bvo.type == "") {
                    CCLog_("Warning received emtpy BuildingVO!");
                    return;
                }
                int _cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
                if (g_playerState->player.balance.gem < _cost_gems) {
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_UPGRADE_BUYRESOURCE"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_UPGRADE_BUYRESOURCE";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["buildingType"] = _building->pbVo.type; //Current Building Type
                eventParameters["buildingToLevel"] = _building->pbVo.level+1;
                eventParameters["buildingCoinCost"] = _building->bVo.cost_coins; //Current Building Coins Cost
                eventParameters["missingResources"] = m_missing_resource_amount; //Current Building Missing Resources
                eventParameters["buildingGemCost"] = _cost_gems; //Gem Cost for Buy Missing Resources for Current Building
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_BUY_RESOURCES_UPGRADE_BUILDING, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_BUY_RESOURCES_UPGRADE_BUILDING, eventParameters);
                
                _building->upgradeState(BuildingUpgradeTypeBuyMissingResources, _cost_gems);
                this->m_modalController->popView();
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            }
        }break;
        case IT_UPGRADE_ABILITY_BUYRESOURCE: {

            SceneVille* _scene = (SceneVille*)g_currentScene;
            
            if (typeid(*(Ability*)m_entity) == typeid(Ability)) { // Ability
                
                if (Ability *_ability = (Ability*)m_entity) {
                    m_type = IT_UPGRADE_ABILITY_BUYRESOURCE;
                    GDAbilityVO _nxt_avo = g_gameContent->Abilites[_ability->aVo.type][_ability->aVo.level];
                    
                    if (_nxt_avo.type == "") {
                        CCLog_("Warning received empty BuildingVO!");
                        return;
                    }
                    int _cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
                    if (g_playerState->player.balance.gem < _cost_gems) {
                        
                        CCDictionary *params = CCDictionary::create();
                        params->setObject(CCString::create("IT_UPGRADE_ABILITY_BUYRESOURCE"), "Source");
                        FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                        
                        CSJson::Value eventParameters;
                        eventParameters["source"] = "IT_UPGRADE_ABILITY_BUYRESOURCE";
                        AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                        
                        launchGemsStore();
                        return;
                    }
                    g_gameAudio->playEffect(AUDIO_FINISH_NOW);

                    
                    
                    //insert case have gem
                    if (Building* _library = g_playerState->getBuilding(kBuildingLibrary, 0)) {

                                            BackendService::getInstance()->send(APIAbilityUpgrade::create(_library,
                                                                                      _nxt_avo.type,
                                                                                      true,
                                                                                      _scene->isoMap,
                                                                                      IsoMapBuilding::onGemsShouldBeating));

                        
                        CCDictionary *params = CCDictionary::create();
                        
                        CSJson::Value eventParameters;
                        eventParameters["abilityType"] = _ability->aVo.type; //Ability Type
                        eventParameters["toAbilityLevel"] = _ability->paVo.level+1;
                        eventParameters["coinCost"] = _ability->aVo.cost_coins; //Ability Coins Cost
                        eventParameters["missingResources"] = m_missing_resource_amount; //Ability Missing Resources
                        eventParameters["gemCost"] = _cost_gems; //Gem Cost for Buy Missing Resources for Ability
                        
                        params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                        
                        FlurryCpp::logEvent(EVENT_GEM_BUY_RESOURCES_UPGRADE_ABILITY, params);
                        
                        AnalyticsClient::getInstance()->logEvent(EVENT_GEM_BUY_RESOURCES_UPGRADE_ABILITY, eventParameters);
                        
                    }
                    
                    _scene->modalController->closeAll();
                }
                
            }
            
        }break;
            
        case IT_CUTTREE_BUYRESOURCE: {
//                      CCLog_("IT_CUTTREE_BUYRESOURCE !");
//            _command->modalController->closeAll();
            
            if (typeid(*(Decoration*)m_entity) == typeid(Decoration)) { // Ability
                
                if (Decoration *_decoration = (Decoration*)m_entity) {
                    m_type = IT_CUTTREE_BUYRESOURCE;
                    
                    int _cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
                    if (g_playerState->player.balance.gem < _cost_gems) {
                        
                        CCDictionary *params = CCDictionary::create();
                        params->setObject(CCString::create("IT_CUTTREE_BUYRESOURCE"), "Source");
                        FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                        
                        CSJson::Value eventParameters;
                        eventParameters["source"] = "IT_CUTTREE_BUYRESOURCE";
                        AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                        
                        launchGemsStore();
                        return;
                    }
                    
                    CCDictionary *params = CCDictionary::create();
                    
                    CSJson::Value eventParameters;
                    eventParameters["treeType"] = _decoration->dVo.type; //Tree Type
                    eventParameters["coinCost"] = _decoration->dVo.cost_coins; //Tree Coins Cost
                    eventParameters["missingResources"] = m_missing_resource_amount; //Tree Missing Resources
                    eventParameters["gemCost"] = _cost_gems; //Gem Cost for Buy Missing Resources for Tree
                    
                    params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                    
                    FlurryCpp::logEvent(EVENT_GEM_BUY_RESOURCES_CUT_TREE, params);
                    
                    AnalyticsClient::getInstance()->logEvent(EVENT_GEM_BUY_RESOURCES_CUT_TREE, eventParameters);
                    
                    g_gameAudio->playEffect(AUDIO_FINISH_NOW);
                    
                    //insert case have gem
                    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                    BackendService::getInstance()->send(APITreeRemove::create(_decoration, true, NULL, CutTreeDialog::onTreeRemoved));
                    m_modalController->closeAll();
                    
                }
                
            }
        }break;
        
        case IT_FUSE_ANIMAL_BUYRESOURCE: {
            
            int _cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
            if (g_playerState->player.balance.gem < _cost_gems) {
                
                CCDictionary *params = CCDictionary::create();
                params->setObject(CCString::create("IT_FUSE_ANIMAL_BUYRESOURCE"), "Source");
                FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                
                CSJson::Value eventParameters;
                eventParameters["source"] = "IT_FUSE_ANIMAL_BUYRESOURCE";
                AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                
                launchGemsStore();
                return;
            }
            
            
            g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            
            if (AnimalTradeDialog* _tradeDialog = (AnimalTradeDialog*)m_modalController->m_views[m_modalController->m_views.size()-2]) { //-2 = previous modal view
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["petType"] = _tradeDialog->vo.type; //Pet Type
                eventParameters["coinCost"] = _tradeDialog->vo.coins_needed_for_fusion; //Pet Fusion Coins Cost
                eventParameters["missingResources"] = m_missing_resource_amount; //Pet Fusion Missing Resources
                eventParameters["gemCost"] = _cost_gems; //Gem Cost for Buy Missing Resources for Pet Fusion
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_BUY_RESOURCES_FUSE_PET, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_BUY_RESOURCES_FUSE_PET, eventParameters);
                
                m_modalController->popView();
                
#ifndef DEBUG_ANIMAL_TRADING__OPTION_DISABLE_TRADE
                _tradeDialog->runTrading(_tradeDialog, true);
#endif
            }
            
        }break;
        case IT_EVOLVE_ANIMAL_BUYRESOURCE: {
            
            int _cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
            if (g_playerState->player.balance.gem < _cost_gems) {
                
                CCDictionary *params = CCDictionary::create();
                params->setObject(CCString::create("IT_EVOLVE_ANIMAL_BUYRESOURCE"), "Source");
                FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                
                CSJson::Value eventParameters;
                eventParameters["source"] = "IT_EVOLVE_ANIMAL_BUYRESOURCE";
                AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                
                launchGemsStore();
                return;
            }
            g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            
            if (AnimalTradeDialog* _tradeDialog = (AnimalTradeDialog*)m_modalController->m_views[m_modalController->m_views.size()-2]) { //-2 = previous modal view
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["petType"] = _tradeDialog->pet->pVo.type; //Pet Type
                eventParameters["coinCost"] = _tradeDialog->pet->pVo.coins_needed_to_evolve; //Pet Evolution Coins Cost
                eventParameters["missingResources"] = m_missing_resource_amount; //Pet Evolution Missing Resources
                eventParameters["gemCost"] = _cost_gems; //Gem Cost for Buy Missing Resources for Pet Evolution
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_BUY_RESOURCES_EVOLVE_PET, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_BUY_RESOURCES_EVOLVE_PET, eventParameters);
                
                m_modalController->popView();
                
#ifndef DEBUG_ANIMAL_TRADING__OPTION_DISABLE_TRADE
                _tradeDialog->runTrading(_tradeDialog, true);
#endif
            }
            
        }break;
        case IT_CLOUD_BUYRESOURCE:{
            
            if (SceneWorldMap* _sceneWorldMap = (SceneWorldMap*)g_currentScene) {
                
                int _cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
                if (g_playerState->player.balance.gem < _cost_gems) {
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_CLOUD_BUYRESOURCE"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_CLOUD_BUYRESOURCE";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["cloudID"] = m_param_1; //Pet Type
                eventParameters["missingResources"] = m_missing_resource_amount; //Pet Evolution Missing Resources
                eventParameters["gemCost"] = _cost_gems; //Gem Cost for Buy Missing Resources for Pet Evolution
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_BUY_RESOURCES_OPEN_CLOUD, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_BUY_RESOURCES_OPEN_CLOUD, eventParameters);
                
                //insert case have gem
                _sceneWorldMap->worldMap->setAnimateWaiting();
                BackendService::getInstance()->send(APIMapCloudOpen::create(m_param_1,
                                                                            true,
                                                                            _sceneWorldMap->worldMap,
                                                                            WorldMap::onCloudOpenned));
                
                this->m_modalController->popView();
                
            }
            
        }break;
            
        case IT_DECORATION_BUYRESOURCE: {
            
            SceneVille *_scene = (SceneVille*)g_currentScene;
            IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
            
            int _cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
            if (g_playerState->player.balance.gem < _cost_gems) {
                
                CCDictionary *params = CCDictionary::create();
                params->setObject(CCString::create("IT_DECORATION_BUYRESOURCE"), "Source");
                FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                
                CSJson::Value eventParameters;
                eventParameters["source"] = "IT_DECORATION_BUYRESOURCE";
                AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                
                launchGemsStore();
                return;
            }
            g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            
            if (Decoration *_decoration = (Decoration*)m_entity) {
                
                CCDictionary *params = CCDictionary::create();
                
                CSJson::Value eventParameters;
                eventParameters["decorationType"] = _decoration->dVo.type; //Decoration Type
                eventParameters["coinCost"] = _decoration->dVo.cost_coins; //Decoration Coin Cost
                eventParameters["missingResources"] = m_missing_resource_amount; //Pet Evolution Missing Resources
                eventParameters["gemCost"] = _cost_gems; //Gem Cost for Buy Missing Resources for Pet Evolution
                
                params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
                
                FlurryCpp::logEvent(EVENT_GEM_BUY_RESOURCES_DECORATION, params);
                
                AnalyticsClient::getInstance()->logEvent(EVENT_GEM_BUY_RESOURCES_DECORATION, eventParameters);
            }
            
            if (_isoMapBuilding->placeHolder) {
                _isoMapBuilding->placeHolder->placeDecoration(DecorationBuyTypeInstant);
            }
            
            this->m_modalController->closeAll();
            
            
        }break;
            
        case IT_BUYCOIN_WITHGEMS: {
            
            int _cost_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
            if (g_playerState->player.balance.gem < _cost_gems) {
                
                CCDictionary *params = CCDictionary::create();
                params->setObject(CCString::create("IT_BUYCOIN_WITHGEMS"), "Source");
                FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                
                CSJson::Value eventParameters;
                eventParameters["source"] = "IT_BUYCOIN_WITHGEMS";
                AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                
                launchGemsStore();
                return;
            }
            g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            
            int resourcesUntilFullCapacity = g_playerState->getTotalCoinStorageCapacity() - g_playerState->player.balance.gold;
            if (m_missing_resource_amount > resourcesUntilFullCapacity) {
                m_missing_resource_amount = resourcesUntilFullCapacity;
            }
            if (m_missing_resource_amount <= 0) return;
            
            CCDictionary *params = CCDictionary::create();
            
            CSJson::Value eventParameters;
            eventParameters["coinBought"] = m_missing_resource_amount; //Buy Coin Amount
            eventParameters["gemCost"] = _cost_gems;
            
            params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
            
            FlurryCpp::logEvent(EVENT_GEM_EXCHANGE_FOR_COIN, params);
            
            BackendService::getInstance()->send(APIGemExchange::create(API_RESOURCE_TYPE_COIN,
                                                                       m_missing_resource_amount,
                                                                       this,
                                                                       InstantDialog::onDidInstantBuyCoinsOrFood));
            AnalyticsClient::getInstance()->logEvent(EVENT_GEM_EXCHANGE_FOR_COIN, eventParameters);
            
            g_sceneManager->showLoading();
            
        }break;
            
        case IT_BUYFOOD_WITHGEMS: {
            
            int _cost_gems = g_gameContent->get_gemcal_for_foods(m_missing_resource_amount);
            if (g_playerState->player.balance.gem < _cost_gems) {
                
                CCDictionary *params = CCDictionary::create();
                params->setObject(CCString::create("IT_BUYFOOD_WITHGEMS"), "Source");
                FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                
                CSJson::Value eventParameters;
                eventParameters["source"] = "IT_BUYFOOD_WITHGEMS";
                AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                
                launchGemsStore();
                return;
            }
            g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            
            int resourcesUntilFullCapacity = g_playerState->getTotalFoodStorageCapacity() - g_playerState->player.balance.food;
            if (m_missing_resource_amount > resourcesUntilFullCapacity) {
                m_missing_resource_amount = resourcesUntilFullCapacity;
            }
            if (m_missing_resource_amount <= 0) return;
            
            CCDictionary *params = CCDictionary::create();
            
            CSJson::Value eventParameters;
            eventParameters["foodBought"] = m_missing_resource_amount; //Buy Food Amount
            eventParameters["gemCost"] = _cost_gems;
            
            params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
            
            FlurryCpp::logEvent(EVENT_GEM_EXCHANGE_FOR_FOOD, params);
            
            BackendService::getInstance()->send(APIGemExchange::create(API_RESOURCE_TYPE_FOOD,
                                                                       m_missing_resource_amount,
                                                                       this,
                                                                       InstantDialog::onDidInstantBuyCoinsOrFood));
            AnalyticsClient::getInstance()->logEvent(EVENT_GEM_EXCHANGE_FOR_FOOD, eventParameters);
            
            g_sceneManager->showLoading();
            
        }break;
            
        case IT_BUYHABITAT_WITHGEMS: {
            
            SceneVille *_scene = (SceneVille*)g_currentScene;
            if (!_scene || typeid(*_scene) != typeid(SceneVille)) return;
            IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
            if (!_isoMapBuilding || typeid(*_isoMapBuilding) != typeid(IsoMapBuilding)) return;
            Building *_habitat = (Building*)m_entity;
            if (!_habitat || typeid(*_habitat) != typeid(Building)) return;
            
            int _cost_gems = int(_habitat->bVo.param_2);
            if (g_playerState->player.balance.gem < _cost_gems) {
                
                CCDictionary *params = CCDictionary::create();
                params->setObject(CCString::create("IT_BUYHABITAT_WITHGEMS"), "Source");
                FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                
                CSJson::Value eventParameters;
                eventParameters["source"] = "IT_BUYHABITAT_WITHGEMS";
                AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                
                launchGemsStore();
                return;
            }
            g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            
            CCDictionary *params = CCDictionary::create();
            
            CSJson::Value eventParameters;
            eventParameters["gemCost"] = _cost_gems; //Gem Cost for Buy Missing Resources for Pet Evolution
            params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
            FlurryCpp::logEvent(EVENT_GEM_BUILD_HABITAT, params);
            
            AnalyticsClient::getInstance()->logEvent(EVENT_GEM_BUILD_HABITAT, eventParameters);
            
            if (_isoMapBuilding->placeHolder) {
                _isoMapBuilding->placeHolder->placeBuilding(BuildingBuildTypeNormal);
            }
            
            this->m_modalController->closeAll();
            return;
            
        }break;
        case IT_ENTER_GEM_SHOP:{
            enterGemsStore();
        }break;
        default:
            break;
    
    }
    
    if (m_bvo.type != "") {
        switch (m_type) {
            case IT_BUILD_BUYRESOURCE: {
                SceneVille *_scene = (SceneVille*)g_currentScene;
                IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
                if (g_playerState->player.balance.gem < g_gameContent->get_gemcal_for_coins(m_missing_resource_amount)) {
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(CCString::create("IT_BUILD_BUYRESOURCE"), "Source");
                    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["source"] = "IT_BUILD_BUYRESOURCE";
                    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
                    
                    launchGemsStore();
                    return;
                }
//                BackendService::getInstance()->coin_buy(m_missing_resource_amount, this, InstantDialog::onDidInstantBuyCoinsOrFood);
                _isoMapBuilding->placingStuffFromPlaceHolder(false, BuildingBuildTypeBuyMissingResources);
                this->m_modalController->closeAll();
                g_gameAudio->playEffect(AUDIO_FINISH_NOW);
            }break;
                
            default:
                break;
        }
    }
    
}

void InstantDialog::updateCurrentResourceValues(void *sender, std::string resp_json) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json* _root = Json_create(resp_json.c_str());
    _playerState->parseChecklistData(_root);
    _playerState->parseItemData(_root);
    _playerState->parseBuildingData(_root);
    _playerState->parseUserBalance(_root);
    _playerState->parseUserRaceData(_root);
    _playerState->parseAbilityData(_root);
    _playerState->parsePetData(_root);
    Json_dispose(_root);
    
    CScene *_scene = (CScene*)g_currentScene;
    if (!_scene) return;
    
    _scene->hudPanel->updateHUD();
    _scene->hudPanel->updateAllResourceBars();
    _scene->hudPanel->updateBadgeAchievement();
    
}

void InstantDialog::onDidInstant(void *sender, std::string resp_str) {
    
    InstantDialog::updateCurrentResourceValues(sender, resp_str);
    
}

void InstantDialog::onDidInstantBuyCoinsOrFood(void *sender, std::string resp_json) {
    
    //TODO: display notification message that 5000 coins or 5000 food has been added
    
    InstantDialog::updateCurrentResourceValues(sender, resp_json);
    
    g_sceneManager->hideLoading();
    
    InstantDialog *_this = (InstantDialog*)sender;
    _this->m_modalController->closeAll();
    
}

void InstantDialog::closeAllDialogs(cocos2d::CCObject* sender) {
    
    InstantDialog *_this = (InstantDialog*)sender;
    _this->m_modalController->closeAll();
    
}
