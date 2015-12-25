//
//  GachaWheel.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/24/2557 BE.
//
//

#include "GachaWheel.h"
#include "GachaPanel.h"

#include "../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

//#include "../Entities/TutorialManager.h"
//extern TutorialManager *g_gameTutorial;

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

USING_NS_CC;

GachaWheel* GachaWheel::create(GachaType type, cocos2d::CCSize size) {
    
    GachaWheel *obj = new GachaWheel();
    if (!obj->init(type, size)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool GachaWheel::init(GachaType type, cocos2d::CCSize size)
{
    if (!CCNode::init()) {
        return false;
    }
    
    setDefaultValues();
    
    m_delegate = NULL;
    m_gachaType = type;
    
    m_rotation = 0;
    
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    this->setContentSize(size);
    
    contentNode = CCNode::create();
    contentNode->setAnchorPoint(ccp(0.5f, 0.5f));
    contentNode->setPosition(this->getContentSize()*0.5f);
    contentNode->setContentSize(this->getContentSize());
    this->addChild(contentNode);
    
    m_items = CCArray::create();
    m_items->retain();
    
    for (int i = 0; i < 18; i++) {
        GachaItem *item = GachaItem::create(type ,(GachaTypeNumber)(i%2), this->getContentSize().height*0.5f , i);
        item->setPosition(this->getContentSize()*0.5f);
        if (type == GachaTypeFree) {
            item->setRotation(-i*(180/9));
        } else {
            item->setRotation(i*(180/9));
        }
        
        contentNode->addChild(item);
        m_items->addObject(item);
    }
    
    this->scheduleUpdate();
    
    return true;
}

GachaWheel::~GachaWheel()
{
    if (m_items) {
        m_items->release();
        m_items = NULL;
    }
}

void GachaWheel::update(float delta)
{
    
//    CCLog_("update gacha");
    
    float rotation = 0;
    if (m_gachaWheelState == GachaWheelStateShowCase) {
        
        m_rotation += delta*m_showcaseVelocity;
        
    } else if (m_gachaWheelState == GachaWheelStateStart) {
        
        if (m_currentTime < m_increaseSpeedDuration) {
            
            rotation = (delta*m_velocity + 0.5*m_increaseAcceleration*pow(delta, 2));
            m_rotation += rotation;
            m_velocity = m_velocity + m_increaseAcceleration*delta;
            
        } else {
            
            if (!isCalurateDecreased) {
                
                m_decreaseAcceleration = -m_velocity/m_decreaseSpeedDuration;
                float estimateRotation = m_velocity*m_decreaseSpeedDuration + 0.5f*m_decreaseAcceleration*pow(m_decreaseSpeedDuration, 2);
                estimateRotation = ((int)estimateRotation/360)*360;
                
                if (m_gachaType == GachaTypeFree) {
                    estimateRotation = estimateRotation + (m_selectedIndex)*20 - 90 + 2*360 - (int)m_rotation%360 + m_extendRotation;
                } else {
                    estimateRotation = estimateRotation - (m_selectedIndex)*20 + 90 + 2*360 - (int)m_rotation%360 + m_extendRotation;
                }
                
                m_estimateRemainingRotation = estimateRotation;
                m_remainingRotation = estimateRotation;
                
                m_decreaseAcceleration = -pow(m_velocity, 2)/(2*estimateRotation);
                m_decreaseSpeedDuration = -m_velocity/m_decreaseAcceleration;
                
                isCalurateDecreased = true;
            }
            
            rotation = (delta*m_velocity + 0.5*m_decreaseAcceleration*pow(delta, 2));
            
            m_rotation += rotation;
            m_velocity = m_velocity + m_decreaseAcceleration*delta;
            
            m_remainingRotation -= rotation;

        }
        
        m_currentTime += delta;
        if (m_currentTime >= (m_increaseSpeedDuration + m_decreaseSpeedDuration)) {
            finish();
//            beforeFinish();
        }
        
    } else if (m_gachaWheelState == GachaWheelStateBeforeFinish){
        
        float v = m_extendRotation*5/m_beforeFinshDuration;
        float remainingTime =  (m_increaseSpeedDuration + m_decreaseSpeedDuration + m_beforeFinshDuration) - m_currentTime;

        if (remainingTime > 0.6*m_beforeFinshDuration || remainingTime < 0.2*m_beforeFinshDuration) {
            rotation = -v*delta;
            m_rotation += rotation;
//            m_rotation -= v*delta;
        } else {
            rotation = v*delta;
            m_rotation += rotation;
//            m_rotation += v*delta;
        }
        
        m_currentTime += delta;
        if (m_currentTime >= (m_increaseSpeedDuration + m_decreaseSpeedDuration + m_beforeFinshDuration)) {
            finish();
        }
    } else {
        
    }
    
    //play effect tick
    if(rotation>0) {
        if((int)m_rotation%20<rotation) {
            g_gameAudio->playEffect(AUDIO_GACHA_TICK);
        }
    }
    
    
    contentNode->setRotation(m_rotation);
    CCObject *object;
    CCARRAY_FOREACH(m_items, object) {
        GachaItem *item = (GachaItem *)object;
        item->updateRotation(m_rotation, m_selectedIndex, m_selectedPetVO, m_selectedItemVO, m_remainingRotation);
    }
    
}

void GachaWheel::setDefaultValues()
{
    isCalurateDecreased = false;
    
    m_selectedIndex = arc4random()%18;
    
    m_currentTime = 0;
    m_increaseSpeedDuration = 1.5f;
    m_decreaseSpeedDuration = 1.5f;
    
    m_beforeFinshDuration = 0.4f*m_decreaseSpeedDuration/3;
    
    m_increaseAcceleration = 250;
    
    m_lockVelocity = 10;
    m_showcaseVelocity = 2;
    m_velocity = 10;
    
    m_extendRotation = 0;//48/m_decreaseSpeedDuration;
    
    m_remainingRotation = 100000;
}

void GachaWheel::start(GDPetVO selectedPetVO)
{
    if (m_gachaWheelState == GachaWheelStateStart) {
        return;
    }
    
    this->stopAllActions();
    
    setDefaultValues();
    m_selectedPetVO = selectedPetVO;
    m_gachaWheelState = GachaWheelStateStart;
}

void GachaWheel::start(GDItemVO selectedItemVO)
{
    if (m_gachaWheelState == GachaWheelStateStart) {
        return;
    }
    
    this->stopAllActions();
    
    setDefaultValues();
    m_selectedItemVO = selectedItemVO;
    m_gachaWheelState = GachaWheelStateStart;
}

void GachaWheel::beforeFinish()
{
    m_gachaWheelState = GachaWheelStateBeforeFinish;
}

void GachaWheel::finish()
{
    g_gameAudio->stopAllEffect();
    g_gameAudio->stopMusic();
    g_gameAudio->playMusicFadeIn(MUSIC_GACHA, 5.0f);
    g_gameAudio->playEffect(AUDIO_GACHA_PRIZE);
    
    m_gachaWheelState = GachaWheelStateFinish;
    
    if (m_delegate) {
        if (m_gachaType == GachaTypeFree) {
            m_delegate->gachaWheelDidEndFreeSpining(this, m_selectedItemVO);
        } else {
            m_delegate->gachaWheelDidEndPremiumSpining(this, m_selectedPetVO);
            
//            if(g_gameTutorial->tutorialMode) {
//                if(g_gameTutorial->gachaMode) {
//                    g_playerState->checklist("speech24");
//                }
//            }
        }
        
    }
    
    this->runAction(CCSequence::create(CCDelayTime::create(3),
                                       CCCallFunc::create(this, callfunc_selector(GachaWheel::showCase)),
                                       NULL));
    
}

void GachaWheel::showCase() {
    
    m_gachaWheelState = GachaWheelStateShowCase;
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if(!_scene) return;
    
    if (_scene->gachaPanel) {
        CREATE_CHAR_BUFFER(_buffer, 16);
        float price = g_gameContent->GameConfig["NORMAL_GACHA_PRICE"];
        snprintf(_buffer, _buffer_size, "%.0f", price);
        //_scene->gachaPanel->premiumLabel->setString(_buffer);
        //_scene->gachaPanel->premiumLabel->setPosition(ccp(_scene->gachaPanel->premiumButton->getContentSize().width*0.5f - _scene->gachaPanel->premiumLabel->getContentSize().width*0.6f, _scene->gachaPanel->premiumButton->getContentSize().height*0.5f));
        //_scene->gachaPanel->gemSprite->setVisible(true);
    }

}

void GachaWheel::visit() {
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    CCRect _rect = CCRect(0,
                          0,
                          _screenSize.width,
                          _screenSize.height);
    
    CCPoint _pos = this->getPosition();
    CCPoint _scl = ccp(this->getScaleX(), this->getScaleY());
    CCNode *_node = this;
    int _count=0;
    
    while (CCNode *_parent = _node->getParent()) {
        if (_count==0) {
            _pos = _parent->convertToWorldSpace(_pos);
        }
        _count++;
        _scl = ccp(fabsf(_scl.x*_parent->getScaleX()), fabsf(_scl.y*_parent->getScaleY()));
        _node = _parent;
    }
    
    CCSize _siz = CCSize(getContentSize().width*_scl.x, getContentSize().height*_scl.y);
    CCPoint _ach = getAnchorPoint();
    
    if (m_gachaType == GachaTypeFree) {
        _rect = CCRect(_pos.x-((_siz.width)*_ach.x),
                       _pos.y-((_siz.height)*_ach.y),
                       _siz.width*0.5f,
                       _siz.height);
    } else {
        _rect = CCRect(_pos.x-((_siz.width)*_ach.x) + _siz.width*0.5f,
                       _pos.y-((_siz.height)*_ach.y),
                       _siz.width,
                       _siz.height);
    }
    
    kmGLPushMatrix();
    glEnable(GL_SCISSOR_TEST);
    
    cocos2d::CCEGLView::sharedOpenGLView()->setScissorInPoints(_rect.origin.x,
                                                               _rect.origin.y,
                                                               _rect.size.width,
                                                               _rect.size.height);
    
    CCNode::visit();
    glDisable(GL_SCISSOR_TEST);
    kmGLPopMatrix();
    
}
