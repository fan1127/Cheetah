//
//  GachaResult.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/31/2557 BE.
//
//

#include "GachaResult.h"

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

#include "../Extensions/Modal/CCModalController.h"
#include "../Helpers/CreateSpriteHelper.h"

extern float sizeMultiplier;

USING_NS_CC;
using namespace extension;

GachaResult* GachaResult::create(GDPetVO petVO, GachaPanel* gachaPanel)
{
    
    GachaResult *obj = new GachaResult();
    if (!obj->init(petVO, gachaPanel)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
}


GachaResult* GachaResult::create(GDItemVO itemVO, GachaPanel* gachaPanel)
{
    GachaResult *obj = new GachaResult();
    if (!obj->init(itemVO, gachaPanel)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
}

bool GachaResult::init(GDPetVO petVO, GachaPanel* gachaPanel)
{
    if (!GachaResult::init(gachaPanel)) {
        return false;
    }
    
    m_petVO = petVO;
    
//    CCString *fileName = CCString::createWithFormat("%s.png",m_petVO.src.c_str());
//    m_itemSprite = CCSprite::createWithSpriteFrameName(fileName->getCString());
//    m_itemSprite->setPosition(this->getContentSize()*0.5f);
//    m_itemSprite->setOpacity(0);
//    this->addChild(m_itemSprite);
    
    m_petSpine = g_sceneManager->sharedSkeletonData->getSkeleton("common/pet/", m_petVO.src.c_str());
    m_petSpine->setPosition(this->getContentSize()*0.5f - ccp(200.f * sizeMultiplier, 100.f * sizeMultiplier));
//    m_petSpine->setPosition(this->getContentSize()*0.5f - ccp(200.f * sizeMultiplier, 200.f * sizeMultiplier));
    m_petSpine->setScale(sizeMultiplier*PET_SCALE*2);
    m_petSpine->setOpacity(0);
    this->addChild(m_petSpine, 2);

    CCParticleSystemQuad* particle = CCParticleSystemQuad::create("common/vfx/gacha_star.plist");
    particle->setScale(5.f);
   
    particle->setPosition(ccp(0, 60.f));
    particle->resetSystem();
    m_petSpine->addChild(particle);

    m_glowSprite1->setPosition(m_petSpine->getPosition() + ccp(0, 50.f * sizeMultiplier));
    m_glowSprite2->setPosition(m_glowSprite1->getPosition());

    CCSprite* stat1 = UIHelper::getSprite(UIHelper::AnimalStatsSpeedIcon);
    CCSprite* stat2 = UIHelper::getSprite(UIHelper::AnimalStatsMobilityIcon);
    CCSprite* stat3 = UIHelper::getSprite(UIHelper::AnimalStatsEnergyIcon);

//    stat1->setPosition(m_petSpine->getPosition() + ccp(300.f*sizeMultiplier, 200.f*sizeMultiplier));
//    stat2->setPosition(m_petSpine->getPosition() + ccp(300.f*sizeMultiplier, 100.f*sizeMultiplier));
//    stat3->setPosition(m_petSpine->getPosition() + ccp(300.f*sizeMultiplier, 0.f*sizeMultiplier));
    stat1->setPosition(m_petSpine->getPosition() + ccp(300.f*sizeMultiplier, 100.f*sizeMultiplier));
    stat2->setPosition(m_petSpine->getPosition() + ccp(300.f*sizeMultiplier, 0.f*sizeMultiplier));
    stat3->setPosition(m_petSpine->getPosition() + ccp(300.f*sizeMultiplier, -100.f*sizeMultiplier));

    addChild(stat1);
    addChild(stat2);
    addChild(stat3);

    CCLabelBMFont* stat1Score = CCLabelHelper::createBMFont(CCString::createWithFormat("%d", petVO.base_top_speed)->getCString(), kFontSizeLarge
                                                            , true);
    stat1Score->setPosition(ccp(80.f*sizeMultiplier, 10.f*sizeMultiplier));
    stat1Score->setAnchorPoint(ccp(-1.f, 0.0f));
    stat1->addChild(stat1Score);

    CCLabelBMFont* stat2Score = CCLabelHelper::createBMFont(CCString::createWithFormat("%d", petVO.base_corner_speed)->getCString(), kFontSizeLarge, true);
    stat2Score->setPosition(ccp(80.f*sizeMultiplier, 10.f*sizeMultiplier));
    stat2Score->setAnchorPoint(ccp(-1.f, 0.0f));
    stat2->addChild(stat2Score);

    CCLabelBMFont* stat3Score = CCLabelHelper::createBMFont(CCString::createWithFormat("%d", petVO.base_jumping)->getCString(), kFontSizeLarge, true);
    stat3Score->setPosition(ccp(80.f*sizeMultiplier, 10.f*sizeMultiplier));
    stat3Score->setAnchorPoint(ccp(-1.f, 0.0f));
    stat3->addChild(stat3Score);

    CCSprite* preferredTrack = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("Animal_stats/_icon_%s.png", petVO.track_preference.c_str())->getCString());
    preferredTrack->setPosition(stat2->getPosition() + ccp(280*sizeMultiplier, -50.0f*sizeMultiplier));
    addChild(preferredTrack);

    CCSprite* preferIcon = UIHelper::getSprite(UIHelper::AnimalStatsLikeIcon);
    preferIcon->setPosition(preferredTrack->getContentSize() * 0.5f);
    preferredTrack->addChild(preferIcon);

    //Animal Class Label
    CCString* str = CCString::createWithFormat("%d", petVO.base_top_speed + petVO.base_corner_speed + petVO.base_jumping);
    CCSprite* horsePower = CreateSpriteHelper::getHorsePowerSprite(str->getCString(), petVO.rarity_stars);
    horsePower->setAnchorPoint(ccp(-1.f, 0.0f));
    horsePower->setPosition(m_petSpine->getPosition() + ccp(0.f*sizeMultiplier, -150.f*sizeMultiplier));

//    horsePower->setAnchorPoint(stat1Score->getAnchorPoint());
//    horsePower->setPosition(m_petSpine->getPosition() + ccp(330.f*sizeMultiplier, 320.f*sizeMultiplier));
    addChild(horsePower);

    CCLabelBMFont* petNameLabel = CCLabelHelper::createBMFont(petVO.title.c_str(), kFontSizeVeryLarge, true);
    petNameLabel->setAlignment(kCCTextAlignmentLeft);
    petNameLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    petNameLabel->setPosition(stat1->getPosition() + ccp(preferredTrack->getContentSize().width * -0.5f, 130.0f * sizeMultiplier));
//    petNameLabel->setPosition(m_petSpine->getPosition() + ccp(0.f*sizeMultiplier, 450.f*sizeMultiplier));
    addChild(petNameLabel);

    CCSkeletonAnimation* raritySpine = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "ball_gacha_open");
//    raritySpine->setPosition(ccp(this->getContentSize().width * 0.5f, (this->getContentSize().height * 0.5f) + (450.f * sizeMultiplier)));
    raritySpine->setPosition(ccp(this->getContentSize().width * 0.5f, (this->getContentSize().height * 0.5f) + (400.f * sizeMultiplier)));
    raritySpine->setScale(sizeMultiplier);
    raritySpine->clearAnimation();
    if(petVO.rarity_stars > 5)
        raritySpine->setAnimation("title_legendary", false);
    else if(petVO.rarity_stars == 5)
        raritySpine->setAnimation("title_super_rare", false);
    else if(petVO.rarity_stars == 4)
        raritySpine->setAnimation("title_very_rare", false);
    else
        raritySpine->setAnimation("rare", false);
    this->addChild(raritySpine);

    //play effect habitat
    if(m_petVO.sound_src.compare("") == 0) g_gameAudio->playEffect(AUDIO_PIG_GRUNT);
    else {
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, "sfx/animalx/%s%s", m_petVO.sound_src.c_str(), AUDIO_EXT);
        g_gameAudio->playEffect(_buffer);
    }
    
    return true;
}

bool GachaResult::init(GDItemVO itemVO, GachaPanel* gachaPanel)
{
    if (!GachaResult::init(gachaPanel)) {
        return false;
    }
    
    m_itemVO = itemVO;
    
    CCString *fileName = CCString::createWithFormat("%s",itemVO.src.c_str());
    m_itemSprite = CCSprite::createWithSpriteFrameName(fileName->getCString());
    m_itemSprite->setPosition(this->getContentSize()*0.5f);
    m_itemSprite->setOpacity(0);
    this->addChild(m_itemSprite, 2);
    return true;
}

bool GachaResult::init(GachaPanel* gachaPanel)
{
    if (!CCLayerColor::initWithColor(ccc4(0, 0, 0, 200))) {
        return false;
    }
    m_gachaPanel = gachaPanel;

    //setTouchEnabled(true);
    //setTouchMode(kCCTouchesOneByOne);
    //setTouchPriority(kCCMenuHandlerPriority);
    
    this->ignoreAnchorPointForPosition(false);
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    
    m_glowSprite1 = CCSprite::createWithSpriteFrameName("_gacha_glow_item.png");
    m_glowSprite1->setPosition(this->getContentSize()*0.5f);
    m_glowSprite1->setScale(4);
    m_glowSprite1->setOpacity(0);
    
    m_glowSprite2 = CCSprite::create("common/vfx/spinning_glow.png");
    m_glowSprite2->setPosition(this->getContentSize()*0.5f);
    m_glowSprite2->setScale(sizeMultiplier*2);
    m_glowSprite2->setOpacity(0);
    
    this->addChild(m_glowSprite2);
    this->addChild(m_glowSprite1);

    CCMenu* okayButton = HUDPanel::createButtonHelper("OK", this, menu_selector(GachaResult::onDismissResultButton));
    okayButton->setPosition(ccp(this->getContentSize().width * 0.5f, (this->getContentSize().height * 0.5f) - (400.f * sizeMultiplier)));
    this->addChild(okayButton);

    return true;
}

void GachaResult::presentResult(cocos2d::CCNode *onNode, bool animated)
{
    onNode->addChild(this);
    this->setPosition(onNode->getContentSize()*0.5f);
    this->setOpacity(0);
    //Show Gacha Result
    this->runAction(CCSequence::create(CCFadeTo::create(0.2f, 20),
                                       //CCCallFunc::create(this, callfunc_selector(GachaResult::playStarPaticle)),
                                       CCCallFunc::create(this, callfunc_selector(GachaResult::animateGlow)),
                                       CCDelayTime::create(0.2f),
                                       CCCallFunc::create(this, callfunc_selector(GachaResult::animateGlow2)),
                                       CCCallFunc::create(this, callfunc_selector(GachaResult::animatePetOrItem)),
                                       NULL));
}

void GachaResult::dismissResult(bool animated)
{
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
//    m_glowSprite2->stopAllActions();
    CCModalController::hideAllChildrensAnimated(this, 0.5f);
    
    this->runAction(CCSequence::create(CCFadeOut::create(0.5f),
                                       CCRemoveSelf::create(),
                                       NULL));

    m_gachaPanel->onResultDismissed();
}

void GachaResult::onDismissResultButton()
{
    dismissResult(true);
}

void GachaResult::animateGlow()
{
    m_glowSprite1->runAction(CCFadeIn::create(0.2f));
    m_glowSprite2->runAction(CCFadeIn::create(0.2f));
}

void GachaResult::animateGlow2()
{
    m_glowSprite1->runAction(CCRepeatForever::create(CCSequence::create(CCFadeOut::create(1.f),
                                                                        CCFadeIn::create(1.f),
                                                                        NULL)));
    
    m_glowSprite2->runAction(CCRepeatForever::create(CCRotateBy::create(5, 360)));
}

void GachaResult::animatePetOrItem()
{
    if (m_petSpine) {
        m_petSpine->runAction(CCFadeIn::create(0.3f));
        m_petSpine->setAnimation("habitat_eat", false);
        m_petSpine->addAnimation("habitat_idle_stand", true);
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, g_gameContent->Word["event_got_pet_msg"].c_str(), m_petVO.title.c_str());
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
    } else {
        m_itemSprite->runAction(CCFadeIn::create(0.3f));
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, g_gameContent->Word["event_got_item_msg"].c_str(), m_itemVO.title.c_str());
        g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
    }
}

/*
bool GachaResult::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    CCPoint point = this->convertTouchToNodeSpace(touch);
    CCRect rect = this->boundingBox();
    if (rect.containsPoint(point)) {
        this->dismissResult(true);
        return true;
    }
    return false;
}
*/


void GachaResult::playStarPaticle()
{
    for (int i = 0; i < arc4random()%12; i++) {
        CCSprite *star = CCSprite::create("common/vfx/star.png");
        star->setPosition(this->getContentSize()*0.5f);
        star->setScale(sizeMultiplier*0.5f*(0.4 + (arc4random()%7)/10.f));
        
        if (m_petSpine) {
            this->addChild(star, m_petSpine->getZOrder() - 1);
        } else {
            this->addChild(star, m_itemSprite->getZOrder() - 1);
        }
        
        int angle = arc4random()%360;
        float fangle = angle*M_PI/180;
        
        float distance = MAX(this->getContentSize().width, this->getContentSize().height)/2.f + MAX(star->getContentSize().width, star->getContentSize().height);
        
        CCPoint destinationPoint = star->getPosition() + ccp(distance*cosf(fangle), distance*sinf(fangle));
        
        star->runAction(CCScaleTo::create(0.7f, star->getScale()*(1.5 + arc4random()%3)));
        star->runAction(CCSequence::create(CCMoveTo::create(0.7f, destinationPoint),
                                           CCRemoveSelf::create(),
                                           NULL));
    }
    
    this->runAction(CCSequence::create(CCDelayTime::create(0.1f),
                                       CCCallFunc::create(this, callfunc_selector(GachaResult::playStarPaticle)),
                                       NULL));
    
    //scheduleOnce(schedule_selector(GachaResult::dismissResult), TIME_TO_AUTO_DISMISS_GACHA_RESULT);
    
}
