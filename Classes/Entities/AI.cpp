//
//  AI.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/11/2557 BE.
//
//

#include "AI.h"

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

//#include "../Entities/TutorialManager.h"
//extern TutorialManager *g_gameTutorial;

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../Entities/Ability.h"
#include "../Entities/IsoMapRacing.h"
#include "../Entities/ShockwaveStraight.h"
#include "Tutorials/TutBuildStructure.h"
#include "Tutorials/TutFirstRace.h"

extern float retinaMultiplier;
extern float sizeMultiplier;

AI* AI::create(IsoMapRacing *isoMapRacing, Pet *pet_ai, Pet *pet_opponent_of_ai)
{
    AI* pRet = new AI();
    pRet->init(isoMapRacing, pet_ai, pet_opponent_of_ai);
    pRet->autorelease();
    return pRet;
}

bool AI::init(IsoMapRacing *isoMapRacing, Pet *pet_ai, Pet *pet_opponent_of_ai)
{
    m_isoMapRacing = isoMapRacing;
    m_pet_opponent_of_ai = pet_opponent_of_ai;
    m_pet_ai = pet_ai;
    
#ifdef DEBUG_AI
    debugLabel = CCLabelHelper::createBMFont("", 100.0f, true);
//    m_isoMapRacing->addChild(debugLabel, 10000000);
#endif
    
    m_startState = NOT_STARTED;
    
    m_decision1 = 0;
    
    float aiHorsePower = ((int)m_pet_ai->ppVo.top_speed+
                          (int)m_pet_ai->ppVo.corner_speed+
                          (int)m_pet_ai->ppVo.jumping);
    
    setHorsePowerScale(aiHorsePower);
    
    m_hasStartedRunning = false;
    
    m_tutorialMode = false;
    m_tutorialLearner = false;
    m_tutorialTeacher = false;
    m_isActive = true;
    m_fullPower = false;
    m_disableDefensiveSkills = false;
    m_aiDifficulty = g_playerState->player.racing.ai_difficulty;

    m_idleTime = 0.f;
    m_idleChance = 0.5f;
    m_maxIdleTime = 3.f;
    m_minIdleTime = 1.f;
    m_stayIdleCount = 0;
    m_useSkillCount = 0;

    m_shockwaveInaccuracy = 2.f;
    m_goodShotThreshold = 0.4f;
    m_goodShotCount = 0;
    m_badShotCount = 0;

    sender = NULL;
    onRecommendSkill = NULL;
    m_canRecommend = false;
    
    recalculateTimeToDoSomething();
    
    setMaxQuotaSlot();
    m_quotaSlotCurrentlyInUse = 0;
    m_timeRemainingToReleaseOneQuotaSlot = 0.0f;
    
    return true;
}

#pragma mark - State Handling

void AI::pushState(AI_STATE state)
{
    if (state != getCurrentState()) {
        if (state == AI_START) {
            //random once every game
            m_randOnce1 = (double)arc4random();
            m_randOnce2 = (double)arc4random();
            
            m_attackInterval = 0.0f;
            m_defenseInterval = 0.0f;
            
            //OPTIONAL: some AI attack more, some AI defends more
            
            m_hasStartedRunning = true;
            if (m_tutorialMode) return;
        }
        
        m_stateStack.push_back(state);
    }
}

void AI::popState(AI_STATE state)
{
    m_stateStack.pop_back();
}

AI_STATE AI::getCurrentState()
{
    return m_stateStack.size() > 0 ? m_stateStack.at(m_stateStack.size() - 1) : AI_NOTHING;
}

void AI::popToState(AI_STATE state)
{
    m_stateStack.clear();
    m_stateStack.push_back(state);
    
    if (state == AI_SCOUT) {
        m_attackReactionTime = getNewAttackReactionTime();
    }
}

#pragma mark - States

void AI::runStateStart(float delta)
{
    
    switch (m_startState) {
        case START_FIRST:
        {
            //1. do nothing (pushState Scout)
            //2. aggressive/smart: wait 2 sec, and drop a banana, then potentially stomp in the other state
            //3. aggressive: dash dash dash (amount of times, depends on total energy) (dash only 30% of all total dashes)
            
            float diffScale = getNewDifficultyScale(0.5f);
            
            if (diffScale > 0.5f) {
                m_decision1 = DEC_DASH_DASH;
            } else {
                m_decision1 = DEC_DO_NOTHING;
            }
            
            if (m_decision1 == DEC_DASH_DASH &&
                HASABILITY(kAbilityDashForward))
            {
                //wait 0.30-0.60 second
                //m_float1 is WaitTime
                m_float1 = randNum(m_randOnce2, 0.3f, 0.6f);
                //randNum(m_randOnce2, 30, 60) * 0.01f;
                //m_int1 is dashTries
                m_int1 = 0;
            }
            else if (m_decision1 == DEC_DO_NOTHING)
            {
                //m_float1 is WaitTime
                //wait 1.00-2.00 second
                
                //DELAY_START_FIRST_OR_LATER = 2
                
                float diffScale2 = getNewDifficultyScale();
                
                if (diffScale2 > 0.8f) {
                    m_float1 = randNum(m_randOnce2, 1.2f, 2.0f);
                } else if (diffScale2 > 0.4f) {
                    m_float1 = randNum(m_randOnce2, 1.5f, 2.2f);
                } else {
//                    m_float1 = randNum(m_randOnce2, 2.0f, 2.8f);
                }
            }
            
            popToState(AI_START_FROM_FRONT);
            runStateStartFromFront(delta);
            return;
        }
            break;
            
        case START_SAME:
        {
            //1. do nothing (pushState Scout)
            //2. defensive: do nothing and wait to parry stomp
            //3. smart: do nothing and wait to dash (if AI natural speed is slower)
            //4. aggressive: stomp immediately
            //5. aggressive: do nothing shortly and shoot forward (if AI natural speed is slower)
            
            
            float diffScale = getNewDifficultyScale(0.5f);
            
            if (diffScale > 0.4f) {
                
                float r = randNum();
                
                if (r > 0.5f) {
                    m_decision1 = DEC_STOMP_AFTER_RAND_TIME;
                } else {
                    m_decision1 = DEC_SHOOT_STRAIGHT_1_AFTER_RAND_TIME;
                }
                
                if (!HASABILITY(kAbilityStomp) &&
                    m_decision1 == DEC_STOMP_AFTER_RAND_TIME) {
                    m_decision1 = DEC_DO_NOTHING;
                }
                if (!HASABILITY(kAbilityShootForward) &&
                    m_decision1 == DEC_SHOOT_STRAIGHT_1_AFTER_RAND_TIME) {
                    m_decision1 = DEC_DO_NOTHING;
                }
            } else {
                m_decision1 = DEC_DO_NOTHING;
            }
            
            switch (m_decision1) {
                case DEC_STOMP_AFTER_RAND_TIME:
                {
                    //wait 0.20-0.50 second
                    //m_float1 is WaitTime
                    m_float1 = randNum(m_randOnce2, 0.2f, 0.5f);
                    //                    m_float1 = randNum(m_randOnce2, 20, 50) * 0.01f;
                }
                    break;
                    
                case DEC_SHOOT_STRAIGHT_1_AFTER_RAND_TIME:
                {
                    //wait 0.20-0.45 second
                    //m_float1 is WaitTime
                    m_float1 = randNum(m_randOnce2, 0.2f, 0.45f);
                    //                    m_float1 = randNum(m_randOnce2, 20, 45) * 0.01f;
                }
                    break;
                    
                case DEC_DO_NOTHING:
                {
                    //wait 0.20-0.45 second
                    //m_float1 is WaitTime
                    m_float1 = randNum(m_randOnce2, 0.4f, 0.9f);
                    //                    m_float1 = randNum(m_randOnce2, 20, 45) * 0.01f;
                }
                    break;
                    
                default:
                    break;
            }
            
            popToState(AI_START_FROM_NEAR);
            runStateStartFromNear(delta);
            return;
        }
            break;
            
        case START_LATER:
        {
            //1. do nothing (pushState Scout)
            
            popToState(AI_START_FROM_BEHIND);
            runStateStartFromBehind(delta);
        }
            return;
            
            break;
            
        default:
            break;
    }
    
//    popToScout();
}

void AI::runStateStartFromFront(float delta)
{
    switch (m_decision1) {
        case DEC_DO_NOTHING:
        {
            if (m_float1 >= 0.0f) {
                m_float1 -= delta;
                
                return;
            }
            
            popToState(AI_SCOUT);
        }
            break;
            
        case DEC_DASH_DASH:
        {
            if (m_float1 >= 0.0f) {
                m_float1 -= delta;
                
                return;
            }
            
            if (m_pet_ai->usingDash()) return;
            
            float percentOfDashCost = (float)m_pet_ai->getUsedCost()/(float)m_pet_ai->getAllCost();
            
            if (percentOfDashCost < 0.1f) {
                useSkill(SKILL_DASH);
                
                //delay for the next dash
                //wait 0.40 to 0.80
                m_float1 = randNum(0.4f, 0.8f);
//                m_float1 = randNum(arc4random(), 40, 80) * 0.01f;
                
                //if dash should be counted as tries, plus 1
                m_int1++;
            }
            
            if (m_int1 >= SETTING_DASH_DASH_maxDashTries) popToState(AI_SCOUT);
        }
            break;
            
        default:
            popToState(AI_SCOUT);
            break;
    }
}

void AI::runStateStartFromNear(float delta)
{
    
    switch (m_decision1) {
        case DEC_STOMP_AFTER_RAND_TIME:
        {
            //m_float1 is WaitTime
            
            if (m_float1 >= 0.0f) {
                m_float1 -= delta;
                
                return;
            }
            
            useSkill(SKILL_STOMP);
            popToState(AI_SCOUT);
        }
            break;
            
        case DEC_SHOOT_STRAIGHT_1_AFTER_RAND_TIME:
        {
            //m_float1 is WaitTime
            
            if (m_float1 >= 0.0f) {
                m_float1 -= delta;
                
                return;
            }
            
            useSkill(tryShockwave());
            popToState(AI_SCOUT);
        }
            break;
            
        case DEC_DO_NOTHING:
        {
            if (m_float1 >= 0.0f) {
                m_float1 -= delta;
                
                return;
            }
            
            popToState(AI_SCOUT);
        }
            break;
            
        default:
            popToState(AI_SCOUT);
            break;
    }
}

void AI::runStateStartFromBehind(float delta)
{
    popToState(AI_SCOUT);
    
    
    //currently not doing anything
}

void AI::runStateScout(float delta)
{
    if (!m_pet_opponent_of_ai->start_running) return;

    if(playerMustWin())
        adjustSpeed();

    if(m_idleTime > 0.f)
    {
        m_idleTime -= delta;
        return;
    }
    
//    CCLog_("assessing the situation");
    
    SKILL _useSkill = SKILL_NONE;
    
    if (m_pet_ai->m_usingSkill) {
//        m_skillToDefend = SKILL_NONE;
        return;
    }
    
    //DEFENDING----------
    
    IF_HASNT_USED_SKILL_YET__THEN_DO defendTrap();
    
    if ((m_skillToDefend != SKILL_NONE) && !m_disableDefensiveSkills)
    {
        
        float timeUntilHit = 0.0f;
        
        if (m_skillToDefend == SKILL_STOMP)
        {
            IF_HASNT_USED_SKILL_YET__THEN_DO defendStomp(&timeUntilHit);
        }
        else if (m_skillToDefend == SKILL_SHOCKWAVE1 ||
                 m_skillToDefend == SKILL_SHOCKWAVE2)
        {
            IF_HASNT_USED_SKILL_YET__THEN_DO defendShockwave(&timeUntilHit);
        }
        
        m_skillToDefend = SKILL_NONE;
        
        //only for defending
        if (_useSkill != SKILL_NONE) {
            
            //wait until shockwave has traveled for X% of the time until is hit
            //or stomp
            float scale = getNewDifficultyScale(0.4f);
            if (scale > 1.0f || m_fullPower) scale = 1.0f;
            float additionalDefenseReaction;

            if(m_tutorialMode && (_useSkill == SKILL_STOMP))
                _useSkill = SKILL_NONE;

            if (_useSkill == SKILL_STOMP)
            {
                additionalDefenseReaction =
                BASE_DEFENSE_REACTION_STOMP
                + ((1.0f-scale) * ADDITIONAL_DEFENSE_REACTION_STOMP);
            }
            else if (_useSkill == SKILL_DASH)
            {
                additionalDefenseReaction =
                BASE_DEFENSE_REACTION_DASH
                + ((1.0f-scale) * ADDITIONAL_DEFENSE_REACTION_DASH);
            }
            
            float waitToUseSkill = timeUntilHit * additionalDefenseReaction;
            
            m_waitToUseSkill = MAX(waitToUseSkill, getNewDefenseReactionTime());
            m_useSkill = _useSkill;
            
            return;
        }
    }
    
    if (m_attackReactionTime > 0.0f) {
        m_attackReactionTime -= delta;
        
        if (m_attackReactionTime > 0.0f) {
            return;
        }
    }
    
    if (m_attackReactionTime < 0.0f) m_attackReactionTime = 0.0f;
    
    //ATTACKING----------
    
    IF_HASNT_USED_SKILL_YET__THEN_DO tryDash(FIND_DASH_DISTANCE);
    if (!m_pet_ai->isHelperPet) IF_HASNT_USED_SKILL_YET__THEN_DO tryTrap();
    IF_HASNT_USED_SKILL_YET__THEN_DO tryStomp();
    IF_HASNT_USED_SKILL_YET__THEN_DO tryShockwave();
    
    //if AI doesn't have shockwave skills, should try to dash further
    
    if (!HASABILITY(kAbilityShootForward) &&
        !HASABILITY(kAbilityDoubleShoot))
    {
        IF_HASNT_USED_SKILL_YET__THEN_DO tryDash(5);
    }
    
    //if all else fails, should we do something?
//    if (useSkill == SKILL_NONE) useSkill = tryLastResortTrap(); //uncomment when you can check if player runs out of energy for a dash
    if (!m_pet_ai->isHelperPet) IF_HASNT_USED_SKILL_YET__THEN_DO tryLastResortDash();
    IF_HASNT_USED_SKILL_YET__THEN m_timeSinceCantDoNothing += delta;
    
    if (m_timeSinceCantDoNothing >= m_timeToDoSomethingAfterCantDoNothing &&
        m_pet_ai->m_summaryTrack < m_pet_opponent_of_ai->m_summaryTrack) {
        
        CCLog_("Do something after m_timeSinceCantDoNothing = %.2f",m_timeSinceCantDoNothing);
        
        if (!m_pet_opponent_of_ai->m_usingStomp) {
            _useSkill = SKILL_DASH; //this may cause AI to dash into a banana, but let's keep it this way
        }
    }
    
    //------

    if(m_tutorialMode && (_useSkill == SKILL_STOMP))
        _useSkill = SKILL_NONE;

    if (_useSkill != SKILL_NONE)
    {
        
        adjustIdleChanceAndDuration();
        if(randNum() <= m_idleChance)
        {
            ++m_stayIdleCount;
            m_idleTime = randNum(m_minIdleTime, m_maxIdleTime);
            CCLog_("decide to stay idle for %f seconds (idle count: %d, skill count: %d, chance: %f)", m_idleTime, m_stayIdleCount, m_useSkillCount, m_idleChance);
            return;
        }
        ++m_useSkillCount;
        
        bool useTheSKill = useSkill(_useSkill);
        
        if (useTheSKill != SKILL_NONE)
        {
            m_attackReactionTime = getNewAttackReactionTime();
            if (m_fullPower) m_attackReactionTime = 0.0f;
        }
    }
}

void AI::adjustIdleChanceAndDuration()
{
    if(playerMustWin())
    {
        m_idleChance = 1.1f;
        return;
    }

    int progress = m_pet_ai->m_summaryTrack - m_pet_opponent_of_ai->m_summaryTrack;
    if(progress < 0)
    {
        //player in front
        progress = abs(progress);
        if(progress > 5 && m_idleChance > 0.1f)
            m_idleChance -= 0.1f;
    }
    else if(progress > 0)
    {
        //ai in front
        if(progress > 5 && m_idleChance < 0.9f)
            m_idleChance += 0.1f;
    }
//    CCLog_("adjusted idle chance: %f", m_idleChance);
}

void AI::runStateAttackFromFront(float delta)
{
    
}

void AI::runStateAttackFromBehind(float delta)
{
    
}

void AI::runStateAttackFromNear(float delta)
{
    
}

void AI::runStateDefendFromFront(float delta)
{
    
}

void AI::runStateDefendFromBehind(float delta)
{
    
}

void AI::runStateTutorial(float delta)
{
    if (!m_hasStartedRunning) return;
    
    SKILL _useSkill = SKILL_NONE;
    
//    if (m_attackReactionTime > 0.0f) {
//        m_attackReactionTime -= delta;
//        
//        if (m_attackReactionTime > 0.0f) {
//            return;
//        }
//    }
//    
//    if (m_attackReactionTime < 0.0f) m_attackReactionTime = 0.0f;
    
    AI_STATE currentState = getCurrentState();
    
    if (m_tutorialLearner) {
        
//        IF_HASNT_USED_SKILL_YET__THEN_DO tryDash(2);
        
        switch (currentState) {
            case TUTORIAL_DASH:
            {
                IF_HASNT_USED_SKILL_YET__THEN_DO tryDash(2);
            }
                break;
                
            case TUTORIAL_DASH_OVER_TRAP:
            {
                IF_HASNT_USED_SKILL_YET__THEN_DO defendTrap();
            }
                break;
                
            case TUTORIAL_DASH_OVER_TRAP_PRACTICE:
            case TUTORIAL_STOMP:
            {
                if (m_pet_ai->m_currTrack == m_pet_opponent_of_ai->m_currTrack
                    || m_pet_ai->m_currTrack == m_pet_opponent_of_ai->m_currTrack)
                {
                    if (!m_pet_ai->m_usingSkill && m_pet_opponent_of_ai->canBeAttacked()) {
                        IF_HASNT_USED_SKILL_YET__THEN_DO SKILL_STOMP;
                    }
                }
                
                //not accurate during tutorial (reason yet unknown)
//                IF_HASNT_USED_SKILL_YET__THEN_DO tryStomp();
            }
                break;
                
            default:
                break;
        }
        
    }
    
    if (m_tutorialTeacher) {
        
        switch (currentState) {
            case TUTORIAL_DASH:
            {
                IF_HASNT_USED_SKILL_YET__THEN_DO tryDash(2);
            }
                break;
                
            case TUTORIAL_DASH_OVER_TRAP:
            {
                IF_HASNT_USED_SKILL_YET__THEN_DO tryDash(2);
                IF_HASNT_USED_SKILL_YET__THEN_DO tryTrap();
            }
                break;
                
            case TUTORIAL_DASH_OVER_TRAP_PRACTICE:
            {
                //do nothing
            }
                break;
                
            case TUTORIAL_STOMP:
            {
                //do nothing
            }
                break;
                
            default:
                break;
        }
    }
    
    if (_useSkill != SKILL_NONE)
    {
        if (m_tutorialLearner)
        {
            //recommend skill
            
            if (m_pet_ai->canAttack() &&
                onRecommendSkill &&
                m_canRecommend)
            {
                m_canRecommend = false;
                onRecommendSkill(sender, _useSkill);
            }
            
//            m_attackReactionTime = 0.0f;
            
        }
        
        if (m_tutorialTeacher)
        {
            useSkill(_useSkill);
//            m_attackReactionTime = 1.0f;//getNewAttackReactionTime();
        }
    }
}

#pragma mark - Difficulty Values Getter

float AI::getNewDifficultyScale()
{
    //horsePowerScale accounts for 70% of the difficulty scale
    //randomness accounts for the rest 30%
    
    //default
    return getNewDifficultyScale(0.3f);
}

float AI::getNewDifficultyScale(float randPercent)
{
    float scale = m_horsePowerScale * (1.0f-randPercent);
    
    if (randPercent > 0.0f) {
        scale += randNum() * randPercent;
    }
    
    return scale;
}

float AI::getNewAttackReactionTime()
{
    //1. first part is 2.5
    //2. minus from 0-1 depending on difficulty
    //3. minus from 0-0.5 depending on randomness
    return (ATTACK_REACTION_TIME * 2.5f) - (ATTACK_REACTION_TIME * m_horsePowerScale) - (ATTACK_REACTION_TIME * randNum());
}

float AI::getNewDefenseReactionTime()
{
    //1. first part is 2.5
    //2. minus from 0-1 depending on difficulty
    //3. minus from 0-0.5 depending on randomness
    return (DEFENSE_REACTION_TIME * 2.5f) - (DEFENSE_REACTION_TIME * m_horsePowerScale) - (DEFENSE_REACTION_TIME * randNum());
}

#pragma mark - Helper Functions
//Assistive functions

bool AI::useSkill(SKILL useSkill_)
{
    if (m_pet_ai->isHelperPet) {
        if (m_pet_ai->m_helperStunAmount <= 0.0f ||
            m_pet_ai->m_helperStunDuration <= 0.0f)
        {
            return SKILL_NONE;
        }
    }
    
    if (useSkill_ != SKILL_NONE && !m_fullPower)
    {
        if (m_quotaSlotCurrentlyInUse >= m_maxQuotaSlot)
        {
//            CCLog_("m_quotaSlotCurrentlyInUse:%d >= m_maxQuotaSlot:%d",m_quotaSlotCurrentlyInUse,m_maxQuotaSlot);
            
            return SKILL_NONE;
        }
        
        m_quotaSlotCurrentlyInUse++;
        CCLog_("m_quotaSlotCurrentlyInUse++ which is now:%d",m_quotaSlotCurrentlyInUse);
    }
    
    if (useSkill_ != SKILL_NONE)
    {
        m_timeSinceCantDoNothing = 0.0f;
        recalculateTimeToDoSomething();
    }
    
    switch (useSkill_) {
            
        case SKILL_DASH:
        {
            return useDash();
        }
            break;
            
        case SKILL_TRAP:
        {
            return useTrap();
        }
            break;
            
        case SKILL_STOMP:
        {
            return useStomp();
        }
            break;
            
        case SKILL_SHOCKWAVE1:
        {
            return useShockwaveStraight1();
        }
            break;
            
        case SKILL_SHOCKWAVE2:
        {
            return useShockwaveStraight2();
        }
            break;
            
        default:
            break;
    }
    
    return false;
}

float AI::randNum() //from 0.0 - 1.0
{
    return static_cast<float>((double)arc4random() / ARC4RANDOM_MAX);
}

float AI::randNum(float min, float max)
{
    return static_cast<float>(min + (((double)arc4random() / ARC4RANDOM_MAX) * (max-min)));
}

float AI::randNum(double randNum, float min, float max)
{
    return static_cast<float>(min + ((randNum / ARC4RANDOM_MAX) * (max-min)));
}

void AI::recalculateTimeToDoSomething()
{
    float min = TIME_TO_DO_SOMETHING_MIN;
    float max = TIME_TO_DO_SOMETHING_MAX;
    if (HASABILITY(kAbilityDoubleShoot)) {
        min *= 3.0f;
        max = min * 2.0f;
    }
    
    m_timeToDoSomethingAfterCantDoNothing = randNum(min, max);
    CCLog_("recalculateTimeToDoSomething min:%.2f,min:%.2f, = %.2f",min,max,m_timeToDoSomethingAfterCantDoNothing);
}

void AI::recalculateTimeRemainingToReleaseOneQuotaSlot()
{
//    float diffScale = getNewDifficultyScale();
    float diffScale = m_aiDifficulty;
    
    float from = TIME_TO_RELEASE_ONE_QUOTA_SLOT_FROM_MINHP + (diffScale * (TIME_TO_RELEASE_ONE_QUOTA_SLOT_FROM_MAXHP - TIME_TO_RELEASE_ONE_QUOTA_SLOT_FROM_MINHP));
    float to = TIME_TO_RELEASE_ONE_QUOTA_SLOT_TO_MINHP + (diffScale * (TIME_TO_RELEASE_ONE_QUOTA_SLOT_TO_MAXHP - TIME_TO_RELEASE_ONE_QUOTA_SLOT_TO_MINHP));
    
    m_timeRemainingToReleaseOneQuotaSlot = randNum(from, to);
    
    CCLog_("recalculateTimeRemainingToReleaseOneQuotaSlot from:%.2f,to:%.2f = %.2f",from,to,m_timeRemainingToReleaseOneQuotaSlot);
}

void AI::setHorsePowerScale(int aiHorsePower)
{
    float maxHorsePowerInGame = g_gameContent->GameConfig["MAX_HORSEPOWER_IN_GAME"] * MAX_HORSEPOWER_RATIO_FOR_AI;
    float minHorsePowerInGame = 0.0f; //because pet grows really fast during the start of the game
    float horsePowerStrengthDivider = maxHorsePowerInGame-minHorsePowerInGame;
    if (horsePowerStrengthDivider <= 1.0f) horsePowerStrengthDivider = 1.0f;
    m_horsePowerScale = (aiHorsePower-minHorsePowerInGame)/horsePowerStrengthDivider;
}

void AI::setMaxQuotaSlot()
{
//    CCLog_("g_playerState->player.racing.ai_difficulty");
//    DEBUG_FLOAT(g_playerState->player.racing.ai_difficulty);
    
    CCLog_("m_ai_Difficulty:%.2f",m_aiDifficulty);
    
    m_maxQuotaSlot = MAX_QUOTA_SLOT_MINHP + (m_aiDifficulty * (MAX_QUOTA_SLOT_MAXHP-MAX_QUOTA_SLOT_MINHP));
    
//    m_maxQuotaSlot = MAX_QUOTA_SLOT_MINHP + (getNewDifficultyScale() * (MAX_QUOTA_SLOT_MAXHP-MAX_QUOTA_SLOT_MINHP));
    CCLog_("m_maxQuotaSlot:%d",m_maxQuotaSlot);
}

CCPoint AI::getNextPosition(Pet *pet,
                            float delta,
                            float current_speed,
                            float *new_speed,
                            float *trackPosition,
                            Track *curr_track,
                            float *dashTime,
                            float *stompTime,
                            float *shockwave1Time,
                            float *shockwave2Time,
                            bool *usingDash,
                            bool *usingStomp,
                            bool *usingShockwave1,
                            bool *usingShockwave2,
                            float dashSpeed,
                            float stompSpeed,
                            float shockwave1Speed,
                            float shockwave2Speed
                            )
{
    pet->getNewSpeed(delta, current_speed, new_speed, curr_track,
                     dashTime, stompTime, shockwave1Time, shockwave2Time,
                     usingDash, usingStomp, usingShockwave1, usingShockwave2,
                     dashSpeed, stompSpeed, shockwave1Speed, shockwave2Speed);
    
    *trackPosition += (*new_speed * delta);
    
    int trackCount = 0;
    curr_track = pet->getTrackFromTrackPosition(curr_track, trackPosition, &trackCount);
    
    return pet->getPositionFromTrack(curr_track, *trackPosition);
}

bool aiLineIntersectRect(const CCPoint& A,
                         const CCPoint& B,
                         const CCPoint& rectA,
                         const CCPoint& rectB,
                         const CCPoint& rectC,
                         const CCPoint& rectD)
{
    if (ccpSegmentIntersect(A, B, rectA, rectB))
        return true;
    if (ccpSegmentIntersect(A, B, rectB, rectC))
        return true;
    if (ccpSegmentIntersect(A, B, rectC, rectD))
        return true;
    if (ccpSegmentIntersect(A, B, rectD, rectA))
        return true;
    
    return false;
}

#pragma mark - Check Functions
//Checking if attack will succeed for both players and opponents

SKILL AI::checkDash(Pet *attackingPet, Pet *defendingPet, int distance)
{
    Ability *_ability = attackingPet->playerState->getAbilityForKey(kAbilityDashForward);
    if (!_ability) return SKILL_NONE;

    if(shouldAILetPlayerWin())
        return SKILL_NONE;

    if (distance <= FIND_DASH_DISTANCE) {
        distance = _ability->DASH_DISTANCE;
    }
    
    Track *findTrack = attackingPet->m_currTrack;
    
    int count = 0;
    while (findTrack) {
        count++;
        if (count > (distance-1)) return SKILL_NONE;
        findTrack = findTrack->nextTrack;
        
        if (defendingPet->m_currTrack == findTrack) {
            return SKILL_DASH;
        }
    }
    
    return SKILL_NONE;
}

SKILL AI::checkStomp(Pet *attackingPet, Pet *defendingPet, float tileSizeRatio)
{
    return checkStomp(attackingPet, defendingPet, tileSizeRatio, SKILL_NONE, SKILL_STOMP);
}

SKILL AI::checkStomp(Pet *attackingPet, Pet *defendingPet, float tileSizeRatio, SKILL defendingPetUseSkill)
{
    return checkStomp(attackingPet, defendingPet, tileSizeRatio, defendingPetUseSkill, SKILL_STOMP);
}

SKILL AI::checkStomp(Pet *attackingPet, Pet *defendingPet, float tileSizeRatio, SKILL defendingPetUseSkill, SKILL attackingPetUseSkill)
{
    if(m_tutorialMode)
        return SKILL_NONE;

    Ability* _ability = attackingPet->playerState->getAbilityForKey(kAbilityStomp);
    if (!_ability) return SKILL_NONE;

    float abilitySpeedPercent = _ability->STOMP_SPEED_PERCENT;
    
    float abilityTime;
    float abilitySpeed;
    
    abilityTime = _ability->STOMP_DELAY_TIME;
    abilitySpeed = attackingPet->m_currSpeed * abilitySpeedPercent;
    
    float seconds = abilityTime;
    if (seconds < AI_DELTA) seconds = AI_DELTA;
    
    //assuming the defendingPet does nothing
    PRE_FIND_POSITION_A_DEFAULT(defendingPet);
    
    //override default values, if the defendingPet will use these abilities..
    if (defendingPetUseSkill != SKILL_NONE) {
        
        a_dashSpeed = 0.0f;
        a_dashTime = 0.0f;
        a_usingDash = false;
        a_stompSpeed = 0.0f;
        a_stompTime = 0.0f;
        a_usingStomp = false;
        a_shockwave1Speed = 0.0f;
        a_shockwave1Time = 0.0f;
        a_usingShockwave1 = false;
        a_shockwave2Speed = 0.0f;
        a_shockwave2Time = 0.0f;
        a_usingShockwave2 = false;
    }
    if (defendingPetUseSkill == SKILL_DASH) {
        Ability* _dash = defendingPet->playerState->getAbilityForKey(kAbilityDashForward);
        if (_dash) {
            float t = _dash->DASH_TIME; float d = _dash->DASH_DISTANCE;
            
            if (t <= 0) t = 0.1f;
            
            a_dashSpeed = d / t;
            a_dashTime = t;
            a_usingDash = true;
        }
    } else if (defendingPetUseSkill == SKILL_SHOCKWAVE1) {
        Ability* _shockwave1 = defendingPet->playerState->getAbilityForKey(kAbilityShootForward);
        if (_shockwave1) {
            a_shockwave1Speed = POWERUP_4_STAGE1_SPEED;
            a_shockwave1Time = _shockwave1->SHOCKWAVE1_DELAY1 + _shockwave1->SHOCKWAVE1_DELAY2;
            a_usingShockwave1 = true;
        }
    } else if (defendingPetUseSkill == SKILL_SHOCKWAVE2) {
        Ability* _shockwave2 = defendingPet->playerState->getAbilityForKey(kAbilityDoubleShoot);
        if (_shockwave2) {
            a_shockwave2Speed = POWERUP_4_STAGE1_SPEED;
            a_shockwave2Time = _shockwave2->SHOCKWAVE2_DELAY1 + _shockwave2->SHOCKWAVE2_DELAY2;
            a_usingShockwave2 = true;
        }
    }
    
    //sets up the attackingPet for the stomping
    PRE_FIND_POSITION_B(attackingPet->m_currSpeed,
                        attackingPet->m_currTrackPosition,
                        attackingPet->m_currTrack,
                        0.0f,
                        abilityTime,
                        0.0f,
                        0.0f,
                        false,
                        true,
                        false,
                        false,
                        0.0f,
                        abilitySpeed,
                        0.0f,
                        0.0f);
    
    while (seconds > 0.0f) {
        seconds -= AI_DELTA;
        
        FIND_POSITION_A(defendingPet);
        FIND_POSITION_B(attackingPet);
    }
    CCPoint defenderPosition = POST_FIND_POSITION_A(defendingPet);
    CCPoint attackerPosition = POST_FIND_POSITION_B(attackingPet);
    
    CCPoint stompPosition = attackerPosition + STOMP_Y_OFFSET;
    
    float stompRange = ccpLength(g_gameContent->tileSize * tileSizeRatio);
    float angle = ccpAngle(stompPosition, defenderPosition);
    CCPoint stompEdge = stompPosition + ccp(cosf(angle) * stompRange, sinf(angle) * stompRange * STOMP_CIRCLE_Y_LENGTH_PETCENT);
    
    float distanceFromStompToEdge = ccpDistance(stompPosition, stompEdge);
    float distanceFromStompToOppo = ccpDistance(stompPosition, defenderPosition);

    if(shouldAILetPlayerWin())
    {
        if(distanceFromStompToOppo <= distanceFromStompToEdge)
            return SKILL_NONE;
        distanceFromStompToOppo -= (stompRange * 1.8f);
    }
    if (distanceFromStompToOppo <= distanceFromStompToEdge)
    {
        return attackingPetUseSkill;
    }
    
    return SKILL_NONE;
}

SKILL AI::checkShockwave(Pet *attackingPet, Pet *defendingPet, float tilePercentShockwave1, float tilePercentShockwave2)
{
    float timeUntilHit; //not used
    return checkShockwave(attackingPet, defendingPet, tilePercentShockwave1, tilePercentShockwave2, &timeUntilHit);
}

SKILL AI::checkShockwave(Pet *attackingPet, Pet *defendingPet, float tilePercentShockwave1, float tilePercentShockwave2, float *timeUntilHit)
{
    Ability* _ability1 = attackingPet->playerState->getAbilityForKey(kAbilityShootForward);
    Ability* _ability2 = attackingPet->playerState->getAbilityForKey(kAbilityDoubleShoot);
    
    if (!_ability1 && !_ability2) return SKILL_NONE;
    
    float ability1DelayB4 = _ability1 ? _ability1->SHOCKWAVE1_DELAY1 : 0;
    float ability2DelayB4 = _ability2 ? _ability2->SHOCKWAVE2_DELAY1 : 0;
    
    Track *startTrack = attackingPet->m_currTrack;
    Track *nextTrack = startTrack->nextTrack;
    
    //    CCLayerColor *color1 = CCLayerColor::create(ccc4(200, 200, 200, 200), g_gameContent->tileSize.width, g_gameContent->tileSize.height);
    //    this->addChild(color1, 1000000000);
    
    float x = nextTrack->getPositionX() - startTrack->getPositionX();
    float y = nextTrack->getPositionY() - startTrack->getPositionY();
    
    float doubleStockWaveAngle = 35.0f;
    
    float angleNormalSide = atan2f(y, x);
    float angleLeftSide = atan2f(y, x) + M_PI*doubleStockWaveAngle/180.f;
    float angleRightSide = atan2f(y, x) - M_PI*doubleStockWaveAngle/180.f;
    
    double updateNormalSideX = SHOCKWAVE_SPEED * cosf(angleNormalSide);
    double updateNormalSideY = SHOCKWAVE_SPEED * sinf(angleNormalSide);
    
    double updateLeftSideX = SHOCKWAVE_SPEED * cosf(angleLeftSide);
    double updateLeftSideY = SHOCKWAVE_SPEED * sinf(angleLeftSide);
    
    double updateRightSideX = SHOCKWAVE_SPEED * cosf(angleRightSide);
    double updateRightSideY = SHOCKWAVE_SPEED * sinf(angleRightSide);
    
    float velocityNormalSide = ccpLength(ccp(updateNormalSideX, updateNormalSideY))/SHOCKWAVE_SPEED_DURATION;
    float velocityLeftSide = ccpLength(ccp(updateLeftSideX, updateLeftSideY))/SHOCKWAVE_SPEED_DURATION;
    float velocityRightSide = ccpLength(ccp(updateRightSideX, updateRightSideY))/SHOCKWAVE_SPEED_DURATION;
    
    //    CCLog_("second per frame : %f",CCDirector::sharedDirector()->getSecondsPerFrame());
    
    float time = 0;
    
    CCPoint positionNormalSide;
    CCPoint positionLeftSide;
    CCPoint positionRightSide;
    
    //starting positions for the Shockwave
    if ((x < 0 && y > 0) || //left-up
        (x > 0 && y < 0))   //right-down
    {
        positionNormalSide = attackingPet->m_currPos;
        positionLeftSide = attackingPet->m_currPos;
        positionRightSide = attackingPet->m_currPos;
        
    } else {
        positionNormalSide = attackingPet->m_currPos - g_gameContent->tileSize*0.5f;
        positionLeftSide = attackingPet->m_currPos - g_gameContent->tileSize*0.5f;
        positionRightSide = attackingPet->m_currPos - g_gameContent->tileSize*0.5f;
    }
    
    CCPoint prevPosNormal;
    CCPoint currPosNormal = positionNormalSide;
    CCPoint prevPosLeft;
    CCPoint currPosLeft = positionLeftSide;
    CCPoint prevPosRight;
    CCPoint currPosRight = positionRightSide;
    
    time = MIN(ability1DelayB4, ability2DelayB4);
    
    //starting Position
    //    CCPoint playerPosition;
    CCPoint defenderPosition = defendingPet->getNextPositionAfterSeconds(time);
    
    float new_speed = defendingPet->m_currSpeed;
    float current_speed = defendingPet->m_currSpeed;
    float trackPosition = defendingPet->m_currTrackPosition;
    Track *curr_track = defendingPet->m_currTrack;
    float dashTime = 0.0f;
    float stompTime = 0.0f;
    float shockwave1Time = 0.0f;
    float shockwave2Time = 0.0f;
    bool usingDash = false;
    bool usingStomp = false;
    bool usingShockwave1 = false;
    bool usingShockwave2 = false;
    
    while (time < (SHOCKWAVE_LIFE_DURATION
                   - MIN(ability1DelayB4, ability2DelayB4)
                   + MAX(ability1DelayB4, ability2DelayB4)))
    {
        prevPosNormal = currPosNormal;
        prevPosLeft = currPosLeft;
        prevPosRight = currPosRight;
        
        if (time >= (ability1DelayB4)) {
            float sNormalSide = velocityNormalSide*AI_DELTA;
            
            positionNormalSide = positionNormalSide + ccp(sNormalSide*cosf(angleNormalSide), sNormalSide*sinf(angleNormalSide));
        }
        
        if (time >= (ability2DelayB4)) {
            float sLeftSide = velocityLeftSide*AI_DELTA;
            float sRightSide = velocityRightSide*AI_DELTA;
            
            positionLeftSide = positionLeftSide + ccp(sLeftSide*cosf(angleLeftSide), sLeftSide*sinf(angleLeftSide));
            positionRightSide = positionRightSide + ccp(sRightSide*cosf(angleRightSide), sRightSide*sinf(angleRightSide));
        }
        
        currPosNormal = positionNormalSide;
        currPosLeft = positionLeftSide;
        currPosRight = positionRightSide;
        
        //used instead of getNextPositionAfterSeconds for faster performance, because the result is cached
        defendingPet->getNewSpeed(AI_DELTA, current_speed, &new_speed, curr_track,
                                  &dashTime, &stompTime, &shockwave1Time, &shockwave2Time,
                                  &usingDash, &usingStomp, &usingShockwave1, &usingShockwave2);
        
        current_speed = new_speed;
        trackPosition += (current_speed * AI_DELTA);
        int trackCount = 0;
        curr_track = defendingPet->getTrackFromTrackPosition(curr_track, &trackPosition, &trackCount);
        defenderPosition = defendingPet->getPositionFromTrack(curr_track, trackPosition);
        
        CCSize size = g_gameContent->tileSize;
        bool letPlayerWin = shouldAILetPlayerWin();

        if (_ability1 && !letPlayerWin) {

            CCRect playerRect = CCRectZero;
            playerRect.size = g_gameContent->tileSize * tilePercentShockwave1;
            playerRect.origin = defenderPosition - playerRect.size*0.5f;
            
            if (aiLineIntersectRect(prevPosNormal,
                                    currPosNormal,
                                    ccp(defenderPosition.x, defenderPosition.y + (-tilePercentShockwave1*size.height)),
                                    ccp(defenderPosition.x + (-tilePercentShockwave1*size.width), defenderPosition.y),
                                    ccp(defenderPosition.x, defenderPosition.y + (tilePercentShockwave1*size.height)),
                                    ccp(defenderPosition.x + (tilePercentShockwave1*size.width), defenderPosition.y)
                                    ))
            {
                *timeUntilHit = time;
                return SKILL_SHOCKWAVE1;
            }
        }
        
        if (_ability2) {

            float inaccuracyX = randNum(-m_shockwaveInaccuracy, m_shockwaveInaccuracy);
            float inaccuracyY = randNum(-m_shockwaveInaccuracy, m_shockwaveInaccuracy);
            if((abs(inaccuracyX) + abs(inaccuracyY)) < m_goodShotThreshold)
                ++m_goodShotCount;
            else
                ++m_badShotCount;

            if(letPlayerWin)
            {
                if(randNum() > 0.5f)
                {
                    inaccuracyX = 2.0f;
                    inaccuracyY = 2.0f;
                }
                else
                {
                    inaccuracyX = -2.0f;
                    inaccuracyY = -2.0f;
                }
            }

            defenderPosition.x += (size.width * inaccuracyX);
            defenderPosition.y += (size.height * inaccuracyY);

            if (aiLineIntersectRect(prevPosLeft,
                                    currPosLeft,
                                    ccp(defenderPosition.x, defenderPosition.y + (-tilePercentShockwave2*size.height)),
                                    ccp(defenderPosition.x + (-tilePercentShockwave2*size.width), defenderPosition.y),
                                    ccp(defenderPosition.x, defenderPosition.y + (tilePercentShockwave2*size.height)),
                                    ccp(defenderPosition.x + (tilePercentShockwave2*size.width), defenderPosition.y)
                                    ))
            {
                adjustShockwaveAccuracy();
                *timeUntilHit = time;
                return SKILL_SHOCKWAVE2;
            }
            
            if (aiLineIntersectRect(prevPosRight,
                                    currPosRight,
                                    ccp(defenderPosition.x, defenderPosition.y + (-tilePercentShockwave2*size.height)),
                                    ccp(defenderPosition.x + (-tilePercentShockwave2*size.width), defenderPosition.y),
                                    ccp(defenderPosition.x, defenderPosition.y + (tilePercentShockwave2*size.height)),
                                    ccp(defenderPosition.x + (tilePercentShockwave2*size.width), defenderPosition.y)
                                    ))
            {
                adjustShockwaveAccuracy();
                *timeUntilHit = time;
                return SKILL_SHOCKWAVE2;
            }
        }
        
//        //TODO:break when shockwave is no longer able to hit playerPet
        
        time += AI_DELTA;
    }
    
    return SKILL_NONE;
}

void AI::adjustShockwaveAccuracy()
{
    float accuracyRatio = (m_goodShotCount+1.f) / (m_badShotCount+1.f);
    if(accuracyRatio <= (1.f / 3.f))
    {
        m_shockwaveInaccuracy -= 0.5f;
    }
    else if(accuracyRatio > 3.f)
    {
        m_shockwaveInaccuracy += 0.5f;
    }
}

void AI::adjustSpeed()
{
    static const int MAX_PET_SPEED = 200;
    static const int MIN_PET_SPEED = 9;
    static const int PLAYER_PET_SPEED = 50;
    static const int AI_PET_SPEED = 15;

    if(m_pet_ai->m_summaryTrack > m_pet_opponent_of_ai->m_summaryTrack)
    {
        m_pet_opponent_of_ai->setStatStraightSpeed(MAX_PET_SPEED);
        m_pet_opponent_of_ai->setStatCornerSpeed(MAX_PET_SPEED);
        m_pet_ai->setStatStraightSpeed(MIN_PET_SPEED);
        m_pet_ai->setStatCornerSpeed(MIN_PET_SPEED);
    }
    else
    {
        m_pet_opponent_of_ai->setStatStraightSpeed(PLAYER_PET_SPEED);
        m_pet_opponent_of_ai->setStatCornerSpeed(PLAYER_PET_SPEED);
        m_pet_ai->setStatStraightSpeed(AI_PET_SPEED);
        m_pet_ai->setStatCornerSpeed(AI_PET_SPEED);
    }
}

bool AI::shouldAILetPlayerWin() const
{
    int totalRaces = m_pet_opponent_of_ai->playerState->player.info.total_races;
    if(totalRaces == 0 || totalRaces > 4)
        return false;

    return isLastLap();
}

bool AI::playerMustWin() const
{
    return isLastLap() && g_playerState->Checklist[TutFirstRace::key()] && !g_playerState->Checklist[TutUpgradeMansion::key()];
}

bool AI::isLastLap() const
{
    int opponentRemainingLaps = m_pet_opponent_of_ai->max_round - m_pet_ai->ran_round;
    int aiRemainingLaps = m_pet_ai->max_round - m_pet_ai->ran_round;
    return opponentRemainingLaps <= 1 || aiRemainingLaps <= 1;
}

void AI::opponentIsAttacked(float stunDuration, SKILL skillUsed)
{
    
}

#pragma mark - Defend Skills
//Defensive skills, check to evade incoming attacks

void AI::opponentUsedSkill(SKILL usedSkill)
{
    m_skillToDefend = usedSkill;
}

void AI::aiIsAttacked(float stunDuration, SKILL skillUsed)
{
    //clear remaining values
    
    m_waitToUseSkill = 0.0f;
    m_useSkill = SKILL_NONE;
    
    m_skillToDefend = SKILL_NONE;
}

SKILL AI::defendTrap()
{
    if (m_pet_ai->m_usingSkill) return SKILL_NONE;
    if (!HASABILITY(kAbilityDashForward)) return SKILL_NONE;
    
    Track *currentTrack = m_pet_ai->m_currTrack;
    
    if (currentTrack->hasTrap() || currentTrack->nextTrack->hasTrap()) {
        return SKILL_DASH;
    }
    
    return SKILL_NONE;
}

SKILL AI::defendStomp(float *timeUntilHit)
{
    if (m_pet_ai->m_usingSkill) return SKILL_NONE;
    if (!m_pet_opponent_of_ai->m_usingStomp) return SKILL_NONE;
    
    Ability* _ability = GETABILITY_FROM_OPPONENT(kAbilityStomp);
    if (!_ability) return SKILL_NONE;

    if(shouldAILetPlayerWin())
        return SKILL_NONE;

    SKILL decision = SKILL_NONE;
    
    if (m_pet_opponent_of_ai->m_summaryTrack >= (m_pet_ai->m_summaryTrack + 1)) {
        
        if (HASABILITY(kAbilityStomp)) {
            decision = SKILL_STOMP;
        }
    } else {
        if (HASABILITY(kAbilityDashForward)) {
            decision = SKILL_DASH;
        }
    }
    
//    float reactionTime = abilityTime*0.7f;
//    
//    switch (decision) {
//            
//        case SKILL_DASH:
//            reactionTime = abilityTime*0.7f;
//            break;
//            
//        case SKILL_STOMP:
//            reactionTime = abilityTime*0.4f;
//            break;
//        default:
//            break;
//    }
    
    //stop AI from having instantanous reaction time
//    if (m_pet_opponent_of_ai->m_stompTime > reactionTime && !m_fullPower) return SKILL_NONE;
    
        //test to see if AI dash, will he survive
//        if (checkStomp(m_pet_opponent_of_ai, m_pet_ai, AI2_STOMP_AREA_RATIO_TO_TILESIZE, SKILL_SHOCKWAVE1)) {
//            return useShockwaveStraight1();
//        } else if (checkStomp(m_pet_opponent_of_ai, m_pet_ai, AI2_STOMP_AREA_RATIO_TO_TILESIZE, SKILL_SHOCKWAVE2)) {
//            return useShockwaveStraight2();
//        } else
        
//        if (checkStomp(m_pet_opponent_of_ai, m_pet_ai, AI2_STOMP_AREA_RATIO_TO_TILESIZE, SKILL_DASH)) {
//            return useDash();
//        } else {
//            return useStomp();
//        }
    
    SKILL skillToUseToDefend = checkStomp(m_pet_opponent_of_ai, m_pet_ai, AI2_STOMP_AREA_RATIO_TO_TILESIZE, SKILL_NONE, decision);
    
    if (skillToUseToDefend != SKILL_NONE) {
        *timeUntilHit = _ability->STOMP_DELAY_TIME;
    }
    
    return skillToUseToDefend;
}

SKILL AI::defendShockwave(float *timeUntilHit)
{
    if (m_pet_ai->m_usingSkill) return SKILL_NONE;
    
//    float reactionTime = -1;
//    float shockwaveTime = -1;
//    float timeForCalculatingReaction = -1;
    
    SKILL skillUsed = SKILL_NONE;
    
    if (m_pet_opponent_of_ai->m_usingShockwaveStraight1) {
//        shockwaveTime = m_pet_opponent_of_ai->m_shockwave1Time;
        skillUsed = SKILL_SHOCKWAVE1;
    } else if (m_pet_opponent_of_ai->m_usingShockwaveStraight2) {
//        shockwaveTime = m_pet_opponent_of_ai->m_shockwave2Time;
        skillUsed = SKILL_SHOCKWAVE2;
    }
    
    if (skillUsed == SKILL_NONE) return SKILL_NONE;
    
//    timeForCalculatingReaction = shockwaveTime;
    
    SKILL decision = SKILL_NONE;
    
//    //if stompCost not multiplied, AI will use stomp to defend too often, and game will be too difficult
//    if (m_dashForwardCost > m_stompCost * 1.5f) {
//        decision = SKILL_DASH;
//    } else {
//        decision = SKILL_STOMP;
//    }
//    
//    if (decision == SKILL_STOMP) {
//        Ability* _ability = GETABILITY(kAbilityStomp);
//        float stompDelayTime = _ability->STOMP_DELAY_TIME;
//        
//        if (stompDelayTime < timeForCalculatingReaction)
//        {
//            timeForCalculatingReaction = stompDelayTime;
//        }
//    }
    
//    reactionTime = timeForCalculatingReaction * 0.15f;
//    reactionTime = 0.2f;
    
//    float timeUntilHit = 0.0f;
    SKILL skillThatHits = checkShockwave(m_pet_opponent_of_ai, m_pet_ai, AI_SHOCKWAVE_TILE_PERCENT_SHOCKWAVE1, AI_SHOCKWAVE_TILE_PERCENT_SHOCKWAVE2, timeUntilHit);
    
    if (skillThatHits != SKILL_NONE) {
        CCLog_("timeUntilHit:%f",*timeUntilHit);
    }
    
    //TODO: evading shockwave should almost always happen at the last minute!! unless may be if the opponent is really close?
    //stop AI from having instantanous reaction time
//    if (timeUntilHit > reactionTime && !m_fullPower) return SKILL_NONE;
    
    //may be check distance instead?

    float distance = ccpDistance(m_pet_ai->getPosition(), m_pet_opponent_of_ai->getPosition());
    float stompArea = ccpLength(g_gameContent->tileSize * STOMP_AREA_RATIO_TO_TILESIZE * AI_SHOCKWAVE_DEFEND_STOMP_HOW_BIG_IS_THE_AREA);
    
//    if (skillThatHits != SKILL_NONE) {
//        CCLog_("distance:%f",distance);
//    }
    
    //TODO:implement reaction time
    
    if (distance <= stompArea &&
        HASABILITY(kAbilityStomp)) {
        decision = SKILL_STOMP;
    } else {
        if (HASABILITY(kAbilityDashForward)) {
            decision = SKILL_DASH;
        }
    }
    
    
//    if (timeUntilHit > AI_SHOCKWAVE_DEFEND_DASH_HOW_FAR) {
//        decision = SKILL_DASH;
//    } else {
//        decision = SKILL_STOMP;
//    }
    
    
    //if skillThatHits matches skillUsed
    if (skillThatHits == skillUsed) {
        CCLog_("evades shockwave now!");
        return decision;
    }
    
    return SKILL_NONE;
}

#pragma mark - Try Skills
//Offensive skills, see if it's possible to launch successful attacks

SKILL AI::tryDash(float distance)
{
    if (//m_pet_opponent_of_ai->m_usingSkill ||
        m_pet_ai->m_usingSkill ||
        m_pet_opponent_of_ai->m_currPetAction == PET_JUMP
        || m_pet_opponent_of_ai->m_currPetAction == PET_FALL_A
        || m_pet_opponent_of_ai->m_currPetAction == PET_FALL_B)
        return SKILL_NONE;
    
    return checkDash(m_pet_ai, m_pet_opponent_of_ai, distance);
}

SKILL AI::tryTrap()
{
    if (m_pet_ai->m_usingSkill) return SKILL_NONE;
    if (!HASABILITY(kAbilityBananaTrap)) return SKILL_NONE;

    if(shouldAILetPlayerWin())
        return SKILL_NONE;

    //stops AI from using banana if it gets too expensive
    m_pet_ai->updateCostForAllSkills();
    
    //stops AI from using banana if it gets too expensive
    if ((m_pet_ai->getTrapCost() * 1.5f) > m_pet_ai->remainingCost()) return SKILL_NONE;
    
    //stops AI from using banana at all if it gets over X times
    if (m_pet_ai->getTrapTimeUsed() > ((m_pet_ai->ran_round+1) * 3)) return SKILL_NONE;
    if (m_pet_ai->getTrapTimeUsed() > 8) return SKILL_NONE;
    if (m_pet_ai->isTutorialMode()) {
        if (m_pet_ai->getTrapTimeUsed() > 2) return SKILL_NONE;
    }
    
    Track *currentTrack = m_pet_ai->m_currTrack;
    
    //stops AI from putting banana repeatedly on the same trap
    if (!currentTrack->prevTrack->trapAvailable()) return SKILL_NONE;
    
    Track *findTrack = currentTrack->prevTrack->prevTrack->prevTrack;
    
    int count = 0;
    while (findTrack) {
        count++;
        
//        CCLog_("Find Track AITrap %d", count);
        
        if (m_pet_opponent_of_ai->m_currTrack == findTrack) {
            return SKILL_TRAP;
        }
        
        findTrack = findTrack->prevTrack;
        if (m_pet_ai->getTrapTimeUsed() <= 3) {
            if (count > 8) break;
        } else {
            if (count > 5) break;
        }
    }
    
    return SKILL_NONE;
}

SKILL AI::tryStomp()
{
    if (//m_pet_opponent_of_ai->m_usingSkill ||
        m_pet_ai->m_usingSkill ||
        m_pet_opponent_of_ai->m_usingStomp || //let defendStomp do the work
        m_pet_opponent_of_ai->m_currPetAction == PET_JUMP
        || m_pet_opponent_of_ai->m_currPetAction == PET_FALL_A
        || m_pet_opponent_of_ai->m_currPetAction == PET_FALL_B)
        return SKILL_NONE;

    return checkStomp(m_pet_ai, m_pet_opponent_of_ai, AI_STOMP_AREA_RATIO_TO_TILESIZE);
}

SKILL AI::tryShockwave()
{
    if (//m_pet_opponent_of_ai->m_usingSkill ||
        m_pet_ai->m_usingSkill ||
        m_pet_opponent_of_ai->m_currPetAction == PET_JUMP
        || m_pet_opponent_of_ai->m_currPetAction == PET_FALL_A
        || m_pet_opponent_of_ai->m_currPetAction == PET_FALL_B)
        return SKILL_NONE;
    
    
//    CCObject *object = NULL;
//    CCARRAY_FOREACH(m_isoMapRacing->getChildren(), object)
//    {
//        ShockwaveStraight* v = dynamic_cast<ShockwaveStraight*>(object);
//        if(v != 0) {
//            
//        }
//        
//        CCNode *selectPetMenu = (CCNode *)object;
//        selectPetMenu->setZOrder(MENU_SCALE_NORMAL_Z_ORDER);
//        
//        
//    }
    
    return checkShockwave(m_pet_ai, m_pet_opponent_of_ai, AI_SHOCKWAVE_TILE_PERCENT_SHOCKWAVE1, AI_SHOCKWAVE_TILE_PERCENT_SHOCKWAVE2);
}

SKILL AI::tryLastResortDash()
{
    if (m_pet_ai->m_usingSkill) return SKILL_NONE;
    if (!HASABILITY(kAbilityDashForward)) return SKILL_NONE;
    
    if (!IS_LAST_ROUND) return SKILL_NONE;
    if (m_pet_opponent_of_ai->m_summaryTrack < m_pet_ai->m_summaryTrack) return SKILL_NONE;
    
    //total remaining track count
//    int remainingTrackCount = (m_pet_ai->m_trackCount * m_pet_ai->max_round) - m_pet_ai->m_summaryTrack;
    
//    //lap remaining track count
//    int remainingTrackCount =
//    m_pet_ai->m_trackCount - (m_pet_ai->m_summaryTrack % (m_pet_ai->ran_round * m_pet_ai->m_trackCount));
//    
//    int trackCountToSearch = remainingTrackCount - 2; //minus 2 because the last trap on the last twos track does not matter if there are traps there, because the pet will slide over to win anyway even if the pet hits the traps
//    
//    Track *currentTrack = m_pet_ai->m_currTrack;
//    Track *findTrack = currentTrack->nextTrack;
//    
//    int trapsFound = 0;
//    
//    int count = 0;
//    while (currentTrack) {
//        count++;
//        
//        if (findTrack->hasTrap()) {
//            trapsFound++;
//        }
//        
//        findTrack = findTrack->nextTrack;
//        if (count > trackCountToSearch) break;
//    }
//    
//    unsigned int dashForwardTimeUsed = m_dashForwardTimeUsed + trapsFound;
//    unsigned int dashForwardCost = m_dashForwardStartingCost + ((m_dashForwardIncreasingCost + (m_dashForwardAddingIncreasingCost*(dashForwardTimeUsed-1))) * m_dashForwardTimeUsed);
//    if (dashForwardCost > remainingCost()) {
//        return SKILL_NONE;
//    }

    if(shouldAILetPlayerWin())
    {
        if((m_pet_ai->m_summaryTrack + 5) >= m_pet_opponent_of_ai->m_summaryTrack)
            return SKILL_NONE;
    }

    return SKILL_DASH;
}

SKILL AI::tryLastResortTrap()
{
    if (m_pet_ai->m_usingSkill) return SKILL_NONE;
    if (!HASABILITY(kAbilityBananaTrap)) return SKILL_NONE;
    
    //stops AI from using banana if it gets too expensive
    m_pet_ai->updateCostForAllSkills();
    if ((m_pet_ai->getTrapCost() * 1.2f) > m_pet_ai->remainingCost()) return SKILL_NONE;
    
    if (!(PLAYER_IS_LAST_ROUND)) return SKILL_NONE;
    if (OPPONENT_IS_LAST_ROUND) return SKILL_NONE;
    //TODO: check if player runs out of energy for a dash
    
    Track *currentTrack = m_pet_ai->m_currTrack;
    
    if (currentTrack->prevTrack->trapReserved()) return SKILL_NONE;
    
    if (currentTrack->prevTrack->prevTrack->trapReserved() ||
        currentTrack->prevTrack->prevTrack->hasTrap()) return SKILL_NONE;
    
    if (currentTrack == m_pet_opponent_of_ai->m_currTrack) return SKILL_NONE;
    if (currentTrack->prevTrack == m_pet_opponent_of_ai->m_currTrack) return SKILL_NONE;
    if (currentTrack->prevTrack->prevTrack == m_pet_opponent_of_ai->m_currTrack) return SKILL_NONE;
    if (currentTrack->prevTrack->prevTrack->prevTrack == m_pet_opponent_of_ai->m_currTrack) return SKILL_NONE;
    
    //if survives to this point, use trap
    return SKILL_TRAP;
}

#pragma mark - Update

void AI::updateAI(float delta)
{
    if (m_quotaSlotCurrentlyInUse > 0 && !m_fullPower) {
        
        m_timeRemainingToReleaseOneQuotaSlot -= delta;
        
//        CCLog_("m_timeRemainingToReleaseOneQuotaSlot is now: %.2f",m_timeRemainingToReleaseOneQuotaSlot);
        
        if (m_timeRemainingToReleaseOneQuotaSlot <= 0.0f)
        {
            m_quotaSlotCurrentlyInUse--;
            
            CCLog_("m_quotaSlotCurrentlyInUse-- which is now:%d",m_quotaSlotCurrentlyInUse);
            
            recalculateTimeRemainingToReleaseOneQuotaSlot();
        }
    }
    
    if (m_tutorialMode) {
        
        AI_STATE currentState = getCurrentState();
        
        switch (currentState) {
            case TUTORIAL_IDLE:
                return;
                break;
                
            case TUTORIAL_DASH:
            case TUTORIAL_DASH_OVER_TRAP:
            case TUTORIAL_DASH_OVER_TRAP_PRACTICE:
            case TUTORIAL_STOMP:
                runStateTutorial(delta);
                break;
                
            default:
                break;
        }
        
        return;
    }
    
    if (!m_isActive) return;
    
    if (m_useSkill != SKILL_NONE) {
        if (m_waitToUseSkill > 0.0f) {
//            CCLog_("waiting to use skill: m_waitToUseSkill:%f",m_waitToUseSkill);
            m_waitToUseSkill -= delta;
        }
        
        if (m_waitToUseSkill <= 0.0f) {
            m_waitToUseSkill = 0.0f;
            
            CCLog_("after waiting completed: m_useSkill:%d",m_useSkill);
            
            useSkill(m_useSkill);
            m_useSkill = SKILL_NONE;
            
//            CCLog_("after using skill: m_useSkill:%d",m_useSkill);
            
            return;
        }
    }
    
    AI_STATE currentState = getCurrentState();
    
    switch (currentState) {
        case AI_NOTHING:
            return;
            break;
            
        case AI_START:
            runStateStart(delta);
            break;
            
        case AI_START_FROM_FRONT:
            runStateStartFromFront(delta);
            break;
            
        case AI_START_FROM_NEAR:
            runStateStartFromNear(delta);
            break;
            
        case AI_START_FROM_BEHIND:
            runStateStartFromBehind(delta);
            break;
            
        case AI_SCOUT:
            runStateScout(delta);
            break;
            
        default:
            break;
    }
    
    return;
}

#pragma mark - Can Skill?

#pragma mark - Use Skill

SKILL AI::useDash()
{
    bool usedSkill = m_pet_ai->useDash();
    if (usedSkill) {
        return SKILL_DASH;
    }
    return SKILL_NONE;
}
SKILL AI::useTrap()
{
    bool usedSkill = m_pet_ai->useTrap();
    if (usedSkill) {
        return SKILL_TRAP;
    }
    return SKILL_NONE;
}
SKILL AI::useStomp()
{
    bool usedSkill = m_pet_ai->useStomp();
    if (usedSkill) {
        return SKILL_STOMP;
    }
    return SKILL_NONE;
}
SKILL AI::useShockwaveStraight1()
{
    bool usedSkill = m_pet_ai->useShockwaveStraight1();
    if (usedSkill) {
        return SKILL_SHOCKWAVE1;
    }
    return SKILL_NONE;
}
SKILL AI::useShockwaveStraight2()
{
    bool usedSkill = m_pet_ai->useShockwaveStraight2();
    if (usedSkill) {
        return SKILL_SHOCKWAVE2;
    }
    return SKILL_NONE;
}


