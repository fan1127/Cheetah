//
//  Pet.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/10/56 BE.
//
//

#include "Pet.h"
USING_NS_CC;
using namespace std;
using namespace extension;

#include "IsoMap.h"

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Scenes/SceneVille.h"
#include "../Scenes/SceneRace.h"
extern CCScene *g_currentScene;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIPetFeed.h"
#include "../Services/APIs/APIPetUpdate.h"

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Entities/PlayerState.h"
extern PlayerState *g_playerState;

#include "../Entities/PlayerState.h"
extern PlayerState *g_opponentState;

#include "../Entities/ShockwaveStraight.h"
#include "../Entities/BananaTrap.h"
#include "../Entities/AI.h"

extern unsigned int currentTime;
extern float sizeMultiplier;
extern float retinaMultiplier;

#include "../Extensions/Modal/CCModalController.h"

#include "../Helpers/UIHelper.h"

Pet* Pet::createWithPetVO(GDPetVO vo_, PlayerPetVO ppvo_) {
    
    Pet* pRet = new Pet();
    pRet->initWithPetVO(vo_, ppvo_);
    pRet->autorelease();
    return pRet;
    
}

bool Pet::initWithPetVO(GDPetVO pvo_, PlayerPetVO ppvo_) {
    
    if (pvo_.type.size() <= 0) {
        return false;
    }
    
    if (!CCNode::init()) {
        return false;
    }
    
    pVo = pvo_;
    ppVo = ppvo_;
    ppVo.type = pvo_.type;
    
    m_active = true;
    m_hunger_timer = 0;
    
    m_tutorialMode = false;
    
    m_energy_current = ppVo.energy;
    
    reload();
    
    m_currPetAction = PET_STANDBY;
    m_prevPetAction = (PetAction)(-1);
    
    m_currPos = CCPointZero;
    
    //because value is based on iPhone
    petScaleFactor = (retinaMultiplier / 0.46875f) * PET_SCALE;

    updateHunger();
    
    m_summaryTrack = 0;
    
    m_spd_a = g_gameContent->GameConfig["SPD_A"];
    m_spd_b = g_gameContent->GameConfig["SPD_B"];
    m_spd_c = g_gameContent->GameConfig["SPD_C"];
    m_spd_mult = g_gameContent->GameConfig["SPD_MULT"];
    m_acc_a = g_gameContent->GameConfig["ACC_A"];
    m_acc_b = g_gameContent->GameConfig["ACC_B"];
    m_acc_c = g_gameContent->GameConfig["ACC_C"];
    m_acc_mult = g_gameContent->GameConfig["ACC_MULT"];
    
    m_groupSpineEffectsBelowPet = CCNode::create();
    this->addChild(m_groupSpineEffectsBelowPet, -1);
    
    m_levelUp = NULL;
    m_expUp = NULL;
    
    isBot = false;
    if (g_opponentState->getPet(ppvo_.puid) != NULL) isBot = true;
    
    m_groupEnergy = CCNode::create();
    
    _energyIcon = UIHelper::getSprite(UIHelper::RaceElementsPowerIcon);
    m_label_energy_info = CCLabelHelper::createBMFont(" ", kFontSizeExtraLarge+5.0f, true,
                                                   ccc3(255, 255, 255),
                                                   kCCLabelAutomaticWidth,
                                                   kCCTextAlignmentRight);
    m_bgEnergy = CCSprite::createWithSpriteFrameName("oldversion/_bg_timeadded.png");
    m_bgEnergy->setAnchorPoint(ccp(0.5f,0.5f));
    m_bgEnergy->setScaleX(2.0f);
    m_bgEnergy->setScaleY(1.5f);
    m_bgEnergy->setOpacity(153);
    m_groupEnergy->addChild(m_bgEnergy);
    
    m_groupEnergy->setPosition(ccp(0, 140.0f * retinaMultiplier));
    
    m_label_energy_info->setAnchorPoint(ccp(1, 0.5f));
    m_label_energy_info->setPosition(ccp(m_bgEnergy->getContentSize().width*-0.1f, 0));
    m_groupEnergy->addChild(m_label_energy_info,3);
    
    _energyIcon->setAnchorPoint(ccp(1, 0.5f));
    _energyIcon->setPosition(ccp(m_bgEnergy->getContentSize().width*1, 0));
    m_groupEnergy->addChild(_energyIcon,2);
    addChild(m_groupEnergy, 1000);
    
    
    m_critLabel = CCLabelHelper::createBMFont(retinaMultiplier, "CRITICAL", kFontSizeSmall, true,
                                              ccc3(255, 255, 255));
    m_critLabel->setPosition(ccp(0, 140.0f * retinaMultiplier));
    m_critLabel->setVisible(false);
    m_critLabel->setScale(0.0f);
    this->addChild(m_critLabel);

    m_groupEnergy->setVisible(false);
    m_groupEnergy->setScale(GROUP_ENERGY_ORIGINAL_SCALE);

    m_currentEffectDashFrame = EFFECT_DASH_MIN_FRAME;
    m_effectDashAnimationFlipper = 1;
    
    CREATE_CHAR_BUFFER(_buf_dash, 32);
    snprintf(_buf_dash, _buf_dash_size, EFFECT_DASH_FILE_FORMAT, m_currentEffectDashFrame);
    
    m_effectDash = CCSprite::createWithSpriteFrameName(_buf_dash);
    m_effectDash->setVisible(false);
    m_effectDash->setPosition(ccp(0.0f, m_effectDash->getContentSize().height*EFFECT_DASH_OFFSET_Y_PERCENT));
    this->addChild(m_effectDash, 100);
    
    m_stat_straight_speed = PPVO_STRAIGHT_SPEED;
    m_stat_corner_speed = PPVO_CORNER_SPEED;
    m_stat_energy = PPVO_ENERGY_STAT;
    
    m_isPaused = false;
    
    m_usingSkill = false;
    m_usingDash = false;
    m_usingStomp = false;
    m_usingShockwaveStraight1 = false;
    m_usingShockwaveStraight2 = false;
    
    m_stunnedTime = 0.0f;
    m_recoveryTime = 0.0f;
    
    m_stompCollideTime = 0.0f;
    
    isHelperPet = false;
    isActive = false;
    
    m_helperStunDuration = 0.0f;
    m_helperStunAmount = 0;
    
    message = "";
    message2 = "";
    
    return true;
    
}

Pet::~Pet() {
    
}

void Pet::initilizeEnergyInformation() {
    
    m_allCost = startSkillCost();
    
    if (isHelperPet) {
        m_dashCost = 0;
        m_bananaTrapCost = 0;
        m_stompCost = 0;
        m_shockwave1Cost = 0;
        m_shockwave2Cost = 0;
        return;
    }
    
    if (Ability *ability = playerState->getAbilityForKey(kAbilityDashForward)) {
        m_dashCost = ability->aVo.starting_cost;
        m_dashStartingCost = ability->aVo.starting_cost;
        m_dashIncreasingCost = ability->aVo.increasing_cost;
        m_dashAddingIncreasingCost = ability->aVo.adding_increasing_cost;
    }
    
    if (Ability *ability = playerState->getAbilityForKey(kAbilityBananaTrap)) {
        m_bananaTrapCost = ability->aVo.starting_cost;
        m_bananaTrapStartingCost = ability->aVo.starting_cost;
        m_bananaTrapIncreasingCost = ability->aVo.increasing_cost;
        m_bananaTrapAddingIncreasingCost = ability->aVo.adding_increasing_cost;
    }
    
    if (Ability *ability = playerState->getAbilityForKey(kAbilityStomp)) {
        m_stompCost = ability->aVo.starting_cost;
        m_stompStartingCost = ability->aVo.starting_cost;
        m_stompIncreasingCost = ability->aVo.increasing_cost;
        m_stompAddingIncreasingCost = ability->aVo.adding_increasing_cost;
    }
    
    if (Ability *ability = playerState->getAbilityForKey(kAbilityShootForward)) {
        m_shockwave1Cost = ability->aVo.starting_cost;
        m_shockwave1StartingCost = ability->aVo.starting_cost;
        m_shockwave1IncreasingCost = ability->aVo.increasing_cost;
        m_shockwave1AddingIncreasingCost = ability->aVo.adding_increasing_cost;
    }
    
    if (Ability *ability = playerState->getAbilityForKey(kAbilityDoubleShoot)) {
        m_shockwave2Cost = ability->aVo.starting_cost;
        m_shockwave2StartingCost = ability->aVo.starting_cost;
        m_shockwave2IncreasingCost = ability->aVo.increasing_cost;
        m_shockwave2AddingIncreasingCost = ability->aVo.adding_increasing_cost;
    }
    
    m_dashTimeUsed = 0;
    m_bananaTrapTimeUsed = 0;
    m_stompTimeUsed = 0;
    m_shockwave1TimeUsed = 0;
    m_shockwave2TimeUsed = 0;
}

float Pet::getExpectedExp() {

    GDPetVO vo = g_gameContent->getPetVO(ppVo.type);
    if (vo.type == "") {
        CCLog_("Pet::getExpectedExp - Pet=%s doesn't exist!", ppVo.type.c_str());
        return -1.0f;
    }
    
    return ((vo.exp_multiplier_for_exp_curve*powf(1.04f, ppVo.level))+(ppVo.level*0.5));
    
}

void Pet::updateAnimation() {
    
    //Change action
    if (m_prevPetAction != m_currPetAction || PET_IS_JUMPING) {

        switch (m_currPetAction) {
                
            default:
                break;
                
            case PET_STANDBY:                   petSpine->setAnimation("habitat_idle_stand", true);             break;
            case PET_TOUCH:                     petSpine->setAnimation("habitat_touch", true);                  break;
            case PET_READY:                     petSpine->setAnimation("track_ready", true);                    break;
            case PET_RUN:                       petSpine->setAnimation("track_run", true);                      break;
                
            case PET_FLY:{
                //if (!petSpine->setAnimation("track_fly", true)) {
                    //m_currPetAction = PET_RUN;
                    //petSpine->setAnimation("track_run", true);
                //}
            }
                break;
                
            case PET_JUMP:                      petSpine->setAnimation("track_jump", false);                    break;
            case PET_FALL_A:                    petSpine->setAnimation("track_falling_a", false);               break;
            case PET_FALL_B:                    petSpine->setAnimation("track_falling_b", false);               break;
            case PET_SLIDE:                     petSpine->setAnimation("track_slide", false);                   break;
            case PET_HABITAT_EAT:               petSpine->setAnimation("habitat_eat", true);                    break;
            case PET_HABITAT_EAT_2:             petSpine->setAnimation("habitat_eat", false);                   break;
            case PET_HABITAT_HUNGRY:            petSpine->setAnimation("habitat_hungry", true);                 break;
            case PET_FROZEN_A:                  petSpine->setAnimation("track_frozen_a", false);                break;
            case PET_FROZEN_B:                  petSpine->setAnimation("track_frozen_b", false);                break;
            case PET_FROZEN_C:                  petSpine->setAnimation("track_frozen_c", false);                break;
            case PET_FROZEN_D:                  petSpine->setAnimation("track_frozen_d", false);                break;
            case PET_FROZEN_E:                  petSpine->setAnimation("track_frozen_e", false);                break;
            case PET_STOMP:                     petSpine->setAnimation("habitat_touch", false);                 break;
                
        }
        
    }
    
}

void Pet::update(float delta_) {
    
    //Random new action
    switch (m_currPetAction) {
        case PET_IDLE_WALK: {
            int n = CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(this);
            if (n == 0) {
                randomIdleAction();
            }
        }break;
        default: {
            if (petSpine->states[0]->animation) {
                if (petSpine->states[0]->time >= petSpine->states[0]->animation->duration) {
                    randomIdleAction();
                }
            }
        }break;
    }
    
    //Change action
    if (m_prevPetAction != m_currPetAction || PET_IS_JUMPING || m_currPetAction == PET_HABITAT_SLEEP) {

        string _animation_name = getAnimationName(m_currPetAction);
        
        string _prev_ani_name = "";
        if (petSpine->states[0]) {
            if (petSpine->states[0]->animation) {
                _prev_ani_name = petSpine->states[0]->animation->name;
            }
        }
        if (_animation_name !="" && _animation_name != _prev_ani_name) {
            //if (petSpine->setAnimation(_animation_name.c_str(), true)) {
                //if (m_currPetAction == PET_IDLE_WALK) {
                    // Make a pet moves
                    //CCPoint _currPos = this->getPosition();
                    //int _angle = rand()%360;
                    //CCPoint _destPos = ccp(cosf(_angle)*g_gameContent->tileSize.width, sinf(_angle)*g_gameContent->tileSize.height);
                    //_destPos = _destPos * sizeMultiplier;
                    //this->flip(_destPos.x > _currPos.x);
                    
                    //CCAction *_walkingAction = CCMoveTo::create(ccpDistance(_currPos, _destPos)/(g_gameContent->tileSize.width*0.5f), _destPos);
                    //this->runAction(_walkingAction);
                    
                //}
                //m_prevPetAction = m_currPetAction;
            //}else {
                //randomIdleAction();
            //}
        }

    }
    
}

void Pet::updatePet(float delta_) {
    
    m_delta += delta_;
    if (m_delta >= 1.0f) {
        m_delta -= 1.0f;
        m_hunger_timer++;
        int _max_timer = ceil(pVo.mvhm/pVo.hunger_rate);
        if (m_hunger_timer > _max_timer) {
            m_hunger_timer = _max_timer;
        }
        
        m_energy_timer++;
        m_energy_current = (m_energy_timer*pVo.energy_rate);
        if (m_energy_current > 100.0f) {
            m_energy_current = 100.0f;
            
            if (ppVo.energy < 100.0f) {
                ppVo.energy = 100.0f;
                BackendService::getInstance()->send(APIPetUpdate::create(this, this, Pet::onPetUpdateCompleted));
            }
            
        }
        
    }
    
    if (isHungry() && !(isSleeping())) {
        showHungry();
    }else {
        hideHungry();
    }
    
}

void Pet::onPetUpdateCompleted(void* sender, string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parsePetData(_root);
    Json_dispose(_root);
    
}

void Pet::updateHunger() {
    
    m_hunger_timer = currentTime - ppVo.hunger_timestamp;
//    CCLog_("hunger: %d - %d = %d", currentTime, ppVo.hunger_timestamp, m_hunger_timer);
    
    m_energy_timer = currentTime - ppVo.energy_timestamp;
//    CCLog_("energy: %d - %d = %d", currentTime, ppVo.energy_timestamp, m_energy_timer);
    
    m_energy_current = ppVo.energy + (m_energy_timer*pVo.energy_rate);
    
}

void Pet::updateCoinsProduced() {
    
    if (ppVo.located_in == "") return;

    Building * _habitat = g_playerState->getBuilding(ppVo.located_in);
    if (!_habitat) return;
    
    _habitat->setInitialCoins();
    
}

void Pet::zeroCoinsProduced() {
    
    ppVo.coins_timestamp = currentTime;
    
}

void Pet::reload()
{
    reload(false, true, ppVo.type);
}

void Pet::reload(bool force_)
{
    reload(force_, true, ppVo.type);
}

void Pet::reload(bool force_, bool save_, string type_)
{
    GDPetVO _pvo = g_gameContent->getPetVO(type_);
    if (_pvo.type == "") {
        CCLog_("(Pet::reloadSpine) - Given pvo=%s doesn't exist!", type_.c_str());
        return;
    }
    
    if (save_) {
        pVo = _pvo;
    }
    
    if (emotionHungry) {
        emotionHungry->removeFromParent();
        emotionHungry = NULL;
    }
    
    if (petSpine) {
        if (force_) {
            petSpine->removeFromParentAndCleanup(true);
            petSpine = NULL;
        }else {
            petSpine->unscheduleUpdate();
            petSpine->scheduleUpdate();
            return;
        }
    }
 
    petSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, _pvo.src.c_str());
    //petSpine->preferenceRootParent = preferenceRootParent;
    petSpine->setContentSize(g_gameContent->tileSize*2);
    petSpine->setScale(sizeMultiplier*PET_STATIC_SCALE);
    this->updateFlip();
    
    setAnimation(PET_TOUCH);
    
    this->addChild(petSpine, 1);
    
}

void Pet::resumeAction() {
    
    m_prevPetAction = (PetAction)-1;
    flip(m_flip);
}

void Pet::setActive(bool active_) {
    
    m_active = active_;
    
}

void Pet::flip(bool flip) {
    float scale = fabsf(petSpine->getScaleX());
    m_flip = flip;
    if (m_flip) {
        petSpine->setScaleX(-scale);
    }else {
        petSpine->setScaleX( scale);
    }
    
}

void Pet::updateFlip()
{
    this->flip(m_flip);
}

void Pet::setAnimation(PetAction action) {
    
    m_currPetAction = action;
    
    updateAnimation();
    
}

string Pet::getAnimationName(PetAction action) {
    
    string _animation_name = "";
    
    switch (action) {
            
        default:
            break;
            
        case PET_IDLE_WALK:                 _animation_name = "habitat_idle_walk";                          break;
        case PET_IDLE_STAND:                _animation_name = "habitat_idle_stand";                         break;
        case PET_IDLE_HEAD_UP:              _animation_name = "habitat_idle_head_up";                       break;
        case PET_IDLE_HEAD_DOWN:            _animation_name = "habitat_idle_head_down";                     break;
        case PET_IDLE_HEAD_SHAKE:           _animation_name = "habitat_idle_head_shake";                    break;
        case PET_IDLE_HEAD_GORE:            _animation_name = "habitat_idle_head_gore";                     break;
        case PET_IDLE_SLEEP_TO_STAND:       _animation_name = "habitat_idle_sleepy_to_stand";               break;
        case PET_HABITAT_SITDOWN:           _animation_name = "habitat_idle_sitdown";                       break;
        case PET_HABITAT_JUMP:              _animation_name = "habitat_touch";                              break;
        case PET_HABITAT_STAND2LEGS:        _animation_name = "habitat_idle_stand2leg";                     break;
        case PET_HABITAT_SCRABBLE:          _animation_name = "habitat_idle_scrabble";                      break;
        case PET_HABITAT_2LEGS:             _animation_name = "habitat_idle_2leg";                          break;
        case PET_HABITAT_STAND_SMOKE:       _animation_name = "habitat_idle_stand_smoke";                   break;
        case PET_HABITAT_FIRE:              _animation_name = "habitat_idle_fire";                          break;
        case PET_HABITAT_FLY:               _animation_name = "habitat_idle_fly";                           break;
        case PET_HABITAT_SMOKE:             _animation_name = "habitat_idle_smoke";                         break;
        case PET_HABITAT_EAT:               _animation_name = "habitat_eat";                                break;
        case PET_HABITAT_EAT_2:             _animation_name = "habitat_eat";                                break;
            
        case PET_HABITAT_HUNGRY:            _animation_name = "habitat_hungry";                             break;
        case PET_HABITAT_HUNGRY_LAID:       _animation_name = "habitat_hungry_sleep";                       break;
        case PET_HABITAT_HUNGRY_SMOKE:      _animation_name = "habitat_hungry_smoke";                       break;
            
        case PET_HABITAT_SLEEP:             _animation_name = "habitat_sleep";                              break;
            
    }
    
    return _animation_name;
}

void Pet::playShowOffAnimation() {
    
    PetAction _defaultAction = PET_HABITAT_EAT;
    
    vector<PetAction> _allShowOffActions;
    _allShowOffActions.push_back(PET_HABITAT_FLY);
    _allShowOffActions.push_back(PET_HABITAT_FIRE);
    _allShowOffActions.push_back(PET_HABITAT_SMOKE);
    _allShowOffActions.push_back(PET_HABITAT_STAND_SMOKE);
    _allShowOffActions.push_back(PET_HABITAT_SCRABBLE);
    _allShowOffActions.push_back(PET_IDLE_HEAD_GORE);
    _allShowOffActions.push_back(PET_IDLE_HEAD_SHAKE);
    _allShowOffActions.push_back(PET_HABITAT_2LEGS);
    _allShowOffActions.push_back(PET_HABITAT_STAND2LEGS);
    _allShowOffActions.push_back(PET_HABITAT_SITDOWN);
    
    
    
    bool succeed = false;
    
    //try random 3 times
    for (int i = 0; i < 3; i++) {
        int rand = arc4random()%_allShowOffActions.size();
        PetAction _action = _allShowOffActions.at(rand);
        if (trySetAnimation(_action)) {
            succeed = true;
            break;
        }
    }
    
    if (!succeed) {
        for (vector<PetAction>::iterator it=_allShowOffActions.begin(); it!=_allShowOffActions.end(); ++it) {
            PetAction _action = (PetAction)(*it);
            
            if (trySetAnimation(_action)) {
                succeed = true;
                break;
            }
        }
    }
    
    if (!succeed) {
        trySetAnimation(_defaultAction);
    }
    
    _allShowOffActions.clear();
}

bool Pet::trySetAnimation(PetAction action) {
    
    if (m_currPetAction == PET_IDLE_WALK) {
        
        int n = CCDirector::sharedDirector()->getActionManager()->numberOfRunningActionsInTarget(this);
        if (n != 0) {
            return false;
        }
    }
    
    if (petSpine->states[0]) {
        
        if (SkeletonData_findAnimation(petSpine->states[0]->data->skeletonData, getAnimationName(action).c_str())) {
            m_currPetAction = action;
            return true;
        }
    }
    
    return false;
}


void Pet::randomIdleAction() {
    
    if (m_energy_current < 100) {
        m_currPetAction = PET_HABITAT_SLEEP;
        return;
    }
    
    //Hungry case
    float _hunger_ratio = getHungerRatio();
    if (m_currPetAction == PET_IDLE_WALK) {
        if (_hunger_ratio >= g_gameContent->GameConfig["HUNGER_ANIMATION_ACTIVE"] && m_active) {
            switch (arc4random()%3) {
                default:
                case 0: m_currPetAction = PET_HABITAT_HUNGRY;       break;
                case 1: m_currPetAction = PET_HABITAT_HUNGRY_LAID;  break;
                case 2: m_currPetAction = PET_HABITAT_HUNGRY_SMOKE; break;
            }
        }else {
            while (m_currPetAction == PET_IDLE_WALK) {
                m_currPetAction = (PetAction)(arc4random()%kPetActionRandomRange);
            }
        }
    }else {
        if (_hunger_ratio >= g_gameContent->GameConfig["HUNGER_ANIMATION_ACTIVE"] && m_active) {
            switch (arc4random()%3) {
                case 0: m_currPetAction = PET_HABITAT_HUNGRY; break;
                case 1: m_currPetAction = PET_HABITAT_HUNGRY; break;
                default: m_currPetAction = PET_IDLE_WALK; break;
            }
            return;
        }
        m_currPetAction = PET_IDLE_WALK;
    }

}

#pragma mark - Running

void Pet::startRunning()
{
//    if (!is_replay) {
//        replayRecord = CCDictionary::create();
//        replayRecord->retain();
//        std::ostringstream ss;
//        ss << running_time;
//        replayRecord->setObject(CCString::create(ss.str()), "delay_start");
//        replayRecord->setObject(CCArray::create(), "obstacles");
//    } else {
//        
//    }
    start_running = true;
    setAnimation(PET_RUN);
}

void Pet::prepareRunning(void *ptr_isomap, Track* startTrack, bool trackDirection, int order) {
    
    IsoMapRacing* isoMapRacing = (IsoMapRacing*)ptr_isomap;
    if (!isoMapRacing) {
        printf("(Pet::prepareRunning) Error: IsoMapRacing is uninitialized!\n");
        return;
    }
    
//    Track *nextTrack = isoMapRacing->getNearestTrack(startTrack->getPosition(), isoMapRacing->tilesTrack[startTrack->ptVo.index], -1);
    
    Track *nextTrack = startTrack->nextTrack;
    
    double distance = sqrt(  pow(startTrack->getPosition().x - nextTrack->getPosition().x, 2)
                           + pow(startTrack->getPosition().y - nextTrack->getPosition().y, 2));
    
    m_distancePerRound = distance*m_trackCount;
    
    m_trackDirection = trackDirection;
    m_distanceBetweenTrack = distance;
    
    //accelerate_factor = distance/80;
    
    m_order = order;
    m_currPos = startTrack->getPosition();
    
    CCPoint t_coord = IsoMap::IsoToCoord(m_currPos, g_gameContent->tileSize*2, g_gameContent->tileShift);
    m_currTrackIndex = int(t_coord.y*(ISO_TILE_COL/2)+t_coord.x);
    
    switch (isoMapRacing->tilesTrack[m_currTrackIndex]) {
        case TTYPE_STRAIGHT_UP_RIGHT:
            if (trackDirection) {
                m_prevTrackIndex = m_currTrackIndex-1;
            }else {
                m_prevTrackIndex = m_currTrackIndex+1;
            }
            break;
        default:
        case TTYPE_STRAIGHT_UP_LEFT:
            if (trackDirection) {
                m_prevTrackIndex = m_currTrackIndex+(ISO_TILE_COL/2);
            }else {
                m_prevTrackIndex = m_currTrackIndex-(ISO_TILE_COL/2);
            }
            break;
    }
    
    m_currPetAction = PET_STANDBY;
    
//    m_currDelta = 0;
    m_currTrackPosition = 0.5f;
    m_summaryTrackPosition = 0.5f;
    firstTrack = startTrack;
    m_currTrack = startTrack;
    m_currSpeed = STARTSPEED;
    
    can_running = false;
    running_time = 0;
    after_running_time = 0;
    this->setPositionToTrack(firstTrack, m_currTrackPosition);


    if(!isPlaybackReplayDataEnabled())
    {
        initializeReplayData();
    }
}

void Pet::initializeReplayData()
{
    float totalTime = running_time + after_running_time;
    
    m_replayData.clearData();
    CCPoint nowPosition = getPosition()/retinaMultiplier;
    m_replayData.addData(nowPosition ,totalTime);
    m_replayData.addData(REPLAY_STATE_ANIM, m_currPetAction, totalTime);
    m_replayData.addData(REPLAY_STATE_FLIP, m_flip ? 1:0, totalTime);
    m_replayData.addData(REPLAY_STATE_TRACK, m_currTrack->ptVo.index, totalTime);
    m_replayData.addData(REPLAY_STATE_SUM_TRACK, m_summaryTrack, totalTime);
    m_replayData.addData(REPLAY_FLOAT_SPEED, m_currSpeed, totalTime);
    m_replayData.addData(REPLAY_FLOAT_TRACK, m_currTrackPosition, totalTime);
    m_replayData.addData(REPLAY_FLOAT_SUM_TRACK, m_summaryTrackPosition, totalTime);
    m_lastSkillTime = 0.f;
}

CCPoint lerpPoint(CCPoint v0, CCPoint v1, float t) {
    return ccp(v0.x * (1-t), v0.y * (1-t)) + ccp(v1.x*t, v1.y*t);
}

void Pet::setPositionToTrack(Track *track, float trackPosition)
{
    m_currPos = this->getPositionFromTrack(track, trackPosition, true);
    this->setPosition(m_currPos);
}

CCPoint Pet::getPositionFromTrack(Track *track, float trackPosition)
{
    return getPositionFromTrack(track, trackPosition, false);
}

CCPoint Pet::getPositionFromTrack(Track *track, float trackPosition, bool doFlip)
{
    CCPoint positionOnPath;
    
    CCPoint currTrackPos = track->getPosition();
    CCPoint nextTrackPos = track->nextTrack->getPosition();
    
    int direction;
    
    bool directionUp = false;
    if (currTrackPos.y < nextTrackPos.y) {
        directionUp = true;
    }
    
    if (currTrackPos.x < nextTrackPos.x) { //right
        
        if (directionUp) { //up
            direction = D_UP_RIGHT;
            
            if (doFlip)
            {
                flip(true);
                m_effectDash->setScaleX(EFFECT_DASH_SCALE);
                m_effectDash->setScaleY(EFFECT_DASH_SCALE);
            }
        } else { //down
            direction = D_DOWN_RIGHT;
            
            if (doFlip)
            {
                flip(true);
                m_effectDash->setScaleX(EFFECT_DASH_SCALE);
                m_effectDash->setScaleY(-EFFECT_DASH_SCALE);
            }
        }
    } else { //left
        
        if (directionUp) { //up
            direction = D_UP_LEFT;
            
            if (doFlip)
            {
                flip(false);
                m_effectDash->setScaleX(-EFFECT_DASH_SCALE);
                m_effectDash->setScaleY(EFFECT_DASH_SCALE);
            }
        } else { //down
            direction = D_DOWN_LEFT;
            
            if (doFlip)
            {
                flip(false);
                m_effectDash->setScaleX(-EFFECT_DASH_SCALE);
                m_effectDash->setScaleY(-EFFECT_DASH_SCALE);
            }
        }
    }
    
    if (T_S_UP_RIGHT || T_S_UP_LEFT)
    {
        CCPoint begin, end;
        
        if (T_S_UP_RIGHT)
        {
            begin = ccp(g_gameContent->tileSize.width * -0.5f,
                        g_gameContent->tileSize.height * -0.5f);
            end   = ccp(g_gameContent->tileSize.width * 0.5f,
                        g_gameContent->tileSize.height * 0.5f);
            
            if (isBot)
            {
                CCPoint shift = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                    g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shift;
                end = end + shift;
            }
            else
            {
                CCPoint shift = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                    g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shift;
                end = end + shift;
            }
            
            if (direction == D_UP_RIGHT)
            {
                positionOnPath = lerpPoint(begin, end, trackPosition);
            }
            else// if (direction == D_DOWN_LEFT)
            {
                positionOnPath = lerpPoint(begin, end, 1-trackPosition);
            }
        }
        else// if (T_S_UP_LEFT)
        {
            begin = ccp(g_gameContent->tileSize.width * 0.5f,
                        g_gameContent->tileSize.height * -0.5f);
            end   = ccp(g_gameContent->tileSize.width * -0.5f,
                        g_gameContent->tileSize.height * 0.5f);
            
            if (isBot)
            {
                CCPoint shift = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                    g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shift;
                end = end + shift;
            }
            else
            {
                CCPoint shift = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                    g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shift;
                end = end + shift;
            }
            
            if (direction == D_UP_LEFT)
            {
                positionOnPath = lerpPoint(begin, end, trackPosition);
            }
            else// if (direction == D_DOWN_RIGHT)
            {
                positionOnPath = lerpPoint(begin, end, 1-trackPosition);
            }
        }
        
    }
    else //curve track
    {
        CCPoint begin, middle, end, begin2Middle, middle2End;
        
        if (track->ptVo.no == TTYPE_OUTER_CORNER_TOP_LEFT)
        {
            begin = ccp(g_gameContent->tileSize.width * 0.5f,
                        g_gameContent->tileSize.height * -0.5f);
            middle= ccp(0, 0);
            end   = ccp(g_gameContent->tileSize.width * 0.5f,
                        g_gameContent->tileSize.height * 0.5f);
            
            if (isBot)
            {
                CCPoint shiftBegin = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                         g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftMiddle = shiftBegin;
                CCPoint shiftEnd = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                       g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shiftBegin;
                middle = middle + shiftMiddle;
                end = end + shiftEnd;
            }
            else
            {
                CCPoint shiftBegin = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                         g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftMiddle = shiftBegin;
                CCPoint shiftEnd = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                       g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shiftBegin;
                middle = middle + shiftMiddle;
                end = end + shiftEnd;
            }
            
            if (direction == D_UP_RIGHT)
            {
                
                begin2Middle = lerpPoint(begin, middle, trackPosition);
                middle2End = lerpPoint(middle, end, trackPosition);
                positionOnPath = lerpPoint(begin2Middle, middle2End, trackPosition);
            }
            else// if (direction == D_DOWN_RIGHT)
            {
                begin2Middle = lerpPoint(begin, middle, 1-trackPosition);
                middle2End = lerpPoint(middle, end, 1-trackPosition);
                positionOnPath = lerpPoint(begin2Middle, middle2End, 1-trackPosition);
            }
        }
        else if (track->ptVo.no == TTYPE_OUTER_CORNER_TOP_RIGHT)
        {
            begin = ccp(g_gameContent->tileSize.width * -0.5f,
                        g_gameContent->tileSize.height * -0.5f);
            middle= ccp(0, 0);
            end   = ccp(g_gameContent->tileSize.width * 0.5f,
                        g_gameContent->tileSize.height * -0.5f);
            
            if (isBot)
            {
                CCPoint shiftBegin = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                         g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftMiddle = ccp(0.0f,
                                          g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftEnd = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                       g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shiftBegin;
                middle = middle + shiftMiddle;
                end = end + shiftEnd;
            }
            else
            {
                CCPoint shiftBegin = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                         g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftMiddle = ccp(0.0f,
                                          g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftEnd = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                       g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shiftBegin;
                middle = middle + shiftMiddle;
                end = end + shiftEnd;
            }
            
            if (direction == D_DOWN_RIGHT)
            {
                
                begin2Middle = lerpPoint(begin, middle, trackPosition);
                middle2End = lerpPoint(middle, end, trackPosition);
                positionOnPath = lerpPoint(begin2Middle, middle2End, trackPosition);
            }
            else// if (direction == D_DOWN_LEFT)
            {
                begin2Middle = lerpPoint(begin, middle, 1-trackPosition);
                middle2End = lerpPoint(middle, end, 1-trackPosition);
                positionOnPath = lerpPoint(begin2Middle, middle2End, 1-trackPosition);
            }
        }
        else if (track->ptVo.no == TTYPE_OUTER_CORNER_BOTTOM_LEFT)
        {
            begin = ccp(g_gameContent->tileSize.width * -0.5f,
                        g_gameContent->tileSize.height * 0.5f);
            middle= ccp(0, 0);
            end   = ccp(g_gameContent->tileSize.width * 0.5f,
                        g_gameContent->tileSize.height * 0.5f);
            
            if (isBot)
            {
                CCPoint shiftBegin = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                         g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftMiddle = ccp(0.0f,
                                          g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftEnd = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                       g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shiftBegin;
                middle = middle + shiftMiddle;
                end = end + shiftEnd;
            }
            else
            {
                CCPoint shiftBegin = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                         g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftMiddle = ccp(0.0f,
                                          g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftEnd = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                       g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shiftBegin;
                middle = middle + shiftMiddle;
                end = end + shiftEnd;
            }
            
            if (direction == D_UP_RIGHT)
            {
                
                begin2Middle = lerpPoint(begin, middle, trackPosition);
                middle2End = lerpPoint(middle, end, trackPosition);
                positionOnPath = lerpPoint(begin2Middle, middle2End, trackPosition);
            }
            else// if (direction == D_UP_LEFT)
            {
                begin2Middle = lerpPoint(begin, middle, 1-trackPosition);
                middle2End = lerpPoint(middle, end, 1-trackPosition);
                positionOnPath = lerpPoint(begin2Middle, middle2End, 1-trackPosition);
            }
        }
        else if (track->ptVo.no == TTYPE_OUTER_CORNER_BOTTOM_RIGHT)
        {
            begin = ccp(g_gameContent->tileSize.width * -0.5f,
                        g_gameContent->tileSize.height * -0.5f);
            middle= ccp(0, 0);
            end   = ccp(g_gameContent->tileSize.width * -0.5f,
                        g_gameContent->tileSize.height * 0.5f);
            
            if (isBot)
            {
                CCPoint shiftBegin = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                         g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftMiddle = ccp(0.0f,
                                          g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftEnd = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                       g_gameContent->tileSize.height * TWOPLAYER_PERCENT_SHIFT);
                begin = begin + shiftBegin;
                middle = middle + shiftMiddle;
                end = end + shiftEnd;
            }
            else
            {
                CCPoint shiftBegin = ccp(g_gameContent->tileSize.width * TWOPLAYER_PERCENT_SHIFT,
                                         g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftEnd = ccp(g_gameContent->tileSize.width * -TWOPLAYER_PERCENT_SHIFT,
                                       g_gameContent->tileSize.height * -TWOPLAYER_PERCENT_SHIFT);
                CCPoint shiftMiddle = shiftEnd;
                begin = begin + shiftBegin;
                middle = middle + shiftMiddle;
                end = end + shiftEnd;
            }
            
            if (direction == D_UP_LEFT)
            {
                
                begin2Middle = lerpPoint(begin, middle, trackPosition);
                middle2End = lerpPoint(middle, end, trackPosition);
                positionOnPath = lerpPoint(begin2Middle, middle2End, trackPosition);
            }
            else// if (direction == D_DOWN_LEFT)
            {
                begin2Middle = lerpPoint(begin, middle, 1-trackPosition);
                middle2End = lerpPoint(middle, end, 1-trackPosition);
                positionOnPath = lerpPoint(begin2Middle, middle2End, 1-trackPosition);
            }
        }
    }
    
    return currTrackPos + positionOnPath;
}

void Pet::getNewSpeed(float delta, float curr_speed, float *new_speed, Track* curr_track)
{
    getNewSpeed(delta, curr_speed, new_speed, curr_track,
                &m_dashTime, &m_stompTime, &m_shockwave1Time, &m_shockwave2Time,
                &m_usingDash, &m_usingStomp, &m_usingShockwaveStraight1, &m_usingShockwaveStraight2,
                m_dashSpeed, m_stompSpeed, m_shockwave1Speed, m_shockwave2Speed,
                &m_usingSkill);
}

void Pet::getNewSpeed(float delta, float curr_speed, float *new_speed, Track* curr_track,
                      float *dashTime, float *stompTime, float *shockwave1Time, float *shockwave2Time,
                      bool *usingDash, bool *usingStomp, bool *usingShockwave1, bool *usingShockwave2)
{
    bool usingSkill;
    getNewSpeed(delta, curr_speed, new_speed, curr_track,
                dashTime, stompTime, shockwave1Time, shockwave2Time,
                usingDash, usingStomp, usingShockwave1, usingShockwave2,
                m_dashSpeed, m_stompSpeed, m_shockwave1Speed, m_shockwave2Speed,
                &usingSkill);
}

void Pet::getNewSpeed(float delta, float curr_speed, float *new_speed, Track* curr_track,
                      float *dashTime, float *stompTime, float *shockwave1Time, float *shockwave2Time,
                      bool *usingDash, bool *usingStomp, bool *usingShockwave1, bool *usingShockwave2,
                      float dashSpeed, float stompSpeed, float shockwave1Speed, float shockwave2Speed)
{
    bool usingSkill;
    getNewSpeed(delta, curr_speed, new_speed, curr_track,
                dashTime, stompTime, shockwave1Time, shockwave2Time,
                usingDash, usingStomp, usingShockwave1, usingShockwave2,
                dashSpeed, stompSpeed, shockwave1Speed, shockwave2Speed,
                &usingSkill);
}

void Pet::getNewSpeed(float delta, float curr_speed, float *new_speed, Track* curr_track,
                      float *dashTime, float *stompTime, float *shockwave1Time, float *shockwave2Time,
                      bool *usingDash, bool *usingStomp, bool *usingShockwave1, bool *usingShockwave2,
                      float dashSpeed, float stompSpeed, float shockwave1Speed, float shockwave2Speed,
                      bool *usingSkill)
{
    *new_speed = curr_speed;
    
    //falling takes priority over everything
    if (m_currPetAction == PET_FALL_A ||
        m_currPetAction == PET_FALL_B ||
        m_currPetAction == PET_SLIDE)
    {
        if (*usingDash) {
            *new_speed = MIN(m_beforeUsingSkillSpeed, curr_speed);
            m_beforeIsAttackedSpeed = *new_speed;
        }
        
        float acceleration = m_beforeIsAttackedSpeed / SECONDS_TO_BRAKE_TO_FULL_STOP;
        *new_speed = *new_speed - (acceleration * delta);
        if (*new_speed < 0.0f) *new_speed = 0.0f;
        
        *dashTime = 0.0f;
        *stompTime = 0.0f;
        *shockwave1Time = 0.0f;
        *shockwave2Time = 0.0f;
        *usingDash = false;
        *usingStomp = false;
        *usingShockwave1 = false;
        *usingShockwave2 = false;
        *usingSkill = false;
        return;
    }
    
    if (*usingDash) {
        if (*dashTime > 0.0f) {
            *dashTime -= delta;
            *new_speed = m_dashSpeed;
        } else {
            if (*new_speed > m_beforeUsingSkillSpeed) *new_speed = m_beforeUsingSkillSpeed;
            *dashTime = 0.0f;
            *usingDash = false;
            *usingSkill = false;
        }
        return;
    }
    else if (*usingStomp) {
        if (*stompTime > 0.0f) {
            *stompTime -= delta;
            *new_speed = m_stompSpeed;
        } else {
            *new_speed = m_beforeUsingSkillSpeed;
            *stompTime = 0.0f;
            *usingStomp = false;
            *usingSkill = false;
        }
        return;
    }
    else if (*usingShockwave1) {
        if (*shockwave1Time > 0.0f) {
            *shockwave1Time -= delta;
            *new_speed = m_shockwave1Speed;
        } else {
            *new_speed = m_beforeUsingSkillSpeed;
            *shockwave1Time = 0.0f;
            *usingShockwave1 = false;
            *usingSkill = false;
        }
        return;
    }
    else if (*usingShockwave2) {
        if (*shockwave2Time > 0.0f) {
            *shockwave2Time -= delta;
            *new_speed = m_shockwave2Speed;
        } else {
            *new_speed = m_beforeUsingSkillSpeed;
            *shockwave2Time = 0.0f;
            *usingShockwave2 = false;
            *usingSkill = false;
        }
        return;
    }
    
    GDTrackVO _tvo = g_gameContent->getTrackVO(curr_track->ptVo.type);
    
    int stat_value = 0;
    float seconds_to_accel_or_brake = 1.0;
    
    if (TRACK_TYPE_STRAIGHT) {
        stat_value = m_stat_straight_speed;
    } else { //CORNER
        stat_value = m_stat_corner_speed;
    }
    
    float target_speed = getTargetSpdFromStat(stat_value);
    
    //if acceleration is needed and is above normal speed
    if ((curr_speed < target_speed) /*&& (m_currSpeed >= STARTSPEED)*/) {
        //more seconds accelerates slower, less seconds accelerates faster
        seconds_to_accel_or_brake = getSecToAccToTargetSpdFromStat(stat_value);
    }
    //if deacceleration is needed
    else if (curr_speed > target_speed) {
        seconds_to_accel_or_brake = SECONDS_TO_BRAKE_TO_TARGET_SPEED;
    }
    //            //if at the starting point of the game, starting at starting speed and less than normal speed
    //            else if ((curr_speed < target_speed) && (curr_speed < STARTSPEED)) {
    //                seconds_to_accel_or_brake = SECONDS_TO_ACCELERATE_TO_NORMAL_SPEED;
    //            }
    //if nothing is needed
    else {
        //unchanged
    }
    
    float acceleration = target_speed / seconds_to_accel_or_brake;
    
    float track_acceleration_multiplier = 1.0f;
    //prefers this track
    if (pVo.track_preference == _tvo.type) {
        track_acceleration_multiplier = _tvo.param_1;
    } else { //does NOT prefer this track
        track_acceleration_multiplier = _tvo.param_2;
    }
    
    acceleration = acceleration * track_acceleration_multiplier;
    
    *new_speed += (acceleration * delta);
    
    if (curr_speed < target_speed) { //accelerating
        if (*new_speed > target_speed) *new_speed = target_speed;
    } else { //deacceleration
        if (*new_speed < target_speed) *new_speed = target_speed;
    }
    
    if (!isHelperPet) {
        if (isBot) {
            if (*new_speed < MINSPEED_FOR_SPEED_DISPLAY) *new_speed = MINSPEED_FOR_SPEED_DISPLAY; //so AI can lose to player at tutorial stage
        } else {
            if (*new_speed < MINSPEED) *new_speed = MINSPEED;
        }
    }
}

Track* Pet::getTrackFromTrackPosition(Track *startingTrack, float *trackPosition, int *trackCount) {
    
    Track *track = startingTrack;
    
    *trackCount = 0;
    while (*trackPosition >= 1.0f) {
        track = track->nextTrack;
        
        *trackPosition = *trackPosition - 1;
        *trackCount = *trackCount + 1;
    }
    
    return track;
}

//more accurate
CCPoint Pet::getNextPositionAfterSeconds(float seconds) {
    
    float new_speed = m_currSpeed;
    float current_speed = m_currSpeed;
    float trackPosition = m_currTrackPosition;
    Track *curr_track = m_currTrack;
    
    float delta = 0.016666667f;
    
    if (seconds < delta) seconds = delta;
    
    float dashTime = m_dashTime;
    float stompTime = m_stompTime;
    float shockwave1Time = m_shockwave1Time;
    float shockwave2Time = m_shockwave2Time;
    bool usingDash = m_usingDash;
    bool usingStomp = m_usingStomp;
    bool usingShockwave1 = m_usingShockwaveStraight1;
    bool usingShockwave2 = m_usingShockwaveStraight2;
    
    while (seconds > 0.0f) {
        seconds -= delta;
        
        getNewSpeed(delta, current_speed, &new_speed, curr_track,
                    &dashTime, &stompTime, &shockwave1Time, &shockwave2Time,
                    &usingDash, &usingStomp, &usingShockwave1, &usingShockwave2);
        current_speed = new_speed;
        
        trackPosition += (current_speed * delta);
        
        int trackCount = 0;
        curr_track = getTrackFromTrackPosition(curr_track, &trackPosition, &trackCount);
    }
    
    return getPositionFromTrack(curr_track, trackPosition);
}

//not accurate if delta is high, not 100% reliable, but is faster performance
CCPoint Pet::getNextPositionFromDelta(float delta) {
    
    float new_speed = m_currSpeed;
    
    float dashTime = m_dashTime;
    float stompTime = m_stompTime;
    float shockwave1Time = m_shockwave1Time;
    float shockwave2Time = m_shockwave2Time;
    bool usingDash = m_usingDash;
    bool usingStomp = m_usingStomp;
    bool usingShockwave1 = m_usingShockwaveStraight1;
    bool usingShockwave2 = m_usingShockwaveStraight2;
    
    getNewSpeed(delta, m_currSpeed, &new_speed, m_currTrack,
                &dashTime, &stompTime, &shockwave1Time, &shockwave2Time,
                &usingDash, &usingStomp, &usingShockwave1, &usingShockwave2);
    
    float trackPosition = m_currTrackPosition;
    trackPosition += (new_speed * delta);
    
    int trackCount = 0;
    Track *track = getTrackFromTrackPosition(m_currTrack, &trackPosition, &trackCount);
    
    return getPositionFromTrack(track, trackPosition);
}

void Pet::collectVisualReplayData()
{
    float totalTime = running_time + after_running_time;

    CCPoint lastPosition = m_replayData.getLastAddedPosition();
    if(lastPosition.getDistanceSq(getPosition()) > 5.f)
        m_replayData.addData(getPosition()/retinaMultiplier, totalTime);

    int animState = m_replayData.getLastAddedState(REPLAY_STATE_ANIM);
    if(animState != m_currPetAction)
        m_replayData.addData(REPLAY_STATE_ANIM, m_currPetAction, totalTime);

    int flipState = m_replayData.getLastAddedState(REPLAY_STATE_FLIP);
    bool flipped = flipState == 1;
    if(flipped != m_flip)
        m_replayData.addData(REPLAY_STATE_FLIP, m_flip ? 1:0, totalTime);
}

void Pet::collectSimReplayData()
{
    float totalTime = running_time + after_running_time;

    float lastSpeed = m_replayData.getLastAddedFloat(REPLAY_FLOAT_SPEED);
    if(fabs(m_currSpeed - lastSpeed) >= 0.1f)
        m_replayData.addData(REPLAY_FLOAT_SPEED, m_currSpeed, totalTime);

    float lastTrackPos = m_replayData.getLastAddedFloat(REPLAY_FLOAT_TRACK);
    if(fabs(m_currTrackPosition - lastTrackPos) >= 0.01f)
        m_replayData.addData(REPLAY_FLOAT_TRACK, m_currTrackPosition, totalTime);

    float lastSumTrack = m_replayData.getLastAddedFloat(REPLAY_FLOAT_SUM_TRACK);
    if(fabs(m_summaryTrackPosition - lastSumTrack) > 0.5f)
        m_replayData.addData(REPLAY_FLOAT_SUM_TRACK, m_summaryTrackPosition, totalTime);

    int lastSumTrackState = m_replayData.getLastAddedState(REPLAY_STATE_SUM_TRACK);
    if(lastSumTrackState != m_summaryTrack)
        m_replayData.addData(REPLAY_STATE_SUM_TRACK, m_summaryTrack, totalTime);

    int lastTrackIndex = m_replayData.getLastAddedState(REPLAY_STATE_TRACK);
    if(lastTrackIndex != m_currTrack->ptVo.index)
        m_replayData.addData(REPLAY_STATE_TRACK, m_currTrack->ptVo.index, totalTime);
}

void Pet::playbackReplayData(float delta)
{
    float totalTime = running_time + after_running_time;
    petSpine->timeScale = isoMapRacing->getTimeScale();

    CCPoint pos = m_replayData.getPositionAtTime(totalTime);
    pos = pos * retinaMultiplier;
    setPosition(pos);
    m_currPos = pos;

    float timeInState;
    PetAction animState = static_cast<PetAction>(m_replayData.getStateAtTime(REPLAY_STATE_ANIM, totalTime, timeInState));
    if (animState != m_currPetAction)
        setAnimation(animState);

    m_currSpeed = m_replayData.getFloatAtTime(REPLAY_FLOAT_SPEED, totalTime);
    m_currTrackPosition = m_replayData.getFloatAtTime(REPLAY_FLOAT_TRACK, totalTime, false);
    m_summaryTrackPosition = m_replayData.getFloatAtTime(REPLAY_FLOAT_SUM_TRACK, totalTime);
    m_summaryTrack = m_replayData.getStateAtTime(REPLAY_STATE_SUM_TRACK, totalTime, timeInState);
    m_currTrack = isoMapRacing->getTrackAtIndex(m_replayData.getStateAtTime(REPLAY_STATE_TRACK, totalTime, timeInState));

    int flipState = m_replayData.getStateAtTime(REPLAY_STATE_FLIP, totalTime, timeInState);
    bool flipped = flipState == 1;
    if (flipped != m_flip)
    {
        flip(flipped);
        flipSpriteDirection();
    }

    int skillEvent;
    float skillTime;
    if(m_replayData.getStateBetweenTime(REPLAY_STATE_SKILL, totalTime-delta, totalTime, skillEvent, skillTime) && (skillTime > m_lastSkillTime))
    {
        m_lastSkillTime = skillTime;
        switch (skillEvent)
        {
        case SKILL_DASH:
            startDashEffect();
            break;
        case SKILL_TRAP:
            spawnTrap();
            break;
        case SKILL_STOMP:
            effectStomp();
            break;
        case SKILL_SHOCKWAVE1:
            spawnShockwave1();
            break;
        case SKILL_SHOCKWAVE2:
            spawnShockwave2();
            break;
        case REPLAY_EVENT_CALL_HELPER:
            isoMapRacing->addHelperPet();
            break;
        case REPLAY_EVENT_REMOVE_HELPER:
            isoMapRacing->removeHelperPet();
            break;
        default:
            break;
        }
    }
    if(m_usingDash)
        updateDashEffect();
}

int Pet::flipSpriteDirection()
{
    CCPoint currTrackPos = m_currTrack->getPosition();
    CCPoint nextTrackPos = m_currTrack->nextTrack->getPosition();

    int direction;
    bool directionUp = false;
    if (currTrackPos.y < nextTrackPos.y)
        directionUp = true;

    if (currTrackPos.x < nextTrackPos.x) //right
    {
        if (directionUp) //up
        {
            direction = D_UP_RIGHT;
            flip(true);
            m_effectDash->setScaleX(EFFECT_DASH_SCALE);
            m_effectDash->setScaleY(EFFECT_DASH_SCALE);
        }
        else //down
        {
            direction = D_DOWN_RIGHT;
            flip(true);
            m_effectDash->setScaleX(EFFECT_DASH_SCALE);
            m_effectDash->setScaleY(-EFFECT_DASH_SCALE);
        }
    }
    else //left
    {
        if (directionUp) //up
        {
            direction = D_UP_LEFT;
            flip(false);
            m_effectDash->setScaleX(-EFFECT_DASH_SCALE);
            m_effectDash->setScaleY(EFFECT_DASH_SCALE);
        }
        else //down
        {
            direction = D_DOWN_LEFT;
            flip(false);
            m_effectDash->setScaleX(-EFFECT_DASH_SCALE);
            m_effectDash->setScaleY(-EFFECT_DASH_SCALE);
        }
    }
    return direction;
}

std::string Pet::serializeReplayData()
{
    std::string data = m_replayData.serialize();
    CCLog_("%s",data.c_str());
    //m_replayData.clearData();
    //m_replayData.deserialize(data);
    //CCAssert(strcmp(data.c_str(), m_replayData.serialize().c_str()) == 0, "Deserialization Failed");
    return data;
}

void Pet::enablePlaybackReplayData(bool enable, const char* replayData)
{
    if(enable)
    {
        CCAssert(replayData != NULL, "replay data is null");
        m_replayData.clearData();
        m_replayData.deserialize(std::string(replayData));
    }
    m_playbackReplayData = enable;
}

bool Pet::isPlaybackReplayDataEnabled() const
{
    return m_playbackReplayData;
}

bool Pet::isPlaybackFinished() const
{
    float totalTime = running_time + after_running_time;
    return m_replayData.isBeyondEndTime(totalTime);
}

float Pet::getReplayDataDuration() const
{
    return m_replayData.getDuration();
}

float Pet::getRemainingReplayTime() const
{
    float totalTime = running_time + after_running_time;
    return m_replayData.getDuration() - totalTime;
}

Ability* Pet::createAbility(const char* abilityName)
{
    Ability* ability = Ability::create();
    ability->retain();
    ability->aVo = GDAbilityVO(g_gameContent->getAbilityVO(abilityName, 1));
    ability->paVo = PlayerAbilityVO(ability->aVo);
    playerState->Abilities[abilityName] = ability;
    return ability;
}

void Pet::callHelper(Pet* pet)
{
    if(m_playbackReplayData)
        return;
    float totalTime = running_time + after_running_time;
    m_replayData.addData(REPLAY_STATE_SKILL, REPLAY_EVENT_CALL_HELPER, totalTime);
    pet->initializeReplayData();
}

void Pet::removeHelper()
{
    if(m_playbackReplayData)
        return;
    float totalTime = running_time + after_running_time;
    m_replayData.addData(REPLAY_STATE_SKILL, REPLAY_EVENT_REMOVE_HELPER, totalTime);
}

Track* Pet::getStartingTrackFromReplayData()
{
    float timeInState;
    return isoMapRacing->getTrackAtIndex(m_replayData.getStateAtTime(REPLAY_STATE_TRACK, 0.f, timeInState));
}

void Pet::updateRunning(void *ptr_isomap , float delta) {
    
    if (!can_running) return;
    if (!m_isFinishedRunning)
        running_time += delta;
    else
        after_running_time += delta;

    if(m_playbackReplayData)
    {
        playbackReplayData(delta);
        return;
    }
    collectVisualReplayData();

    if (!start_running) return;
    IsoMapRacing* isoMapRacing = (IsoMapRacing*)ptr_isomap;
    if (!isoMapRacing) { printf("(Pet::updateRunning) Error: IsoMapRacing is uninitialized!\n"); return; }
    SceneRace* _scene = (SceneRace*)g_currentScene;
    if (!_scene) { printf("(Pet::updateRunning) Error: SceneRaceCommand is uninitialized!\n"); return; }
    if (!petSpine) { printf("(Pet::updateRunning) Error: petSpine is uninitialized!\n"); return; }
    GDPetVO _pvo = g_gameContent->getPetVO(ppVo.type);
    if (_pvo.type == "") { printf("failed!\n"); return; }
    
//    m_currDelta += delta;
    
    if (m_stompCollideTime > 0.0f) {
        m_stompCollideTime -= delta;
        if (m_stompCollideTime < 0.0f)
        {
            m_stompCollideTime = 0.0f;
            stompCollide();
        }
    }
    
    if (m_stunnedTime > 0.0f) {
        m_stunnedTime -= delta;
        if (m_stunnedTime < 0.0f)
        {
            m_stunnedTime = 0.0f;
            petIsAttackedRecovered(NULL);
        }
    }
    
    if (!m_isAttacked)
    {
        if (m_recoveryTime > 0.0f) {
            m_recoveryTime -= delta;
            if (m_recoveryTime < 0.0f) m_recoveryTime = 0.0f;
        }
    }
    
    //Check to continue running after playing other animations (except when isAttacked)
    if (petSpine->states[0]->animation
        && petSpine->states[0]->time >= petSpine->states[0]->animation->duration
//        && !m_needStopOnNextTrack
        && m_currSpeed != 0
        && !m_isAttacked)
    {
        setAnimation(PET_RUN);
    }
    
    float new_speed = m_currSpeed;
    
    if (m_isFinishedRunning) {
        m_finishedRunningTime += delta;
        
        if (m_usingDash) {
            m_usingDash = false; //stop dashing when entering the finish line
        }
        
        //brake to full stop when is finishing
        float acceleration = m_beforeIsFinishedRunningSpeed / SECONDS_TO_BRAKE_TO_FULL_STOP;
        new_speed = m_currSpeed - (acceleration * delta);
        if (new_speed < 0.0f) new_speed = 0.0f;
        
        petSpine->timeScale = 1;
        
        float time = (petResult == WIN_RESULT ? DELAY_TIME_TO_RESULT_SCREEN_AFTER_WINNING : DELAY_TIME_TO_RESULT_SCREEN_AFTER_LOSING);
        
        if (m_finishedRunningTime >= time) {
            
            if ((petSpine->states[0]->time > petSpine->states[0]->animation->duration || !PET_IS_JUMPING) && !end_running) {
                end_running = true;
                
                isoMapRacing->stopRunningWithPetPlayer(this);
            }
        }
    } else {
        getNewSpeed(delta, m_currSpeed, &new_speed, m_currTrack);
    }
    
    if (m_usingDash) {
        
        CREATE_CHAR_BUFFER(_buf_dash, 32);
        snprintf(_buf_dash, _buf_dash_size, EFFECT_DASH_FILE_FORMAT, m_currentEffectDashFrame);
        
        m_effectDash->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(_buf_dash));
        CCLog_("m_currentEffectDashFrame:%d",m_currentEffectDashFrame);
        
        if (m_currentEffectDashFrame <= EFFECT_DASH_MAX_FRAME &&
            m_currentEffectDashFrame >= EFFECT_DASH_MIN_FRAME )
        {
            
//            if (m_currentDashTimeFrame >= m_effectDashTimePerFrame) {
//                
//                CCLog_("m_currentEffectDashFrame:%d->%d",m_currentEffectDashFrame,m_currentEffectDashFrame+1);
//                m_currentEffectDashFrame++;
//                m_currentDashTimeFrame = 0.0f;
//            } else {
//                m_currentDashTimeFrame += delta;
//                
//                CCLog_("m_currentDashTimeFrame:%f",m_currentDashTimeFrame);
//            }
//            
//            m_effectDash->setVisible(true);
            
//            CCLog_("m_currentEffectDashFrame:%d->%d",m_currentEffectDashFrame,m_currentEffectDashFrame+1);
            
            
            //m_currentEffectDashFrame++;
            
            m_currentEffectDashFrame += m_effectDashAnimationFlipper;
            
            if (m_currentEffectDashFrame == EFFECT_DASH_MIN_FRAME ||
                m_currentEffectDashFrame == EFFECT_DASH_MAX_FRAME)
            {
                m_effectDashAnimationFlipper *= -1;
            }
            
            m_effectDash->setVisible(true);
        } else {
            CCLog_("m_currentEffectDashFrame visible-false:%d",m_currentEffectDashFrame);
            m_effectDash->setVisible(false);
        }
        
        
//        if (m_currentEffectDashFrame <= EFFECT_DASH_MAX_FRAME &&
//            m_currentEffectDashFrame >= EFFECT_DASH_MIN_FRAME) {
//            CCLog_("m_currentEffectDashFrame:%d",m_currentEffectDashFrame);
//            m_effectDash->setVisible(true);
//        } else {
//            CCLog_("m_currentEffectDashFrame visible-false:%d",m_currentEffectDashFrame);
//            m_effectDash->setVisible(false);
//        }
        
    } else {
        m_effectDash->setVisible(false);
    }
    
    m_currSpeed = new_speed;
    m_currTrackPosition += (m_currSpeed * delta);
    m_summaryTrackPosition += (m_currSpeed * delta);

    int trackCount = 0;
    m_currTrack = getTrackFromTrackPosition(m_currTrack, &m_currTrackPosition, &trackCount);
    m_summaryTrack += trackCount;

    collectSimReplayData();
    
    this->setPositionToTrack(m_currTrack, m_currTrackPosition);
    
#ifdef DEBUG_TEST_STOMP_AREA
    float distance = ccpLength(g_gameContent->tileSize * STOMP_AREA_RATIO_TO_TILESIZE);
    distance-=0.01f;
    
//    testTime += delta;
//    if (testTime > 2.0f) {
//        testTime = 0;
//        testAngle = CC_DEGREES_TO_RADIANS(arc4random()%360);
//    }
    
//    320, bottom right, 230 bottom left, 135 top right, 0 right
//    testAngle = CC_DEGREES_TO_RADIANS(320);
    
    testAngle += (delta * 1.5f);
    
    if (isBot) {
        this->setPosition(opponentPet->getPosition()
                          +STOMP_Y_OFFSET
//                          +ccp(cosf(testAngle) * distance, sinf(testAngle) * distance));
                          +ccp(cosf(testAngle) * distance, sinf(testAngle) * distance * STOMP_CIRCLE_Y_LENGTH_PETCENT));
    } else
        this->setPositionToTrack(firstTrack, 0);
#endif
    
#ifdef DEBUG_TEST_SHOCKWAVE_STRAIGHT
//#define DEBUG_SELECT_UUID "9caf271c-505c-4b51-83d6-1c1c641deba3" //for testing shockwave straight and double shoot
    if (isBot) {
        Track *theTrack = firstTrack;
        int target = 10;
        int count = 0;
        while (count < target) {
            theTrack = theTrack->nextTrack;
            count++;
        }
        this->setPositionToTrack(theTrack, 0.2);
        m_currTrack = theTrack;
    } else {
        Track *theTrack = firstTrack;
        int target = 4;
        int count = 0;
        while (count < target) {
            theTrack = theTrack->nextTrack;
            count++;
        }
        this->setPositionToTrack(theTrack, 0.8);
        m_currTrack = theTrack;
    }
#endif
    
    if (!m_usingShockwaveStraight1 &&
        !m_usingShockwaveStraight2 &&
        !m_usingStomp &&
        !m_isFinishedRunning)
    {
        setTimeScaleDependingOnSpeed();
    }
    
    if (!m_isFinishedRunning &&
        ran_round < m_summaryTrack/m_trackCount &&
        !isHelperPet)
    {
        ran_round = m_summaryTrack/m_trackCount;
        isoMapRacing->updateLapWithPetPlayer(this);
        
        if (ran_round >= max_round) {
            m_beforeIsFinishedRunningSpeed = m_currSpeed;
            m_isFinishedRunning = true;
            isoMapRacing->finishRunningWithPetPlayer(this);
            
            opponentPet->m_beforeIsFinishedRunningSpeed = opponentPet->m_currSpeed;
            opponentPet->m_isFinishedRunning = true;
        }
    }
}

void Pet::setTimeScaleDependingOnSpeed()
{
    if (PET_IS_RUNNING || PET_IS_JUMPING) {
        petSpine->timeScale = MIN(2.5f, m_currSpeed*1.5*TIME_SCALE_FACTOR);
    } else if (m_currPetAction == PET_FALL_A ||
               m_currPetAction == PET_FALL_B) {
        petSpine->timeScale = PET_FALLING_TIME_SCALE*TIME_SCALE_FACTOR;
    }
}

void Pet::pauseRacing()
{
    m_isPaused = true;
    this->pauseSchedulerAndActions();
    m_beforePausedTimeScale = petSpine->timeScale;
    petSpine->timeScale = 0.0f;
    if (m_critLabel) m_critLabel->pauseSchedulerAndActions();
    if (m_groupEnergy) m_groupEnergy->pauseSchedulerAndActions();
    if (m_effectGlow) m_effectGlow->pauseSchedulerAndActions();
    
    if (m_stompSpine) {
        m_stompSpine->timeScale = 0.0f;
        m_stompSpine->pauseSchedulerAndActions();
    }
    
    CCObject *object = NULL;
    CCARRAY_FOREACH(m_groupSpineEffectsBelowPet->getChildren(), object)
    {
        CCSkeletonAnimation *spine = (CCSkeletonAnimation *)object;
        spine->timeScale = 0.0f;
    }
}

void Pet::resumeRacing()
{
    m_isPaused = false;
    this->resumeSchedulerAndActions();
    petSpine->timeScale = m_beforePausedTimeScale;
    if (m_critLabel) m_critLabel->resumeSchedulerAndActions();
    if (m_groupEnergy) m_groupEnergy->resumeSchedulerAndActions();
    if (m_effectGlow) m_effectGlow->resumeSchedulerAndActions();
    
    if (m_stompSpine) {
        m_stompSpine->timeScale = 1.0f;
        m_stompSpine->resumeSchedulerAndActions();
    }
    
    CCObject *object = NULL;
    CCARRAY_FOREACH(m_groupSpineEffectsBelowPet->getChildren(), object)
    {
        CCSkeletonAnimation *spine = (CCSkeletonAnimation *)object;
        spine->timeScale = 1.0f;
    }
}

float Pet::getTargetSpdFromStat(int stat_)
{
    //https://www.desmos.com/calculator/u0axuhywem
    return ((-m_spd_b + sqrt(m_spd_b*m_spd_b - (4.0f * m_spd_a * (m_spd_c - (double)stat_)))) / (2.0f * m_spd_a)) * m_spd_mult;
}

float Pet::getSecToAccToTargetSpdFromStat(int stat_)
{
    //https://www.desmos.com/calculator/oyhw7xeazw
    return (((-m_acc_b - sqrt(m_acc_b*m_acc_b - (4.0f * m_acc_a * (m_acc_c - (double)stat_)))) / (2.0f * m_acc_a) * 10.0f)) * m_acc_mult;
}

void Pet::setDefaultSpeed(CCObject *sender) {
    
    if (m_currSpeed < STARTSPEED) m_currSpeed = STARTSPEED;
    
    m_beforeUsingSkillSpeed = m_currSpeed;
}

#pragma mark - Draw

void Pet::draw()
{
    
#ifdef DEBUG_TEST_STOMP_AREA2
    //    if (!testStomp) return;
    //
    //    ccDrawColor4B(255, 0, 0, 230);
    //    ccPointSize(30);
    //    glLineWidth(100.0f);
    //    ccDrawCircle(testStompPosition, testStompRange, CC_DEGREES_TO_RADIANS(90.0f), 50, true);
    //    ccDrawLine(testStompPosition, ccp(testStompRange, 0.0f));
    //    ccDrawRect(CCPointZero, CCDirector::sharedDirector()->getWinSize());
#endif
}

#pragma mark - Get Information

float Pet::getMinSpeed()
{
    return MINSPEED_FOR_SPEED_DISPLAY;
}

float Pet::getTopSpeed()
{
    float higherStatValue = MAX(m_stat_straight_speed, m_stat_corner_speed);
    
    return getTargetSpdFromStat(higherStatValue);
}

float Pet::getSpeedDisplay()
{
    float speedToBeDisplayed = m_currSpeed;//(m_currSpeed-STARTSPEED);
    if (speedToBeDisplayed < 0.0f) speedToBeDisplayed = 0.0f;
    
    return speedToBeDisplayed * g_gameContent->GameConfig["CURR_SPEED_TO_SPEED_DISPLAY_RATIO"];
}

int Pet::getLevel() {
    
    return ppVo.level;
    
}

float Pet::getExpValue() {
    
    return ppVo.exp;
    
}

float Pet::getExpMax() {
    
    return pVo.mvhm * g_gameContent->GameConfig["FOOD_TO_EXP_RATIO"] * (float)g_gameContent->FeedTimes[ppVo.level];
    
}

float Pet::getExpRatio() {
    
    return getExpValue() / getExpMax();
    
}

int Pet::getExpGain() {
    
    return getHungerFoodToFull() * g_gameContent->GameConfig["FOOD_TO_EXP_RATIO"];
    
}

float Pet::getExpGainRatio() {
    
    return (getExpValue() + getExpGain()) / getExpMax();
    
}

float Pet::getHungerValue() {
    
    float out = (pVo.hunger_rate * m_hunger_timer);
    if (out < 0) out = 0.0f;
    if (out > pVo.mvhm) out = pVo.mvhm;
    return out;
    
}

float Pet::getHungerMax() {

    return pVo.mvhm;
    
}

float Pet::getHungerRatio() {
    
    return getHungerValue() / getHungerMax();
    
}

float Pet::getHungerFoodToFull() {
    
    return getHungerValue();
    
}

float Pet::getHungerFoodToFullRatio() {
    
    return (getHungerValue() + getHungerFoodToFull()) / getHungerMax();
    
}

int Pet::getSecondsLeftToHungerDrain() {
    
    return  static_cast<int>((getHungerMax() - getHungerValue()) / pVo.hunger_rate);
    
}

int Pet::getRarityStars() {
    
    return pVo.rarity_stars;
    
}

float Pet::getCurrentEnergy() {
    
    return m_energy_current;
    
}

int Pet::getSecondsLeftToFullEnergy() {
    
    return (int)round((100.0f - m_energy_current)/pVo.energy_rate);
    
}

float Pet::getCoinsProducedPerSecond() {
    
    float produce_rate_min_to_max_range = pVo.cprmax - pVo.cprmin;
    float pet_level_in_range_of_0_to_1 = (ppVo.level/pVo.max_level);
    float coins_produced_per_second = pVo.cprmin + (pet_level_in_range_of_0_to_1 * produce_rate_min_to_max_range);
    
    return coins_produced_per_second;
    
}

float Pet::getCoinsProducedCapacity() {
    
    float capacity_rate_min_to_max_range = pVo.mvcmax - pVo.mvcmin;
    float pet_level_in_range_of_0_to_1 = (ppVo.level/pVo.max_level);
    float coins_produced_capacity = pVo.mvcmin + (pet_level_in_range_of_0_to_1 * capacity_rate_min_to_max_range);
    
    return coins_produced_capacity;
    
}

bool Pet::isInAnimalPen() {
    
    return !(strcmp(ppVo.located_in.c_str(), "") == 0);
    
}

void Pet::printMessage() {
    
    if (message != "") {
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, message);
        message = "";
    }
    
    if (message2 != "") {
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, message2);
        message2 = "";
    }
}


#pragma mark - Show Emotion while in Habitat

bool Pet::isHungry()
{
    return getHungerRatio() >= g_gameContent->GameConfig["HUNGER_ANIMATION_ACTIVE"];
}

bool Pet::isSleeping()
{
    return getCurrentEnergy() < 100.0f;
}

void Pet::showHungry() {
    
    if (g_sceneManager->sceneSelector == SCENE_VILLE) {
        if (SceneVille* _scene = (SceneVille*)g_currentScene) {
            if (!emotionHungry) {
                emotionHungry = CCPhatTouchableSprite::createWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsBubbleEmotion)),HUD_MENU_TOUCH_PRIORITY+1);
                emotionHungry->onHit = onHungryIconTouched;
                emotionHungry->rootNode = _scene->isoMap->getParent();
                emotionHungry->setSender(this);
                CCSprite* _food_icon = CCSprite::createWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsHungryEmotion)));
                _food_icon->setPosition(emotionHungry->getContentSize()*0.5f);
                emotionHungry->addChild(_food_icon);
                emotionHungry->setPosition(ccp(emotionHungry->getContentSize().width*0.75f, emotionHungry->getContentSize().height));
                emotionHungry->setOpacity(0);
                addChild(emotionHungry);
            }
            
            if (emotionHungry->numberOfRunningActions() == 0 && emotionHungry->getOpacity() <= 0.0f) {
                emotionHungry->runAction(CCFadeTo::create(0.1f, 255));
                emotionHungry->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(1.0f, ccp(0.0f, 10.0f*sizeMultiplier)),
                                                                                    CCMoveBy::create(1.0f, ccp(0.0f,-10.0f*sizeMultiplier)),
                                                                                    NULL)));
                CCModalController::showAllChildrensAnimated(emotionHungry, 0.1f);
            }
        }
    }
    
}

void Pet::onHungryIconTouched(void *inst_, void *sender_) {
    
    if (Pet* _this = (Pet*)sender_) {
        if (g_sceneManager->sceneSelector == SCENE_VILLE) {
            if (SceneVille* _scene = (SceneVille*)g_currentScene) {
                if (!_this->getParent()) {
                    return;
                }
                if (IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)_scene->isoMap) {
                    if (_isoMapBuilding->entityAction->getEntity()) {
                        _isoMapBuilding->entityAction->finish(_this->getParent()->getParent(), true);
                    }else {
                        _isoMapBuilding->entityAction->start(_this->getParent()->getParent());
                    }
                }
                
            }
        }
    }
    
}

void Pet::hideHungry() {
    
    if (emotionHungry) {
        if (emotionHungry->getOpacity() > 0.0f) {
            emotionHungry->runAction(CCSequence::create(CCFadeTo::create(0.1f, 0), CCCallFunc::create(this, callfunc_selector(Pet::delHungry)), NULL));
            CCModalController::hideAllChildrensAnimated(emotionHungry, 0.1f);
        }
    }
    
}

void Pet::delHungry(cocos2d::CCObject* sender_) {
    
    if (emotionHungry) {
        emotionHungry->removeFromParentAndCleanup(true);
        emotionHungry = NULL;
    }
    
}

#pragma mark - Hunger Bar Functions

void Pet::setHungerTimer(int timer_) {

    m_hunger_timer = timer_;
    
}

void Pet::feedBegin() {
    
    m_food_fed = 0.0f;
    m_feed_done = false;
    
}

bool Pet::feed() {
    
    float _to_full = getHungerFoodToFull();
    float _feed_amount = _to_full;//pVo.mvhm / _hunger_feed_time;

    if (_feed_amount > _to_full) {
        _feed_amount = _to_full;
    }

    //not enough food to feed
    bool notEnoughFood = (g_playerState->player.balance.food < _feed_amount);
    if (notEnoughFood) {

        SceneVille* _scene = (SceneVille*)g_currentScene;
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        int missing_food = _feed_amount - g_playerState->player.balance.food;
        
        InstantDialog *_dialog =
        InstantDialog::createWithResourceType(_scene->modalController,
                                              RESOURCE_TYPE_FOOD,
                                              missing_food,
                                              kDialogSmallSize);
        
        if(_dialog && _scene) {
            _dialog->setPosition(_screenSize*0.5f);
            _scene->addChild(_scene->modalController, kModalControllerZOrder);
            _scene->modalController->pushView(_dialog);
        }

        return false;
    }
    
    if (_feed_amount > 0.0f) {
        
        //play effect feed animal
        g_gameAudio->playEffect(AUDIO_FEED_ANIMAL);
        
        int _hunger_time_subtract = round(_feed_amount / pVo.hunger_rate);
        _feed_amount = int(_hunger_time_subtract * pVo.hunger_rate);
        
        if (g_playerState->player.balance.food >= (int)_feed_amount) {
            g_playerState->player.balance.food -= (int)_feed_amount;
        }
        
        m_food_fed += _feed_amount;
        showExp(_feed_amount);
        m_hunger_timer -= _hunger_time_subtract;
       
        if (m_hunger_timer < 0) {
            m_hunger_timer = 0;
        }
        
        return true;
    }
    
    return false;
    
}

void Pet::feedEnd() {
    
    if (!m_feed_done) {
        m_feed_done = true;
        
        CCLog_("Fed = %d", (int)m_food_fed);
        CCLog_("Exp Gained = %d", (int)round(m_food_fed*g_gameContent->GameConfig["FOOD_TO_EXP_RATIO"]));
        
        BackendService::getInstance()->send(APIPetFeed::create(this,
                                                               (int)m_food_fed,
                                                               this,
                                                               Pet::onPetFed));
    }
    
}

void Pet::showExp(int feedAmount) {
    
    //int exp_earned_ = feedAmount * g_gameContent->GameConfig["HUNGER_FEED_TIME"];
    
    if (feedAmount <= 0) {
        return;
    }
    
    //if PET is at MAX level, do not show +EXP
    
    if (this->ppVo.level >= this->pVo.max_level) {
        playLevelUpEffect(true);
    }

    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "-%d", feedAmount);
    CCLabelBMFont* _label_food = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium, true);
    _label_food->setOpacity(0.0f);
    _label_food->setColor(kTextColorFood);
    _label_food->setPosition(ccp(cosf(rand())*100.0f*retinaMultiplier, (50.0f+sinf(rand())*100.0f)*retinaMultiplier));
    getParent()->addChild(_label_food, 4);
    
    _label_food->runAction(CCSequence::create(CCFadeTo::create(0.1f, 255),
                                              CCDelayTime::create(0.5f), CCFadeTo::create(0.5f, 0),
                                              CCRemoveSelf::create(),
                                              NULL));
    
}


void Pet::playLevelUpEffect(bool expUp) {
    
    cleanEffect();

    Building* building = g_playerState->getBuilding(ppVo.located_in);
    CCAssert(building, "this pet has no home");

    m_levelUp = CCShareSkeletonAnimation::sharedSkeletonAnimation()->getSkeleton("common/vfx/", "effect_level_up");
    m_levelUp->setScale(sizeMultiplier);
    m_levelUp->setAnimation(expUp ? "exp_level_up" : "animal_level_up", false);
    building->addChild(m_levelUp , 1);
    m_levelUp->runAction(CCSequence::create(CCDelayTime::create(m_levelUp->states[0]->animation->duration),
                                           CCCallFunc::create(this, callfunc_selector(Pet::cleanEffectLevelUp)),
                                           NULL));

    float duration = m_levelUp->states[0]->animation->duration;
    float defaultScaleX = petSpine->getScaleX();
    float defaultScaleY = petSpine->getScaleY();
    
    petSpine->runAction(CCSequence::create(CCDelayTime::create(duration*0.3f),
                                           CCScaleTo::create(0.1, defaultScaleX*1.4f, defaultScaleY*1.4f),
                                           CCScaleTo::create(0.1f, defaultScaleX, defaultScaleY),
                                           NULL));
    
    g_gameAudio->playEffect(AUDIO_PET_LEVEL_UP);
}


#pragma mark - Pet Race Functions

void Pet::setActiveGlowing(bool active) {
    
    if (!m_effectGlow) {
        // Test
        m_effectGlow = CCSprite::create(); //CCSprite::createWithSpriteFrameName("_effect_glow.png");
        m_effectGlow->setPosition(m_effectGlow->getContentSize()*0.05f);
        m_effectGlow->setOpacity(0);
        this->addChild(m_effectGlow, -100);
    }
    is_active_grow = active;
    if (is_active_grow) {
        m_effectGlow->runAction(CCFadeIn::create(0.2f));
    } else {
        m_effectGlow->runAction(CCFadeOut::create(0.2f));
    }
    
}

bool Pet::isActiveGlowing()
{
    return is_active_grow;
}

bool Pet::isFalling() const
{
    return m_currPetAction == PET_FALL_A || m_currPetAction == PET_FALL_B;
}

#pragma mark - Getting Attacked

float Pet::isCritical()
{
    if (opponentPet->isHelperPet) return 0.0f;
    
    //if opponent pet is 30% of a lap behind the you, you will get a critical
    if ((opponentPet->m_summaryTrack + (int)(opponentPet->m_trackCount * 0.33f)) //current track + (third of one lap)
        < m_summaryTrack)
    {
        return CRITICAL_STUN_DURATION_MULT;
    }
    
    return 0.0f;
}

void Pet::petIsAttacked(Pet *attackingPet, PetAction petAction, float stunDuration, SKILL skillUsed)
{
    petIsAttacked(attackingPet, petAction, stunDuration, skillUsed, isCritical());
}

void Pet::petIsAttacked(Pet *attackingPet, PetAction petAction, float stunDuration, SKILL skillUsed, float critical)
{
    if (m_currPetAction == PET_FALL_A
        || m_currPetAction == PET_FALL_B
        || m_currPetAction == PET_SLIDE
        || m_currPetAction == PET_TOUCH
        || PET_IS_JUMPING) {
        return;
    }
    
    if (critical > 1.0f) {
        stunDuration *= critical;
    }
    
//    CCLog_("petIsAttacked!");
    
    isoMapRacing->petIsAttacked(attackingPet, this, stunDuration, skillUsed);
    
    if (skillUsed == SKILL_TRAP) {
        m_recoveryTime = RECOVERY_TIME_BY_TRAP;
    }
    
    //play effect fall by skill speed
    g_gameAudio->playEffect(AUDIO_SKILL_HIT);
    
    m_isAttacked = true;
    m_beforeIsAttackedSpeed = m_currSpeed;
    
    //force random falling animation
    if (petAction == PET_FALL_A
        || petAction == PET_FALL_B) {
        if (arc4random()%2==0) {
            petAction = PET_FALL_A;
        }else {
            petAction = PET_FALL_B;
        }
    }
    
    setAnimation(petAction);
    
//    if (m_hasObstacleOnNextTrack) {
//        m_needStopOnNextTrack = 1;
//    }else {
//        m_needSlideOnNextTrack = 0;
//    }
    
    if (petAction == PET_SLIDE) {
        this->runAction(CCSequence::create(CCDelayTime::create(this->petSpine->states[0]->animation->duration*0.3f),
                                           CCCallFunc::create(this, callfunc_selector(Pet::smokeFallingAnimation)),
                                           NULL));
    } else {
        this->runAction(CCSequence::create(CCDelayTime::create(this->petSpine->states[0]->animation->duration*0.9f),
                                           CCCallFunc::create(this, callfunc_selector(Pet::smokeFallingAnimation)),
                                           NULL));
    }
    
    m_stunnedTime = stunDuration;
    
//    CCDelayTime *delayTime = CCDelayTime::create(stunDuration);
//    CCCallFunc *callFunction = CCCallFunc::create(this, callfunc_selector(Pet::petIsAttackedRecovered));
//    this->runAction(CCSequence::create(delayTime, callFunction, NULL));
}

void Pet::petIsAttackedRecovered(CCObject *sender)
{
    m_isAttacked = false;
    
    if (m_isFinishedRunning) return;
    
    setAnimation(PET_RUN);
    
    setDefaultSpeed(sender);
}

void Pet::petIsAttackedWithEffect(Pet *attackingPet, PetAction petAction, float stunDuration, SKILL skillUsed)
{
    petIsAttackedWithEffect(attackingPet, petAction, stunDuration, skillUsed, isCritical());
}

void Pet::petIsAttackedWithEffect(Pet *attackingPet, PetAction petAction, float stunDuration, SKILL skillUsed, float critical)
{
    if (m_currPetAction == PET_FALL_A
        || m_currPetAction == PET_FALL_B
        || m_currPetAction == PET_TOUCH
        || m_currPetAction == PET_SLIDE
        || PET_IS_JUMPING) {
        return;
    }
    
    CCSkeletonAnimation *spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "hit_by_shockwave");
    spine->setAnchorPoint(ccp(0.5f, 0.5f));
    spine->setScale(sizeMultiplier);
    
//    CCPoint point = m_currPos;
//    spine->setPosition(point);
    
    spine->setPosition(this->getPosition());
//    this->getParent()->addChild(spine, this->getZOrder() + 1);
    isoMapRacing->groupSpineEffectsAboveAnimals()->addChild(spine);
    spine->setAnimation("idle", false);
    
    spine->runAction(CCSequence::create(CCDelayTime::create(spine->states[0]->animation->duration),
                                        CCRemoveSelf::create(),
                                        NULL));
    if (critical > 1.0f) {
        showCritical();
    }
    
    petIsAttacked(attackingPet, petAction, stunDuration, skillUsed, critical);
}

bool Pet::canAttack()
{
    return (!m_isFinishedRunning
            && m_currPetAction != PET_JUMP
            && m_currPetAction != PET_FALL_A
            && m_currPetAction != PET_FALL_B
            && m_currPetAction != PET_SLIDE
            && m_currPetAction != PET_FLY
            && !m_usingSkill
            && !m_usingDash
            && !m_usingStomp
            && !m_usingShockwaveStraight1
            && !m_usingShockwaveStraight2
            && m_recoveryTime <= 0.0f);
}

bool Pet::canBeAttacked()
{
    return (!m_isFinishedRunning
            && m_currPetAction != PET_JUMP
            && m_currPetAction != PET_FALL_A
            && m_currPetAction != PET_FALL_B
            && m_currPetAction != PET_SLIDE
            && m_currPetAction != PET_FLY
            && !m_usingStomp
            && m_recoveryTime <= 0.0f);
}

void Pet::smokeFallingAnimation()
{
    g_gameAudio->playEffect(AUDIO_HIT_FALL);
    
    CCSkeletonAnimation *spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "smoke_falling");
    spine->setAnchorPoint(ccp(0.5f, 0.5f));
    spine->setScale(sizeMultiplier*0.7f);
    
//    CCPoint point = m_currPos;
//    
//    spine->setPosition(point);
//    this->addChild(spine, -1);
    m_groupSpineEffectsBelowPet->addChild(spine);
    spine->setAnimation("idle", false);
    
    spine->runAction(CCSequence::create(CCDelayTime::create(spine->states[0]->animation->duration),
                                        CCRemoveSelf::create(),
                                        NULL));
}

#pragma mark - Energy Cost

int Pet::startSkillCost()
{
#ifdef DEBUG_INFINITEENERGY
    return DEBUG_INFINITEENERGY;
#endif
    int e = (int)(m_stat_energy * g_gameContent->GameConfig["ENERGY_STAT_TO_STARTING_ENERGY_RATIO"]);
    e += g_gameContent->GameConfig["ADDITIONAL_ENERGY"];
#ifdef DEBUG_ADDITIONALENERGY
    e += DEBUG_ADDITIONALENERGY;
#endif
    return e;
}

void Pet::regenCost()
{
    int e = (int)(m_stat_energy * g_gameContent->GameConfig["ENERGY_STAT_TO_ENERGY_PER_LAP_RATIO"]);
    e += g_gameContent->GameConfig["ADDITIONAL_ENERGY"];
#ifdef DEBUG_ADDITIONALENERGY
    e += DEBUG_ADDITIONALENERGY;
#endif
    m_allCost += e;
}

void Pet::decreaseCost(int cost)
{
    m_usedCost += cost;
    
    if (isBot) {
        showEnergyInfo(ccc3(245, 90, 32));
    }
}

unsigned int Pet::remainingCost()
{
    return m_allCost - m_usedCost;
}

void Pet::updateCostForAllSkills()
{
    //determine cost at the current timeUsed
    
    float scale = 1.0f;
    float scaleForDash = 1.0f;
    
    //HANDICAP
    if (isBot) {
        float scaleForBot = (1.0f-isoMapRacing->m_AI_on_opponent->getNewDifficultyScale(0.0f)) * BOT_MAX_SKILL_COST_MULT_HANDICAP;
        scaleForDash = (scale + (scaleForBot * 0.5f)); //affects dash just 50%
        scale = scale + scaleForBot;
    }
    
    m_dashCost = (int)(scaleForDash * (m_dashStartingCost + ((m_dashIncreasingCost + (m_dashAddingIncreasingCost*(m_dashTimeUsed-1))) * m_dashTimeUsed)));
    
    m_bananaTrapCost = (int)(scale * (m_bananaTrapStartingCost + ((m_bananaTrapIncreasingCost + (m_bananaTrapAddingIncreasingCost*(m_bananaTrapTimeUsed-1))) * m_bananaTrapTimeUsed)));
    
    m_stompCost = (int)(scale * (m_stompStartingCost + ((m_stompIncreasingCost + (m_stompAddingIncreasingCost*(m_stompTimeUsed-1))) * m_stompTimeUsed)));
    
    m_shockwave1Cost = (int)(scale * (m_shockwave1StartingCost + ((m_shockwave1IncreasingCost + (m_shockwave1AddingIncreasingCost*(m_shockwave1TimeUsed-1))) * m_shockwave1TimeUsed)));
    
    m_shockwave2Cost = (int)(scale * (m_shockwave2StartingCost + ((m_shockwave2IncreasingCost + (m_shockwave2AddingIncreasingCost*(m_shockwave2TimeUsed-1))) * m_shockwave2TimeUsed)));
    
    if (isHelperPet) {
        m_dashCost = 0;
        m_bananaTrapCost = 0;
        m_stompCost = 0;
        m_shockwave1Cost = 0;
        m_shockwave2Cost = 0;
    }
}

void Pet::showEnergyInfo()
{
    showEnergyInfo(remainingCost(), ccc3(255, 255, 255));
}

void Pet::showEnergyInfo(const cocos2d::ccColor3B &color)
{
    showEnergyInfo(remainingCost(), color);
}

void Pet::showEnergyInfo(int energy, const cocos2d::ccColor3B &color)
{
    if (g_sceneManager->sceneSelector != SCENE_RACE) return;
    if (isTutorialMode()) return;
    
    CREATE_CHAR_BUFFER(_buffer, 32);
    
    if (energy < 0) {
        snprintf(_buffer, _buffer_size, "-%d", energy*-1);
    } else {
        snprintf(_buffer, _buffer_size, "%d", energy);
    }
    
    if (m_critLabel->numberOfRunningActions() > 0) {
        m_critLabel->stopAllActions();
        m_critLabel->setVisible(false);
        m_critLabel->setScale(0.0f);
    }
    
    if (m_label_energy_info) {
        m_bgEnergy->setColor(color);
        m_label_energy_info->setString(_buffer);
        m_groupEnergy->setScale(0.5f * GROUP_ENERGY_ORIGINAL_SCALE);
        m_groupEnergy->setVisible(true);
        m_groupEnergy->stopAllActions();
        m_groupEnergy->
        runAction(CCSequence::create(CCEaseBackOut::create(CCScaleTo::create(0.1f, 1.0f * GROUP_ENERGY_ORIGINAL_SCALE)),
                                     CCDelayTime::create(1.0f),
                                     CCScaleTo::create(0.1f, 0.0f),
                                     CCHide::create(),
                                     NULL));
    }
}

void Pet::showCritical()
{
    if (g_sceneManager->sceneSelector != SCENE_RACE) return;
    
    if (m_groupEnergy->numberOfRunningActions() > 0) {
        m_groupEnergy->stopAllActions();
        m_groupEnergy->setVisible(false);
        m_groupEnergy->setScale(0.0f);
    }
    
    m_critLabel->setScale(0.5f);
    m_critLabel->setVisible(true);
    m_critLabel->stopAllActions();
    m_critLabel->runAction(CCSequence::create(CCEaseBackOut::create(CCScaleTo::create(0.1f, 1.0f)),
                                              CCDelayTime::create(0.75f),
                                              CCScaleTo::create(0.1f, 0.0f),
                                              CCHide::create(),
                                              NULL));
}

#pragma mark - Can Use Skill?

bool Pet::canDash()
{
    return remainingCost() >= m_dashCost;
}

bool Pet::canTrap()
{
    return remainingCost() >= m_bananaTrapCost;
}

bool Pet::canStomp()
{
    return remainingCost() >= m_stompCost;
}

bool Pet::canShockwave1()
{
    return remainingCost() >= m_shockwave1Cost;
}

bool Pet::canShockwave2()
{
    return remainingCost() >= m_shockwave2Cost;
}

#pragma mark - Use Skills

bool Pet::useDash()
{
    if (m_isFinishedRunning || m_usingDash || m_usingSkill || m_currPetAction == PET_JUMP || m_currPetAction == PET_FALL_A || m_currPetAction == PET_FALL_B)
        return false;
    
    updateCostForAllSkills();
    if (!canDash())
        return false;
    
    Ability *ability = playerState->getAbilityForKey(kAbilityDashForward);
    
    if (!ability)
        ability = createAbility(kAbilityDashForward);
    
    m_usingSkill = true;
    m_usingDash = true;
    
    isoMapRacing->skillIsUsed(this, SKILL_DASH);
    
    decreaseCost(m_dashCost);
    m_dashTimeUsed++;
    updateCostForAllSkills();
    
    m_beforeUsingSkillSpeed = m_currSpeed;
    
    CCLog_("m_currentEffectDashFrame startDash:%d",m_currentEffectDashFrame);
    
    this->stopActionByTag(POWERUP_1_TAG);
    
    float time = ability->aVo.param_2;
    float distance = ability->aVo.param_1;
    
    if (time <= 0) {
        time = 0.1f;
    }
    
    m_dashSpeed = distance / time;
    m_dashTime = time;
    
    setAnimation(PET_FLY);
    
    m_currentEffectDashFrame = EFFECT_DASH_MIN_FRAME;
//    m_effectDashTimePerFrame = m_dashTime / EFFECT_DASH_MAX_FRAME;
//    m_currentDashTimeFrame = 0.0f;
    m_effectDashAnimationFlipper = 1;
    
    g_gameAudio->playEffect(AUDIO_SKILL_SPEED);
    
    CCLog_("time : %f",time);
    CCLog_("useDash m_dashSpeed : %f",m_dashSpeed);
    CCSequence *sequence = CCSequence::create(CCDelayTime::create(time),
                                              CCCallFunc::create(this, callfunc_selector(Pet::stopDashing)),
                                              NULL);
    
    sequence->setTag(POWERUP_1_TAG);
    this->runAction(sequence);

    if(!isPlaybackReplayDataEnabled())
        m_replayData.addData(REPLAY_STATE_SKILL, SKILL_DASH, running_time);

    return true;
}

void Pet::startDashEffect()
{
    m_usingDash = true;
    Ability *ability = playerState->getAbilityForKey(kAbilityDashForward);
    if(!ability)
        ability = createAbility(kAbilityDashForward);

    float time = ability->aVo.param_2;

    g_gameAudio->playEffect(AUDIO_SKILL_SPEED);

    CCSequence *sequence = CCSequence::create(CCDelayTime::create(time),
            CCCallFunc::create(this, callfunc_selector(Pet::stopDashEffect)),
            NULL);
    this->runAction(sequence);

    m_currentEffectDashFrame = EFFECT_DASH_MIN_FRAME;
    m_effectDashAnimationFlipper = 1;

    m_effectDash->setVisible(true);
}

void Pet::updateDashEffect()
{
    CREATE_CHAR_BUFFER(_buf_dash, 32);
    snprintf(_buf_dash, _buf_dash_size, EFFECT_DASH_FILE_FORMAT, m_currentEffectDashFrame);
    m_effectDash->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(_buf_dash));

    m_currentEffectDashFrame += m_effectDashAnimationFlipper;
    if (m_currentEffectDashFrame <= EFFECT_DASH_MIN_FRAME || m_currentEffectDashFrame >= EFFECT_DASH_MAX_FRAME)
        m_effectDashAnimationFlipper *= -1;

    flipSpriteDirection();
}

void Pet::stopDashEffect()
{
    m_usingDash = false;
    m_effectDash->setVisible(false);
}

void Pet::stopDashing()
{
    m_canAttack = false;
    
    if (m_isAttacked) return;
    
    setAnimation(PET_RUN);
}

bool Pet::useTrap()
{
    if (m_usingSkill
        || m_isFinishedRunning
        || m_currPetAction == PET_FALL_B
        || m_currPetAction == PET_FALL_A
        || m_currPetAction == PET_SLIDE
        || PET_IS_JUMPING
        || m_currTrack->prevTrack->trapReserved()) return false;
    
    updateCostForAllSkills();
    if (!canTrap()) return false;
    
    isoMapRacing->skillIsUsed(this, SKILL_TRAP);
    
    decreaseCost(m_bananaTrapCost);
    m_bananaTrapTimeUsed++;
    updateCostForAllSkills();
    
    spawnTrap();

    if(!isPlaybackReplayDataEnabled())
        m_replayData.addData(REPLAY_STATE_SKILL, SKILL_TRAP, running_time);

    return true;
}

void Pet::spawnTrap()
{
    g_gameAudio->playEffect(AUDIO_SKILL_BANANA);
    m_currTrack->prevTrack->reserveTrap(true);
    BananaTrap::create(this);
}

bool Pet::useStomp()
{
    if (m_usingStomp ||
        m_usingSkill ||
        m_isFinishedRunning ||
        m_currPetAction == PET_JUMP ||
        m_currPetAction == PET_FALL_A ||
        m_currPetAction == PET_FALL_B ||
        m_currPetAction == PET_SLIDE)
        return false;
    
    updateCostForAllSkills();
    if (!canStomp())
        return false;
    
    isoMapRacing->skillIsUsed(this, SKILL_STOMP);

    decreaseCost(m_stompCost);
    m_stompTimeUsed++;
    updateCostForAllSkills();
    
    m_beforeUsingSkillSpeed = m_currSpeed;

    m_usingStomp = true;
    m_usingSkill = true;
    
    g_gameAudio->playEffect(AUDIO_RACE_JUMP);

    setAnimation(PET_STOMP);
    effectStomp();

    if(!isPlaybackReplayDataEnabled())
        m_replayData.addData(REPLAY_STATE_SKILL, SKILL_STOMP, running_time);

    return true;
}


void Pet::effectStomp()
{
    Ability *_ability = playerState->getAbilityForKey(kAbilityStomp);
    if(!_ability)
        _ability = createAbility(kAbilityStomp);

    m_stompTime = _ability->aVo.param_1;

    if(!isPlaybackReplayDataEnabled())
    {
        m_stompSpeed = m_currSpeed * _ability->aVo.param_2;
        float duration = petSpine->states[petSpine->states.size() - 1]->animation->duration;
        float jumpingDuration = duration * 0.31f; //0.31 = percentage of the exact frame that we want
        float desiredDuration = m_stompTime;
        float timeScale = 1.0f / (desiredDuration / jumpingDuration);
        petSpine->timeScale = timeScale;
        if (m_isPaused) m_beforePausedTimeScale = timeScale;
    }

    this->runAction(CCSequence::create(CCDelayTime::create(m_stompTime),
            CCCallFunc::create(this, callfunc_selector(Pet::playEffectStomp)),
            NULL));
}

void Pet::playEffectStomp()
{
    g_gameAudio->playEffect(AUDIO_SKILL_STOMP);
    
    if (m_stompSpine == NULL) {
        m_stompSpine = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "vfx_stomp");
        this->addChild(m_stompSpine, 0);
        m_stompSpine->setScale(sizeMultiplier*SHOCKWAVE_EFFECT_SCALE);
    }
    
    m_stompSpine->setVisible(true);
    
    m_stompSpine->clearAnimation();
    m_stompSpine->setAnimation("idle", false);

    float spineAnimationDuration = m_stompSpine->states[0]->animation->duration;
    
    //stomp attack should happens at the 4th frame out of total 20 frames, which is 4/20 = 0.2
    m_stompCollideTime = spineAnimationDuration * 0.2f;
    stompCollide();
    useStompFinished();
    
    m_stompSpine->runAction(CCSequence::create(//CCCallFunc::create(this, callfunc_selector(Pet::useStompFinished)),
                                        CCDelayTime::create(spineAnimationDuration),// * (1.0f-actualStompFrame)),
                                        CCHide::create(),
                                        NULL));
}

void Pet::stompCollide()
{
    if (!opponentPet->canBeAttacked()) return;
    
    CCPoint stompPosition = this->getPosition() + STOMP_Y_OFFSET;
    CCPoint opponentPosition = opponentPet->getPosition();
    
    float stompRange = ccpLength(g_gameContent->tileSize * STOMP_AREA_RATIO_TO_TILESIZE);
    float angle = ccpAngle(stompPosition, opponentPosition);
    CCPoint stompEdge = stompPosition + ccp(cosf(angle) * stompRange, sinf(angle) * stompRange * STOMP_CIRCLE_Y_LENGTH_PETCENT);
    
    float distanceFromStompToEdge = ccpDistance(stompPosition, stompEdge);
    float distanceFromStompToOppo = ccpDistance(stompPosition, opponentPosition);
    
    //TODO: checking banana collision is still circle collision detection not oval shape
    float area = ccpLength(g_gameContent->tileSize * STOMP_AREA_RATIO_TO_TILESIZE);
    
    for (vector<Track*>::iterator it=g_opponentState->Tracks.begin(); it!=g_opponentState->Tracks.end(); ++it) {
        if (Track* _track = (Track*)(*it)) {
            if (_track->hasTrap()) {
                float distanceBetweenTrack = ccpDistance(this->getPosition(), _track->getPosition());
                if (distanceBetweenTrack <= area) {
                    _track->removeBananaTrap();
                }
            }
        }
    }
    
    if (distanceFromStompToOppo <= distanceFromStompToEdge)
    {
        Ability* _ability = playerState->getAbilityForKey(kAbilityStomp);
        
        if (_ability) {
            
            if (isHelperPet) {
                if (m_helperStunDuration >= 0.0f &&
                    m_helperStunAmount >= 0)
                {
                    opponentPet->petIsAttackedWithEffect(this, PET_FALL_A, m_helperStunDuration, SKILL_STOMP);
                }
            }
            else
            {
                opponentPet->petIsAttackedWithEffect(this, PET_FALL_A, _ability->aVo.stun_duration, SKILL_STOMP);
            }
        }
    }
}

void Pet::useStompFinished()
{
    setAnimation(PET_RUN);
    
    CCNikScrollView *scrollView = (CCNikScrollView*)this->getParent()->getParent()->getParent();
    
    CCPoint point = scrollView->getPosition();
    
    //float oneFrame = 0.03f;
    
    CCSequence *screenShakeSequence = CCSequence::create(CCMoveTo::create(0.03f, point + ccp(0,-30)),
                                                         CCMoveTo::create(0.05f, point + ccp(0,15)),
                                                         CCMoveTo::create(0.07f, point + ccp(0,-7.5)),
                                                         CCMoveTo::create(0.010f, point + ccp(0,3.75)),
                                                         CCMoveTo::create(0.014f, point + ccp(0,0)),
                                                         NULL);
    
    scrollView->runAction(screenShakeSequence);
}

bool Pet::useShockwaveStraight1()
{
    if (m_usingShockwaveStraight1
        || m_usingShockwaveStraight2
        || m_usingSkill
        || m_isFinishedRunning
        || m_currPetAction == PET_JUMP
        || m_currPetAction == PET_FALL_A
        || m_currPetAction == PET_FALL_B
        || m_currPetAction == PET_SLIDE)
        return false;
    
    updateCostForAllSkills();
    if (!canShockwave1())
        return false;
    
    Ability* _ability = playerState->getAbilityForKey(kAbilityShootForward);
    if (!_ability)
        return false;
    
    isoMapRacing->skillIsUsed(this, SKILL_SHOCKWAVE1);
    
    m_usingShockwaveStraight1 = true;
    m_usingSkill = true;
    
    decreaseCost(m_shockwave1Cost);
    m_shockwave1TimeUsed++;
    updateCostForAllSkills();
    
    m_beforeUsingSkillSpeed = m_currSpeed;
    
    g_gameAudio->playEffect(AUDIO_SKILL_WAVE);

    m_shockwave1Speed = POWERUP_4_STAGE1_SPEED;
    petSpine->timeScale = 1;
    setAnimation(PET_READY);
    
    m_shockwave1Time = _ability->aVo.param_1 + _ability->aVo.param_2;
    
    this->runAction(CCSequence::create(CCDelayTime::create(_ability->aVo.param_1),
                                       CCCallFunc::create(this, callfunc_selector(Pet::shockwaveStraight1Stage2)),
                                       NULL));

    if(!isPlaybackReplayDataEnabled())
        m_replayData.addData(REPLAY_STATE_SKILL, SKILL_SHOCKWAVE1, running_time);

    return true;
}

void Pet::spawnShockwave1()
{
    Ability* _ability = playerState->getAbilityForKey(kAbilityShootForward);

    if(!_ability)
        _ability = createAbility(kAbilityShootForward);

    m_shockwave1Speed = POWERUP_4_STAGE1_SPEED;
    petSpine->timeScale = 1;
    setAnimation(PET_READY);

    m_shockwave1Time = _ability->aVo.param_1 + _ability->aVo.param_2;

    this->runAction(CCSequence::create(CCDelayTime::create(_ability->aVo.param_1),
            CCCallFunc::create(this, callfunc_selector(Pet::shockwaveStraight1Stage2)),
            NULL));
}

void Pet::spawnShockwave2()
{
    Ability* _ability = playerState->getAbilityForKey(kAbilityDoubleShoot);

    if(!_ability)
        _ability = createAbility(kAbilityDoubleShoot);

    m_shockwave2Speed = POWERUP_4_STAGE1_SPEED;
    petSpine->timeScale = 1;
    setAnimation(PET_READY);

    m_shockwave2Time = _ability->aVo.param_1 + _ability->aVo.param_2;

    this->runAction(CCSequence::create(CCDelayTime::create(_ability->aVo.param_1),
            CCCallFunc::create(this, callfunc_selector(Pet::shockwaveStraight2Stage2)),
            NULL));
}


void Pet::shockwaveStraight1Stage2()
{
    if (m_currPetAction == PET_FALL_A ||
        m_currPetAction == PET_FALL_B ||
        m_currPetAction == PET_SLIDE)
    {
//        m_shockwave1Time = 0.0f;
//        m_usingSkill = false;
        return;
    }
    
    ShockwaveStraight::create(this, this->opponentPet, ShockWaveTypeNormalSide);
    
    g_gameAudio->playEffect(AUDIO_SKILL_WAVE_FLY);
    Ability* _ability = playerState->getAbilityForKey(kAbilityShootForward);
    
    if(!_ability)
        _ability = createAbility(kAbilityShootForward);

    this->runAction(CCSequence::create(CCDelayTime::create(_ability->aVo.param_2),
                                       CCCallFunc::create(this, callfunc_selector(Pet::shockwaveStraight1Stage3)),
                                       NULL));
}

void Pet::shockwaveStraight1Stage3()
{
//    m_usingShockwaveStraight1 = false;
//    m_usingSkill = false;
    
    if (m_isAttacked) return;
    
    setAnimation(PET_RUN);
//    m_currSpeed = m_beforeUsingSkillSpeed;
}

bool Pet::useShockwaveStraight2()
{
    if (m_usingShockwaveStraight1
        || m_usingShockwaveStraight2
        || m_usingSkill
        || m_isFinishedRunning
        || m_currPetAction == PET_JUMP
        || m_currPetAction == PET_FALL_A
        || m_currPetAction == PET_FALL_B
        || m_currPetAction == PET_SLIDE)
        return false;
    
    updateCostForAllSkills();
    if (!canShockwave2())
        return false;
    
    Ability* _ability = playerState->getAbilityForKey(kAbilityDoubleShoot);
    
    if(!_ability)
        return false;
    
    isoMapRacing->skillIsUsed(this, SKILL_SHOCKWAVE2);
    
    m_usingShockwaveStraight2 = true;
    m_usingSkill = true;
    
    decreaseCost(m_shockwave2Cost);
    m_shockwave2TimeUsed++;
    updateCostForAllSkills();
    
    m_beforeUsingSkillSpeed = m_currSpeed;
    
    g_gameAudio->playEffect(AUDIO_SKILL_WAVE);
    
    m_shockwave2Speed = POWERUP_4_STAGE1_SPEED;
    petSpine->timeScale = 1;
    setAnimation(PET_READY);
    
    m_shockwave2Time = _ability->aVo.param_1 + _ability->aVo.param_2;
    
    this->runAction(CCSequence::create(CCDelayTime::create(_ability->aVo.param_1),
                                       CCCallFunc::create(this, callfunc_selector(Pet::shockwaveStraight2Stage2)),
                                       NULL));

    if(!isPlaybackReplayDataEnabled())
        m_replayData.addData(REPLAY_STATE_SKILL, SKILL_SHOCKWAVE2, running_time);

    return true;
}

void Pet::shockwaveStraight2Stage2()
{
    if (m_currPetAction == PET_FALL_A ||
        m_currPetAction == PET_FALL_B ||
        m_currPetAction == PET_SLIDE)
    {
//        m_shockwave2Time = 0.0f;
//        m_usingSkill = false;
        return;
    }
    
    ShockwaveStraight::create(this, this->opponentPet, ShockWaveTypeLeftSide);
    ShockwaveStraight::create(this, this->opponentPet, ShockWaveTypeRightSide);
    
    g_gameAudio->playEffect(AUDIO_SKILL_WAVE_FLY);
    Ability* _ability = playerState->getAbilityForKey(kAbilityDoubleShoot);
    
    if(!_ability)
        _ability = createAbility(kAbilityDoubleShoot);

    this->runAction(CCSequence::create(CCDelayTime::create(_ability->aVo.param_2),
                                       CCCallFunc::create(this, callfunc_selector(Pet::shockwaveStraight2Stage3)),
                                       NULL));
}

void Pet::shockwaveStraight2Stage3()
{
//    m_usingShockwaveStraight2 = false;
//    m_usingSkill = false;
    
    if (m_isAttacked) return;
    
    setAnimation(PET_RUN);
    
//    m_currSpeed = m_beforeUsingSkillSpeed;
}

#pragma mark - Clean up

void Pet::cleanEffect() {
    
    cleanEffectLevelUp();
}

void Pet::cleanEffectLevelUp()
{
    if (m_levelUp != NULL) {
        m_levelUp->removeFromParentAndCleanup(true);
        m_levelUp = NULL;
    }
}

#pragma mark - Callback

void Pet::onPetFed(void *sender, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseChecklistData(_root);
    _playerState->parsePetData(_root);
    _playerState->parseUserBalance(_root);
    Json_dispose(_root);
    
    if (typeid(*g_currentScene) != typeid(SceneVille)) return;
    SceneVille* _scene = (SceneVille*)g_currentScene;
    
    _scene->hudPanel->updateResourceBar(HUDPanelBarFoods);

}
