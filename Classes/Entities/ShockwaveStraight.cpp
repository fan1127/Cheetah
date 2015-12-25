//
//  ShockwaveStraight.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/17/2557 BE.
//
//

#include "ShockwaveStraight.h"

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../Entities/PlayerState.h"
extern PlayerState *g_opponentState;

extern float sizeMultiplier;
extern float retinaMultiplier;

#include "../Entities/Pet.h"

#include "../Entities/IsoMapRacing.h"

#include "../Settings.h"

using namespace std;

ShockwaveStraight *ShockwaveStraight::create(Pet *usingSkillPet, Pet *targetPet, ShockWaveType type)
{
    ShockwaveStraight* pRet = new ShockwaveStraight();
    pRet->init(usingSkillPet, targetPet, type);
    pRet->autorelease();
    return pRet;
}

bool ShockwaveStraight::init(Pet *usingSkillPet, Pet *targetPet, ShockWaveType type)
{
    if (!CCSprite::init()) {
        return false;
    }
    
    hitTargetPet = false;
    
    m_isStarted = false;
    m_isPaused = false;
    scheduleUpdate();
    
    m_type = type;
    m_usingSkillPet = usingSkillPet;
    m_targetPet = targetPet;
    
//    m_usingSkillPet->getParent()->addChild(this, m_usingSkillPet->getZOrder());
    
    m_usingSkillPet->isoMapRacing->shockwaveArray()->addObject(this);
    m_usingSkillPet->isoMapRacing->addChild(this);
    
    startShockwaveStraight();
    
    return true;
}

////Doesn't work
//bool ccpLineIntersectCircle(const CCPoint& A, const CCPoint& B, const CCPoint& C, float r)
//{
//    
//    float slope = (B.y - A.y)/(B.x - A.x);
//    float negativeReciprocal = (1.0f/slope) * -1.0f;
//    CCPoint perpendicularNormalizedSlope = ccpNormalize(ccp(1, negativeReciprocal));
//    if (slope == 0) {
//        perpendicularNormalizedSlope = ccp(-1, 0);
//    }
//    
//    float distanceFromCenter = r;
//    
//    CCPoint pointA = perpendicularNormalizedSlope * distanceFromCenter;
//    CCPoint pointB = perpendicularNormalizedSlope * -distanceFromCenter;
//    
//    return ccpSegmentIntersect(A, B, pointA, pointB);
//}

bool ccpLineIntersectRect(const CCPoint& A,
                          const CCPoint& B,
                          const CCPoint& rectA,
                          const CCPoint& rectB,
                          const CCPoint& rectC,
                          const CCPoint& rectD)
{
    if (ccpSegmentIntersect(A, B, rectA, rectB)) return true;
    if (ccpSegmentIntersect(A, B, rectB, rectC)) return true;
    if (ccpSegmentIntersect(A, B, rectC, rectD)) return true;
    if (ccpSegmentIntersect(A, B, rectD, rectA)) return true;
    
    return false;
}

void ShockwaveStraight::pauseShockwave()
{
    m_isPaused = true;
    if (m_spine) m_spine->timeScale = 0.0f;
}

void ShockwaveStraight::resumeShockwave()
{
    m_isPaused = false;
    if (m_spine) m_spine->timeScale = 1.0f;
}

void ShockwaveStraight::update(float delta)
{
    if (!m_isStarted) return;
    if (m_isPaused) return;
    
    this->setZOrder(int(m_usingSkillPet->isoMapRacing->getContentSize().height - this->getPosition().y));
    
    CCPoint prevPos = this->getPosition();
    
    float s = m_velocity*delta;
    CCPoint currPos = this->getPosition() + ccp(s*cosf(m_angle), s*sinf(m_angle));
    
    this->setPosition(currPos);
    
    m_runningTime += delta;

//    Track *startTrack = m_usingSkillPet->m_currTrack;
    
    for (vector<Track*>::iterator it=g_opponentState->Tracks.begin(); it!=g_opponentState->Tracks.end(); ++it) {
        if (Track* _track = (Track*)(*it)) {
            if (_track->hasTrap()) {
                
                CCPoint pos = _track->getPosition();
                CCSize size = g_gameContent->tileSize;
                
                float tilePercent = SHOCKWAVE_TILE_PERCENT_FOR_BANANA;
                if (ccpLineIntersectRect(prevPos,
                                         currPos,
                                         ccp(pos.x, pos.y + (-tilePercent*size.height)),
                                         ccp(pos.x + (-tilePercent*size.width), pos.y),
                                         ccp(pos.x, pos.y + (tilePercent*size.height)),
                                         ccp(pos.x + (tilePercent*size.width), pos.y)
                                         ))
                {
                    _track->removeBananaTrap();
                }
            }
        }
    }
    
//    CCRect rect = CCRectZero;
//    rect.size = g_gameContent->tileSize;
//    rect.origin = m_targetPet->m_currPos - rect.size*0.5f;
    
    float tilePercent = SHOCKWAVE_TILE_PERCENT_FOR_PLAYER;
    CCPoint pos = m_targetPet->getPosition();
    CCSize size = g_gameContent->tileSize;
    
    if (ccpLineIntersectRect(prevPos,
                             currPos,
                             ccp(pos.x, pos.y + (-tilePercent*size.height)),
                             ccp(pos.x + (-tilePercent*size.width), pos.y),
                             ccp(pos.x, pos.y + (tilePercent*size.height)),
                             ccp(pos.x + (tilePercent*size.width), pos.y)
                             )
        && m_targetPet->canBeAttacked())
    {
        //can only hit once
        if (!hitTargetPet) {
            if (m_usingSkillPet->playerState->hasAbilityForKey(m_type == ShockWaveTypeNormalSide ? kAbilityShootForward : kAbilityDoubleShoot)
                && !(m_usingSkillPet->isHelperPet && m_targetPet->isPlayerPet))
            {
                Ability* _ability = m_usingSkillPet->playerState->getAbilityForKey(m_type == ShockWaveTypeNormalSide ? kAbilityShootForward : kAbilityDoubleShoot);
                
                hitTargetPet = true;
                
                if (m_usingSkillPet->isHelperPet) {
                    if (m_usingSkillPet->m_helperStunDuration >= 0.0f &&
                        m_usingSkillPet->m_helperStunAmount >= 0)
                    {
                        m_targetPet->petIsAttackedWithEffect(m_usingSkillPet,
                                                             PET_FALL_A,
                                                             m_usingSkillPet->m_helperStunDuration,
                                                             (m_type == ShockWaveTypeNormalSide ? SKILL_SHOCKWAVE1 : SKILL_SHOCKWAVE2));
                    }
                }
                else
                {
                    m_targetPet->petIsAttackedWithEffect(m_usingSkillPet,
                                                         PET_FALL_A,
                                                         _ability->aVo.stun_duration,
                                                         (m_type == ShockWaveTypeNormalSide ? SKILL_SHOCKWAVE1 : SKILL_SHOCKWAVE2));
                }
                
                stopShockwaveStraight();
            }
        }
    }
    
    if (m_runningTime >= SHOCKWAVE_LIFE_DURATION) {
        stopShockwaveStraight();
    }
}

void ShockwaveStraight::startShockwaveStraight()
{
    Track *startTrack = m_usingSkillPet->m_currTrack;
    Track *nextTrack = startTrack->nextTrack;
    
    this->setContentSize(g_gameContent->tileSize);
    
//    CCLayerColor *color1 = CCLayerColor::create(ccc4(200, 200, 200, 200), this->getContentSize().width, this->getContentSize().height);
//    this->addChild(color1, 1000000000);
    
    float x = nextTrack->getPositionX() - startTrack->getPositionX();
    float y = nextTrack->getPositionY() - startTrack->getPositionY();
    
    float doubleStockWaveAngle = 35.0f;
    
    if (m_type == ShockWaveTypeNormalSide) {
        m_angle = atan2f(y, x);
    } else if (m_type == ShockWaveTypeLeftSide) {
        m_angle = atan2f(y, x) + M_PI*doubleStockWaveAngle/180.f;
    } else {
        m_angle = atan2f(y, x) - M_PI*doubleStockWaveAngle/180.f;
    }
    
    CCLog_("retinaMultiplier:%f", retinaMultiplier);
    CCLog_("SHOCKWAVE_SPEED:%f", SHOCKWAVE_SPEED);
    
    double updatex = SHOCKWAVE_SPEED * cosf(m_angle);
    double updatey = SHOCKWAVE_SPEED * sinf(m_angle);
    
    m_spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "wind_spin");
    m_spine->setAnchorPoint(ccp(0.5f, 0.5f));
    m_spine->setPosition(this->getContentSize()*0.5f);
    this->addChild(m_spine, 0);
    m_spine->setAnimation("idle", false);
    
    m_spine->setScaleX(x < 0 ? sizeMultiplier : -sizeMultiplier);
    m_spine->setScaleY(y < 0 ? sizeMultiplier : -sizeMultiplier);
    
    if (m_type == ShockWaveTypeLeftSide) {
        m_spine->setRotation(-doubleStockWaveAngle);
    } else if (m_type == ShockWaveTypeRightSide) {
        m_spine->setRotation(doubleStockWaveAngle);
    }
    
    CCPoint point;

    if ((x < 0 && y > 0) || (x > 0 && y < 0)) {
        point = m_usingSkillPet->m_currPos;
    } else {
        point = m_usingSkillPet->m_currPos - this->getContentSize()*0.5f;
    }
    
    this->setPosition(point);
    
    m_velocity = ccpLength(ccp(updatex, updatey))/SHOCKWAVE_SPEED_DURATION;
    
    this->runAction(CCSequence::create(CCCallFunc::create(this, callfunc_selector(ShockwaveStraight::startUpdate)),
                                       NULL));
}

void ShockwaveStraight::startUpdate()
{
//    scheduleUpdate();
    m_isStarted = true;
}

void ShockwaveStraight::stopShockwaveStraight()
{
    unscheduleUpdate();
    this->removeFromParent();
}