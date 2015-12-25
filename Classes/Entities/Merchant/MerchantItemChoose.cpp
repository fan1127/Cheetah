//
//  MerchantItemChoose.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/20/2557 BE.
//
//

#include "MerchantItemChoose.h"
USING_NS_CC;
using namespace std;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIMerchantItemCache.h"

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../PlayerState.h"
extern PlayerState* g_playerState;

extern float sizeMultiplier;
extern int currentTime;

#include "../../Extensions/CCMenuItemScaleSprite.h"

string MerchantItemChoose::key() {
    
    return "merchant_item_choose";
    
}

bool MerchantItemChoose::check() {
    
    if (!g_playerState->Checklist[MerchantItemChoose::key()]) {
        return true;
    }
    
    return false;
    
}

MerchantItemChoose* MerchantItemChoose::create() {
    
    if (MerchantItemChoose::check()) {
        MerchantItemChoose* pRet = new MerchantItemChoose();
        if (pRet) {
            if (pRet->init()) {
                pRet->autorelease();
                return pRet;
            }
        }
        CC_SAFE_DELETE(pRet);
    }
    
    return NULL;
    
}

bool MerchantItemChoose::init() {
    
    if (!CTutUnit::init()) {
        return false;
    }
    
    m_key = MerchantItemChoose::key();
    
    m_chosenItem = "";
    m_chosenItemDidCallback = false;
    
    CTutSpeech* _tutChooseItem = new CTutSpeech(CT_CHR_MERCHANT, CT_CHR_SIDE_LEFT,
                                                kMerchantIdle, true,
                                                g_gameContent->Word["tut_merchant_speech_2"].c_str(),
                                                CT_SPC_OPTION_CHOOSE_ITEM,
                                                this, MerchantItemChoose::onItemChosen);
    _tutChooseItem->setAfterPerformCallback(this, MerchantItemChoose::onAfterTutChooseItemPerform);
    processItem(_tutChooseItem->keys);
    m_tuts.push_back(_tutChooseItem);
    
    string messageThankyou = "";
    if(!(PlayerState::getInstance()->merchantHasOnlyOneLastItemForSell()))
    {
        messageThankyou = g_gameContent->Word["tut_merchant_speech_3"];
    }
    else
    {
        messageThankyou = g_gameContent->Word["tut_merchant_speech_4"];
    }
    m_tuts.push_back(new CTutSpeech(CT_CHR_MERCHANT, CT_CHR_SIDE_LEFT,
                                    kMerchantHappyJump, false,
                                    kMerchantIdle, true,
                                    messageThankyou.c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, MerchantItemChoose::onTapTheBG));
    
    CCSprite* _closeSprite = UIHelper::getSprite(UIHelper::NSCloseButton);
    CCMenuItemSprite *_closeItem = CCMenuItemScaleSprite::create(_closeSprite, this, menu_selector(MerchantItemChoose::onClose));
    m_closeMenu = CCMenu::create(_closeItem, NULL);
    
    m_closeMenu->setVisible(false);
    m_closeMenu->setEnabled(false);
    m_closeMenu->setTouchPriority(kCCMenuHandlerPriority-2);
    addChild(m_closeMenu, 100);
    
    //NOT_HERE
    
    return true;
    
}

void MerchantItemChoose::update(float delta) {
    
    if (m_tutSelector >= m_tuts.size() && !m_chosenItemDidCallback) {
        m_chosenItemDidCallback = true;
        if (onMerchantItemChosen) {
            onMerchantItemChosen(sender, m_chosenItem);
        }
    }
    
    CTutUnit::update(delta);
    
}

void MerchantItemChoose::onAfterTutChooseItemPerform(void *sender) {
    
    MerchantItemChoose *_this = (MerchantItemChoose*)sender;
    if (!_this) return;
    
    _this->m_closeMenu->setVisible(true);
    _this->m_closeMenu->setEnabled(true);
    _this->m_closeMenu->setPosition(_this->chooseDialogSprite->getPosition()
                                    + _this->chooseDialogSprite->getContentSize()*0.5f
                                    + ccp(-20.0f*sizeMultiplier, -20.0f*sizeMultiplier));
    
}

void MerchantItemChoose::processItem(string *keys) {
    
    if (g_playerState->player.info.merchant_items == "") {
        
        map<string, int> _missing_items;

        // find all missing requred items in order to evolve from all living pets
        for (vector<Pet*>::iterator it = g_playerState->Pets.begin(); it != g_playerState->Pets.end(); ++it) {
            Pet* _pet = (Pet*)(*it);
            if (_pet->ppVo.located_in == "") continue;// skip if pet is not living in habitat
            CCLog_("Pet: %s", _pet->pVo.title.c_str());
            for (vector<FloatKeyValue>::iterator it2 = _pet->pVo.next_evolution.begin(); it2!=_pet->pVo.next_evolution.end(); ++it2) {
                FloatKeyValue _kv = *it2;
                GDPetVO _pet_vo = g_gameContent->getPetVO(_kv.key);
                if (_pet_vo.type != "") {
                    CCLog_(" >> %s", _pet_vo.title.c_str());
                }
            }
            CCLog_("> Required:");
            for (vector<IntKeyValue>::iterator it3 = _pet->pVo.items_needed_to_evolve.begin(); it3!=_pet->pVo.items_needed_to_evolve.end(); ++it3) {
                IntKeyValue _kv2 = *it3;
                GDItemVO _item_vo = g_gameContent->getItemVO(_kv2.key);
                if (_item_vo.type == "") continue;
                int _num = g_playerState->getNItem(_kv2.key);
                CCLog_(" >>> %s (%d/%d)",_item_vo.title.c_str(), _num, _kv2.value);
                if (_num < _kv2.value) {
                    _missing_items[_kv2.key] = 1;
                }
            }
        }
        
        // Seperate rare and non-rare items
        vector<string> _missing_items_nonrare;
        vector<string> _missing_items_rare;
        CCLog_("Missing Items:");
        for (map<string, int>::iterator it = _missing_items.begin(); it != _missing_items.end(); ++it) {
            string _item = (*it).first;
            CCLog_(" >> %s", _item.c_str());
            GDItemVO _item_vo = g_gameContent->getItemVO(_item);
            if (_item_vo.rarity_point != 2) {
                _missing_items_nonrare.push_back(_item);
            }else {
                _missing_items_rare.push_back(_item);
            }
        }
        
        // deduct or add up to make a list of 2 and 1
        while (_missing_items_nonrare.size() > 2) {
            _missing_items_nonrare.erase(_missing_items_nonrare.begin() + rand()%_missing_items_nonrare.size());
        }
        while (_missing_items_nonrare.size() < 2) {
            _missing_items_nonrare.push_back(g_gameContent->getRandomItemExcept("!=", 2, _missing_items_nonrare));
        }
        while (_missing_items_rare.size() > 1) {
            _missing_items_rare.erase(_missing_items_rare.begin() + rand()%_missing_items_rare.size());
        }
        while (_missing_items_rare.size() < 1) {
            _missing_items_rare.push_back(g_gameContent->getRandomItemExcept("==", 2, _missing_items_rare));
        }
        
        //compose a string version of result
        int i = 0;
        string _offering_items = "";
        string _connector = "";
        for (vector<string>::iterator it = _missing_items_nonrare.begin(); it != _missing_items_nonrare.end(); ++it) {
            string _item = *it;
            CCLog_(" >> %s", _item.c_str());
            keys[i] = _item.c_str();
            _offering_items.append(_connector);
            _offering_items.append(_item);
            _connector = ",";
            i++;
        }
        for (vector<string>::iterator it = _missing_items_rare.begin(); it != _missing_items_rare.end(); ++it) {
            string _item = *it;
            CCLog_(" >> %s", _item.c_str());
            keys[i] = _item.c_str();
            _offering_items.append(_connector);
            _offering_items.append(_item);
            _connector = ",";
            i++;
        }
        
        // store it, to use it the future;
        CCLog_("%s", _offering_items.c_str());
        g_playerState->player.info.merchant_items = _offering_items;
        
        BackendService::getInstance()->send(APIMerchantItemCache::create(g_playerState->player.info.merchant_items,
                                                                         this,
                                                                         MerchantItemChoose::onItemCached));
        
    }else {
        
        CCLog_("%s", g_playerState->player.info.merchant_items.c_str());
        
        string _items = g_playerState->player.info.merchant_items;
        int _pos = 0;
        string _delimiter = ",";
        int i = 0;
        while ((_pos = (int)_items.find(_delimiter)) != string::npos) {
            string _item = _items.substr(0, _pos);
            if (i < 3) keys[i] = _item;
            i++;
            _items.erase(0, _pos + _delimiter.length());
        }
        if (_items != "") {
            string _item = _items;
            if (i < 3) keys[i] = _item;
            i++;
        }

    }
    
}

#pragma mark - callback functions

void MerchantItemChoose::onItemCached(void *sender, std::string resp)
{
    CREATE_CHAR_BUFFER(merchant_cache_timestamp_str, 32);
    snprintf(merchant_cache_timestamp_str, merchant_cache_timestamp_str_size, "%d", currentTime);
    CCUserDefault::sharedUserDefault()->setStringForKey(kMerchantItemCacheTimeKey, merchant_cache_timestamp_str);
}

void MerchantItemChoose::onTapTheBG(void* sender, std::string result) {
    
    MerchantItemChoose* _this = (MerchantItemChoose*)sender;
    if (!_this) return;
    
    CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector];
    if (!_tut || typeid(*_tut) != typeid(CTutSpeech)) return;
    
    if (_tut->option != CT_SPC_OPTION_NONE) return;
    
    _this->goToNextTutorial();
    
}

void MerchantItemChoose::onItemChosen(void *sender, std::string result) {
    
    MerchantItemChoose* _this = (MerchantItemChoose*)sender;
    if (!_this) return;
    
    CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector];
    if (!_tut || typeid(*_tut) != typeid(CTutSpeech)) return;

    _this->m_closeMenu->setVisible(false);
    _this->m_chosenItem = result;
    _this->goToNextTutorial();
    
    g_gameAudio->playEffect(AUDIO_MERCHANT_BUY);
}

void MerchantItemChoose::onClose(cocos2d::CCObject *object_) {
    
    m_tutSelector = (int)m_tuts.size();
    if (onMerchantItemChosen) {
        onMerchantItemChosen(sender, "");
    }
    
    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
    
}