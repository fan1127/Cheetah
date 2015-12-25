//
//  EntityAction.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/6/2557 BE.
//
//

#include "EntityAction.h"
USING_NS_CC;
using namespace std;

#include "../AppMacros.h"
extern TargetDevice targetDevice;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../Scenes/SceneVille.h"
#include "../Scenes/SceneVisit.h"
#include "../Scenes/SceneMatch.h"
extern CCScene* g_currentScene;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIPetHost.h"
#include "../Services/APIs/APIPetStore.h"

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

//#include "../Entities/TutorialManager.h"
//extern TutorialManager *g_gameTutorial;

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../UIs/Dialogs/CutTreeDialog.h"
#include "../UIs/Dialogs/AnimalDialog.h"
#include "../UIs/Dialogs/AnimalMarketDialog.h"
//#include "../UIs/Dialogs/AnimalBarnDialog.h"
#include "../UIs/Dialogs/ItemStorageDialog.h"
#include "../UIs/Dialogs/MiniInfoDialog.h"
#include "../UIs/Dialogs/MiniUpgradeDialog.h"
//#include "../UIs/Dialogs/InstantDialog.h"
#include "../UIs/Dialogs/AbilityDialog.h"
#include "../UIs/Dialogs/NotEnoughSpaceDialog.h"

#include "../DialogSettings.h"

#include "../Extensions/Modal/CCModalController.h"
#include "../Extensions/CCMenuItemScaleSprite.h"
#include "../Helpers/CCStringHelper.h"

#include "Decoration.h"

#include "Tutorials/TutFeedPet.h"

#include "../Helpers/CCLabelUpdater.h"
#include "../UIs/Dialogs/NotificationDialog.h"
#include "../Helpers/AppRating/AppRating.h"

extern float retinaMultiplier;
extern float sizeMultiplier;
extern float isIPhoneMultiplier;

bool EntityAction::init() {
    
    if (!CCNode::init()) {
        return false;
    }
  
    m_state = EAS_STANDBY;
    
    m_entity = NULL;
    m_entity_next = NULL;
    m_entityTypeCanBeTouched = "";
    m_entityTypeCanNOTBeTouched = "";
    m_entityTypeCanBeOpenned = "";
//    m_raySprite = NULL;
//    for (int i=0; i<kEntityActionProgressBarMax; i++) {
//        m_progress_bar[i] = NULL;
//    }
    
//    for (int i=0; i<kEntityActionTotalButton; i++) {
//        m_menu[i] = NULL;
//    }
//    m_menu = CCArray::create();
//    m_menu->retain();
    
    m_compLayer = CCLayer::create();
    addChild(m_compLayer, 1);
    
    m_menuLayer = CCLayer::create();
    addChild(m_menuLayer, 2);
    
    _clear();
    
    m_entityLabel = CCLabelHelper::createBMFont(retinaMultiplier, "", kEntityLabelFontSize, true);
    addChild(m_entityLabel);

    habitatAction = EntityHabitatAction::create();
    addChild(habitatAction);
    
    viewMode = IMV_PLAYABLE;
    
    m_feedTutorialRunning = false;
    
    return true;
    
}

//EntityAction::~EntityAction() {
//    
//    m_menu->release();
//    m_menu = NULL;
//    
//}

//void EntityAction::cleanup() {
//    
//    if (m_entityLabel != NULL) {
//        m_entityLabel->removeFromParent();
//    }
//    m_entityLabel = NULL;
//    
//    if (habitatAction != NULL) {
//        habitatAction->removeFromParentAndCleanup(true);
//    }
//    habitatAction = NULL;
//    
//    _clear();
//    
//    CCNode::cleanup();
//    
//}

void EntityAction::_clear() {

//    if (m_raySprite != NULL) {
//        m_raySprite->stopAllActions();
//        m_raySprite->removeFromParent();
//    }
//    m_raySprite = NULL;
    
    for (int i=0; i<kEntityActionProgressBarMax; i++) {
        if (m_progress_bar[i] != NULL) {
            m_progress_bar[i]->removeFromParent();
        }
        m_progress_bar[i] = NULL;
    }
    
//    for (int i=0; i<kEntityActionTotalButton; i++) {
//        CCLog_("clear -> %d", i);
//        if (m_menu[i] != NULL) {
//            m_menu[i]->stopAllActions();
//            m_menu[i]->removeFromParent();
////            m_menu[i]->release();
//        }
//        m_menu[i] = NULL;
//    }
//    for (int i=0; i<m_menu->count(); i++) {
//        if (CCMenu* _menu = (CCMenu*)m_menu->objectAtIndex(i)) {
//            _menu->stopAllActions();
//            _menu->removeFromParent();
//            _menu = NULL;
//        }
//    }
//    m_menu->removeAllObjects();

    m_compLayer->removeAllChildren();
    m_menuLayer->removeAllChildren();
}

void *EntityAction::getEntity() {
    
    return m_entity;
    
}

float EntityAction::getScrollViewScale() {
    
    float scale = 1.0f;
    
    CScene *scene = (CScene*)g_currentScene;
    if (scene) {
        
        scale = scene->scrollView->getScale();
    }
    
    return scale;
}

float EntityAction::getEntityActionScale() {
    // scales EntityAction so it's not too small when zoomed out
    
    float scale = 1.0f;
    
    float scaleMultiplier = 2.0f; //increase or decrease for bigger scale relating to zoom level
    
    scale = 2.0f-(((getScrollViewScale()-1.0f)*scaleMultiplier)+1.0f);
    scale = MAX(1.0f, scale);
    scale = MIN(3.0f, scale);
    
    return scale;
}

void EntityAction::showProgressBarIfConditionMeets(void *building_) {
    
    Building *_building = (Building*)building_;
    
    if (_building == NULL)
        return;
    
    if (typeid(*_building) != typeid(Building))
        return;
    
    if (m_progress_bar_new) {
        m_progress_bar_new->removeFromParentAndCleanup(true);
        m_progress_bar_new = NULL;
    }
    
    if (_building->pbVo.status == BUILDING_STATUS_UPGRADE ||
        _building->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
        
        m_progress_bar_new = ProgressBar::create(_building);
        addChild(m_progress_bar_new);
    }
    
    if (_building->bVo.building_type == BUILDING_TYPE_SKILLS) {
        
        if (Ability* _ability = g_playerState->getUpgradingAbility()) {
            _ability->update(0);
            
            m_progress_bar_new = ProgressBar::create(_ability);
            addChild(m_progress_bar_new);
        }
        
    } else if (_building->bVo.building_type == BUILDING_TYPE_HABITAT) {
        
        if (_building->pet) {
            _building->pet->updatePet(0);
            if (_building->pet->ppVo.energy < 100) {
                
                m_progress_bar_new = ProgressBar::create(_building->pet);
                addChild(m_progress_bar_new);
            }
        }
    }
    
}

void EntityAction::hideProgressBarIfConditionMeets(void *building_) {
    
    Building *_building = (Building*)building_;
    
    if (_building == NULL)
        return;
    
    if (typeid(*_building) != typeid(Building))
        return;
    
    bool isUpgradingOrDuringConstruction = (_building->pbVo.status == BUILDING_STATUS_UPGRADE ||
                                            _building->pbVo.status == BUILDING_STATUS_CONSTRUCT);
    
    if (_building->bVo.building_type == BUILDING_TYPE_SKILLS) {
        
        Ability* _ability = g_playerState->getUpgradingAbility();
        
        if (_ability == NULL &&
            !isUpgradingOrDuringConstruction) {
            
            if (m_progress_bar_new) {
                m_progress_bar_new->removeFromParentAndCleanup(true);
                m_progress_bar_new = NULL;
            }
        }
        
    } else if (_building->bVo.building_type == BUILDING_TYPE_HABITAT) {
        
        if (_building->pet) {
            _building->pet->updatePet(0);
            if (_building->pet->ppVo.energy >= 100 &&
                !isUpgradingOrDuringConstruction) {
                
                if (m_progress_bar_new) {
                    m_progress_bar_new->removeFromParentAndCleanup(true);
                    m_progress_bar_new = NULL;
                }
            }
        }
        
    } else {
        
        if (!isUpgradingOrDuringConstruction) {
            
            if (m_progress_bar_new) {
                m_progress_bar_new->removeFromParentAndCleanup(true);
                m_progress_bar_new = NULL;
            }
        }
    }
    
}

bool EntityAction::start(void *entity_) {
    
    return start(entity_, false);
    
}

bool EntityAction::start(void* entity_, bool only_feed_action_) {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    int k;
    
    if (m_entity)
        return false;

    float entityActionScale = getEntityActionScale();
    setScale(entityActionScale);
    
    m_entity_type = getEntityType(entity_);
    
    if (m_entity_type == ET_UNKNOWN)
        return false;
    
    if (m_entity_type == ET_DECORATION_REMOVABLE ||
        m_entity_type == ET_UNWANTED_TREE) {
        
        if (m_progress_bar_new) {
            m_progress_bar_new->removeFromParentAndCleanup(true);
            m_progress_bar_new = NULL;
        }
    }
    
    if (m_entity_type != ET_UNKNOWN &&
        m_entity_type != ET_DECORATION_REMOVABLE &&
        m_entity_type != ET_UNWANTED_TREE) {
        
        if (Building *_building = (Building*)entity_) {
            _building->entityActionStart(this);
            
            showProgressBarIfConditionMeets(_building);
        }
    }
    
    if (Building* _building = (Building*)entity_) {
        
        if (m_entityTypeCanNOTBeTouched != "") {
            if (_building->bVo.type == m_entityTypeCanNOTBeTouched) {
                return false;
            }
        }
        
        if (m_entityTypeCanBeTouched != "") {
            
            switch (m_entity_type) {
                case ET_BUILDING_HQ:
                case ET_BUILDING_UNCLE_HOUSE:
                case ET_BUILDING_TICKETBOOTH:
                case ET_BUILDING_COIN_STORAGE:
                case ET_BUILDING_HABITAT:
                case ET_BUILDING_ANIMAL:
                case ET_BUILDING_MARKET:
                case ET_BUILDING_INVENTORY:
                case ET_BUILDING_ASSOCIATION:
                case ET_BUILDING_PRODUCER:
                case ET_BUILDING_FOOD_STORAGE:
                case ET_BUILDING_TRAINING:
                case ET_BUILDING_EXPLORER:
                case ET_BUILDING_LIBRARY: {
                    if (_building->bVo.type != m_entityTypeCanBeTouched) {
                        return false;
                    }
                }
                    break;
                default:
                    break;
            }
        }
    }
    
    
    m_entity = entity_;
    
    switch(g_sceneManager->sceneSelector) {
        case SCENE_VILLE: {
            if (SceneVille *_scene = (SceneVille*)g_currentScene) {
                if (_scene->mapToScreenUI) {
                    _scene->mapToScreenUI->closeAllUIs();
                }
            }
        }break;
        default: {
            
        }break;
    }
    
    _clear();
    
    setToAppropriatePosition();
    
    switch (m_entity_type) {
            
        default:
        case ET_UNKNOWN: {
            return false;
        }break;
            
        case ET_UNWANTED_TREE: {
            
            if (Decoration *_unwanted_tree = (Decoration*)m_entity) {
                
                _unwanted_tree->createDecoration();
                _unwanted_tree->spineDecoration->stopAllActions();
                _unwanted_tree->spineDecoration->runAction(CCRepeatForever::create(CCSequence::create(CCTintTo::create(0.5f, 100, 100, 100),
                                                                                     CCTintTo::create(0.5f, 255, 255, 255), NULL)));
                m_entityLabel->setString(_unwanted_tree->dVo.title.c_str());
                m_entityLabel->setOpacity(0);
                
                if (viewMode == IMV_PLAYABLE) { addButton(0, EAB_CUT); }
                
            }
            
        }break;
            
        case ET_DECORATION_REMOVABLE: {
            
            if (Decoration *_decoration = (Decoration*)m_entity) {
                
                _decoration->createDecoration();
                _decoration->spineDecoration->stopAllActions();
                _decoration->spineDecoration->runAction(CCRepeatForever::create(CCSequence::create(CCTintTo::create(0.5f, 100, 100, 100),
                                                                                     CCTintTo::create(0.5f, 255, 255, 255), NULL)));
                
                m_entityLabel->setString(_decoration->dVo.title.c_str());
                m_entityLabel->setOpacity(0);
                
                if (viewMode == IMV_PLAYABLE) { addButton(0, EAB_KEEP); }
                
            }
            
        }break;
            
        case ET_BUILDING_HQ:
        case ET_BUILDING_UNCLE_HOUSE:
        case ET_BUILDING_PRODUCER:
        case ET_BUILDING_FOOD_STORAGE:
        case ET_BUILDING_INVENTORY:
        case ET_BUILDING_ASSOCIATION:
        case ET_BUILDING_MARKET:
        case ET_BUILDING_TICKETBOOTH:
        case ET_BUILDING_COIN_STORAGE:
        case ET_BUILDING_HABITAT:
        case ET_BUILDING_ANIMAL:
        case ET_BUILDING_EXPLORER:
        case ET_BUILDING_LIBRARY: {
            
            if (m_entity_type == ET_BUILDING_HABITAT) {
                if (m_feedTutorialRunning) {
                    
                    only_feed_action_ = true;
                }
            }
            
            if (Building *_building = (Building*)m_entity) {
                
                if (_building->bVo.building_type == BUILDING_TYPE_TICKETBOOTH && viewMode == IMV_PLAYABLE) {
                    if (_building->getResourceProduced() >= _building->getAmountToShowCoinIcon() &&
                        !_building->didTryToCollect &&
                        _building->pbVo.status == BUILDING_STATUS_DELIVERED) {
                        m_entity_type = ET_UNKNOWN;
                        scheduleOnce(schedule_selector(EntityAction::onCollect), kEntityActionTransitionSpeed);
                        return false;
                    }
                }
                
                if (_building->bVo.building_type == BUILDING_TYPE_HABITAT &&
                    _building->pbVo.status == BUILDING_STATUS_DELIVERED &&
                    _building->product &&
                    _building->pet &&
                    viewMode == IMV_PLAYABLE &&
                    !_building->didTryToCollect) {
                    m_entity_type = ET_UNKNOWN;
                    scheduleOnce(schedule_selector(EntityAction::onCollect), kEntityActionTransitionSpeed);
                    return false;
                }
                
                if (_building->bVo.building_type == BUILDING_TYPE_PRODUCER && viewMode == IMV_PLAYABLE) {
                    if (_building->getResourceProduced() >= _building->getAmountToShowFoodIcon() &&
                        !_building->didTryToCollect &&
                        _building->pbVo.status == BUILDING_STATUS_DELIVERED) {
                        m_entity_type = ET_UNKNOWN;
                        scheduleOnce(schedule_selector(EntityAction::onCollect), kEntityActionTransitionSpeed);
                        return false;
                    }
                }
                
                GDBuildingVO _nxt_bvo = g_gameContent->getBuildingVO(_building->pbVo.type, _building->pbVo.level+1);
                
                
                if (_building->bVo.type == kBuildingUncleHouse)
                {
                    snprintf(_buffer, _buffer_size, "%s", _building->bVo.title.c_str());
                }
                else if (_building->bVo.building_type == BUILDING_TYPE_HABITAT)
                {
                    if (_building->pet)
                    {
                        if (_building->pet->ppVo.level >= _building->pet->pVo.max_level &&
                            (g_gameContent->Word["entityaction_pet_title_level_max"] != ""))
                        {
                            snprintf(_buffer, _buffer_size, g_gameContent->Word["entityaction_pet_title_level_max"].c_str(), _building->pet->pVo.title.c_str());
                        }
                        else
                        {
                            snprintf(_buffer, _buffer_size, g_gameContent->Word["entityaction_pet_title"].c_str(), _building->pet->pVo.title.c_str(), _building->pet->ppVo.level);
                        }
                    }
                    else
                    {
                        snprintf(_buffer, _buffer_size, "%s", g_gameContent->Word["entityaction_pet_empty_title"].c_str());
                    }
                }
                else
                {
                    snprintf(_buffer, _buffer_size, g_gameContent->Word["entityaction_title"].c_str(), _building->bVo.title.c_str(), _building->pbVo.level);
                }
                
                m_entityLabel->setString(_buffer);
                m_entityLabel->setOpacity(0);
                
                if (_building->bVo.building_type == BUILDING_TYPE_HABITAT && _building->pet) {
                    
                    if ((_building->pet->isHungry())
                        &&
                        !(_building->pet->isSleeping()))
                    {
                        habitatAction->start(_building);
                    }
                    
                }
                if (!only_feed_action_) {
                    
                    if (_building->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
                        
                        //play effect under construction
                        g_gameAudio->playEffect(AUDIO_UNDERCONSTRUCTION);
                        
                        if (_building->bVo.building_type == BUILDING_TYPE_HABITAT) {
                            if (viewMode == IMV_PLAYABLE) { addButton(0, EAB_INSTANT_BUILD); }
                        } else {
                            addButton(0, EAB_INFO);
                            if (viewMode == IMV_PLAYABLE) { addButton(1, EAB_INSTANT_BUILD); }
                        }
                        
                    } else if (_building->pbVo.status == BUILDING_STATUS_UPGRADE) {
                        
                        //play effect under construction
                        g_gameAudio->playEffect(AUDIO_UNDERCONSTRUCTION);
                        
                        m_entityLabel->setPositionY(_building->bVo.src_height*retinaMultiplier*0.75f);
                        
                        switch (m_entity_type) {
                            case ET_BUILDING_HABITAT: {
    //                            addButton(0, EAB_INFO);
    //                            if (_building->pet) {
    //                                addButton(1, EAB_SET_AS_HOST);
    //                                addButton(1, EAB_SEND_TO_BARN);
    //                                addButton(2, EAB_VIEW);
    //                            }else {
    ////                                addButton(3, EAB_INSTANT_UPGRADE);
    //                            }
                            }break;
                            case ET_BUILDING_HQ:
                            case ET_BUILDING_UNCLE_HOUSE:
                            case ET_BUILDING_COIN_STORAGE:
                            case ET_BUILDING_FOOD_STORAGE:
                            case ET_BUILDING_PRODUCER:
                            case ET_BUILDING_ASSOCIATION:
                            case ET_BUILDING_TICKETBOOTH:
                            case ET_BUILDING_EXPLORER: {
                                addButton(0, EAB_INFO);
                                if (viewMode == IMV_PLAYABLE) { addButton(1, EAB_INSTANT_UPGRADE); }
                            }break;
                            case ET_BUILDING_LIBRARY:
                            default: {
                                addButton(0, EAB_INFO);
                                addButton(1, EAB_VIEW);
                                if (viewMode == IMV_PLAYABLE) { addButton(2, EAB_INSTANT_UPGRADE); }
                            }break;
                        }
                        
                    }else {
                        
                        switch (m_entity_type) {
                            case ET_BUILDING_UNCLE_HOUSE: {
                                k = 0;
                                addButton(k, EAB_INFO); k++;
                            }break;
                            case ET_BUILDING_TICKETBOOTH:
                            case ET_BUILDING_PRODUCER: {
                                k = 0;
                                addButton(k, EAB_INFO); k++;
                                if (_nxt_bvo.type != "") {
                                    if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_UPGRADE); k++; }
                                }
                                if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_COLLECT); k++; }
                                
                            }break;
                            case ET_BUILDING_HABITAT: {
                                k = 0;
                                bool _instant_energy = false;
                                if (_building->isPetContained()) {
                                    if (_building->pet->isSleeping()) {
                                        _instant_energy = true;
                                        m_entityLabel->setPositionY(_building->bVo.src_height*retinaMultiplier*0.75f);
                                    }
                                    if (!_building->m_bIsHost && _instant_energy) {
                                        if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_SET_AS_HOST); k++; }
                                        addButton(k, EAB_VIEW); k++;
                                        if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_INSTANT_ENERGY); k++; }
                                    } else if (!_building->m_bIsHost && !_instant_energy) {
                                        if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_SET_AS_HOST); k++; }
                                        addButton(k, EAB_VIEW); k++;
                                    } else if (_building->m_bIsHost && _instant_energy) {
                                        addButton(k, EAB_VIEW); k++;
                                        if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_INSTANT_ENERGY); k++; }
                                    } else if (_building->m_bIsHost && !_instant_energy) {
                                        addButton(k, EAB_VIEW); k++;
                                    }
                                }
                            }break;
                            case ET_BUILDING_HQ:
                            case ET_BUILDING_COIN_STORAGE:
                            case ET_BUILDING_FOOD_STORAGE:
                            case ET_BUILDING_ASSOCIATION:
                            case ET_BUILDING_EXPLORER: {
                                k = 0;
                                addButton(k, EAB_INFO); k++;
                                if (_nxt_bvo.type != "") {
                                    if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_UPGRADE); k++; }
                                }
                            }break;
                            case ET_BUILDING_LIBRARY: {
                                k = 0;
                                addButton(k, EAB_INFO); k++;
                                bool _upgrading = g_playerState->getUpgradingAbility();
                                if (_upgrading) {
                                    m_entityLabel->setPositionY(_building->bVo.src_height*retinaMultiplier*0.75f);
                                    
                                }else {
                                    if (_nxt_bvo.type != "") {
                                        if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_UPGRADE); k++; }
                                    }
                                }
                                addButton(k, EAB_VIEW);  k++;
                                if (_upgrading) {
                                    if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_INSTANT_ABILITY); k++; }
                                }
                            }break;
                            default: {
                                k = 0;
                                addButton(k, EAB_INFO); k++;
                                if (_nxt_bvo.type != "") {
                                    if (viewMode == IMV_PLAYABLE) { addButton(k, EAB_UPGRADE); k++; }
                                }
                                addButton(k, EAB_VIEW);  k++;
                            }break;
                        }
                                        
                        //==================== sound effect ===========================
                        
                        switch (m_entity_type) {
                            case ET_BUILDING_UNCLE_HOUSE: {
                                g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                            }break;
                            case ET_BUILDING_TICKETBOOTH: {
                                //play effect ticket booth
                                g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                            }break;
                            case ET_BUILDING_PRODUCER: {
                                //play effect food produce
                                g_gameAudio->playEffect(AUDIO_FOOD_PRODUCTION);
                            }break;
                            case ET_BUILDING_HABITAT: {
                                if(_building->pet) {
                                    //play effect habitat
                                    if(_building->pet->pVo.sound_src.compare("") == 0) g_gameAudio->playEffect(AUDIO_PIG_GRUNT);
                                    else {
                                        CREATE_CHAR_BUFFER(_buffer, 64);
                                        snprintf(_buffer, _buffer_size, "sfx/animalx/%s%s", _building->pet->pVo.sound_src.c_str(), AUDIO_EXT);
                                        g_gameAudio->playEffect(_buffer);
                                    }
                                }
                                else {
                                    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                                }
                            }break;
                            case ET_BUILDING_INVENTORY: {
                                //play effect INVENTORY
                                g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                                
                            }break;
                            case ET_BUILDING_HQ: {
                                //play effect HQ
                                g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                                
                            }break;
                            case ET_BUILDING_COIN_STORAGE: {
                                //play effect coin storage
                                g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                                
                            }break;
                            case ET_BUILDING_FOOD_STORAGE: {
                                //play effect food storage
                                g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                                
                            }break;
                            case ET_BUILDING_ASSOCIATION: {
                                //play effect association
                                g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                                
                            }break;
                            case ET_BUILDING_EXPLORER: {
                                //play effect explorer
                                g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                            }break;
                            default: {
                                //play effect general
                                g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
                            }break;
                        }
                        
                        
                    }
                    
                }
                
                for (int i=0; i<2; i++) {
                    m_callAttention[i] = CCRepeatForever::create(CCSequence::create(CCTintTo::create(0.5f, 100, 100, 100),
                                                                                    CCTintTo::create(0.5f, 255, 255, 255), NULL));
                }
                
                _building->spineBuilding->stopAllActions();
                _building->spineBuilding->runAction(m_callAttention[0]);
                if (_building->bVo.building_type == BUILDING_TYPE_HABITAT) {
                    _building->spineBuildingFront->runAction(m_callAttention[1]);
                }
                
            }
            
        }break;
            
    }
   
    int _nbar = 0;
    for (int i=0; i<kEntityActionProgressBarMax; i++) {
        if (m_progress_bar[i]) {
            _nbar ++;
        }
    }
    float _barsY[kEntityActionProgressBarMax];
    switch (_nbar) {
        case 2: {
            _barsY[0] = -56.0f;
            _barsY[1] = -128.0f;
        }break;
        case 1: {
            _barsY[0] = -128.0f;
        }break;
        default:
            break;
    }
    
    int _count = 0;
    if (m_menuLayer->getChildren()) {
        _count = m_menuLayer->getChildren()->count();
    }
    
//    for (int i=0; i<3; i++) {
//        if (m_menu[i] != NULL) {
//            _nmenu++;
//        }
//    }
    CCPoint _multipliers[3];
    CCSprite* _raySprite = NULL;
    float _raySprite_y_mult = 1.0f;
    switch (_count) {
        case 3: {
            _multipliers[0] = ccp(-1.05f, 0.05f);
            _multipliers[1] = ccp( 0.00f, 0.2f);
            _multipliers[2] = ccp( 1.05f, 0.05f);
             _raySprite = CCSprite::createWithSpriteFrameName("_grad_3.png");
        }break;
        case 2: {
            _multipliers[0] = ccp(-0.55f, 0.12f);
            _multipliers[1] = ccp( 0.55f, 0.12f);
            _raySprite = CCSprite::createWithSpriteFrameName("_grad_2.png");
            _raySprite_y_mult = 1.4f;
        }break;
        case 1: {
            _multipliers[0] = ccp( 0.00f, 0.3f);
            _raySprite = CCSprite::createWithSpriteFrameName("_grad_1.png");
            _raySprite_y_mult = 1.3f;
        }break;
        default:
            break;
    }
    
    if (Building *_building = (Building*)m_entity) {
        _building->runAction(CCSequence::create(CCScaleTo::create(0.075f, 1.14f),
                                                CCScaleTo::create(0.075f, 1.0f),
                                                NULL));
    }
    
    if (_raySprite) {
        
        _raySprite->setPositionY(-_raySprite->getContentSize().height*0.5f*_raySprite_y_mult);
        _raySprite->setOpacity(0);
        m_compLayer->addChild(_raySprite, 1);
        
        for (int i=0; i<_nbar; i++) {
            if (m_progress_bar[i]) {
                m_progress_bar[i]->setPositionY(_barsY[i]*retinaMultiplier);
            }
        }
        
        for (int i=0; i<_count; i++) {
            if (CCMenu* _menu = (CCMenu*)m_menuLayer->getChildren()->objectAtIndex(i)) {
        
                _menu->setPosition(ccp(_menu->getContentSize().width*(_multipliers[i].x*0.85f),
                                       _menu->getPositionY()-(_menu->getContentSize().height*0.5f)));
                
                _menu->stopAllActions();
                _menu->runAction(CCSequence::create(
                                                    CCDelayTime::create(kEntityActionTransitionMenuDelay*(i+1))
                                                    ,
                                                    CCShow::create()
                                                    ,
                                                    CCEaseExponentialOut::create(
                                                                                 CCMoveTo::create(kEntityActionTransitionMenuSpeed,
                                                                                                  ccp(_menu->getContentSize().width*_multipliers[i].x,
                                                                                                      _raySprite->getPositionY()-_raySprite->getContentSize().height*_multipliers[i].y))
                                                                                 )
                                                    ,
                                                    NULL
                                                    )
                                 );
            }
        }
    }
    CCModalController::showAllChildrensAnimated(this, kEntityActionTransitionSpeed);
    m_state = EAS_SHOW_MENU;
    
    
    scheduleUpdate();
    
    
    return true;
}

void EntityAction::update(float delta_)
{
    setToAppropriatePosition();
    
    if (m_entity_type != ET_UNKNOWN &&
        m_entity_type != ET_DECORATION_REMOVABLE &&
        m_entity_type != ET_UNWANTED_TREE) {
        
        if (Building *_building = (Building*)m_entity) {
            
            hideProgressBarIfConditionMeets(_building);
        }
    }
}

void EntityAction::setToAppropriatePosition()
{
    float appropriateScale = MIN(1.0f, MAX(0.5f, getScrollViewScale()));
    
    switch (m_entity_type) {
            
        default:
        case ET_UNKNOWN: {
            return;
        }break;
            
        case ET_UNWANTED_TREE: {
            
            if (Decoration *_unwanted_tree = (Decoration*)m_entity) {
                
                setPosition(_unwanted_tree->getPosition());
                m_entityLabel->setPositionY(_unwanted_tree->dVo.src_height*appropriateScale*retinaMultiplier*0.6f);
            }
            
        }break;
            
        case ET_DECORATION_REMOVABLE: {
            
            if (Decoration *_decoration = (Decoration*)m_entity) {
                
                setPosition(_decoration->getPosition());
                m_entityLabel->setPositionY(_decoration->dVo.src_height*appropriateScale*retinaMultiplier*0.6f);
            }
            
        }break;
            
        case ET_BUILDING_HQ:
        case ET_BUILDING_UNCLE_HOUSE:
        case ET_BUILDING_PRODUCER:
        case ET_BUILDING_FOOD_STORAGE:
        case ET_BUILDING_INVENTORY:
        case ET_BUILDING_ASSOCIATION:
        case ET_BUILDING_MARKET:
        case ET_BUILDING_TICKETBOOTH:
        case ET_BUILDING_COIN_STORAGE:
        case ET_BUILDING_HABITAT:
        case ET_BUILDING_ANIMAL:
        case ET_BUILDING_EXPLORER:
        case ET_BUILDING_LIBRARY: {
            
            if (Building *_building = (Building*)m_entity) {
                setPosition(_building->getPosition());
                m_entityLabel->setPositionY(_building->bVo.src_height*appropriateScale*retinaMultiplier*0.6f);
                
                if (_building->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
                    
                    m_entityLabel->setPositionY(_building->bVo.src_height*appropriateScale*retinaMultiplier*0.75f);
                    
                } else if (_building->pbVo.status == BUILDING_STATUS_UPGRADE) {
                    
                    m_entityLabel->setPositionY(_building->bVo.src_height*appropriateScale*retinaMultiplier*0.75f);
                    
                } else {
                    
                    if (m_entity_type == ET_BUILDING_HABITAT) {
                        
                        if (_building->pet) {
                            if (_building->pet->isSleeping()) {
                                m_entityLabel->setPositionY(_building->bVo.src_height*appropriateScale*retinaMultiplier*0.75f);
                            }
                        }
                    } else if (m_entity_type == ET_BUILDING_HABITAT) {
                        
                        bool _upgrading = g_playerState->getUpgradingAbility();
                        if (_upgrading) {
                            m_entityLabel->setPositionY(_building->bVo.src_height*appropriateScale*retinaMultiplier*0.75f);
                        }
                    }
                }
            }
            
        }break;
            
    }
    
    if (m_progress_bar_new) {
        m_progress_bar_new->setPosition(ccp(m_entityLabel->getPositionX(),
                                            m_entityLabel->getPositionY() - 50.0f*sizeMultiplier));
    }
}

void EntityAction::finish() {
    
    finish(NULL, true);
    
}

void EntityAction::finish(void *entity_next_) {
    
    finish(entity_next_, true);
    
}

void EntityAction::finish(void *entity_next_, bool also_finish_sub_action_) {

    if (!m_entity) {
        _restart();
        return;
    }

    if (m_entity == entity_next_) {
        return;
    }
    
    
    if (m_entity_type != ET_UNKNOWN &&
        m_entity_type != ET_DECORATION_REMOVABLE &&
        m_entity_type != ET_UNWANTED_TREE) {
        
        if (Building *_building = (Building*)m_entity) {
            _building->entityActionFinish(this);
            
            hideProgressBarIfConditionMeets(_building);
        }
    }
    
    m_entity_next = entity_next_;
    
    if (this->numberOfRunningActions() > 0) {
        return;
    }
    
    if (also_finish_sub_action_) {
        if (habitatAction) {
            habitatAction->finish();
        }
    }
    
    unscheduleUpdate();
    
    switch (m_entity_type) {
            
        default:
        case ET_UNKNOWN: {
            CCLog_("Unknown EntityActionFor!");
            _restart();
            return;
        }break;
            
        case ET_UNWANTED_TREE:
        case ET_DECORATION_REMOVABLE: {
            
            if (Decoration *_decoration = (Decoration*)m_entity) {
                
                _decoration->spineDecoration->stopAllActions();
                _decoration->spineDecoration->runAction(CCTintTo::create(kEntityActionTransitionSpeed, 255, 255, 255));
                
            }
            
        }break;
            
        case ET_BUILDING_HQ:
        case ET_BUILDING_UNCLE_HOUSE:
        case ET_BUILDING_PRODUCER:
        case ET_BUILDING_FOOD_STORAGE:
        case ET_BUILDING_INVENTORY:
        case ET_BUILDING_ASSOCIATION:
        case ET_BUILDING_MARKET:
        case ET_BUILDING_TICKETBOOTH:
        case ET_BUILDING_COIN_STORAGE:
        case ET_BUILDING_HABITAT:
        case ET_BUILDING_ANIMAL:
        case ET_BUILDING_TRAINING:
        case ET_BUILDING_EXPLORER:
        case ET_BUILDING_LIBRARY: {
            
            if (Building *_building = (Building*)m_entity) {
                
                _building->spineBuilding->stopAllActions();
                _building->spineBuilding->runAction(CCTintTo::create(kEntityActionTransitionSpeed, 255, 255, 255));
                
                if (_building->bVo.building_type == BUILDING_TYPE_HABITAT) {
                    _building->spineBuildingFront->stopAllActions();
                    _building->spineBuildingFront->runAction(CCTintTo::create(kEntityActionTransitionSpeed, 255, 255, 255));
                }
                
            }
            
        }break;
            
    }
    
    for (int i=0; i<m_menuLayer->getChildrenCount(); i++) {
        if (CCMenu* _menu = (CCMenu*)m_menuLayer->getChildren()->objectAtIndex(i)) {
        
            _menu->stopAllActions();
            _menu->runAction(CCSequence::create(CCDelayTime::create(kEntityActionTransitionMenuDelay*i),
                                                CCMoveTo::create(kEntityActionTransitionMenuSpeed*0.25f, CCPointZero),
                                                CCRemoveSelf::create(),
                                                NULL));
        }
    }

    this->stopAllActions();    
    this->runAction(CCSequence::create(CCDelayTime::create(kEntityActionTransitionMenuDelay*3 + kEntityActionTransitionSpeed*0.25f),
                                       CCCallFunc::create(this, callfunc_selector(EntityAction::_restart)),
                                       NULL));
    
    CCModalController::hideAllChildrensAnimated(this,
                                                kEntityActionTransitionMenuDelay*3 + kEntityActionTransitionSpeed*0.25f
                                                ,
                                                habitatAction);
    m_state = EAS_STANDBY;
    
}

//void EntityAction::clearMenuAtIndex(CCObject *object)
//{
////    if (CCInteger *index = (CCInteger*)object) {
////        int i = index->getValue();
////        CCLog_("%d", i);
////        if (m_menu[i] != NULL) {
////            m_menu[i]->removeFromParent();
////            m_menu[i] = NULL;
////        }
////    }
//    if (CCMenu* _menu = (CCMenu*)object) {
//        
//        if (m_menu->containsObject(_menu)) {
//            m_menu->removeObject(_menu);
//            _menu->removeFromParent();
//            _menu = NULL;
//        }
//        
//    }
//    
//}

void EntityAction::addProgressBar(int index_, void *entity_) {
    
    if (index_ >= kEntityActionProgressBarMax || index_ < 0) {
        return;
    }
    
    m_progress_bar[index_] = ProgressBar::create(entity_);
    m_progress_bar[index_]->setScale(1.25f);
    addChild(m_progress_bar[index_], 3);
    
}

void EntityAction::addButton(int index_, EntityActionButton button_) {
    
    if (index_ >= kEntityActionTotalButton || index_ < 0) {
        return;
    }
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    Building *_building = (Building*)m_entity;
    
    if (m_entityTypeCanBeOpenned != "") {
        
        if (_building->bVo.type == m_entityTypeCanBeOpenned) {
            if (m_entity_type == ET_BUILDING_HABITAT) {
                if (button_ != EAB_VIEW && _building->pbVo.status != BUILDING_STATUS_CONSTRUCT) { //players shouldn't see send to barn and set leader buttons in this case
                    return;
                }
            }
        }
        
        switch (m_entity_type) {
            case ET_BUILDING_HQ:
            case ET_BUILDING_UNCLE_HOUSE:
            case ET_BUILDING_TICKETBOOTH:
            case ET_BUILDING_COIN_STORAGE:
            case ET_BUILDING_HABITAT:
            case ET_BUILDING_ANIMAL:
            case ET_BUILDING_MARKET:
            case ET_BUILDING_INVENTORY:
            case ET_BUILDING_ASSOCIATION:
            case ET_BUILDING_PRODUCER:
            case ET_BUILDING_FOOD_STORAGE:
            case ET_BUILDING_TRAINING:
            case ET_BUILDING_EXPLORER:
            case ET_BUILDING_LIBRARY:
            {
                if (_building->bVo.type != m_entityTypeCanBeOpenned) {
                    if (button_ != EAB_INFO) {
                        return;
                    }
                }
            }
                break;
            default:
                break;
        }
    }
    
    switch (button_) {
        case EAB_INFO: {
            
            CCSprite *_btnBase = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsInfoButton));
            CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_btnBase, this, menu_selector(EntityAction::onInfo));
            CCMenu* _menu = CCMenu::create(_menuItem, NULL);
            _menu->setContentSize(_btnBase->getContentSize());
            _menu->setPosition(CCPointZero);
            _menu->setOpacity(0);
            m_menuLayer->addChild(_menu, 2, index_);

        }break;
        case EAB_UPGRADE: {
            
            CCSprite *_btnBase2 = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsUpgradeButton));
            CCMenuItemSprite *_menuItem2 = CCMenuItemScaleSprite::create(_btnBase2, this, menu_selector(EntityAction::onUpgrade));
            CCMenu* _menu = CCMenu::create(_menuItem2, NULL);
            _menu->setContentSize(_btnBase2->getContentSize());
            _menu->setPosition(CCPointZero);
            _menu->setOpacity(0);
            m_menuLayer->addChild(_menu, 2, index_);
            
        }break;
        case EAB_VIEW: {
            
            CCSprite *_btnBase3 = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsOverviewButton));
            CCMenuItemSprite *_menuItem3 = CCMenuItemScaleSprite::create(_btnBase3, this, menu_selector(EntityAction::onView));
            CCMenu* _menu = CCMenu::create(_menuItem3, NULL);
            _menu->setContentSize(_btnBase3->getContentSize());
            _menu->setPosition(CCPointZero);
            _menu->setOpacity(0);
            m_menuLayer->addChild(_menu, 2, index_);
            
        }break;
        case EAB_COLLECT: {
            
            UIHelper::UISprites spriteEnum = UIHelper::UISprites(UIHelper::UISpritesEnd);
            
            bool _canCollect = false;
            if (_building->bVo.building_type == BUILDING_TYPE_TICKETBOOTH) {
                _canCollect = _building->getResourceProduced() >= _building->getAmountEnoughToCollectCoin();
                if (_canCollect) {
                    spriteEnum = UIHelper::UISprites(UIHelper::TraceBuildingsOptionsCollectCoinsButton);
                }else {
                    spriteEnum = UIHelper::UISprites(UIHelper::TraceBuildingsOptionsCollectCoinsInactiveButton);
                }
            }else {
                _canCollect = _building->getResourceProduced() >= _building->getAmountEnoughToCollectFood();
                if (_canCollect) {
                    spriteEnum = UIHelper::UISprites(UIHelper::TraceBuildingsOptionsCollectFoodButton);
                }else {
                    spriteEnum = UIHelper::UISprites(UIHelper::TraceBuildingsOptionsCollectFoodInactiveButton);
                }
            }
            
            CCAssert((spriteEnum != UIHelper::UISprites(UIHelper::UISpritesEnd)), "spriteEnum must be coins or food button");
            
            CCSprite *_btnBase3 = UIHelper::getSprite(UIHelper::UISprites(spriteEnum));
            CCMenuItemSprite *_menuItem3 = NULL;
            if (_canCollect) {
                _menuItem3 = CCMenuItemScaleSprite::create(_btnBase3, this, menu_selector(EntityAction::onCollect));
            }else {
                _menuItem3 = CCMenuItemScaleSprite::create(_btnBase3, NULL, NULL);
            }
            CCMenu* _menu = CCMenu::create(_menuItem3, NULL);
            _menu->setContentSize(_btnBase3->getContentSize());
            _menu->setPosition(CCPointZero);
            _menu->setOpacity(0);
            m_menuLayer->addChild(_menu, 2, index_);
            
        }break;
        case EAB_INSTANT_BUILD:
        case EAB_INSTANT_UPGRADE: {
            
            CCSprite *_btnBase = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsSpeedUpButton));
            _btnBase->setPosition(kOffsetForBigButtons);
            
            CCSprite *_gemIcon = UIHelper::getSprite(UIHelper::UISprites(UIHelper::NSClaimGemBlue));
            _btnBase->addChild(_gemIcon);
            _gemIcon->setScale(0.75f);
            int gemval = EntityAction::getGemCost(_building);
            snprintf(_buffer, _buffer_size, "%d", gemval);
            CCLabelBMFont *_gemValue = CCLabelHelper::createBMFont(retinaMultiplier, _buffer, kFontSizeLarge, true);
            CCLabelUpdater *labelUpdater = CCLabelUpdater::createWithLabel(_gemValue,
                                                                           _building,
                                                                           EntityAction::getGemCost);
            _btnBase->addChild(labelUpdater);
            
            CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_btnBase, this, menu_selector(EntityAction::onInstant));
            _menuItem->setTag(2);
            CCMenu* _menu = CCMenu::create(_menuItem, NULL);
            _menu->setContentSize(_btnBase->getContentSize());
            _menu->setPosition(CCPointZero);
            _menu->setOpacity(0);
            m_menuLayer->addChild(_menu, 2, index_);
            
            UIHelper::setTwoNodesToMiddlePosition(_gemIcon,
                                                  _gemValue,
                                                  5.0f*sizeMultiplier,
                                                  ccp(_btnBase->getContentSize().width*0.5f,
                                                      _btnBase->getContentSize().height*0.3f));
            
        }break;
        case EAB_INSTANT_ENERGY: {
            CCSprite *_btnBase = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsSpeedUpButton));
            _btnBase->setPosition(kOffsetForBigButtons);
            
            CCSprite *_gemIcon = UIHelper::getSprite(UIHelper::UISprites(UIHelper::NSClaimGemBlue));
            _btnBase->addChild(_gemIcon);
            _gemIcon->setScale(0.75f);
            
            CREATE_CHAR_BUFFER(_buffer, 64);
            int gemval = EntityAction::getGemCost(_building->pet);
            snprintf(_buffer, _buffer_size, "%d", gemval);
            CCLabelBMFont *_gemValue = CCLabelHelper::createBMFont(retinaMultiplier, _buffer, kFontSizeLarge, true);
            CCLabelUpdater *labelUpdater = CCLabelUpdater::createWithLabel(_gemValue,
                                                                           _building->pet,
                                                                           EntityAction::getGemCost);
            _btnBase->addChild(labelUpdater);
            
            CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_btnBase, this, menu_selector(EntityAction::onInstant));
            _menuItem->setTag(3);
            CCMenu* _menu = CCMenu::create(_menuItem, NULL);
            _menu->setContentSize(_btnBase->getContentSize());
            _menu->setPosition(CCPointZero);
            _menu->setOpacity(0);
            m_menuLayer->addChild(_menu, 2, index_);
            
            UIHelper::setTwoNodesToMiddlePosition(_gemIcon,
                                                  _gemValue,
                                                  5.0f*sizeMultiplier,
                                                  ccp(_btnBase->getContentSize().width*0.5f,
                                                      _btnBase->getContentSize().height*0.3f));
            
        }break;
        case EAB_INSTANT_ABILITY: {
            
            if (Ability* _ability = g_playerState->getUpgradingAbility()) {
            
                int _cost_gems = g_gameContent->get_gemcal_for_ability_finishnow(_ability->getSecondsLeftToFinish());
                snprintf(_buffer, _buffer_size, "%d", _cost_gems);
                
                CCSprite *_btnBase = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsSpeedUpButton));
                _btnBase->setPosition(kOffsetForBigButtons);
                
                CCSprite *_gemIcon = UIHelper::getSprite(UIHelper::UISprites(UIHelper::NSClaimGemBlue));
                _btnBase->addChild(_gemIcon);
                _gemIcon->setScale(0.75f);
                
                int gemval = EntityAction::getGemCost(_ability);
                snprintf(_buffer, _buffer_size, "%d", gemval);
                CCLabelBMFont *_gemValue = CCLabelHelper::createBMFont(retinaMultiplier, _buffer, kFontSizeLarge, true);
                CCLabelUpdater *labelUpdater = CCLabelUpdater::createWithLabel(_gemValue,
                                                                               _ability,
                                                                               EntityAction::getGemCost);
                _btnBase->addChild(labelUpdater);
                
                CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_btnBase, this, menu_selector(EntityAction::onInstant));
                _menuItem->setTag(4); //
                CCMenu* _menu = CCMenu::create(_menuItem, NULL);
                _menu->setContentSize(_btnBase->getContentSize());
                _menu->setPosition(CCPointZero);
                _menu->setOpacity(0);
                m_menuLayer->addChild(_menu, 2, index_);
                
                UIHelper::setTwoNodesToMiddlePosition(_gemIcon,
                                                      _gemValue,
                                                      5.0f*sizeMultiplier,
                                                      ccp(_btnBase->getContentSize().width*0.5f,
                                                          _btnBase->getContentSize().height*0.3f));
                
            }
            
        }break;
        case EAB_CUT: {
            
            g_gameAudio->playEffect(AUDIO_UI_CLICK);
            
            CCSprite *_btnBase = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsAxeButton));
            
            CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_btnBase, this, menu_selector(EntityAction::onCut));
            CCMenu* _menu = CCMenu::create(_menuItem, NULL);
            _menu->setContentSize(_btnBase->getContentSize());
            _menu->setPosition(CCPointZero);
            _menu->setOpacity(0);
            m_menuLayer->addChild(_menu, 2, index_);
            
        }break;
        case EAB_KEEP: {
            
            g_gameAudio->playEffect(AUDIO_UI_CLICK);
            
            CCSprite *_btnBase = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsPackButton));
            
            CCMenuItemSprite *_menuItem = CCMenuItemScaleSprite::create(_btnBase, this, menu_selector(EntityAction::onKeep));
            CCMenu* _menu = CCMenu::create(_menuItem, NULL);
            _menu->setContentSize(_btnBase->getContentSize());
            _menu->setPosition(CCPointZero);
            _menu->setOpacity(0);
            m_menuLayer->addChild(_menu, 2, index_);
            
        }break;
        case EAB_SET_AS_HOST: {
            
            CCSprite *_btnBase3 = UIHelper::getSprite(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsSetAsLeaderButton));
            
            CCMenuItemSprite *_menuItem3 = CCMenuItemScaleSprite::create(_btnBase3, this, menu_selector(EntityAction::onSetAsHost));
            CCMenu* _menu = CCMenu::create(_menuItem3, NULL);
            _menu->setContentSize(_btnBase3->getContentSize());
            _menu->setPosition(CCPointZero);
            _menu->setOpacity(0);
            m_menuLayer->addChild(_menu, 2, index_);
            
        }break;
            
        default: {
            CCLog_("Unknown Action Button!");
        }break;
            
    }
    
}

void EntityAction::_restart() {
    
//    CCLog_("restart entity");
    
    if (m_entity_next) {
        m_entity = NULL;
//        check(m_entity_next);
//        m_entity_type = getEntityType(m_entity_next);
//        m_entity = m_entity_next;
//        m_entity_next = NULL;
        start(m_entity_next);
        m_entity_next = NULL;
    }else {
        m_entity = NULL;
        m_entity_next = NULL;
//        if (onReleaseMe) {
//            onReleaseMe(m_inst, m_sender);
//        }
//        _clear();
    }
    
}

#pragma mark Button Handlering Functions

void EntityAction::onInfo(CCObject *sender_) {
    
    this->finish();
    
    if (Building *_building = (Building*)m_entity) {
        if (typeid(*_building) == typeid(Building)) {
            
            switch (g_sceneManager->sceneSelector) {
                case SCENE_VILLE:
                case SCENE_VISIT:
                case SCENE_VISIT2:
                case SCENE_MATCH:
                case SCENE_MATCH2: {
                    
                    if (CScene *_scene = (CScene*)g_currentScene) {
                        if (_scene->mapToScreenUI) {
                            if (_scene->mapToScreenUI->getChildrenCount() == 0) {
                                if (MiniInfoDialog *_dialog = MiniInfoDialog::createWithEntity(m_entity)) {
                                    g_gameAudio->playEffect(AUDIO_UI_CLICK);
                                    _scene->mapToScreenUI->addChild(_dialog);
                                    return;
                                }
                            }
                        }
                    }
                    
                }break;
                    
                default:
                    break;
            }
            
        }
    }
    
    CCLog_("Warning!! : No mini-dialog has implemented yet!");

}

void EntityAction::onUpgrade(CCObject *sender_) {
    
    if (Building *_building = (Building*)m_entity) {
        if (typeid(*_building) == typeid(Building)) {
            if (SceneVille *_scene = (SceneVille*)g_currentScene) {
                if (_scene->mapToScreenUI) {
                    if (_scene->mapToScreenUI->getChildrenCount() == 0) {
                        if (MiniUpgradeDialog *_dialog = MiniUpgradeDialog::createWithEntity(m_entity)) {
                            m_upgradeDialog = _dialog;
                            g_gameAudio->playEffect(AUDIO_UI_CLICK);
                            
                            _scene->mapToScreenUI->addChild(_dialog);
                            this->finish();
                            return;
                        }
                    }
                }
            }
        }
    }
    CCLog_("no dialog implemented yet!");
    
}

void EntityAction::onView(CCObject *sender_) {
    
    if (Building *_building = (Building*)m_entity) {
        
        if (typeid(*_building) == typeid(Building)) {
            
            CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
            
            switch (g_sceneManager->sceneSelector) {
                    
                case SCENE_VILLE:
                case SCENE_VISIT:
                case SCENE_VISIT2:
                case SCENE_MATCH:
                case SCENE_MATCH2: {
                    
                    if (CScene* _scene = (CScene*)g_currentScene) {
                        if (_scene->modalController->getNStackView() == 0) {
                            
                            g_gameAudio->playEffect(AUDIO_UI_CLICK);
                            
                            if (_building->bVo.building_type == BUILDING_TYPE_INVENTORY) {
                                
                                ItemStorageDialog *_dialog = ItemStorageDialog::create(_scene->modalController, (PlayerState*)_scene->isoMap->mapPlayerState);
                                _dialog->setPosition(_screenSize*0.5f);
                                if (g_sceneManager->sceneSelector == SCENE_VILLE) {
                                    ((SceneVille*)g_currentScene)->itemStorageDialog = _dialog;
                                }
                                _scene->addChild(_scene->modalController, kModalControllerZOrder);
                                _scene->modalController->pushView(_dialog);
                                
                            } else if (_building->bVo.building_type == BUILDING_TYPE_HABITAT) {
                                
                                AnimalDialog *_dialog = AnimalDialog::create(_scene->modalController, m_entity, AnimalDialogType_1, viewMode);
                                _dialog->setPosition(_screenSize*0.5f);
                                _scene->addChild(_scene->modalController, kModalControllerZOrder);
                                _scene->modalController->pushView(_dialog);
                                
                            } else if (_building->bVo.building_type == BUILDING_TYPE_FUSION) {
                                
                                AnimalMarketDialog *_dialog = AnimalMarketDialog::create(_scene->modalController, (PlayerState*)_scene->isoMap->mapPlayerState);
                                _dialog->setPosition(_screenSize*0.5f);
                                _scene->addChild(_scene->modalController, kModalControllerZOrder);
                                _scene->modalController->pushView(_dialog);
                                
                            } else if (_building->bVo.building_type == BUILDING_TYPE_ANIMALDORM) {

                                //! this is just for testing
                                //! AppRating::Instance().showRateDialog();
//                                PushNotifications::Instance().notify(PushNotifications::FirstTimeAnimalWakeUp);
//                                g_sceneManager->testSpineLoaderPerformance();

                                _animalBarnDialog = AnimalBarnDialog::create(_scene->modalController, (PlayerState*)_scene->isoMap->mapPlayerState);
                                _animalBarnDialog->setPosition(_screenSize*0.5f);
                                _scene->addChild(_scene->modalController, kModalControllerZOrder);
                                _scene->modalController->pushView(_animalBarnDialog);

                            } else if (_building->bVo.building_type == BUILDING_TYPE_SKILLS) {
                                
                                AbilityDialog *_dialog = AbilityDialog::create(_scene->modalController, (PlayerState*)_scene->isoMap->mapPlayerState);
                                _dialog->setPosition(_screenSize*0.5f+kDialogAbilityPosition);
                                _scene->addChild(_scene->modalController, kModalControllerZOrder);
                                _scene->modalController->pushView(_dialog);
                                
                            }
                        }
                    }

                }break;
                
                default: {
                }break;
                    
            }
            
        }
        
        this->finish();
    }
    
    
}

void EntityAction::onInstant(cocos2d::CCObject *sender_) {
    
    if (Building *_building = (Building*)m_entity) {
        if (typeid(*_building) == typeid(Building)) {
            
            if (CCMenuItemSprite *item = (CCMenuItemSprite*)sender_) {
                g_gameAudio->playEffect(AUDIO_UI_CLICK);

                SceneVille *_scene = (SceneVille*)g_currentScene;
                CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                
                switch (item->getTag()) {
                    case 1: { // Instant Build
                        
                        
                    }break;
                    case 2: { // Instant Upgrade
                        
                        if (!_scene->modalController->getParent()) {
                            _instantUpgradeDialog = InstantDialog::create(_scene->modalController, m_entity);
                            if (_instantUpgradeDialog) {
                                _instantUpgradeDialog->setPosition(_screenSize*0.5f);
                                _scene->addChild(_scene->modalController, kModalControllerZOrder);
                                _scene->modalController->pushView(_instantUpgradeDialog);
                            }
                        }
                        
                    }break;
                        
                    case 3: { // Instant Energy
                        
                        if (!_scene->modalController->getParent()) {
                            if (InstantDialog *_dialog = InstantDialog::createWithType(_scene->modalController, IT_INSTANT_ENERGY, ((Building*)m_entity)->pet, NULL)) {
                                _dialog->setPosition(_screenSize*0.5f);
                                _scene->addChild(_scene->modalController, kModalControllerZOrder);
                                _scene->modalController->pushView(_dialog);
                            }
                        }
                        
                    }break;
                        
                    case 4: { // Finish Ability
                        
                        if (Ability* _ability = g_playerState->getUpgradingAbility()) {
                            if (!_scene->modalController->getParent()) {
                                if (InstantDialog *_dialog = InstantDialog::createWithType(_scene->modalController, IT_FINISH_ABILITY, _ability, NULL)) {
                                    _dialog->setPosition(_screenSize*0.5f);
                                    _scene->addChild(_scene->modalController, kModalControllerZOrder);
                                    _scene->modalController->pushView(_dialog);
                                }
                            }
                        }
                        
                    }break;
                        
                    default:
                        break;
                }
                
                this->finish();
                
            }
            
        }
    }
    
}

void EntityAction::onCollect(CCObject *sender_) {
    
    if (Building *_building = (Building*)m_entity) {
        if (typeid(*_building) == typeid(Building)) {
            bool _collect = false;
            if ((_building->bVo.building_type == BUILDING_TYPE_PRODUCER) &&
                (_building->getResourceProduced() >= _building->getAmountEnoughToCollectFood())) {
                _collect = true;
#if FEATURE_COINS_FROM_PET
            }else if ((_building->bVo.building_type == BUILDING_TYPE_TICKETBOOTH ||
                       _building->bVo.building_type == BUILDING_TYPE_HABITAT) &&
#else 
            }else if ((_building->bVo.building_type == BUILDING_TYPE_TICKETBOOTH) &&
#endif
                      (_building->getResourceProduced() >= _building->getAmountEnoughToCollectCoin())) {
                _collect = true;
            }
            if (_collect) {
                _building->collect();
                this->finish();
                return;
            }
        }
    }
    
}

void EntityAction::onCut(CCObject *sender_) {
    
    if (Decoration *_unwanted_tree = (Decoration*)m_entity) {
        
        Building* _hq = g_playerState->getBuilding(kBuildingHQ, 0);
        if (_hq) {
            if (_hq->pbVo.level < _unwanted_tree->dVo.unlocks_at_level) {
                CREATE_CHAR_BUFFER(_buffer, 128);
                snprintf(_buffer, _buffer_size, g_gameContent->Word["cut_tree_warning_msg"].c_str(), _unwanted_tree->dVo.title.c_str(), _hq->bVo.title.c_str(), _unwanted_tree->dVo.unlocks_at_level);
                g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR, _buffer);
//                this->finish();
                return;
            }
        }

        if (SceneVille* _scene = (SceneVille*)g_currentScene) {
            if (!_scene->modalController->getParent()) {
                
                g_gameAudio->playEffect(AUDIO_UI_CLICK);
                
                CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                CutTreeDialog *_cutTreeDialog = CutTreeDialog::create(_scene->modalController, _unwanted_tree);
                _cutTreeDialog->setPosition(_screenSize*0.5f);
                _scene->addChild(_scene->modalController, kModalControllerZOrder);
                _scene->modalController->pushView(_cutTreeDialog);
            }
        }
        
    }
    
    this->finish();
    
}

void EntityAction::onKeep(cocos2d::CCObject *sender_) {
    
    this->finish();
    
    if (Decoration *_decoration = (Decoration*)m_entity) {
        
        _decoration->keep();
    
    }
    
}

void EntityAction::onSendToBarn(CCObject *sender_) {
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    Building *_building = (Building*)m_entity;
    if (!_building ||
        typeid(*_building) != typeid(Building) ||
        _building->bVo.type != BUILDING_TYPE_HABITAT) return;

    bool unfinish_animaldorm = false;
    bool not_enough_space = false;
    Building *animalDorm = g_playerState->getBuilding(kBuildingAnimal, 0);
    
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
        if (_scene->modalController->getNStackView() == 0) {
            InstantDialog *_dialog = InstantDialog::create(_scene->modalController, animalDorm);
            _dialog->setPosition(_screenSize*0.5f);
            _scene->modalController->pushView(_dialog);
        }
        return;
    }
    
    if (not_enough_space) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, g_gameContent->Word["animal_barn_is_full_msg"].c_str());
        return;
    }
    
    BackendService::getInstance()->send(APIPetStore::create(_building->pet,
                                                            this,
                                                            EntityAction::onSendToBarnCompleted));
    _building->removePetFromBuilding();
    finish();
    
}
    
                      
void EntityAction::onSendToBarnCompleted(void* sender, string resp_str) {
    PlayerState* _playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserRaceData(_root);
    _playerState->parsePetData(_root);
    Json_dispose(_root);
}

void EntityAction::onSetAsHost(CCObject *sender_) {
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    
    if (Building *_building = (Building*)m_entity) {
        if (typeid(*_building) == typeid(Building)) {
            if (_building->bVo.type == BUILDING_TYPE_HABITAT) {
                if (Pet* _pet = (Pet*)_building->pet) {
                    
                    CREATE_CHAR_BUFFER(_buffer, 64);
                    
                    snprintf(_buffer, _buffer_size, g_gameContent->Word["info_pet_is_now_host_animal"].c_str()
                            ,_pet->pVo.title.c_str());
                    g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
                    
                    for (std::vector<Building*>::iterator it=g_playerState->Buildings.begin(); it!=g_playerState->Buildings.end(); ++it) {
                        if (Building *__building = (Building*)(*it)) {
                            if (__building->bVo.building_type == BUILDING_TYPE_HABITAT) {
                                __building->setPetAsHost(false);
                            }
                        }
                    }
                    
                    _building->setPetAsHost(true);
                    BackendService::getInstance()->send(APIPetHost::create(_pet,
                                                                           NULL,
                                                                           AnimalDialog::onPetHosted));
                    this->finish();
                    return;
                    
                }else { // habitat without pet
                    
                }
            }
        }
    }
}

int EntityAction::getGemCost(Building *_building)
{
    int seconds = _building->getSecondsLeftToFinish();
    int gemval = g_gameContent->get_gemcal_for_building_finishnow(seconds);
//    CCLog_("EntityAction::getGemCostFromBuilding %d => %d", seconds, gemval);
    return gemval;
}

int EntityAction::getGemCost(Pet *_pet)
{
    int seconds = _pet->getSecondsLeftToFullEnergy();
    int gemval = g_gameContent->get_gemcal_for_pet_wakeup(seconds);
    CCLog_("EntityAction::getGemCostFromPet %d => %d", seconds, gemval);
    return gemval;
}

int EntityAction::getGemCost(Ability *_ability)
{
    int seconds = _ability->getSecondsLeftToFinish();
    int gemval = g_gameContent->get_gemcal_for_ability_finishnow(seconds);
    CCLog_("EntityAction::getGemCostFromAbility %d => %d", seconds, gemval);
    return gemval;
}

