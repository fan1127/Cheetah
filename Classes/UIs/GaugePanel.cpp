//
//  GaugePanel.cpp
//  Cheetah
//
//  Created by Kaweerut on 2/6/57 BE.
//
//

#include "GaugePanel.h"
#include "../Services/GameContentService.h"
#include "../Entities/PlayerState.h"
#include "../Services/GameAudioService.h"
#include "AppMacros.h"

extern float sizeMultiplier;
extern TargetDevice targetDevice;
extern GameAudioService* g_gameAudio;
using namespace std;

extern GameContentService* g_gameContent;
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;

GaugePanel* GaugePanel::create(string preselectedData) {
    
    GaugePanel *pRet = new GaugePanel();
    if (pRet->init(preselectedData)) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return NULL;
}

bool GaugePanel::init(string preselectedData)
{
    if (!CCNode::init())
    {
        return false;
    }
    
    _barData = this->createGameData(preselectedData);
    
    _currentSpeed = GaugePanel_SPEED;
    _isIncrease = true;

    _gaugeBarBG = UIHelper::getSprite(UIHelper::RaceElementsBarBorder);
    _pinSprite = UIHelper::getSprite(UIHelper::RaceElementsArrows);

    this->addChild(_gaugeBarBG, 1);

    auto upShift = 8*sizeMultiplier;

    _frameWidth = 1013 * sizeMultiplier; //this must be input manually, which is the number of pixels wide that represents the actual m_frame within the bg
    auto _frameHeight = 75.0f * sizeMultiplier;

    m_frame = CCNode::create();
    m_frame->setPosition(ccp(_frameWidth * -0.5f, 0.0f));
    this->addChild(m_frame);

    auto bg = UIHelper::getSprite(UIHelper::RaceElementsPowerBarWhite);
    bg->setOpacity(255);
    bg->setPosition(ccp(-m_frame->getPositionX(), m_frame->getPositionY() + upShift));
    m_frame->addChild(bg);

//    auto pattern = createPatternSprite(/*bg->getContentSize().width, bg->getContentSize().height*/1000, 1000);
//    pattern->setPosition(ccp(-m_frame->getPositionX(), m_frame->getPositionY() + upShift));
//    pattern->getSprite()->removeFromParent();
//    m_frame->addChild(pattern->getSprite());

    // create normal zone
    CCArray *arr = (CCArray*)_barData->objectForKey("normal");
    for (int i=0; i<arr->count(); i++) {
        if (i%2 ==0) {
            CCInteger *firstInt = (CCInteger*)arr->objectAtIndex(i);
            CCInteger *secondInt = (CCInteger*)arr->objectAtIndex(i+1);
            float firstPercent = firstInt->getValue()/100.0f;
            float secondPercent = secondInt->getValue()/100.0f;
            float xPos1 = _frameWidth * firstPercent;
            float xPos2 = _frameWidth * secondPercent;

            auto *sprite = UIHelper::getScalableSprite(UIHelper::RaceElementsBarYellow);
            sprite->setOpacityModifyRGB(false);
            sprite->setOpacity(255);
            sprite->setAnchorPoint(ccp(0.0f, 0.5f));
            sprite->setContentSize(CCSizeMake(xPos2-xPos1, _frameHeight));
            sprite->setPosition(ccp(xPos1, m_frame->getPositionY() + upShift));

            m_frame->addChild(sprite);

            // create borders
            auto borderLeft = UIHelper::getSprite(UIHelper::RaceElementsPowerBarBorder);
            borderLeft->setPosition(ccp(xPos1, m_frame->getPositionY() + upShift));
            auto borderRight = UIHelper::getSprite(UIHelper::RaceElementsPowerBarBorder);
            borderRight->setPosition(ccp(xPos2, m_frame->getPositionY() + upShift));

            m_frame->addChild(borderLeft);
            m_frame->addChild(borderRight);
        }
    }

    // create good zone
    arr = (CCArray*)_barData->objectForKey("good");
    for (int i=0; i<arr->count(); i++) {
        if (i%2 ==0) {
            CCInteger *firstInt = (CCInteger*)arr->objectAtIndex(i);
            CCInteger *secondInt = (CCInteger*)arr->objectAtIndex(i+1);
            float firstPercent = firstInt->getValue()/100.0f;
            float secondPercent = secondInt->getValue()/100.0f;
            float xPos1 = _frameWidth * firstPercent;
            float xPos2 = _frameWidth * secondPercent;

            auto sprite = UIHelper::getScalableSprite(UIHelper::RaceElementsBarGreen);
            sprite->setOpacityModifyRGB(false);
            sprite->setOpacity(255);
            sprite->setAnchorPoint(ccp(0.0f, 0.5f));
            sprite->setContentSize(CCSizeMake(xPos2-xPos1, _frameHeight));
            sprite->setPosition(ccp(xPos1, m_frame->getPositionY() + upShift));

            m_frame->addChild(sprite);


            // create borders
            auto borderLeft = UIHelper::getSprite(UIHelper::RaceElementsPowerBarBorder);
            borderLeft->setPosition(ccp(xPos1, m_frame->getPositionY() + upShift));
            auto borderRight = UIHelper::getSprite(UIHelper::RaceElementsPowerBarBorder);
            borderRight->setPosition(ccp(xPos2, m_frame->getPositionY() + upShift));

            m_frame->addChild(borderLeft);
            m_frame->addChild(borderRight);
        }
    }
    
    _pinSprite->setPosition(ccp(0.0f,
                                0.0f) + m_frame->getPosition());
    this->addChild(_pinSprite, 2);
    
    cursorSprite = CCSprite::createWithSpriteFrameName("oldversion/_tutorial_cursor.png");
    cursorSprite->setPosition(ccp(0, _gaugeBarBG->getContentSize().height*0.6f + cursorSprite->getContentSize().height*0.6f));
    cursorSprite->setVisible(false);
    this->addChild(cursorSprite);
    
//    this->adjustScaleSize();
    
    return true;
}

CCRenderTexture* GaugePanel::createPatternSprite(int width, int height) const
{
    CCSprite* bgtile = UIHelper::getSprite(UIHelper::RaceElementsBarPattern);

    CCRenderTexture* rt = CCRenderTexture::create(8, 8, kCCTexture2DPixelFormat_RGBA8888);

    rt->begin();
    bgtile->setPosition(ccp(rt->getContentSize().width/2, rt->getContentSize().height/2));
    bgtile->visit();
    rt->end();


    ccTexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
    rt->getSprite()->getTexture()->setTexParameters(&params);

    rt->getSprite()->setTextureRect(CCRectMake(0, 0, width, height));

    return rt;

//
//    [rt.sprite setTextureRect:CGRectMake(0, 0, winsize.width, winsize.height)];
//    [self addChild:rt];
//    rt.position = ccp(winsize.width/2, winsize.height/2);
}

GaugePanel::~GaugePanel()
{
    if (_gameDatas)
    {
        _gameDatas->release();
        _gameDatas = NULL;
    }
}

void GaugePanel::adjustScaleSize()
{
    this->setScale(sizeMultiplier);
    this->setScaleX(sizeMultiplier*-1.0f);
}

void GaugePanel::update(float delta)
{

//    auto children = m_frame->getChildren();
//    for (int i=0; i<children->count(); i++)
//    {
//        auto sprite = dynamic_cast<CCSprite *>(children->objectAtIndex(i));
//
//        if (sprite)
//        {
//            CCLog_("Sprite opacity: %d", (int)sprite->getOpacity());
//            //sprite->setOpacity(255);
//        }
//    }

//    this->setOpacity(255);
//
//    _gaugeBarBG->setOpacity(255);

    if (_pinSprite->getPositionX() <= m_frame->getPositionX() && !_isIncrease )
    {
        _currentSpeed *= -1.0f;
        _isIncrease = true;
        
        _pinSprite->setPositionX(0.0f + m_frame->getPositionX());
        g_gameAudio->stopAllEffect();
        g_gameAudio->playEffect(AUDIO_POWER_UP_01);
        
        return;
    }
    else if (_pinSprite->getPositionX()  >= m_frame->getPositionX() + _frameWidth && _isIncrease)
    {
        _currentSpeed *= -1.0f;
        _isIncrease = false;
        
        _pinSprite->setPositionX(_frameWidth + m_frame->getPositionX());
        g_gameAudio->stopAllEffect();
        g_gameAudio->playEffect(AUDIO_POWER_UP_02);
        
        return;
    }
    
    CCFloat *myFloat = (CCFloat*)_barData->objectForKey("speed");
    _pinSprite->setPositionX(_pinSprite->getPositionX()+ (_currentSpeed*delta*myFloat->getValue()));
}

//GaugePanel::~GaugePanel()
//{
//
//}

//void GaugePanel::cleanup() {
//    
//    
//    
//}

#include <stdint.h>
uint32_t xor128(uint32_t seed) {
    static uint32_t x = seed;//123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;
    
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    return w = w ^ (w >> 19) ^ t ^ (t >> 8);
}

CCDictionary* GaugePanel::createGameData(string preselectedData) {
    
    if (_gameDatas == NULL) {
        _gameDatas = new CCArray();
        _gameDatas->init();
        _gameDatas->retain();
    }
    
    _gameDatas->removeAllObjects();
    
    int preselectedIndex = -1;
    int countIndex = -1;
    
    for (vector<GDGameStartBarVO>::iterator it = g_gameContent->GameStartBar.begin(); it != g_gameContent->GameStartBar.end(); ++it) {
        GDGameStartBarVO _vo = *it;
        if (_vo.type == "" || g_playerState->player.balance.fan < _vo.min_fans) {
            continue;
        }
        
        CCDictionary *dict = CCDictionary::create();
        
        //GOOD START
        CCArray *good_start_array = CCArray::create();
        for (vector<GSBRange>::iterator it = _vo.good_start.begin(); it != _vo.good_start.end(); ++it)
        {
            GSBRange _range = *it;
            good_start_array->addObject(CCInteger::create(_range.from));
            good_start_array->addObject(CCInteger::create(_range.to));
        }
        dict->setObject(good_start_array, "good");
        
        //NORMAL START
        CCArray *normal_start_array = CCArray::create();
        for (vector<GSBRange>::iterator it = _vo.normal_start.begin(); it != _vo.normal_start.end(); ++it)
        {
            GSBRange _range = *it;
            normal_start_array->addObject(CCInteger::create(_range.from));
            normal_start_array->addObject(CCInteger::create(_range.to));
        }
        dict->setObject(normal_start_array, "normal");
        
        //SPEED
        dict->setObject(CCFloat::create(_vo.speed), "speed");
        
        countIndex++;
        
        if (preselectedData != "" &&
            preselectedData == _vo.type)
        {
            preselectedIndex = countIndex;
        }
        
        _gameDatas->addObject(dict);
    }
    
    int index;
    
    if (preselectedIndex != -1) {
        index = preselectedIndex;
    } else {
        //convert opponent uuid to seed
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, "%s", g_opponentState->player.uuid.c_str());
        
        unsigned int sum = 0;
        for (unsigned int i=0; i<strlen(_buffer); i++) {
            sum += (unsigned int)_buffer[i];
        }
        
        index = sum % _gameDatas->count();
    }
    
    //CCDictionary* dict = (CCDictionary*)_gameDatas->objectAtIndex(index);
    return (CCDictionary*)_gameDatas->objectAtIndex(index);
}

void GaugePanel::show()
{
    this->setVisible(true);
}

void GaugePanel::hide()
{
    this->setVisible(false);
}

void GaugePanel::start()
{
    scheduleUpdate();
}

void GaugePanel::stop()
{
    unscheduleUpdate();
}

bool GaugePanel::isReady()
{
    if (_pinSprite->getPositionX() != m_frame->getPositionX()) {
        return false;
    }
    
    return true;
}

void GaugePanel::resetData()
{
    if (!this->isReady()) {
        _currentSpeed = GaugePanel_SPEED;
        _isIncrease = true;
        _pinSprite->setPositionX(m_frame->getPositionX());
    }
}

GaugeRESULT GaugePanel::getCurrentPinResult()
{
    GaugeRESULT res = GaugeRESULT_NONE;
    
    float pinPosX = _pinSprite->getPositionX();
    
    // create normal zone
    CCArray *arr = (CCArray*)_barData->objectForKey("normal");
    for (int i=0; i<arr->count(); i++) {
        if (i%2 ==0) {
            CCInteger *firstInt = (CCInteger*)arr->objectAtIndex(i);
            CCInteger *secondInt = (CCInteger*)arr->objectAtIndex(i+1);
            float firstPercent = firstInt->getValue()/100.0f;
            float secondPercent = secondInt->getValue()/100.0f;
            float xPos1 = _frameWidth*firstPercent + m_frame->getPositionX();
            float xPos2 = _frameWidth*secondPercent + m_frame->getPositionX();
            if (pinPosX >= xPos1 && pinPosX <= xPos2) {
                res = GaugeRESULT_NORMAL;
                break;
            }
        }
    }
    
    
    if (res == GaugeRESULT_NONE)
    {
        // create normal zone
        arr = (CCArray*)_barData->objectForKey("good");
        for (int i=0; i<arr->count(); i++) {
            if (i%2 ==0) {
                CCInteger *firstInt = (CCInteger*)arr->objectAtIndex(i);
                CCInteger *secondInt = (CCInteger*)arr->objectAtIndex(i+1);
                float firstPercent = firstInt->getValue()/100.0f;
                float secondPercent = secondInt->getValue()/100.0f;
                float xPos1 = _frameWidth*firstPercent + m_frame->getPositionX();
                float xPos2 = _frameWidth*secondPercent + m_frame->getPositionX();
                if (pinPosX >= xPos1 && pinPosX <= xPos2) {
                    res = GaugeRESULT_GOOD;
                    break;
                }
            }
        }
    }

    return res;
}

void GaugePanel::restart()
{
//    _pinSprite->setPositionX(0.0f);
}

void GaugePanel::animateCursor()
{
    //    cursorSprite->setVisible(true);
    //    CCSequence *sequence = CCSequence::create(CCMoveBy::create(0.1f, ccp(0, 10)),
    //                                              CCMoveBy::create(0.1f, ccp(0, -10)),
    //                                              NULL);
    //
    //    cursorSprite->runAction(CCRepeatForever::create(sequence));
}

void GaugePanel::stopAnimatedCursor()
{
//    cursorSprite->stopAllActions();
//    cursorSprite->setVisible(false);
}
