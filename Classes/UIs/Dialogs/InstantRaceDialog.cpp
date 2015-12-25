//
//  InstantRaceDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/31/2557 BE.
//
//

#include "InstantRaceDialog.h"
USING_NS_CC;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Scenes/SceneMatch.h"
extern CCScene* g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIPetWakeUp.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

//#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;
//extern PlayerState* g_opponentState;

extern float sizeMultiplier;
extern float isIPhoneMultiplier;

#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"

#include "../../UIs/Dialogs/IAPDialog.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/CCStringHelper.h"

#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"

#include "../../Helpers/FlurryCpp.h"
#include "../../DialogSettings.h"

#include "../../AppMacros.h"
#include "../../Helpers/StringHelper.h"

#include "../../../extensions/CCArmature/external_tool/Json/lib_json/value.h"
#include "../../../extensions/CCArmature/external_tool/Json/lib_json/writer.h"

extern TargetDevice targetDevice;

InstantRaceDialog* InstantRaceDialog::create(CCModalController *parentModalController_, InstantRaceType type_, PlayerState* mapPlayerState_) {
    
    InstantRaceDialog* pRet = new InstantRaceDialog();
    pRet->m_type = type_;
    pRet->init(parentModalController_, mapPlayerState_, cocos2d::CCSize(1000.0f, 700.0f));
    pRet->autorelease();
    return pRet;
}

bool InstantRaceDialog::init(CCModalController* parentModalController_, PlayerState* mapPlayerState_, CCSize size) {
    
    if (!SimpleDialog::init(parentModalController_, size))
    {
        return false;
    }

    mapPlayerState = mapPlayerState_;
    m_missing_resource_amount = 0;

    return compose();
}

bool InstantRaceDialog::compose()
{
    CREATE_CHAR_BUFFER(_buffer, 128);

    GDExperienceLevelVO _exp_vo = g_gameContent->getNextExperienceLevelVO(g_playerState->player.racing.level-1);
    int race_fee = _exp_vo.race_fee;
    m_missing_resource_amount = race_fee - g_playerState->player.balance.gold;

    createTitle("");
    createDescription("");

    switch (m_type) {
        case IT_INSTANT_COINS: {
            
            //to upper
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["missing_resources_title"].c_str());
            char const* buffer = UIDisplayHelper::stringToUpperCase(_buffer).c_str();
            snprintf(_buffer, _buffer_size, "%s", buffer);
            setTitle(_buffer);

            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["buy_missing_resources_msg"].c_str());
            setDescription(_buffer);
            
            snprintf(_buffer, _buffer_size, "%s", CCStringHelper::createWithNumberFormat(m_missing_resource_amount).c_str());

            createIconLine(_buffer, SimpleDialog::Coin);
            
            addInstantButton(g_gameContent->get_gemcal_for_coins(m_missing_resource_amount));
            
        }break;
        case IT_INSTANT_ENERGY_FOR_RACE: {
            
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["wakeup_now"].c_str());
            char const* buffer = UIDisplayHelper::stringToUpperCase(_buffer).c_str();
            snprintf(_buffer, _buffer_size, "%s", buffer);
            setTitle(_buffer);

            setDescription(g_gameContent->Word["instant_energy_msg"].c_str());
            addInstantButton(g_gameContent->get_gemcal_for_pet_wakeup(mapPlayerState->PetRunner->getSecondsLeftToFullEnergy()));
            
        }break;
        case IT_INSTANT_ENERGY_AND_GOLD: {
            
            snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["wakeup_now"].c_str());
            char const* buffer = UIDisplayHelper::stringToUpperCase(_buffer).c_str();
            snprintf(_buffer, _buffer_size, "%s", buffer);
            setTitle(_buffer);

            setDescription(g_gameContent->Word["instant_energy_msg"].c_str());
            int energy=g_gameContent->get_gemcal_for_pet_wakeup(mapPlayerState->PetRunner->getSecondsLeftToFullEnergy());
            int gold=g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
            
            snprintf(_buffer, _buffer_size, "%s", CCStringHelper::createWithNumberFormat(m_missing_resource_amount).c_str());

            createIconLine(_buffer, SimpleDialog::Coin);

            addInstantButton(energy+gold);
            
        } break;

        default:
            break;
    }

    autoResize(300*sizeMultiplier, 200*sizeMultiplier);

    return true;
}

void InstantRaceDialog::addInstantButton(int value_)
{
    createButton(StringHelper::toString(value_).c_str(), SimpleDialog::Center, SimpleDialog::Gem);
}

void InstantRaceDialog::launchGemsStore() {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    IAPDialog *_dialog = IAPDialog::create(m_modalController, RESOURCE_TYPE_GEM);
    _dialog->setPosition(_screenSize*0.5f + kDialogIAPPosition);
    m_modalController->pushView(_dialog);
    
}

void InstantRaceDialog::onButton(CCObject *sender_) {
    
    int required_gems ;
    if( m_type==IT_INSTANT_ENERGY_FOR_RACE) {
        
        required_gems=  g_gameContent->get_gemcal_for_pet_wakeup(mapPlayerState->PetRunner->getSecondsLeftToFullEnergy());
        if (g_playerState->player.balance.gem < required_gems) {
            
            CCDictionary *params = CCDictionary::create();
            params->setObject(CCString::create("IT_INSTANT_ENERGY_FOR_RACE"), "Source");
            FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
            
            CSJson::Value eventParameters;
            eventParameters["source"] = "IT_INSTANT_ENERGY_FOR_RACE";
            AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);

            launchGemsStore();
            return;
        }
        
        CCDictionary *params = CCDictionary::create();
        
        CSJson::Value eventParameters;
        eventParameters["petType"] = mapPlayerState->PetRunner->pVo.type;
        eventParameters["petTimeLeft"] = mapPlayerState->PetRunner->getSecondsLeftToFullEnergy();
        eventParameters["gem"] = required_gems;
        
        params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
        
        FlurryCpp::logEvent(EVENT_GEM_WAKE_UP_PET_TO_RACE, params);
        
        AnalyticsClient::getInstance()->logEvent(EVENT_GEM_WAKE_UP_PET_TO_RACE, eventParameters);

        SceneMatch *_sceneMatch = (SceneMatch *) g_currentScene;
        g_sceneManager->showLoading();
        BackendService::getInstance()->send(APIPetWakeUp::create(mapPlayerState->PetRunner,
                                                                 _sceneMatch->matchPanel,
                                                                 MatchPanel::onPetWokeUp));
        this->m_modalController->closeAll();
        g_gameAudio->playEffect(AUDIO_FINISH_NOW);


    } else if (m_type == IT_INSTANT_ENERGY_AND_GOLD) {

        int required_gems1 = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
        int required_gems2 = g_gameContent->get_gemcal_for_pet_wakeup(mapPlayerState->PetRunner->getSecondsLeftToFullEnergy());
        int sum = required_gems1 + required_gems2;
        if (g_playerState->player.balance.gem < sum) {
            
            CCDictionary *params = CCDictionary::create();
            
            CSJson::Value eventParameters;
            eventParameters["petType"] = mapPlayerState->PetRunner->pVo.type;
            eventParameters["petTimeLeft"] = mapPlayerState->PetRunner->getSecondsLeftToFullEnergy();
            eventParameters["raceMiss"] = m_missing_resource_amount;
            eventParameters["gem"] = sum;
            
            params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
            
            FlurryCpp::logEvent(EVENT_GEM_WAKE_UP_PET_AND_BUY_COINS_TO_RACE, params);
            
            AnalyticsClient::getInstance()->logEvent(EVENT_GEM_WAKE_UP_PET_AND_BUY_COINS_TO_RACE, eventParameters);

            launchGemsStore();
            return;
        }

        SceneMatch *_sceneMatch = (SceneMatch *) g_currentScene;
        g_sceneManager->showLoading();
        BackendService::getInstance()->send(APIPetWakeUp::create(mapPlayerState->PetRunner,
                                                                 _sceneMatch->matchPanel,
                                                                 MatchPanel::onPetWokeUp));
        this->m_modalController->closeAll();
        g_gameAudio->playEffect(AUDIO_FINISH_NOW);

    } else { //Instant - not enough coins

        required_gems = g_gameContent->get_gemcal_for_coins(m_missing_resource_amount);
        if (g_playerState->player.balance.gem < required_gems) {
            
            CCDictionary *params = CCDictionary::create();
            params->setObject(CCString::create("IT_BUY_COINS_TO_RACE"), "Source");
            FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
            
            CSJson::Value eventParameters;
            eventParameters["source"] = "IT_BUY_COINS_TO_RACE";
            AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);

            launchGemsStore();
            return;
        }
        
        CCDictionary *params = CCDictionary::create();
        
        CSJson::Value eventParameters;
        eventParameters["petType"] = mapPlayerState->PetRunner->pVo.type;
        eventParameters["raceMiss"] = m_missing_resource_amount;
        eventParameters["gem"] = required_gems;
        
        params->setObject(ccs(AnalyticsClient::write(eventParameters)), "gemUsageInfo");
        
        FlurryCpp::logEvent(EVENT_GEM_BUY_COINS_TO_RACE, params);
        
        AnalyticsClient::getInstance()->logEvent(EVENT_GEM_BUY_COINS_TO_RACE, eventParameters);

        g_gameAudio->playEffect(AUDIO_FINISH_NOW);
        SceneMatch *_sceneMatch = (SceneMatch *) g_currentScene;
        this->m_modalController->popView();
        _sceneMatch->matchPanel->instant = true;
        _sceneMatch->matchPanel->deductGemsBeforeLaunch(required_gems);
    }
}
