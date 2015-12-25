//
//  AnimalMarketDialog.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/26/2557 BE.
//
//

#include "AnimalMarketDialog.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace std;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/BackendService.h"
extern BackendService *g_gameBackend;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
//extern CCPoint screenFactor;
extern float sizeMultiplier;

#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCNodeHelper.h"
#include "MiniDialog.h"

#include "../../Extensions/CCClippingLayer.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCNikScrollViewButton.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCMenuHelper.h"

#include "../../Helpers/UIDisplayHelper.h"
#include "../../UIs/Dialogs/AnimalTradeDialog.h"
#include "../../UIs/Dialogs/AnimalMarketItem.h"
#include "../../Helpers/StringHelper.h"
#include "../../Helpers/CustomEventsHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/UIHelper.h"

AnimalMarketDialog* AnimalMarketDialog::create(CCModalController* parentModalController_, PlayerState* playerState_) {
    
    if (AnimalMarketDialog *pRet = new AnimalMarketDialog()) {
        pRet->m_modalController = parentModalController_;
        pRet->m_playerState = playerState_;
        if (pRet->init(parentModalController_, kDialogAnimalBarnSize)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
    }
    return NULL;
}

bool AnimalMarketDialog::init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize)
{
    if (!ScrollDialog::init(parentModalController_, dialogSize))
    {
        return false;
    }

    //set title
    addTitle("");

    m_selectButtonEnabled = true;

#ifdef kDialogFooterTextEnabled
    if (m_playerState == g_playerState) {
        m_label_footer = CCLabelHelper::createBMFont(g_gameContent->Word["market_footer_msg"].c_str(), StyleTextAboveButton);
        m_label_footer->setPositionY(-this->getContentSize().height*0.42f);
        this->addChild(m_label_footer);
    }
#endif

    addHelpButton("note_animal_market_dialog", menu_selector(AnimalMarketDialog::onNote));

    createScrollContent(ANIMALMARKET_DIALOG_SETTINGS);

    this->updateContent();
    
    m_openAnimalDialogDelay = 0.0f;
    
    scheduleUpdate();

    return true;
}

void AnimalMarketDialog::updateContent()
{
    Building* _market = m_playerState->getBuilding(kBuildingFusion, 0);

    if (!_market)
    {
        return;
    }
    
    setTitle(_market->bVo.title.c_str());

    int i = 1;
    int j = 0;
    float marginWidth = m_scrollView->getContentSize().width*0.05f;
    float marginHeight = m_scrollView->getContentSize().height*0.1f;

    AnimalMarketItem *_lastItem = NULL;
    
    int countPetsToDisplay = 0;
    
    vector<GDPetVO> _fusionPets;
    
    //for fusion
    for (vector<GDPetVO>::iterator it=g_gameContent->Pet.begin(); it!=g_gameContent->Pet.end(); ++it) {
        GDPetVO _vo = (GDPetVO)(*it);
        if (_vo.type != "") {
            if (_vo.items_needed_for_fusion.size() > 0)
            {
                countPetsToDisplay++;
                _fusionPets.push_back(_vo);
            }
        }
    }
    
    vector<Pet*> _evoPets;
    for (vector<Pet*>::iterator it=m_playerState->Pets.begin(); it!=m_playerState->Pets.end(); ++it) {
        if (Pet *_pet = (Pet*)(*it)) {
            if (_pet->pVo.type != "" && _pet->pVo.next_evolution.size() > 0)
            {
                countPetsToDisplay++;
                _evoPets.push_back(_pet);
            }
        }
    }
    
//    bool displaySpine = true;
//    if (countPetsToDisplay > MAX_PETS_FOR_LOADING_SPINE) {
//        displaySpine = false;
//    }
    
    bool displaySpine = false;
    
    vector<Pet*> _pets;
    
    for (vector<Pet*>::iterator it=_evoPets.begin(); it!=_evoPets.end(); ++it) {
        if (Pet *_pet = (Pet*)(*it)) {
            
            int _unlocks_at_level = 0;
            
            if (_pet->pVo.next_evolution.size() > 1) {
                
                //Find highest unlocks_at_level
                for (int i=0; i<_pet->pVo.next_evolution.size(); i++) {
                    FloatKeyValue _tmp_fkv = _pet->pVo.next_evolution[i];
                    if (_tmp_fkv.key != "") {
                        GDPetVO _tmp_pet_vo = g_gameContent->getPetVO(_tmp_fkv.key);
                        if (_unlocks_at_level < _tmp_pet_vo.unlocks_at_level) {
                            _unlocks_at_level = _tmp_pet_vo.unlocks_at_level;
                        }
                    }
                }
            } else {
                
                FloatKeyValue _tmp_fkv = _pet->pVo.next_evolution[0];
                GDPetVO _tmp_pet_vo = g_gameContent->getPetVO(_tmp_fkv.key);
                _unlocks_at_level = _tmp_pet_vo.unlocks_at_level;
            }
            
            if (_market->pbVo.level >= _unlocks_at_level) { //already unlocked
                
                FloatKeyValue _key = _pet->pVo.next_evolution[rand()%_pet->pVo.next_evolution.size()];
                GDPetVO _nextVO = g_gameContent->getPetVO(_key.key);
                AnimalMarketItem *_item = AnimalMarketItem::create(m_playerState, _nextVO, _pet, (_pet->pVo.next_evolution.size() > 1), displaySpine);
                _item->setTarget(this, menu_selector(AnimalMarketDialog::selectAnimalItem), CCNikScrollViewButtonStateTouchUp);
                _item->setDelegate(m_scrollView);
                _item->setPosition(ccp(marginWidth
                                       + (ANIMAL_ITEM_WIDTH + marginWidth)*j
                                       ,
                                       ((m_scrollView->getContentSize().height
                                         - (ANIMAL_ITEM_HEIGHT*2 + marginHeight))*0.5f
                                        + (ANIMAL_ITEM_HEIGHT + marginHeight)*i)));
                
                m_scrollView->addChild(_item);
                _lastItem = _item;
                i--;
                if (i < 0) {
                    j++;
                    i = 1 ;
                }
                
                
            } else {
                _pets.push_back(_pet);
            }
        }
    }
    _evoPets.clear();
    
    //evo for pet in habitats
    
    for (vector<Pet*>::iterator it=_pets.begin(); it!=_pets.end(); ++it) {
        if (Pet *_pet = (Pet*)(*it)) {
//            GDPetVO _vo = g_gameContent->getPetVO(_pet->ppVo.type);
            if (_pet->pVo.type != "" && _pet->ppVo.located_in != "" && _pet->pVo.next_evolution.size() > 0) {
                
                FloatKeyValue _key = _pet->pVo.next_evolution[rand()%_pet->pVo.next_evolution.size()];
                GDPetVO _nextVO = g_gameContent->getPetVO(_key.key);
                AnimalMarketItem *_item = AnimalMarketItem::create(m_playerState, _nextVO, _pet, (_pet->pVo.next_evolution.size() > 1), displaySpine);
                _item->setTarget(this, menu_selector(AnimalMarketDialog::selectAnimalItem), CCNikScrollViewButtonStateTouchUp);
                _item->setDelegate(m_scrollView);
                _item->setPosition(ccp(marginWidth
                                       + (ANIMAL_ITEM_WIDTH + marginWidth)*j
                                       ,
                                       ((m_scrollView->getContentSize().height
                                         - (ANIMAL_ITEM_HEIGHT*2 + marginHeight))*0.5f
                                        + (ANIMAL_ITEM_HEIGHT + marginHeight)*i)));
                
                m_scrollView->addChild(_item);
                _lastItem = _item;
                i--;
                if (i < 0) {
                    j++;
                    i = 1 ;
                }
            }
        }
    }
    
    //evo for pet in barns
    
    for (vector<Pet*>::iterator it=_pets.begin(); it!=_pets.end(); ++it) {
        if (Pet *_pet = (Pet*)(*it)) {
            //            GDPetVO _vo = g_gameContent->getPetVO(_pet->ppVo.type);
            if (_pet->pVo.type != "" && _pet->ppVo.located_in == "" && _pet->pVo.next_evolution.size() > 0) {
                
                FloatKeyValue _key = _pet->pVo.next_evolution[rand()%_pet->pVo.next_evolution.size()];
                GDPetVO _nextVO = g_gameContent->getPetVO(_key.key);
                AnimalMarketItem *_item = AnimalMarketItem::create(m_playerState, _nextVO, _pet, (_pet->pVo.next_evolution.size() > 1), displaySpine);
                _item->setTarget(this, menu_selector(AnimalMarketDialog::selectAnimalItem), CCNikScrollViewButtonStateTouchUp);
                _item->setDelegate(m_scrollView);
                _item->setPosition(ccp(marginWidth
                                       + (ANIMAL_ITEM_WIDTH + marginWidth)*j,
                                       ((m_scrollView->getContentSize().height
                                         - (ANIMAL_ITEM_HEIGHT*2 + marginHeight))*0.5f
                                        + (ANIMAL_ITEM_HEIGHT + marginHeight)*i)
                                       -(8.0f*sizeMultiplier)));
                
                m_scrollView->addChild(_item);
                _lastItem = _item;
                i--;
                if (i < 0) {
                    j++;
                    i = 1 ;
                }
            }
        }
    }
    
    _pets.clear();
    
    
    float delay = 0.0f;
    
    //fusion
    for (vector<GDPetVO>::iterator it=_fusionPets.begin(); it!=_fusionPets.end(); ++it) {
        
        GDPetVO _vo = (GDPetVO)(*it);
        
        bool unlocked = (_market->pbVo.level >= _vo.unlocks_at_level);
        
        AnimalMarketItem *_item = AnimalMarketItem::create(m_playerState, _vo, NULL, false, displaySpine, !unlocked);
        
        if (unlocked) { //unlocked
            _item->setTarget(this, menu_selector(AnimalMarketDialog::selectAnimalItem), CCNikScrollViewButtonStateTouchUp);
        }
        
        _item->setDelegate(m_scrollView);
        _item->setPosition(ccp(marginWidth
                               + (ANIMAL_ITEM_WIDTH + marginWidth)*j
                               ,
                               ((m_scrollView->getContentSize().height
                                 - (ANIMAL_ITEM_HEIGHT*2 + marginHeight))*0.5f
                                + (ANIMAL_ITEM_HEIGHT + marginHeight)*i)
                               -(8.0f*sizeMultiplier)));
        
        m_scrollView->addChild(_item);
        
        if (_item->canTrade &&
            CustomEventsHelper::Instance().shouldAnimateIdle(kIdleAnimationChoosePet))
        {
            _item->animateIdleWithDelay(delay);
            delay += 0.3f;
        }
        
        _lastItem = _item;
        i--;
        if (i < 0) {
            j++;
            i = 1 ;
        }
    }
    _fusionPets.clear();

    if (!_lastItem) {
        return;
    }
    m_scrollView->setContentView(CCSizeMake(MAX(_lastItem->getPositionX() + _lastItem->getContentSize().width, m_scrollView->getContentSize().width), m_scrollView->getContentSize().height));

}

void AnimalMarketDialog::update(float delta_) {
    
    if (m_openAnimalDialogDelay > 0.0f)
        m_openAnimalDialogDelay -= delta_;
}

void AnimalMarketDialog::selectAnimalItem(AnimalMarketItem *item) {
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    if (!m_selectButtonEnabled) return;
    
    if (m_openAnimalDialogDelay > 0.0f) return;
    m_openAnimalDialogDelay = 0.2f;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    AnimalTradeDialog *_dialog = AnimalTradeDialog::create(this->m_modalController, item->vo, item->pet, m_playerState);
    _dialog->setPosition(_screenSize*0.5f);
    this->m_modalController->pushView(_dialog);
    
}

void AnimalMarketDialog::onNote(cocos2d::CCObject *obj_) {
    
    CREATE_CHAR_BUFFER(_buffer, 512);
    snprintf(_buffer, _buffer_size, g_gameContent->Word["note_animal_market_dialog"].c_str(),
            g_gameContent->getBuildingVO(kBuildingFusion, 1).title.c_str(),
            g_gameContent->getBuildingVO(kBuildingFusion, 1).title.c_str());
    
    CCLabelBMFont* _note_label = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, false, kTextColorDark, getContentSize().width*0.4f, kCCTextAlignmentCenter);
    CCSize _contentSize = CCNodeHelper::getContentSize(_note_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog* _hintPopOver = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _note_label->setPosition(_hintPopOver->getContentSize()*0.5f);
    _hintPopOver->addChild(_note_label);
    _hintPopOver->presentPopover(m_help, true);
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
}
