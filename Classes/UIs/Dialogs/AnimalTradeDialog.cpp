//
//  AnimalTradeDialog.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/26/2557 BE.
//
//

#include "AnimalTradeDialog.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace std;

#include <spine/Json.h>
using namespace cocos2d::extension;

#include "../../Settings.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/CreateSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"
#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIPetFuse.h"
#include "../../Services/APIs/APIPetEvolve.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
//extern CCPoint screenFactor;
extern float sizeMultiplier;
extern float retinaMultiplier;
extern float margin;
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCNodeHelper.h"
#include "MiniDialog.h"

#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Extensions/CCClippingLayer.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCNikScrollViewButton.h"
#include "../../Extensions/CCNikButton.h"
#include "../../UIs/Dialogs/AnimalTradeItem.h"
#include "../../Services/GameAudioService.h"
#include "../../UIs/Dialogs/ItemStoragePopoverDialog.h"
#include "../../UIs/Dialogs/ItemStorageItem.h"
#include "../../UIs/Dialogs/SearchItemDialog.h"
#include "../../Extensions/CCPopOverDialog.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/FlurryCpp.h"
#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

extern GameAudioService *g_gameAudio;
AnimalTradeDialog* AnimalTradeDialog::create(CCModalController* parentModalController_, GDPetVO vo_, Pet *pet_, PlayerState* playerState_)
{
    AnimalTradeDialog *pRet = new AnimalTradeDialog();
    pRet->init(parentModalController_, vo_, pet_, playerState_, kDialogTradeAnimalSize);
    pRet->autorelease();
    return pRet;
}

bool AnimalTradeDialog::init(CCModalController* parentModalController_, GDPetVO vo_, Pet *pet_, PlayerState* playerState_, CCSize size)
{
    if (!ScrollDialog::init(parentModalController_, size))
    {
        return false;
    }

    m_selectButtonEnabled = true;

    m_animalPopOver = NULL;
    CREATE_CHAR_BUFFER(_buffer, 64);
//    CREATE_CHAR_BUFFER(_buffer2, 64);

    m_complete = false;
    m_not_enough_items = false;
    m_not_enough_coins = false;
    m_not_enough_animal_level = false;
    m_not_enough_market_level = false;
    m_not_enough_space_to_fuse = false;

    m_tradeEffectRunning = false;

    vo = vo_;
    pet = pet_;
    m_playerState = playerState_;

    m_unlocks_at_level = vo.unlocks_at_level;

    dialogGroup = CCNode::create();
    this->addChild(dialogGroup);

    const char *targetPet = vo.title.c_str();

    if (pet) {
        if (kNextEvoIsUnknown) {
            targetPet = g_gameContent->Word["unknown"].c_str();
        }

        snprintf(_buffer, _buffer_size, "Upgrade to %s", targetPet);

    } else {
        snprintf(_buffer, _buffer_size, g_gameContent->Word["animal_trade_fusion"].c_str(), targetPet);
    }

    createFrontBackground(UIHelper::AchievementBG);
    createScrollContent(ANIMALTRADE_DIALOG_SETTINGS);

    // lower the clipping area a little bit
    auto yShift = 0.05f;
    m_clippingLayer->setContentSize(ccp(m_clippingLayer->getContentSize().width, m_clippingLayer->getContentSize().height*(1.0f - yShift)));
    m_clippingLayer->setPositionY(m_clippingLayer->getPositionY() + m_clippingLayer->getContentSize().height*yShift*0.5f);

    m_frontBackground->setZOrder(1);

    addTitle(_buffer);

    // change parrent
    replaceControlsParent(dialogGroup);

    float petSpineAndArrowY = 0.08f;
    float labelY = 0.12f;
    float horsepowerY = 0.05f; //0.05f 0.02f
//    auto xShift = 0.0f;
    auto verticalPos = 0.0f;

    if (pet) {

        CCSprite *_arrowSprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalArrow);
        _arrowSprite->setPosition(ccp(getContentSize().width * 0.1f + _arrowSprite->getContentSize().width * 0.45f,
                getContentSize().height * (0.095f + petSpineAndArrowY)));
        dialogGroup->addChild(_arrowSprite, 2);

        double _petScale = sizeMultiplier;//*0.925926667f;

        CCSkeletonAnimation *_petSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, pet->pVo.src.c_str());
        _petSpine->setScale(_petScale);
        _petSpine->setAnimation("habitat_idle_stand", true);
        _petSpine->timeScale = 1.0f;
        _petSpine->setPosition(ccp(getContentSize().width * 0.1f - 50*sizeMultiplier,
                getContentSize().height * petSpineAndArrowY));
        dialogGroup->addChild(_petSpine, 2);

        const char *petLevelFormat = g_gameContent->Word["trade_pet_required_max_level"].c_str();

        if (kAnimalIsAtMaxLevel) {
            CCSprite *_mark = UIHelper::getSprite(UIHelper::HUDCheckmarkSmall);
            _mark->setPosition(_petSpine->getPosition() +
                    ccp(getContentSize().width * 0.03f,
                            getContentSize().width * 0.01f));
            _mark->setScale(1.25f);
            dialogGroup->addChild(_mark, 2);

            petLevelFormat = g_gameContent->Word["trade_pet_level_max"].c_str();
        }

        CCSkeletonAnimation *_targetPetSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, vo.src.c_str());
        _targetPetSpine->setScale(_petScale * 1.1f);
        _targetPetSpine->setAnimation("track_run", true);


        CCString *str = CCString::createWithFormat("%d",
                ((int) pet->ppVo.top_speed +
                        (int) pet->ppVo.corner_speed +
                        (int) pet->ppVo.jumping));

        CCSprite *horsePowerPet = CreateSpriteHelper::getHorsePowerSprite(str->getCString(), pet->pVo.rarity_stars);
        horsePowerPet->setPosition(_petSpine->getPosition()
                - ccp(0,
                getContentSize().height * horsepowerY));
//        horsePowerPet->setScale(0.75f);
        dialogGroup->addChild(horsePowerPet, 3);

        const char *targetPetTitle = vo.title.c_str();

        _targetPetSpine->timeScale = 1.0f;
        _targetPetSpine->setPosition(ccp(getContentSize().width * 0.1f + _arrowSprite->getContentSize().width * 1.1f + 100*sizeMultiplier,
                getContentSize().height * petSpineAndArrowY));
        dialogGroup->addChild(_targetPetSpine, 2);

//        snprintf(_buffer2, _buffer2_size, petLevelFormat, pet->ppVo.level, pet->pVo.max_level);
        snprintf(_buffer, _buffer_size, "%s\n%s", pet->pVo.title.c_str(), petLevelFormat);

        CCLabelBMFont *_petLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium - 5.0f, false, kTextColorLightBrown,
                getContentSize().width * 0.225f, kCCTextAlignmentCenter);
        if (!kAnimalIsAtMaxLevel) {
            _petLabel->setColor(kTextColorOrange);
        }
        _petLabel->setAnchorPoint(ccp(0.5f, 1.0f));
        _petLabel->setPosition(_petSpine->getPosition()
                - ccp(0,
                getContentSize().height * labelY));
        dialogGroup->addChild(_petLabel, 2);

        verticalPos = _petLabel->getPositionX();

        if (pet) {
            if (kNextEvoIsUnknown) {
                str = CCString::create("??");

                //Find highest unlocks_at_level
                for (int i = 0; i < pet->pVo.next_evolution.size(); i++) {
                    FloatKeyValue _tmp_fkv = pet->pVo.next_evolution[i];
                    if (_tmp_fkv.key != "") {
                        GDPetVO _tmp_pet_vo = g_gameContent->getPetVO(_tmp_fkv.key);
                        if (m_unlocks_at_level < _tmp_pet_vo.unlocks_at_level) {
                            m_unlocks_at_level = _tmp_pet_vo.unlocks_at_level;
                        }
                    }
                }
                _targetPetSpine->setColor(ccBLACK);

                targetPetTitle = g_gameContent->Word["unknown"].c_str();
            } else {
                if (kAnimalIsAtMaxLevel) {
                    str = CCString::createWithFormat("%d",
                            ((int) pet->ppVo.top_speed +
                                    (int) pet->ppVo.corner_speed +
                                    (int) pet->ppVo.jumping));
                } else {
                    str = CCString::createWithFormat("%d",
                            ((int) vo.base_top_speed +
                                    (int) vo.base_corner_speed +
                                    (int) vo.base_jumping));
                }
            }
        }

        CCSprite *horsePowerTargetPet = CreateSpriteHelper::getHorsePowerSprite(str->getCString(), vo.rarity_stars);
        horsePowerTargetPet->setPosition(_targetPetSpine->getPosition()
                - ccp(0,
                getContentSize().height * horsepowerY));
//        horsePowerTargetPet->setScale(0.75f);
        dialogGroup->addChild(horsePowerTargetPet, 3);

        snprintf(_buffer, _buffer_size, "%s\n%s", targetPetTitle, g_gameContent->Word["trade_pet_level_1"].c_str());

        CCLabelBMFont *_targetPetLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium - 5.0f, false, kTextColorLightBrown,
                getContentSize().width * 0.225f, kCCTextAlignmentCenter);
        _targetPetLabel->setAnchorPoint(ccp(0.5f, 1.0f));
        _targetPetLabel->setPosition(_targetPetSpine->getPosition()
                - ccp(0,
                getContentSize().height * labelY));
        dialogGroup->addChild(_targetPetLabel, 2);

        m_targetPetTitle = targetPetTitle;

    } else {
        double _petScale = sizeMultiplier * 1.2f;

        auto xShift = 40*sizeMultiplier;

        CCSprite *_arrowSprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalArrow);
        _arrowSprite->setPosition(ccp(-_arrowSprite->getContentSize().width * 0.5f + xShift,
                getContentSize().height * (0.097f + petSpineAndArrowY)));

        dialogGroup->addChild(_arrowSprite, 0);

        CCSkeletonAnimation *_targetPetSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, vo.src.c_str());
        _targetPetSpine->setScale(_petScale);
        _targetPetSpine->setAnimation("track_run", true);
        _targetPetSpine->timeScale = 1.0f;
        _targetPetSpine->setPosition(ccp(getContentSize().width * 0.2f + xShift,
                getContentSize().height * petSpineAndArrowY));
        dialogGroup->addChild(_targetPetSpine, 2);

        CCString *str = CCString::createWithFormat("%d",
                ((int) vo.base_top_speed +
                        (int) vo.base_corner_speed +
                        (int) vo.base_jumping));

        CCSprite *horsePowerTargetPet = CreateSpriteHelper::getHorsePowerSprite(str->getCString(), vo.rarity_stars);
        horsePowerTargetPet->setPosition(_targetPetSpine->getPosition()
                - ccp(0,
                getContentSize().height * (horsepowerY * 1.2f)));
//        horsePowerTargetPet->setScale(0.75f);
        horsePowerTargetPet->setPositionX(horsePowerTargetPet->getPositionX()/* + xShift*/);
        dialogGroup->addChild(horsePowerTargetPet, 3);

        snprintf(_buffer, _buffer_size, "%s\n%s", vo.title.c_str(), g_gameContent->Word["trade_pet_level_1"].c_str());

        CCLabelBMFont *_targetPetLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, true, kTextColorWhite, getContentSize().width * 0.5f, kCCTextAlignmentCenter);
        _targetPetLabel->setAnchorPoint(ccp(0.5f, 1.0f));
        _targetPetLabel->setPosition(_targetPetSpine->getPosition()
                - ccp(0,
                getContentSize().height * (labelY * 1.2f)));

        _targetPetLabel->setPositionX(_targetPetLabel->getPositionX()/* + xShift*/);

        dialogGroup->addChild(_targetPetLabel, 2);

//        dialogGroup->setPositionX(dialogGroup->getPositionX() + xShift);

        m_targetPetTitle = vo.title;
    }

    bool _show_button = true;
    Building *_market = m_playerState->getBuilding(kBuildingFusion, 0);
    if (_market) {
        if (_market->pbVo.level < m_unlocks_at_level) {
            _show_button = false;
        }
    }

    if (!_show_button) {

        if (pet) {
            if (pet->pVo.next_evolution.size() > 1) {
                snprintf(_buffer, _buffer_size, g_gameContent->Word["pet_unlocks_at_level_msg_evolution"].c_str(), g_gameContent->Word["unknown"].c_str(), _market->bVo.title.c_str(), m_unlocks_at_level);
            } else {
                GDPetVO _petvo = g_gameContent->getPetVO(pet->pVo.next_evolution[0].key);

                snprintf(_buffer, _buffer_size, g_gameContent->Word["pet_unlocks_at_level_msg_evolution"].c_str(), _petvo.title.c_str(), _market->bVo.title.c_str(), m_unlocks_at_level);
            }
        } else {
            snprintf(_buffer, _buffer_size, g_gameContent->Word["pet_unlocks_at_level_msg"].c_str(), vo.title.c_str(), _market->bVo.title.c_str(), m_unlocks_at_level);
        }

        CCLabelBMFont *_tradeLabel = CCLabelHelper::createBMFont(_buffer, StyleTextAboveButton);
        _tradeLabel->setColor(kTextColorOrange);
        _tradeLabel->setWidth(500.0f * sizeMultiplier);
        _tradeLabel->setAlignment(kCCTextAlignmentCenter);
        _tradeLabel->setPosition(ccp(getContentSize().width * 0.18f, -getContentSize().height * 0.26f));
        dialogGroup->addChild(_tradeLabel, 2);

    } else {

#ifndef DEBUG_ANIMAL_TRADING_ON_VISIT
        if (m_playerState == g_playerState) {
#endif

        CCSprite *_bgTradeSprite = UIHelper::getSprite(UIHelper::UpgradeAndTradeItemsForAnimalBuyButton);

        CCSprite *_coinSprite = UIHelper::getSprite(UIHelper::NSBuyCoinIcon);
        _coinSprite->setPosition(ccp(_bgTradeSprite->getContentSize().width * 0.4f - _coinSprite->getContentSize().width * 0.5f, _bgTradeSprite->getContentSize().height * 0.35f));
        _bgTradeSprite->addChild(_coinSprite);

        m_label_coin_value = CCLabelHelper::createBMFont("", kFontSizeSmall, true, kTextColorYellow);
        m_label_coin_value->setAnchorPoint(ccp(0.0f, 0.5f));
        m_label_coin_value->setPosition(ccp(_coinSprite->getPositionX() +  40*sizeMultiplier, _coinSprite->getPositionY()));
        _bgTradeSprite->addChild(m_label_coin_value);

        CCMenuItemScaleSprite *_menuItemTrade = CCMenuItemScaleSprite::create(_bgTradeSprite, this, menu_selector(AnimalTradeDialog::onButton));
        menuTrade = CCMenu::create(_menuItemTrade, NULL);
        menuTrade->setContentSize(_menuItemTrade->getContentSize());

        float x = pet!=NULL?0.2f:0.23f;

        menuTrade->setPosition(ccp(getContentSize().width * x, -getContentSize().height * 0.39f + 100 * sizeMultiplier));
        dialogGroup->addChild(menuTrade, 2);

        CCLabelBMFont *_tradeLabel = CCLabelHelper::createBMFont(g_gameContent->Word["trade"].c_str(), kFontSizeMedium, true);
        _tradeLabel->setPosition(_bgTradeSprite->getContentSize().width*0.5f, _bgTradeSprite->getContentSize().height*0.77f);
        _bgTradeSprite->addChild(_tradeLabel, 1);
        //_bgTradeSprite->setPositionY(_bgTradeSprite->getPositionY()  + 50);


#ifndef DEBUG_ANIMAL_TRADING_ON_VISIT
        }
#endif

    }

    addHelpButton("note_item_storage_dialog", menu_selector(AnimalTradeDialog::onNote));

    this->updateContent();
    
    return true;
}

void AnimalTradeDialog::updateContent() {
    
    int i=0, j=0, k=0;
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    Building* _building_market = m_playerState->getBuilding(kBuildingFusion, 0);
    
    int numberOfRow = 0;
    CCSize sizeItem = CCSizeMake(m_scrollView->getContentSize().width*0.5f, m_scrollView->getContentSize().width*0.5f);
    
    m_complete = false;
    m_not_enough_items = false;
    m_not_enough_coins = false;
    m_not_enough_animal_level = false;
    m_not_enough_market_level = false;
    
#ifdef DEBUG_ALL_ITEMS_NEEDED_TO_FUSE_AND_EVOLVE
    //test all pets to see if items in the items_needed_to_evolve list exists or not
    {
        AnimalTradeItem *_item ;
        
        int countTest = 0;
        
        //177
        for (vector<GDPetVO>::iterator it=g_gameContent->Pet.begin(); it!=g_gameContent->Pet.end(); ++it) {
            
            GDPetVO _pet_vo = *it;
            
            countTest++;
            CCLog_("countTest:%d",countTest);
            
            for (vector<IntKeyValue>::iterator it = _pet_vo.items_needed_to_evolve.begin(); it != _pet_vo.items_needed_to_evolve.end(); ++it) {
                IntKeyValue _item_needed = (IntKeyValue)(*it);
                
                CCLog_("_item_needed.key:%s, _item_needed.value:%d", _item_needed.key.c_str(), _item_needed.value);
                
                if (_item_needed.key != "") {
                    GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                    if (_itvo.type != "") {
                        
                        _item = AnimalTradeItem::create(_itvo, _item_needed, sizeItem, m_playerState);
                        _item->setTarget(this, menu_selector(AnimalTradeDialog::selectedItemUpgrade), CCNikScrollViewButtonStateTouchUp);
                        _item->setPosition(ccp(_item->getContentSize().width*0.5f + _item->getContentSize().width*i,
                                               m_scrollView->getContentView().height - _item->getContentSize().height*(0.5f + j)));
                        m_scrollView->addChild(_item);
                        
                        if (_item->isComplete()) {
                            k++;
                            CCLog_("%s isCompleted: k is now:%d", _item->itemVO.src.c_str(), k);
                        } else {
                            CCLog_("%s is NOT completed: k is still :%d", _item->itemVO.src.c_str(), k);
                        }
                        
                        i++;
                        if (i >= 2) {
                            i = 0;
                            j++;
                        }
                        
                    } else {
                        CCLog_("HOOFIT-Warning!!!!!:items_needed_to_evolve potential bug for _item_needed.key:%s", _item_needed.key.c_str());
                    }
                }
            }
            
            
            
            for (vector<IntKeyValue>::iterator it = vo.items_needed_for_fusion.begin(); it != vo.items_needed_for_fusion.end(); ++it) {
                IntKeyValue _item_needed = (IntKeyValue)(*it);
                if (_item_needed.key != "") {
                    GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                    if (_itvo.type != "") {
                        
                        AnimalTradeItem *_item = AnimalTradeItem::create(_itvo, _item_needed, sizeItem, m_playerState);
                        _item->setTarget(this, menu_selector(AnimalTradeDialog::selectedItemUpgrade), CCNikScrollViewButtonStateTouchUp);
                        _item->setPosition(ccp(_item->getContentSize().width*0.5f + _item->getContentSize().width*i,
                                               m_scrollView->getContentView().height - _item->getContentSize().height*(0.5f + j)));
                        m_scrollView->addChild(_item);
                        
                        if (_item->isComplete()) {
                            k++;
                        }
                        
                        i++;
                        if (i >= 2) {
                            i = 0;
                            j++;
                        }
                        
                    } else {
                        CCLog_("HOOFIT-Warning!!!!!:items_needed_for_fusion potential bug for _item_needed.key:%s", _item_needed.key.c_str());
                    }
                }
            }
        }
    }
#endif
    int marginBottom = 25;
    if (pet) { //evolve
        
        int countItemsNeededToEvolve = 0;
        
        numberOfRow = (int)ceil(pet->pVo.items_needed_to_evolve.size()/2.f);
        int scrollWidth =m_scrollView->getContentSize().width;
        int scrollHeight =sizeItem.height*numberOfRow;
        
        
        m_scrollView->setContentView(CCSizeMake(scrollWidth, scrollHeight + marginBottom));
        AnimalTradeItem *_item ;
        for (vector<IntKeyValue>::iterator it = pet->pVo.items_needed_to_evolve.begin(); it != pet->pVo.items_needed_to_evolve.end(); ++it) {
            IntKeyValue _item_needed = (IntKeyValue)(*it);
            
            CCLog_("_item_needed.key:%s, _item_needed.value:%d", _item_needed.key.c_str(), _item_needed.value);
            
            if (_item_needed.key != "") {
                GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                if (_itvo.type != "") {
                    
                     _item = AnimalTradeItem::create(_itvo, _item_needed, sizeItem, m_playerState);
                    _item->setTarget(this, menu_selector(AnimalTradeDialog::selectedItemUpgrade), CCNikScrollViewButtonStateTouchUp);
                    _item->setPosition(ccp(_item->getContentSize().width*0.5f + _item->getContentSize().width*i,
                                           m_scrollView->getContentView().height - _item->getContentSize().height*(0.5f + j)));
                    m_scrollView->addChild(_item);
                    
                    countItemsNeededToEvolve++;
                    
                    if (_item->isComplete()) {
                        k++;
                        CCLog_("%s isCompleted: k is now:%d", _item->itemVO.src.c_str(), k);
                    } else {
                        CCLog_("%s is NOT completed: k is still :%d", _item->itemVO.src.c_str(), k);
                    }
                    
                    i++;
                    if (i >= 2) {
                        i = 0;
                        j++;
                    }
                    
                } else {
                    CCLog_("HOOFIT-Warning!!!!!: potential bug for _item_needed.key:%s", _item_needed.key.c_str());
                }
            }
        }
        
//        snprintf(_buffer, _buffer_size, "%d ",  pet->pVo.coins_needed_to_evolve );
        
        snprintf(_buffer, _buffer_size, "%s ",  CCStringHelper::createWithNumberFormat(pet->pVo.coins_needed_to_evolve).c_str() );
        
        if (m_label_coin_value) {
            m_label_coin_value->setString(_buffer);
        }
        
        CCLog_("k:%d, pVo.items_needed_to_evolve.size():%lu", k, pet->pVo.items_needed_to_evolve.size());
        
        if (countItemsNeededToEvolve != pet->pVo.items_needed_to_evolve.size()) {
            CCLog_("HOOFIT-Warning!!!!: potential bug count items needed to evolve mismatch");
        }
        
        if (k < pet->pVo.items_needed_to_evolve.size()) { //if (k < countItemsNeededToEvolve) {
            m_not_enough_items = true;
        }
        
    }else { //fuse
    
        numberOfRow = (int)ceil(vo.items_needed_for_fusion.size()/2.f);
        m_scrollView->setContentView(CCSizeMake(m_scrollView->getContentSize().width, sizeItem.height*numberOfRow+marginBottom));

        for (vector<IntKeyValue>::iterator it = vo.items_needed_for_fusion.begin(); it != vo.items_needed_for_fusion.end(); ++it) {
            IntKeyValue _item_needed = (IntKeyValue)(*it);
            if (_item_needed.key != "") {
                GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                if (_itvo.type != "") {
                    
                    AnimalTradeItem *_item = AnimalTradeItem::create(_itvo, _item_needed, sizeItem, m_playerState);
                    _item->setTarget(this, menu_selector(AnimalTradeDialog::selectedItemUpgrade), CCNikScrollViewButtonStateTouchUp);
                    _item->setPosition(ccp(_item->getContentSize().width*0.5f + _item->getContentSize().width*i,
                                           m_scrollView->getContentView().height - _item->getContentSize().height*(0.5f + j)));
                    m_scrollView->addChild(_item);
                    
                    if (_item->isComplete()) {
                        k++;
                    }
                    
                    i++;
                    if (i >= 2) {
                        i = 0;
                        j++;
                    }
                    
                }
            }
        }
        //comment by panya
//        snprintf(_buffer, _buffer_size, "%d", vo.coins_needed_for_fusion);
        snprintf(_buffer, _buffer_size, "%s ",  CCStringHelper::createWithNumberFormat(vo.coins_needed_for_fusion).c_str() );
        
        if (m_label_coin_value) {
            m_label_coin_value->setString(_buffer);
        }
        
        if (k < vo.items_needed_for_fusion.size()) {
            m_not_enough_items = true;
        }
    }
    
    int _coins_needed = 0;
    
    //if evolve
    if (pet) {
        if (!kAnimalIsAtMaxLevel) {
            m_not_enough_animal_level = true;
        }
        
        _coins_needed = pet->pVo.coins_needed_to_evolve;
    }
    //if fuse
    else {
        _coins_needed = vo.coins_needed_for_fusion;
    }
    
    //both----
    if (m_playerState->player.balance.gold < _coins_needed) {
        m_not_enough_coins = true;
    }
    
    if (_building_market->pbVo.level < m_unlocks_at_level) {
        m_not_enough_market_level = true;
    }
    
    if (!(m_not_enough_items ||
          m_not_enough_animal_level ||
//          m_not_enough_coins || //user should still have m_complete=true even if they don't have enough coins, so they can purchase coins at the instant dialog if everything else is complete
          m_not_enough_market_level))
    {
        m_complete = true;
    }
    //end both----
    
    //if fuse
    if (!pet) {
        if (Building *_building = m_playerState->getBuilding(kBuildingAnimal, 0)) {
            int n_homeless_pet = m_playerState->getNHomelessPet();
            int n_slot = _building->bVo.param_1;
            if (n_homeless_pet >= n_slot &&
                m_playerState->getAvailableHabitat() == NULL) {
                
                m_not_enough_space_to_fuse = true;
                m_complete = false;
            }
        } else {
            m_complete = false;
            g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, "No animal barn");
        }
    }
    
//    m_scrollView->setContentView(CCSizeMake(m_scrollView->getContentSize().width, sizeItem.height*numberOfRow));
    m_scrollView->setContentOffset(ccp(0, MAX(0, m_scrollView->getContentView().height - m_scrollView->getContentSize().height)));
}

void AnimalTradeDialog::selectedItemUpgrade(AnimalTradeItem *item) {
    
    if (m_tradeEffectRunning) return;
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    CCSize _popoverSize = CCSizeZero;
    
    // 1. create all elements and sum the content size
    CCLabelBMFont *_titleLabel = CCLabelHelper::createBMFont(item->itemVO.title.c_str(), kFontSizeMedium, true);
    CCSize _contentSize = CCNodeHelper::getContentSize(_titleLabel);
    _popoverSize.height += _contentSize.height + kPopOverDialogSpacing*sizeMultiplier;
    if (_popoverSize.width < _contentSize.width) {
        _popoverSize.width = _contentSize.width;
    }
    
    CCLabelBMFont *_descriptionLabel = CCLabelHelper::createBMFont(item->itemVO.description.c_str(), kFontSizeMedium-5.0f, false, kTextColorDark, kPopOverDialogNarmalWidth*sizeMultiplier, kCCTextAlignmentCenter);
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
    
    popover->presentPopover(item, true);

}

//void AnimalTradeDialog::onClose(CCObject *sender_) {
//
//    if (!m_closeButtonEnabled) return;
//    if (m_tradeEffectRunning) return;
//
//    g_gameAudio->playEffect(AUDIO_CLOSE_CLICK);
//
//    if (this->numberOfRunningActions() > 0) return;
//
//    this->m_modalController->popView();
//}

void AnimalTradeDialog::onHitBG(void *inst_, void *sender_) {
    
    CCLog_("Block Touch!");
    
}

void AnimalTradeDialog::animateTrade(CCObject *sender, GDPetVO petVo_, GDPetVO vo_)
{
    m_tradeEffectRunning = true;
    
    dialogGroup->setVisible(false);
    dialogGroup->setScale(0.0f); //also disable touches
    
    if (m_closeButton != NULL) {
        m_closeButton->setScale(0.0f);
        m_closeButton->setVisible(false);
        m_closeButton->setEnabled(false);
    }
    if (m_header != NULL) {
        m_header->setVisible(false);
    }
    if (m_help != NULL) {
        m_help->setScale(0.0f);
        m_help->setVisible(false);
        m_help->setEnabled(false);
    }
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    CCPhatTouchableSprite *dotBg = CCPhatTouchableSprite::createWithSpriteFrameName("trade_effect_dot.png");
    dotBg->setSender(this);
    dotBg->onHit = AnimalTradeDialog::onHitBG;
    dotBg->setScaleX(_screenSize.width*20);
    dotBg->setScaleY(_screenSize.height*20);
    dotBg->setColor(ccc3(0, 0, 0));
    addChild(dotBg, 99998, DOT_BG_TAG);
    
    CCSprite *_solid_black = blankSpriteWithSize(_screenSize);
    CCPhatTouchableSprite *m_fade_sprite = new CCPhatTouchableSprite();
    m_fade_sprite->autorelease();
    m_fade_sprite->initWithTexture(_solid_black->getTexture());
    m_fade_sprite->setAnchorPoint(CCPointZero);
//    m_fade_sprite->setColor(ccBLACK);
    m_fade_sprite->setOpacity(0);
    m_fade_sprite->setSender(this);
    m_fade_sprite->onHit = onHitBG;
    m_fade_sprite->setPosition(_screenSize*-0.25f); //for some unknown reason, this works where other values don't
    addChild(m_fade_sprite, 99997, BLOCK_TOUCH_SPRITE_TAG);
    
    CCNode *_itemsGroup = CCNode::create();
    CCSpriteBatchNode *_batchNode = CCSpriteBatchNode::create("common/particle_effect/animal_trade_effect.png", 100);
    
    addChild(_itemsGroup, 99999, ITEMS_GROUP_TAG);
    addChild(_batchNode, 100000, FUSE_BATCHNODE_TAG);
    
    
    //ITEMS GROUP---------
    
    CCNode *node1 = CCNode::create();
    CCNode *node2 = CCNode::create();
    CCNode *node3 = CCNode::create();
    CCNode *node4 = CCNode::create();
    CCNode *node5 = CCNode::create();
    
    _itemsGroup->addChild(node1, 0, 1);
    _itemsGroup->addChild(node2, 0, 2);
    _itemsGroup->addChild(node3, 0, 3);
    _itemsGroup->addChild(node4, 0, 4);
    _itemsGroup->addChild(node5, 0, 5);
    
    _itemsGroup->setScale(ITEMS_SCALE *sizeMultiplier);
    
    float baseAngle = 0;
    float howMany = 5;
    
    //get 5 items
    
    vector<GDItemVO> itemList;
    
    if (petVo_.type != "") {
        for (vector<IntKeyValue>::reverse_iterator it = petVo_.items_needed_to_evolve.rbegin(); it != petVo_.items_needed_to_evolve.rend(); ++it) {
            IntKeyValue _item_needed = (IntKeyValue)(*it);
            if (_item_needed.key != "") {
                GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                if (_itvo.type != "") {
                    
                    itemList.push_back(_itvo);
                }
            }
        }
    } else {
        
        for (vector<IntKeyValue>::reverse_iterator it = vo_.items_needed_for_fusion.rbegin(); it != vo_.items_needed_for_fusion.rend(); ++it) {
            IntKeyValue _item_needed = (IntKeyValue)(*it);
            if (_item_needed.key != "") {
                GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                if (_itvo.type != "") {
                    
                    itemList.push_back(_itvo);
                }
            }
        }
    }
    
    //itemList.size() must be more than 0, and afterwards must be 5 in total
    if (itemList.size() < 5) {
        int neededMoreHowMany = 5 - itemList.size();
        
        for (int i = 0; i < neededMoreHowMany; i++) {
            itemList.push_back(itemList.at(arc4random()%itemList.size()));
        }
    }
    
    for (int i = 1; i <= 5; i++) {
        
        CCNode *node = _itemsGroup->getChildByTag(i);
        CCSprite *light = CCSprite::createWithSpriteFrameName("light_center_fx.png");
        node->addChild(light, 0, 1);
        
        light->setScale(2.0f *sizeMultiplier);
        
        
        GDItemVO _itvo = itemList.at(i-1);
        CCSprite *_itemSprite = CCSprite::createWithSpriteFrameName(_itvo.src.c_str());
        if (!_itemSprite) {
            _itemSprite = CCSprite::createWithSpriteFrameName("yarn.png");
        }
        node->addChild(_itemSprite, 1, 2);
        
        
        float distance = light->getContentSize().width * 4.0f *sizeMultiplier;
        
        float angle = baseAngle + ((i-1) * (360.0f/howMany));
        
        node->setPosition(ccp(cosf(CC_DEGREES_TO_RADIANS(angle)) * distance, sinf(CC_DEGREES_TO_RADIANS(angle)) * distance));
        
    }
    
    CCPoint petOffset = ccp(10.0f * sizeMultiplier,
                            -75.0f * sizeMultiplier);
    
    CCSkeletonAnimation *_petSpine;
    //Evolution will have existing pet before replacing to new pet
    if (pet) {
        _petSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, petVo_.src.c_str());
        _petSpine->setScale(1.0f * sizeMultiplier);
        _petSpine->setAnimation("habitat_idle_stand", true);
        _petSpine->timeScale = 1.0f;
        _petSpine->setPosition(petOffset);
        addChild(_petSpine, 100001, PET_SPINE_TAG);
    }
    
    CCSkeletonAnimation *_targetPetSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, vo_.src.c_str());
    _targetPetSpine->setAnimation("habitat_idle_stand", true);
    _targetPetSpine->timeScale = 1.0f;
    _targetPetSpine->setVisible(false);
    _targetPetSpine->setScale(0.5f * sizeMultiplier);
    _targetPetSpine->setPosition(petOffset);
    addChild(_targetPetSpine, 100002, TARGET_PET_SPINE_TAG);
    
    //BATCH NODE----------
    
    CCSprite *flash = CCSprite::createWithSpriteFrameName("flash.png");
    CCSprite *twist = CCSprite::createWithSpriteFrameName("twist.png");
    CCSprite *twist2 = CCSprite::createWithSpriteFrameName("ww.png");
    CCSprite *light_center_fx = CCSprite::createWithSpriteFrameName("light_center_fx.png");
    
    flash->setScale(1.85f *mult1 *sizeMultiplier);
    twist->setScale(1.77f *mult1 *sizeMultiplier);
    twist2->setScale(1.54f *mult1 *sizeMultiplier);
    light_center_fx->setScale(2.8f *mult1 *sizeMultiplier);
    
    _batchNode->addChild(flash, 1);
    _batchNode->addChild(twist, 2);
    _batchNode->addChild(twist2, 3);
    _batchNode->addChild(light_center_fx, 10000);

    //should happen at the same time as stars explosion
    flash->runAction(CCSequence::create(CCDelayTime::create(FIRST_STAGE_DURATION),
                                        CCScaleTo::create(0.14f, 80.0f), //46
                                        CCHide::create(),
                                        NULL));
    
    if (petVo_.type != "") {
        _petSpine->runAction(CCSequence::create(CCScaleTo::create(FIRST_STAGE_DURATION, 1.25f * sizeMultiplier),
                                                CCScaleTo::create(0.14f, 0.0f),
                                                NULL));
    }
    
    _targetPetSpine->runAction(CCSequence::create(CCDelayTime::create(FIRST_STAGE_DURATION),
                                                  CCShow::create(),
                                                  CCEaseBackOut::create(CCScaleTo::create(0.25f, 2.0f * sizeMultiplier)),
                                                  NULL));
    
    _itemsGroup->runAction(CCSpawn::create(CCSequence::create(CCRotateBy::create(FIRST_STAGE_DURATION_2, SPIN_ANGLE*0.25f),
                                                              CCRotateBy::create(SECOND_STAGE_DURATION_2, SPIN_ANGLE*0.45f),
                                                              NULL),
                                           CCSequence::create(CCScaleTo::create(FIRST_STAGE_DURATION_2, ITEMS_SCALE_TO_2 *sizeMultiplier),
                                                              CCScaleTo::create(SECOND_STAGE_DURATION_2, ITEMS_SCALE_TO_3 *sizeMultiplier),
                                                              NULL),
                                           NULL));
    
    twist->runAction(CCSpawn::create(CCRotateBy::create(SPIN_DURATION, -SPIN_ANGLE),
                                     CCSequence::create(CCScaleTo::create(FIRST_STAGE_DURATION, 4.12f *mult1 *sizeMultiplier),
                                                        CCEaseBackOut::create(CCScaleTo::create(SECOND_STAGE_DURATION, 8.29f *mult2 *sizeMultiplier)),
                                                        NULL),
                                     NULL));
    twist2->runAction(CCSpawn::create(CCRotateBy::create(SPIN_DURATION, SPIN_ANGLE),
                                      CCSequence::create(CCScaleTo::create(FIRST_STAGE_DURATION, 3.57f *mult1 *sizeMultiplier),
                                                         CCEaseBackOut::create(CCScaleTo::create(SECOND_STAGE_DURATION, 7.21f *mult2 *sizeMultiplier)),
                                                         NULL),
                                      NULL));
    light_center_fx->runAction(CCSpawn::create(CCRotateBy::create(SPIN_DURATION, SPIN_ANGLE),
                                               CCSequence::create(CCScaleTo::create(FIRST_STAGE_DURATION, 6.46f *mult1 *sizeMultiplier),
                                                                  CCEaseBackOut::create(CCScaleTo::create(SECOND_STAGE_DURATION, 13.02f *mult2 *sizeMultiplier)),
                                                                  NULL),
                                               NULL));
    
    this->runAction(CCSequence::create(CCDelayTime::create(1.0f),
                                       CCCallFunc::create(this, callfunc_selector(AnimalTradeDialog::playerFuseEvolveSound)),
                                       CCDelayTime::create(1.7f),
                                       CCCallFunc::create(this, callfunc_selector(AnimalTradeDialog::playerFuseEvolveSound2)),
                                       NULL));
    
    m_fuseAnimationTime = 0.0f;
    m_starsExploded = false;
    m_timeUntilUpdateStarts = 0.85f;
    m_timeUntilRandomLightParticle = TIME_UNTIL_RANDOM_LIGHT_PARTICLE;
    scheduleUpdate();
    
    runAction(CCSequence::create(CCDelayTime::create(5.0f),
                                 CCCallFunc::create(this, callfunc_selector(AnimalTradeDialog::clearTradeAnimation)),
                                 NULL));
    
}

void AnimalTradeDialog::playerFuseEvolveSound(CCObject *sender)
{
    g_gameAudio->playEffect(AUDIO_FUSE_EVOLVE);
}

void AnimalTradeDialog::playerFuseEvolveSound2(CCObject *sender)
{
    CCLog_("playedplayedplayedplayed");
    CCLog_("%s",AUDIO_FUSE_EVOLVE_2);
    g_gameAudio->playEffect(AUDIO_FUSE_EVOLVE_2);
}

float AnimalTradeDialog::randNum() //from 0.0 - 1.0
{
    return (double)arc4random() / ARC4RANDOM_MAX;
}

float AnimalTradeDialog::randNum(float min, float max)
{
    return min + (((double)arc4random() / ARC4RANDOM_MAX) * (max-min));
}

float AnimalTradeDialog::randNum(double randNum, float min, float max)
{
    return min + ((randNum / ARC4RANDOM_MAX) * (max-min));
}

void AnimalTradeDialog::update(float delta_)
{
    m_fuseAnimationTime += delta_;
    
    if (m_timeUntilUpdateStarts > 0.0f) {
        m_timeUntilUpdateStarts -= delta_;
        
        if (m_timeUntilUpdateStarts > 0.0f) {
            return;
        }
    }
    
    if (m_timeUntilRandomLightParticle > 0.0f) {
        m_timeUntilRandomLightParticle -= delta_;
        
        if (m_timeUntilRandomLightParticle > 0.0f) {
            return;
        }
    }
    
    if (m_fuseAnimationTime >= 2.933f && !m_starsExploded) {
        m_starsExploded = true;
        CCNode *node = getChildByTag(FUSE_BATCHNODE_TAG);
        for (int i = 0; i < 200; i++) {
            CCSprite *star = CCSprite::createWithSpriteFrameName("trade_effect_star.png");
            node->addChild(star, 10000);
            
            float angle = randNum() * 360.0f;
            float rand = randNum();
            float distance = rand * star->getContentSize().width * 5.0f *sizeMultiplier;
            
            star->setScale((1.00f + (0.75f * (1.0f-rand))) *sizeMultiplier);
            
            CCPoint point = ccp(cosf(CC_DEGREES_TO_RADIANS(angle)) * distance, sinf(CC_DEGREES_TO_RADIANS(angle)) * distance);
            star->runAction(CCSequence::create(CCSpawn::create(CCEaseExponentialOut::create(CCMoveTo::create(0.75f, point)),
                                                               CCScaleTo::create(0.75f, 0.0f),
                                                               NULL),
                                               CCRemoveSelf::create(),
                                               NULL));
        }
    }
    
    m_timeUntilRandomLightParticle = TIME_UNTIL_RANDOM_LIGHT_PARTICLE;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    CCSprite *light = CCSprite::createWithSpriteFrameName("light_center_fx.png");
    light->setScale((PARTICLE_SCALE + (randNum(-1.0f, 1.0f) * (PARTICLE_SCALE * 0.384f))) *sizeMultiplier); //+- variation 38.4%
    
    float randX = randNum(-1.0f, 1.0f) * (_screenSize.width*0.5f);
    float randY = randNum(-1.0f, 1.0f) * (_screenSize.height*0.5f);
    
    CCPoint point;
    
    if (randNum() > 0.5f) {
        
        if (randNum() > 0.5f) {
            point = ccp(randX, (_screenSize.height*0.5f));
        } else {
            point = ccp(randX, (_screenSize.height*-0.5f));
        }
    } else {
        
        if (randNum() > 0.5f) {
            point = ccp((_screenSize.width*0.5f), randY);
        } else {
            point = ccp((_screenSize.width*-0.5f), randY);
        }
    }
    
    light->setPosition(point);
    CCNode *node = getChildByTag(FUSE_BATCHNODE_TAG);
    node->addChild(light, 500);
    light->runAction(CCSequence::create(CCMoveTo::create(1.25f, CCPointZero),
                                        CCRemoveSelf::create(),
                                        NULL));
    
    //then at random particle every
}

void AnimalTradeDialog::clearTradeAnimation(CCObject *sender) {
    
    unscheduleUpdate();
    CCNode *node = getChildByTag(BLOCK_TOUCH_SPRITE_TAG);
    CCNode *node0 = getChildByTag(DOT_BG_TAG);
    CCNode *node1 = getChildByTag(ITEMS_GROUP_TAG);
    CCNode *node2 = getChildByTag(FUSE_BATCHNODE_TAG);
    CCNode *node3 = getChildByTag(PET_SPINE_TAG);
    CCNode *node4 = getChildByTag(TARGET_PET_SPINE_TAG);
    if (node != NULL)
        node->removeFromParentAndCleanup(true);
    if (node0 != NULL)
        node0->removeFromParentAndCleanup(true);
    if (node1 != NULL)
        node1->removeFromParentAndCleanup(true);
    if (node2 != NULL)
        node2->removeFromParentAndCleanup(true);
    if (node3 != NULL)
        node3->removeFromParentAndCleanup(true);
    if (node4 != NULL)
        node4->removeFromParentAndCleanup(true);
    
    scheduleOnce(schedule_selector(AnimalTradeDialog::clearTradeAnimation2), 0.5f);
   
}

void AnimalTradeDialog::clearTradeAnimation2() {
    
     m_tradeEffectRunning = false;
    
    if (BackendService::getInstance()->isWaitingForResponse()) {
        g_sceneManager->showLoading();
        return;
    }
    
    clearTradeAnimation3();
    
}

void AnimalTradeDialog::clearTradeAnimation3() {
    
    if (m_tradeEffectRunning) return;

    if (m_resp_json_str_ != "") {
        if (pet) {
            runPetEvolved(m_resp_json_str_);
        } else {
            runPetFused(m_resp_json_str_);
        }
    }
    
    m_modalController->closeAll();
    g_sceneManager->hideLoading();
    
}

void AnimalTradeDialog::runTrading(CCObject *sender_, bool instant) {
    
    if (pet) { //evolve
        BackendService::getInstance()->send(APIPetEvolve::create(m_playerState->getBuilding(pet->ppVo.located_in),
                                                                 pet,
                                                                 instant,
                                                                 this,
                                                                 AnimalTradeDialog::onPetEvolved));
        if (pet->pVo.next_evolution.size() > 1) {
            m_modalController->setHitBGToCloseAllEnabled(false);
            setCloseButtonEnabled(false);
            setSelectButtonEnabled(false);
            return; // because we don't want to run animation immediately
        }
    }else { //fuse
        Building* _availableHabitat = PlayerState::getInstance()->getAvailableHabitat();
        APIPetFuse* _request = APIPetFuse::create(vo, _availableHabitat, instant, this, AnimalTradeDialog::onPetFused);
        if (_request) {
            BackendService::getInstance()->send(_request); //if sent
        }else {
            _availableHabitat->markForPetFuse(vo, instant, this, AnimalTradeDialog::onPetFused);
        }
    }
    
    if (pet) {
        animateTrade(sender_, pet->pVo, vo);
    } else {
        //if pet is NULL, create an empty GDPetVo and send it instead
        animateTrade(sender_, GDPetVO(), vo);
    }
    
}

void AnimalTradeDialog::onButton(CCObject *sender_) {
    
    if (!m_selectButtonEnabled) return;
    if (m_tradeEffectRunning) return;
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
  
    if (m_complete) {
        
        if (m_not_enough_coins) {
            //            g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, g_gameContent->Word["trade_warning_not_enough_coins"]);
            
            if (_scene->modalController->getParent()) {
                CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                
                InstantDialog *_dialog = NULL;
                if (pet) {
                    _dialog = InstantDialog::createWithType(_scene->modalController, IT_EVOLVE_ANIMAL_BUYRESOURCE, pet ,this);
                }else {
                    _dialog = InstantDialog::createWithType(_scene->modalController, IT_FUSE_ANIMAL_BUYRESOURCE, &vo ,this);
                }
                
                if (_dialog) {
                    _dialog->setPosition(_screenSize*0.5f);
                    _scene->modalController->pushView(_dialog);
                }
            }
        } else {
            
            CCLog_("Animal trade success!");
            
#ifndef DEBUG_ANIMAL_TRADING__OPTION_DISABLE_TRADE
            runTrading(sender_, false);
#endif
        }
        
        
    }else {
        
//        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
//        g_sceneManager->notification->setMessagePosition(ccp(_screenSize.width*0.5f, _screenSize.height*0.75f));

        CREATE_CHAR_BUFFER(_buffer, 64);
        
        if (m_not_enough_items) {
            g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, g_gameContent->Word["trade_warning_not_enough_items"]);
        }
        else if (m_not_enough_animal_level) {
            snprintf(_buffer, _buffer_size, g_gameContent->Word["trade_warning_not_enough_animal_level"].c_str(), pet->pVo.title.c_str(), pet->pVo.max_level);
            
            g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, _buffer);
        }
        
        if (m_not_enough_market_level) {
            Building* _building_market = m_playerState->getBuilding(kBuildingFusion, 0);
            
            snprintf(_buffer, _buffer_size, g_gameContent->Word["trade_warning_not_enough_market_level"].c_str(), _building_market->bVo.title.c_str(), m_unlocks_at_level);
            
            g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, _buffer);
        }
        
        if (m_not_enough_space_to_fuse) {
            g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, g_gameContent->Word["animal_barn_is_full_msg"].c_str());
        }
    }
}

void AnimalTradeDialog::onPetFused(void* sender, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseChecklistData(_root);
    _playerState->parseUserRaceData(_root);
    _playerState->parseUserBalance(_root);
    _playerState->parseItemData(_root);
    _playerState->parsePetData(_root);
    Json_dispose(_root);
    
    AnimalTradeDialog *_this = (AnimalTradeDialog*)sender;
    if (!_this || typeid(*_this) != typeid(AnimalTradeDialog)) return;
    if (_this->m_tradeEffectRunning) {
        _this->m_resp_json_str_ = resp_str;
    } else {
        _this->runPetFused(resp_str);
    }
    _this->clearTradeAnimation3();
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille)) return;
    
    _scene->hudPanel->updateHUD();
    _scene->hudPanel->updateHUDTrack(_scene->isoMap->isTrackComplete());
    _scene->hudPanel->resourceBars[HUDPanelBarCoins]->addTargetAmount(g_playerState->player.balance.gold,
                                                                      g_playerState->getTotalCoinStorageCapacity(),
                                                                      AUDIO_NONE);
    _scene->hudPanel->resourceBars[HUDPanelBarCoins]->processTargetAmount();

}

void AnimalTradeDialog::onPetEvolved(void *sender, string resp_str) {
  
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    g_playerState->parseUserBalance(_root);
    g_playerState->parseItemData(_root);
    g_playerState->parsePetData(_root);
    Json_dispose(_root);
    
    AnimalTradeDialog* _this = (AnimalTradeDialog*)sender;
    if (!_this) return;
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*g_currentScene) != typeid(SceneVille))
        return;
    
    if (_this->m_tradeEffectRunning) {
        _this->m_resp_json_str_ = resp_str;
    } else {
        _this->m_resp_json_str_ = resp_str;
        GDPetVO _pet_vo = g_gameContent->getPetVO(_this->pet->ppVo.type);
        _this->animateTrade(_this, _this->pet->pVo, _pet_vo);
        _this->m_modalController->setHitBGToCloseAllEnabled(true);
        _this->setCloseButtonEnabled(true);
        _this->setSelectButtonEnabled(true);
    }
    
    _scene->hudPanel->updateHUD();
    _scene->hudPanel->updateHUDTrack(_scene->isoMap->isTrackComplete());
    _scene->hudPanel->resourceBars[HUDPanelBarCoins]->addTargetAmount(g_playerState->player.balance.gold,
                                                                      g_playerState->getTotalCoinStorageCapacity(),
                                                                      AUDIO_NONE);
    _scene->hudPanel->resourceBars[HUDPanelBarCoins]->processTargetAmount();

}


void AnimalTradeDialog::runPetFused(std::string resp_json_str_)
{
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if(_scene) {
        _scene->hudPanel->checkTrade();
    }

    //JsonAllocator allocator;
    Json *_root = Json_create(resp_json_str_.c_str());
    if (!_root) {
        return;
    }
    
    if (Json *_pets = Json_getItem(_root, "pet")) {
        
        if (Json *_pet = Json_getItemAt(_pets, 0)) {
            
            string puid = Json_getString(_pet, "puid", "");
            if (puid != "") {
                if (Pet* _pet = m_playerState->getPet(puid)) {
                    
                    if (_pet->ppVo.located_in != "") {
                        _pet->updateHunger();
                        
                  
                        Building* _buildingLocatedIn = m_playerState->getBuilding(_pet->ppVo.located_in);
                        _scene->scrollView->zoomOnPosition(_buildingLocatedIn->getPosition()
                                                             -_scene->isoMap->getContentSize()/4.f
                                                             +ccp(0,_buildingLocatedIn->bVo.src_height*sizeMultiplier*0.4f)
                                                             ,1.3f
                                                             ,1.0f);
                        _scene->isoMap->addPetToBuilding(_pet->ppVo.puid, _pet->ppVo.located_in);
                    
                    } else {
                    
                        if (Building *_building = m_playerState->getBuilding(kBuildingAnimal, 0)) {
                            
                            int n_homeless_pet = m_playerState->getNHomelessPet();
                            int n_slot = _building->bVo.param_1 + (_building->pbVo.level - 1);
                            if (n_homeless_pet < n_slot) {
                                _scene->scrollView->zoomOnPosition(_building->getPosition()
                                                                     -_scene->isoMap->getContentSize()/4.f
                                                                     +ccp(0,_building->bVo.src_height*sizeMultiplier*0.5f)
                                                                     ,1.3f
                                                                     ,1.0f);
                                
                                std::vector<GDPetVO> pets;
                                pets.push_back(_pet->pVo);
                                _building->addAnimalToBarn(pets);
                                pets.clear();
                            }
                        }
                        
                    }
                    
                    CREATE_CHAR_BUFFER(_buffer, 64);
                    snprintf(_buffer, _buffer_size, g_gameContent->Word["event_got_pet_msg"].c_str(), _pet->pVo.title.c_str());
                    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(ccs(_pet->pVo.type.c_str()), "petType");
                    FlurryCpp::logEvent(EVENT_FUSE_PET, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["petType"] = _pet->pVo.type;
                    AnalyticsClient::getInstance()->logEvent(EVENT_FUSE_PET, eventParameters);
                    
                    FBAppEventsWrapper::logEvent(EVENT_FUSE_PET);
                }
            }
        }
    }
    
    if ((m_resp_json_str_ != "")) {
        m_resp_json_str_ = "";
    }
    
    Json_dispose(_root);
 
}

void AnimalTradeDialog::runPetEvolved(std::string resp_json_str_)
{
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if(_scene) {
        _scene->hudPanel->checkTrade();
    }

    //JsonAllocator allocator;
    Json *_root = Json_create(resp_json_str_.c_str());
    if (!_root) {
        return;
    }
    
    if (Json *_pets = Json_getItem(_root, "pet")) {
        
        if (Json *_pet = Json_getItemAt(_pets, 0)) {
            
            string puid = Json_getString(_pet, "puid", "");
            if (puid != "") {
                if (Pet* _pet = m_playerState->getPet(puid)) {
                    
                    _pet->reload(true);
                    
                    if (_pet->ppVo.located_in != "") {
                        if (SceneVille* _scene = (SceneVille*)g_currentScene) {
                            Building* _buildingLocatedIn = m_playerState->getBuilding(_pet->ppVo.located_in);
                            _scene->scrollView->zoomOnPosition(_buildingLocatedIn->getPosition()
                                                                 -_scene->isoMap->getContentSize()/4.f
                                                                 +ccp(0,_buildingLocatedIn->bVo.src_height*sizeMultiplier*0.3f)
                                                                 ,1.3f
                                                                 ,1.0f);
                            
                            //TODO: should delay 0.5f second before playing level up effect
                            
                            _pet->playLevelUpEffect();
                        }
                        
                    } else {
                        if (SceneVille* _scene = (SceneVille*)g_currentScene) {
                            if (Building *_building = m_playerState->getBuilding(kBuildingAnimal, 0)) {
                                
                                _scene->scrollView->zoomOnPosition(_building->getPosition()
                                                                     -_scene->isoMap->getContentSize()/4.f
                                                                     +ccp(0,_building->bVo.src_height*sizeMultiplier*0.5f)
                                                                     ,1.3f
                                                                     ,1.0f);
                                _pet->reload(true);
                                
                                std::vector<GDPetVO> pets;
                                pets.push_back(_pet->pVo);
                                _building->addAnimalToBarn(pets);
                                pets.clear();
                            }
                        }
                    }
                    
                    CREATE_CHAR_BUFFER(_buffer, 64);
                    snprintf(_buffer, _buffer_size, g_gameContent->Word["event_got_pet_msg"].c_str(), _pet->pVo.title.c_str());
                    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
                    
                    CCDictionary *params = CCDictionary::create();
                    params->setObject(ccs(_pet->pVo.type.c_str()), "petType");
                    FlurryCpp::logEvent(EVENT_EVOLVE_PET, params);
                    
                    CSJson::Value eventParameters;
                    eventParameters["petType"] = _pet->pVo.type;
                    AnalyticsClient::getInstance()->logEvent(EVENT_EVOLVE_PET, eventParameters);
                    
                    FBAppEventsWrapper::logEvent(EVENT_EVOLVE_PET);
                }
            }
        }
    }
    
    if ((m_resp_json_str_ != "")) {
        m_resp_json_str_ = "";
    }
    
    Json_dispose(_root);
    
}

void AnimalTradeDialog::onSearch(cocos2d::CCObject *sender) {
    
    if (m_tradeEffectRunning) return;
    
    if (CCNikButton* _button = (CCNikButton*)sender) {
        
        if (AnimalTradeItem* _item = (AnimalTradeItem*)_button->getUserData()) {
            
            if (m_modalController->getNStackView() == 2) {
                if (SearchItemDialog* _dialog = SearchItemDialog::create(m_modalController, _item->itemVO)) {
                    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                    _dialog->setPosition(_screenSize*0.5f);
                    m_modalController->pushView(_dialog);
                }
            }
            
            if (CCPopOverDialog* _popOver = (CCPopOverDialog*)_button->getParent()->getParent()) {
                _popOver->dismissPopover(true);
            }
            
        }
        
    }
    
}

void AnimalTradeDialog::onNote(cocos2d::CCObject *obj_) {
    
    CREATE_CHAR_BUFFER(_buffer, 512);
    snprintf(_buffer, _buffer_size, g_gameContent->Word["note_animal_trade_dialog"].c_str(), m_targetPetTitle.c_str());
    
    CCLabelBMFont* _note_label = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium-5.0f, false, kTextColorDark, getContentSize().width*0.4f, kCCTextAlignmentCenter);
    CCSize _contentSize = CCNodeHelper::getContentSize(_note_label);
    CCSize _padding = kPopOverDialogPadding*sizeMultiplier;
    
    CCPopOverDialog* _hintPopOver = CCPopOverDialog::create(_contentSize + _padding, CCPopoverArrowDirectionUp);
    _note_label->setPosition(_hintPopOver->getContentSize()*0.5f);
    _hintPopOver->addChild(_note_label);
    _hintPopOver->presentPopover(m_help, true);
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
}
