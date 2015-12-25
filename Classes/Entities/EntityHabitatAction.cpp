//
//  EntityHabitatAction.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/12/2557 BE.
//
//

#include "EntityHabitatAction.h"
USING_NS_CC;

#include "../AppMacros.h"
extern TargetDevice targetDevice;

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../Entities/PlayerState.h"
extern PlayerState *g_playerState;

#include "../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../UIs/Dialogs/CutTreeDialog.h"
#include "../Extensions/CCMenuItemStateSprite.h"

#include "../Extensions/Modal/CCModalController.h"

#include "Decoration.h"

#define FOOD_AMOUNT_OFFSET_TO_FOOD_ICON ccp(-25.0f*sizeMultiplier, 150.0f*sizeMultiplier)

extern float sizeMultiplier;

bool EntityHabitatAction::init() {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);
    setContentSize(CCDirector::sharedDirector()->getWinSize());
    
    m_state = EHAS_STANDBY;
    m_entity = NULL;

    m_raySprite = NULL;
    m_iconFood = NULL;
    m_arrowSprite = NULL;
    
    _clear();
    
    scheduleUpdate();
    
    return true;
    
}

void EntityHabitatAction::finish() {
    
    if (!m_entity) {
        _restart();
        return;
    }
    
    if (m_iconFood != NULL) {
        CCModalController::hideAllChildrensAnimated(this, kEntityActionTransitionSpeed);
        m_iconFood->stopAllActions();
        m_iconFood->runAction(CCSequence::create(CCMoveTo::create(kEntityActionTransitionSpeed, CCPointZero),
                                                 CCCallFunc::create(this, callfunc_selector(EntityHabitatAction::_restart)),
                                                 NULL));
    }else {
        _restart();
    }
    
    m_state = EHAS_STANDBY;
    
}


void EntityHabitatAction::update(float delta_) {
    
    if (m_foodAmountUpdater == NULL)
        return;
    
    if (m_foodAmountUpdater->getLabel() == NULL)
        return;
    
    if (m_foodAmountUpdater->labelValue > g_playerState->player.balance.food) {
        
        m_foodAmountUpdater->getLabel()->setColor(kTextColorOrange2);
    } else {
        
        m_foodAmountUpdater->getLabel()->setColor(kTextColorWhite);
    }
}

void EntityHabitatAction::start(void* entity_) {
    
    m_entity = entity_;
    
    _clear();
    if (Building *_habitat = (Building*)m_entity) {
        
        m_raySprite = CCSprite::createWithSpriteFrameName("_grad_1.png");
        m_raySprite->setRotation(100);
        m_raySprite->setPosition(ccp(-(g_gameContent->tileSize.width*(_habitat->bVo.grid_col+_habitat->bVo.grid_row)*0.125f) - (m_raySprite->getContentSize().height*0.5f),
                                     (g_gameContent->tileSize.height*(_habitat->bVo.grid_col+_habitat->bVo.grid_row)*0.125f)/* + (m_raySprite->getContentSize().height*0.25f)*/));
        addChild(m_raySprite);
        
        m_iconFood = CCSprite::createWithSpriteFrameName("_icon_food_open.png");
        m_iconFood->setOpacity(255);
        m_iconFood->setPosition(CCPointZero);//_habitat->getPosition()
        addChild(m_iconFood);
        
        m_arrowSprite = CCSprite::createWithSpriteFrameName("_arrow.png");
        m_arrowSprite->setOpacity(255);
        m_arrowSprite->setRotation(-30);
        m_arrowSprite->setPosition(m_iconFood->getContentSize()*0.25f
                                   +DEFAULT_ICON_FOOD_POSITION);
        m_arrowSprite->setVisible(false);
        addChild(m_arrowSprite);
        //m_iconFood->addChild(m_arrowSprite);
        
        CCModalController::showAllChildrensAnimated(this, kEntityActionTransitionSpeed);
        m_iconFood->stopAllActions();
        CCPoint _pt = DEFAULT_ICON_FOOD_POSITION;
        CCLog_("%f,%f", _pt.x, _pt.y);
        m_iconFood->runAction(CCSequence::create(CCSpawn::create(CCMoveTo::create(kEntityActionTransitionSpeed, _pt),
                                                                 CCScaleTo::create(kEntityActionTransitionSpeed, 1.0f),
                                                                 CCFadeTo::create(kEntityActionTransitionSpeed, 255),
                                                                 NULL),
                                                 CCCallFunc::create(this, callfunc_selector(EntityHabitatAction::runFoodIconIdleAnimation)),
                                                 CCCallFunc::create(this, callfunc_selector(EntityHabitatAction::showArrowWithAnimation)),
                                                 NULL));
        
        if (_habitat->pet) {
            CREATE_CHAR_BUFFER(_buffer, 64);
            int seconds = EntityHabitatAction::getHungerFoodToFullFromPet(_habitat->pet);
            snprintf(_buffer, _buffer_size, "%d", seconds);
            CCLabelBMFont *_gemValue = CCLabelHelper::createBMFont(_buffer, kFontSizeExtraLarge + 5.0f, true);
            m_foodAmountUpdater = CCLabelUpdater::createWithLabel(_gemValue,
                                                                  _habitat->pet,
                                                                  EntityHabitatAction::getHungerFoodToFullFromPet);
            m_foodAmountUpdater->setPosition(_pt + FOOD_AMOUNT_OFFSET_TO_FOOD_ICON);
            addChild(m_foodAmountUpdater);
        }
        
        m_state = EHAS_SHOW_MENU;
     }
    
}

int EntityHabitatAction::getHungerFoodToFullFromPet(Pet *_pet)
{
    int seconds = (int)_pet->getHungerFoodToFull();
//    CCLog_("EntityHabitatAction::getHungerFoodToFullFromPet:%d", seconds);
    return seconds;
}

void EntityHabitatAction::showArrowWithAnimation() {
    
    if (isFeed) return;
    
    if (m_arrowSprite) {
        m_arrowSprite->stopAllActions();
        
        m_arrowSprite->setScale(0.5f);
        m_arrowSprite->setVisible(true);
        m_arrowSprite->runAction(CCEaseBackOut::create(CCScaleTo::create(0.1f, 1.0f)));
    }
}

void EntityHabitatAction::playFoodIconIdleAnimation() {
    
    if (m_iconFood) {
        m_iconFood->stopAllActions();
        m_iconFood->runAction(CCSequence::create(CCRotateTo::create(ICON_FOOD_IDLE_ANIM_TIME, ICON_FOOD_IDLE_ANGLE_DIFF),
                                                 CCRotateTo::create(ICON_FOOD_IDLE_ANIM_TIME, -ICON_FOOD_IDLE_ANGLE_DIFF),
                                                 CCRotateTo::create(ICON_FOOD_IDLE_ANIM_TIME, ICON_FOOD_IDLE_ANGLE_DIFF),
                                                 CCRotateTo::create(ICON_FOOD_IDLE_ANIM_TIME, -ICON_FOOD_IDLE_ANGLE_DIFF),
                                                 CCCallFunc::create(this, callfunc_selector(EntityHabitatAction::runFoodIconIdleAnimation)),
                                                 NULL));
    }
    
}

void EntityHabitatAction::runFoodIconIdleAnimation() {
    
    if (m_iconFood) {
        m_iconFood->stopAllActions();
        m_iconFood->runAction(CCRepeatForever::create(CCSequence::create(CCDelayTime::create(2.5f),
                                                                         CCRotateTo::create(ICON_FOOD_IDLE_ANIM_TIME, ICON_FOOD_IDLE_ANGLE_DIFF),
                                                                         CCRotateTo::create(ICON_FOOD_IDLE_ANIM_TIME, -ICON_FOOD_IDLE_ANGLE_DIFF),
                                                                         CCRotateTo::create(ICON_FOOD_IDLE_ANIM_TIME, ICON_FOOD_IDLE_ANGLE_DIFF),
                                                                         CCRotateTo::create(ICON_FOOD_IDLE_ANIM_TIME, -ICON_FOOD_IDLE_ANGLE_DIFF),
                                                                         NULL)));
    }
}

void EntityHabitatAction::_clear() {
    
    if (getParent()) {
        if (m_raySprite != NULL) {
            m_raySprite->stopAllActions();
            m_raySprite->removeFromParent();
        }
        m_raySprite = NULL;
        
        if (m_arrowSprite != NULL) {
            m_arrowSprite->stopAllActions();
            m_arrowSprite->removeFromParent();
        }
        m_arrowSprite = NULL;
        
        if (m_iconFood != NULL) {
            m_iconFood->stopAllActions();
            m_iconFood->removeFromParent();
        }
        m_iconFood = NULL;
        
        if (m_foodAmountUpdater != NULL) {
            m_foodAmountUpdater->stopAllActions();
            m_foodAmountUpdater->removeFromParent();
        }
        m_foodAmountUpdater = NULL;
    }
    
}

void EntityHabitatAction::_restart() {
    
    isFinishing = false;
    
    _clear();
    
}


void EntityHabitatAction::onFeed(cocos2d::CCObject *sender_){
    isFeed = true;
}

void EntityHabitatAction::cancelFeed(cocos2d::CCObject *sender_){
    isFeed = false;
}

bool EntityHabitatAction::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    
//    if (isFeed) return false;
    if (!m_iconFood) return false;
    CCPoint location = convertTouchToNodeSpace(pTouch);
    
    if (m_iconFood->boundingBox().containsPoint(location)) {
        isFeed = true;
        m_arrowSprite->setVisible(!isFeed);
        
        m_iconFood->stopAllActions();
        CCPoint targetPosition = location + ICON_FOOD_ON_TOUCH_OFFSET;
        m_iconFood->setPosition(ccp(targetPosition.x,
                                    location.y));
        m_foodAmountUpdater->setPosition(m_iconFood->getPosition() + FOOD_AMOUNT_OFFSET_TO_FOOD_ICON);
        
        m_foodAmountUpdater->runAction(CCMoveTo::create(ICON_FOOD_TOUCH_ANIM_TIME,
                                                        location
                                                        + ICON_FOOD_ON_TOUCH_OFFSET
                                                        + FOOD_AMOUNT_OFFSET_TO_FOOD_ICON));
        m_iconFood->runAction(CCMoveTo::create(ICON_FOOD_TOUCH_ANIM_TIME, location + ICON_FOOD_ON_TOUCH_OFFSET));
        
        if (EntityAction *_ea = (EntityAction*)this->getParent()) {
            if (typeid(*_ea) == typeid(EntityAction)) {
                if (Building *_building = (Building*)_ea->getEntity()) {
                    _building->hideProgressBarAnimated(kEntityActionTransitionSpeed);
                    
                    if (_building->pet) {
                        
                        _building->pet->feedBegin();
                    }
                    m_tmp = _building;
                }
                m_state = EHAS_GRASP_FOOD;
                _ea->finish(NULL, false);
            }
        }
    } else {
        if (EntityAction *_ea = (EntityAction*)this->getParent()) {
            if (typeid(*_ea) == typeid(EntityAction)) {
                Building *_building = (Building*)_ea->getEntity();
                if (_building) {
                    
                }else {
                    finish();
                }
            }
        }
    }
    
    return isFeed;
    
}

void EntityHabitatAction::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    if (m_iconFood == NULL) {
        return;
    }
    
    CCPoint location = convertTouchToNodeSpace(pTouch);
    m_iconFood->stopAllActions();
    m_iconFood->setPosition(location + ICON_FOOD_ON_TOUCH_OFFSET);
    m_foodAmountUpdater->stopAllActions();
    m_foodAmountUpdater->setPosition(location
                                     + ICON_FOOD_ON_TOUCH_OFFSET
                                     + FOOD_AMOUNT_OFFSET_TO_FOOD_ICON);
    
    if (Building *_habitat = (Building*)m_entity) {
        CCRect rect = CCRectZero;
        rect.size = CCSizeMake(_habitat->bVo.src_width, _habitat->bVo.src_height)*0.5f;
        rect.origin = CCPointZero - rect.size*0.5f;
        
        if (rect.containsPoint(location)) {
            eat();
        }
    }
    
}

void EntityHabitatAction::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {

    isFeed = false;

    if (Building *_building = (Building*)m_tmp) {
        _building->showProgressBarAnimated(kEntityActionTransitionSpeed);
        if (_building->pet) {
  
            if (_building->pet->m_food_fed > 0) {
                _building->pet->feedEnd();
                this->finish();
                m_state = EHAS_FED;
            } else {
                m_state = EHAS_REVERT;
                if (m_iconFood) {
                    m_iconFood->setPosition(DEFAULT_ICON_FOOD_POSITION);
                    m_foodAmountUpdater->setPosition(DEFAULT_ICON_FOOD_POSITION + FOOD_AMOUNT_OFFSET_TO_FOOD_ICON);
                }
                
                playFoodIconIdleAnimation();
                this->runAction(CCSequence::create(CCDelayTime::create(ICON_FOOD_IDLE_ANIM_TIME*2.0f),
                                                   CCCallFunc::create(this, callfunc_selector(EntityHabitatAction::showArrowWithAnimation)),
                                                   NULL));
            }
        }
    }
}

void EntityHabitatAction::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {


    ccTouchEnded(pTouch, pEvent);
}

void EntityHabitatAction::delayFeed() {
    
    feedingAnimated = false;
    
}

void EntityHabitatAction::eat() {
    
    if (feedingAnimated) return;
    if (Building *_habitat = (Building*)m_entity) {
        if (Pet *pet = _habitat->pet) {
            if (pet->feed()) {
                
                feedingAnimated = true;
                CCSprite *_iconFood = CCSprite::createWithSpriteFrameName("_icon_food_enable.png");
                _iconFood->setPosition(ccp(_habitat->bVo.src_width*0.1f*(arc4random()%10)*(arc4random()%2 ? 1 : -1), _habitat->bVo.src_height));
                _iconFood->setOpacity(255);
                addChild(_iconFood);
                _iconFood->runAction(CCSequence::create(CCJumpTo::create(0.4f, pet->getPosition(), 20 + arc4random()%10, 1),
                                                        CCRemoveSelf::create(),
                                                        NULL));
                _iconFood->runAction(CCScaleTo::create(0.4f, 0.3f));
                
                this->runAction(CCSequence::create(CCDelayTime::create(0.1*(arc4random()%5)),
                                                   CCCallFunc::create(this, callfunc_selector(EntityHabitatAction::delayFeed)),
                                                   NULL));
                
                if (eatingAnimating) return;
                pet->runAction(CCSequence::create(CCDelayTime::create(0.4f),
                                                  CCScaleTo::create(0.1f, 1.1f),
                                                  CCEaseElasticOut::create(CCScaleTo::create(1.0f, 1.0f)),
                                                  CCCallFunc::create(this, callfunc_selector(EntityHabitatAction::endEat)),
                                                  NULL));
                
                if (SceneVille* _scene = (SceneVille*)g_currentScene) {
                    _scene->hudPanel->resourceBars[HUDPanelBarFoods]->addTargetAmount(g_playerState->player.balance.food, g_playerState->getTotalFoodStorageCapacity(), AUDIO_NONE);
                    _scene->hudPanel->resourceBars[HUDPanelBarFoods]->processTargetAmount();
                }
                
                if (pet->m_currPetAction == PET_HABITAT_HUNGRY) {
                    pet->setAnimation(PET_IDLE_STAND);
                } else if (pet->m_currPetAction == PET_HABITAT_HUNGRY_LAID) {
                    pet->setAnimation(PET_IDLE_SLEEP_TO_STAND);
                }
                
                
            }
            
            pet->feedEnd();
            m_entity = NULL;
            finish();
        }
    }
}

void EntityHabitatAction::endEat() {
    
    eatingAnimating = false;
    
}

CCPoint EntityHabitatAction::getDefaultIconFoodPosition() {
    
    if (m_raySprite && m_iconFood) {
        return ccp(m_raySprite->getPositionX()-m_iconFood->getContentSize().width*0.6f,
                   m_raySprite->getPositionY());
    }
    return CCPointZero;
    
}
