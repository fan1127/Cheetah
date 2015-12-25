//
//  BananaTrap.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/22/2557 BE.
//
//

#include "BananaTrap.h"

#include "../Entities/Pet.h"
#include "../Entities/Track.h"

#include "../Entities/IsoMapRacing.h"

#include "../Entities/PlayerState.h"

#include "../Services/GameContentService.h"

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

extern GameContentService *g_gameContent;

#include "AppMacros.h"
extern TargetDevice targetDevice;

BananaTrap *BananaTrap::create(Pet *usingSkillPet)
{
    BananaTrap* pRet = new BananaTrap();
    pRet->init(usingSkillPet);
    pRet->autorelease();
    return pRet;
}

bool BananaTrap::init(Pet *usingSkillPet) {
    
    if (!usingSkillPet || !usingSkillPet->getParent()) {
        return false;
    }
    
    if (!CCSprite::initWithSpriteFrameName("_banana.png")) {
        return false;
    }
    
    if (targetDevice == TD_IPAD_RETINA) {
        m_sizeMultiplier = 0.5f;
    }else {
        m_sizeMultiplier = 0.7f;
    }
    
    m_addTrapDidFinish = false;
    
    m_usingSkillPet = usingSkillPet;
    m_trap = m_usingSkillPet->m_currTrack->prevTrack;
    
    float x1 = m_usingSkillPet->getPosition().x;
    float y1 = m_usingSkillPet->getPosition().y;
    
    float x2 = m_trap->getPosition().x;
    float y2 = m_trap->getPosition().y;
    
    this->setPosition(ccp(x1, y1) + ccp(0, g_gameContent->tileSize.height*0.5f));
    this->setScale(m_sizeMultiplier*0.6f);

    m_startY = this->getPositionY();
    m_endY = y2;

    m_usingSkillPet->isoMapRacing->effectsArray()->addObject(this);
    m_usingSkillPet->isoMapRacing->addChild(this);

    float trapDelayTime = 1.f;
    if(m_usingSkillPet->isoMapRacing->isTutorialMode())
    {
        Pet* playerPet = m_usingSkillPet->isoMapRacing->m_pet_player;
        Pet* opponentPet = m_usingSkillPet->isoMapRacing->m_pet_opponent;
        int distance = playerPet->m_summaryTrack - opponentPet->m_summaryTrack;
        if((playerPet == m_usingSkillPet) && (distance >= 0 && distance <= 6))
            trapDelayTime = 0.2f;
    }

    float timeScale = 1.f / m_usingSkillPet->isoMapRacing->getTimeScale();
    m_inAirDelayTime = trapDelayTime * timeScale;
    m_timeInAir = 0.f;

    if (x2 - x1 >= 0) {
        this->runAction(CCRotateBy::create(trapDelayTime * timeScale, 360));
    } else {
        this->runAction(CCRotateBy::create(trapDelayTime * timeScale, -360));
    }

    this->runAction(CCSequence::create(CCScaleTo::create(trapDelayTime * timeScale, m_sizeMultiplier*0.9f),
                                       CCScaleTo::create(0.1f * timeScale, m_sizeMultiplier*0.8f),
                                       NULL));
    this->runAction(CCSequence::create(CCJumpTo::create(m_inAirDelayTime, ccp(x2, y2), g_gameContent->tileSize.height, 1),
                                       CCCallFunc::create(this, callfunc_selector(BananaTrap::addTrapFinished)),
                                       NULL));
    
    scheduleUpdate();
    
    return true;
}

void BananaTrap::update(float delta)
{
    delta *= m_usingSkillPet->isoMapRacing->getTimeScale();

    m_trapActivationTime += delta;
    if (m_trapActivationTime > DEFAULT_TRAP_ACTIVATION_TIME) m_trapActivationTime = DEFAULT_TRAP_ACTIVATION_TIME;
    
    if (m_addTrapDidFinish) {
        Pet *pet1 = m_usingSkillPet;
        Pet *pet2 = m_usingSkillPet->opponentPet;
        
        
        CCRect rectPet1 = CCRectZero;
        rectPet1.size = g_gameContent->tileSize;
        rectPet1.origin = pet1->m_currPos - rectPet1.size*0.5f;
        
        CCRect rectPet2 = CCRectZero;
        rectPet2.size = g_gameContent->tileSize;
        rectPet2.origin = pet2->m_currPos - rectPet1.size*0.5f;

        if(pet1->isPlaybackReplayDataEnabled())
        {
            CCAssert(pet2->isPlaybackReplayDataEnabled(), "only one animal is in replay mode?");

            if ((pet1->isFalling() && rectPet1.containsPoint(this->getPosition())) ||
                (pet2->isFalling() && rectPet2.containsPoint(this->getPosition())))
            {
                removeTrap();
                g_gameAudio->playEffect(AUDIO_SLIP_BANANA);
            }
            return;
        }

        if (rectPet1.containsPoint(this->getPosition())
            && pet1->canBeAttacked())
        {
            if (m_usingSkillPet->playerState->hasAbilityForKey(kAbilityBananaTrap))
            {
                m_trapActivationTime -= (delta * 2.0f);
                
                if (!pet1->usingDash()
                    && m_trapActivationTime <= 0.0f)
                {
                    Ability* _ability = m_usingSkillPet->playerState->getAbilityForKey(kAbilityBananaTrap);
                    pet1->petIsAttacked(m_usingSkillPet, PET_FALL_A, _ability->aVo.stun_duration, SKILL_TRAP, 0.0f); //forces no critical for banana trap
                    removeTrap();
                    
                    g_gameAudio->playEffect(AUDIO_SLIP_BANANA);
                }
            }
        }

        if (rectPet2.containsPoint(this->getPosition())
            && pet2->canBeAttacked())
        {
            if (m_usingSkillPet->playerState->hasAbilityForKey(kAbilityBananaTrap))
            {
                m_trapActivationTime -= (delta * 2.0f);
                
                if (!pet2->usingDash()
                    && m_trapActivationTime <= 0.0f)
                {
                    Ability* _ability = m_usingSkillPet->playerState->getAbilityForKey(kAbilityBananaTrap);
                    pet2->petIsAttacked(m_usingSkillPet, PET_FALL_A, _ability->aVo.stun_duration, SKILL_TRAP, 0.0f); //forces no critical for banana trap
                    removeTrap();
                    
                    g_gameAudio->playEffect(AUDIO_SLIP_BANANA);
                }
            }
        }
        
    } else {

        m_timeInAir += delta;
        float t = m_timeInAir / m_inAirDelayTime;
        if(t > 1.0f)
            t = 1.0f;
        float newY = m_startY + (t * (m_endY - m_startY));
        this->setZOrder(static_cast<int>(roundf(m_usingSkillPet->isoMapRacing->getContentSize().height - newY)));
    }
}

void BananaTrap::addTrapFinished()
{
    m_trapActivationTime = DEFAULT_TRAP_ACTIVATION_TIME;
    
    m_addTrapDidFinish = true;
    m_trap->addTrap(this);
}


void BananaTrap::removeTrap()
{
    unscheduleUpdate();
    
    m_trap->removeTrap();
    
    this->setZOrder(m_usingSkillPet->getZOrder() + 1);
    
    float defaultScale = this->getScale();
    this->runAction(CCSequence::create(CCScaleTo::create(0.08f, defaultScale*0.8f),
                                       CCScaleTo::create(0.08f, defaultScale*1.2f),
                                       CCRemoveSelf::create(),
                                       NULL));
    
    CCSprite *fxSprite = CCSprite::create("common/vfx/fx_gacha_to_barn_b2.png");
    fxSprite->setPosition(this->getPosition());
    fxSprite->setScale(0);
//    m_usingSkillPet->getParent()->addChild(fxSprite);
    m_usingSkillPet->isoMapRacing->groupNodeEffect()->addChild(fxSprite);
    fxSprite->setZOrder(this->getZOrder() - 1);
    
    fxSprite->runAction(CCSequence::create(CCDelayTime::create(0.1f),
                                           CCScaleTo::create(0.2f, m_sizeMultiplier*0.5f),
                                           NULL));
    
    fxSprite->runAction(CCSequence::create(CCDelayTime::create(0.2f),
                                           CCFadeOut::create(0.2f),
                                           CCRemoveSelf::create(),
                                           NULL));
    
    for (int i = 0; i < 82; i++) {
        CCSprite *sprite = CCSprite::create("common/vfx/banana_particle1.png");
        sprite->setPosition(this->getPosition() + ccp(0, -this->getContentSize().height*0.25f));
        float scale = (1 - (arc4random()%47)*0.01)*m_sizeMultiplier*0.1f;
//        CCLog_("%f",scale);
        sprite->setScale(scale);
//        m_usingSkillPet->getParent()->addChild(sprite);
        m_usingSkillPet->isoMapRacing->groupNodeEffect()->addChild(sprite);
        sprite->setZOrder(this->getZOrder() + 1);
        
        CCPoint point = ccp(g_gameContent->tileSize.width*(arc4random()%75*0.01)*(arc4random()%2 ? 1 : -1), g_gameContent->tileSize.height*(arc4random()%75*0.01)*(arc4random()%2 ? 1 : -1));
        
        float duration = (12 + arc4random()%7)/30.f;
        
        sprite->runAction(CCSequence::create(CCDelayTime::create(0.1f),
                                             CCJumpBy::create(duration, point, g_gameContent->tileSize.height*(0.5 + (arc4random()%55*0.01)), 1),
                                             CCRemoveSelf::create(),
                                             NULL));
        
        sprite->runAction(CCSequence::create(CCDelayTime::create(0.1f),
                                             CCScaleTo::create(duration, sprite->getScale()*(4 + arc4random()%7)/10.f),
                                             NULL));
        
        
    }
}