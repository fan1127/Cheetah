//
//  CollectableItems.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/11/2556 BE.
//
//

#include "CollectableItems.h"
USING_NS_CC;
using namespace std;

#include "../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

extern float sizeMultiplier;

#include "../Extensions/ExplosionParticle.h"

CollectableItem *CollectableItem::createWithSpriteFrameName(const char *framename_) {
    
    CollectableItem *pRet = new CollectableItem();
    pRet->initWithSpriteFrameName(framename_);
    pRet->autorelease();
    return pRet;
    
}

bool CollectableItem::initWithSpriteFrameName(const char *framename_) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_itemSprite = CCSprite::createWithSpriteFrameName(framename_);
    this->addChild(m_itemSprite);
    
    this->setContentSize(m_itemSprite->getContentSize());
    
    collectingAmount = 0;
    
    return true;
    
}

bool CollectableItem::updateMoving(float delta_) {
    
//    m_movingSpeed[0] += delta_*50;
//    m_movingSpeed[1] += delta_*100;
//    
//    CCPoint _currentPosition = this->getPosition();
//    
//    float x1, y1, z1, x2, y2, z2, x3, y3, z3, a;
//    float _currentScale, _movingSpeed;
//    
//    x1 = m_destPosition[1].x - m_initPosition.x;
//    y1 = m_destPosition[1].y - m_initPosition.y;
//    z1 = sqrtf(x1*x1 + y1*y1);
//    
//    x2 = m_destPosition[1].x - m_destPosition[0].x;
//    y2 = m_destPosition[1].y - m_destPosition[0].y;
//    z2 = sqrtf(x2*x2 + y2*y2);
//    
//    x3 = m_destPosition[0].x - _currentPosition.x;
//    y3 = m_destPosition[0].y - _currentPosition.y;
//    z3 = sqrtf(x3*x3 + y3*y3);
//    
//    _currentScale = ((z2/z1)*(m_initScale-1))+1;
//    this->setScale(_currentScale);
//    
//    int _reached = 0;
//    _movingSpeed = m_movingSpeed[1];
//    if (z3 >= m_movingSpeed[0]) {
//        a = atan2f(y3, x3);
//        _currentPosition.x += m_movingSpeed[0] * cosf(a);
//        _currentPosition.y += m_movingSpeed[0] * sinf(a);
//        
//        this->setPosition(_currentPosition);
//    }else {
//        _reached ++;
//        _movingSpeed = m_movingSpeed[1] + m_movingSpeed[0];
//    }
//    
//    if (z2 >= m_movingSpeed[1]) {
//        a = atan2f(y2, x2);
//        m_destPosition[0].x += _movingSpeed * cosf(a);
//        m_destPosition[0].x += _movingSpeed * sinf(a);
//    }else {
//        _reached ++;
//    }
//    
//    if (_reached < 2) {
//        
//        return true;
//    }
    
    CCPoint currentPosition = this->getPosition() + ccp(m_velocityX*delta_, m_velocityY*delta_);
    this->setPosition(currentPosition);
    
    return false;
}

void CollectableItem::setDest(CCPoint dest_position_, float dest_scale_) {

    
    m_velocityX = 100;
    m_velocityY = 100;
    
    m_initScale = this->getScale();
    m_destScale = dest_scale_;
    
    m_initPosition = this->getPosition();
    m_destPosition[1] = dest_position_;
    float x = m_destPosition[1].x - m_initPosition.x;
    float y = m_destPosition[1].y - m_initPosition.y;
    float d = sqrtf(x*x + y*y);
    m_destPosition[0] = dest_position_ + ccp(d, 0.0f);
    m_destPosition[1] = dest_position_;
    
    m_movingSpeed[0] = 1.0f;
    m_movingSpeed[1] = 1.0f;
}

CollectableItems* CollectableItems::create(std::string icon_framename_) {
    
    CollectableItems* pRet = new CollectableItems();
    pRet->init(icon_framename_);
    pRet->autorelease();
    return pRet;
    
}

bool CollectableItems::init(std::string icon_framename_) {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    m_inst = this;
    m_sender = NULL;
    
    m_icon_framename = icon_framename_;
//    m_items.clear();
    m_running = false;
//    scheduleUpdate();
    
    return true;
    
}

void CollectableItems::update(float delta_) {
    

    
}

void CollectableItems::setDest(CCPoint dest_position_, float dest_scale_) {
    
    m_destPosition = dest_position_;
    m_destScale = dest_scale_;
    
}

bool CollectableItems::add(CCPoint position_, float scale_, int collecting_amount_,GDBuildingVO bVo) {
    float maximum_storage;
    CollectableItem *_item = CollectableItem::createWithSpriteFrameName(m_icon_framename.c_str());
    _item->collectingAmount = collecting_amount_;
    _item->setPosition(position_);
    _item->setScale(scale_);
    _item->setDest(m_destPosition, m_destScale);
    this->addChild(_item);

    CCPoint point1 = position_ + ccp(0, _item->getContentSize().height*scale_);
    CCPoint point2 = m_destPosition - ccp( _item->getContentSize().width, _item->getContentSize().height);
    int maximum_frame;
    char const * sprite_name;
    char const * sprite_name_index;


    // Coins and Foods float fly to HUD
    if(m_icon_framename.find("coin") != std::string::npos || m_icon_framename.find("food") != std::string::npos) {
        if (m_icon_framename.find("coin") != std::string::npos) {
            maximum_storage = bVo.param_2;
            maximum_frame = 7;
            sprite_name = "oldversion/_effect_coin_roll_0.png";
            sprite_name_index = "oldversion/_effect_coin_roll_%d.png";
        }else{
            maximum_storage = bVo.param_1;
            maximum_frame = 11;
            sprite_name = "oldversion/bag_roll_0.png";
            sprite_name_index = "oldversion/bag_roll_%d.png";
        }
        _item->runAction(CCSequence::create(CCScaleTo::create(0.1f, scale_ * 1.5f),
                                            CCCallFuncO::create(this, callfuncO_selector(CollectableItems::explosionParticle), _item),
                                            CCDelayTime::create(0.1f),
                                            CCRemoveSelf::create(),
                                            NULL));


        float distance = MIN(this->getContentSize().width, this->getContentSize().height) * 0.2f;

        int startAngle = (10 + arc4random() % 16);
        int coin_animate_amount;

        float coin_percentage =   (float)collecting_amount_/maximum_storage ;
        //CCLog_("coin percent: %.3f, coin collect: %d, coin max: %.3f",coin_percentage,collecting_amount_,maximum_storage);
        if (coin_percentage < 0.3f)
            coin_animate_amount = 50;
        else if(coin_percentage < 0.7)
            coin_animate_amount = 180;
        else
            coin_animate_amount = 360;


        for (int i = 0; i < coin_animate_amount; i += (10 + arc4random() % 16)) {
            CCSprite *collect_sprite = CCSprite::createWithSpriteFrameName(sprite_name);
            CCArray *filenames = CCArray::create();
            int num = rand() % (maximum_frame+1);

            for (int i = 0; i <= maximum_frame; i++) {
                filenames->addObject(CCString::createWithFormat(sprite_name_index, ((i + num) % (maximum_frame+1))));
            }
            CCAnimation *animation = CCAnimation::create();
            CCObject *obj;
            CCARRAY_FOREACH(filenames, obj) {
                    CCString *filename = (CCString *) obj;
                    animation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(filename->getCString()));
                }
            filenames = CCArray::create();
            animation->setLoops(10);
            animation->setDelayPerUnit(0.04f);
            CCAnimate *animate = CCAnimate::create(animation);
            collect_sprite->runAction(CCRepeatForever::create(CCSequence::create(animate, CCDelayTime::create(0.0f), NULL)));


            collect_sprite->setPosition(position_);
            //k->setScale(sizeMultiplier*0.5f);
            this->addChild(collect_sprite);

            int angle = startAngle + i;//arc4random()%360;
            float fangle = (angle * M_PI / 180) - 30;

            CCPoint minimunDestinationPoint = collect_sprite->getPosition() + ccp(distance * cosf(fangle), distance * sinf(fangle)) * 0.3f;
            float minimundistance = sqrt(pow(minimunDestinationPoint.x - collect_sprite->getPosition().x, 2) + pow(minimunDestinationPoint.y - collect_sprite->getPosition().y, 2));

            CCPoint destinationPoint = collect_sprite->getPosition() + ccp(distance * cosf(fangle), distance * sinf(fangle)) * (0.3f + arc4random() % 71 / 100.f);
            float distanceFromCenter = sqrt(pow(destinationPoint.x - collect_sprite->getPosition().x, 2) + pow(destinationPoint.y - collect_sprite->getPosition().y, 2));

            float time = distanceFromCenter / minimundistance * 0.3f;

            collect_sprite->runAction(CCSequence::create(CCEaseOut::create(CCMoveTo::create(0.4f, destinationPoint), 1 + arc4random() % 101 / 100.f),
                    CCEaseIn::create(CCMoveTo::create(time, m_destPosition), 2 + arc4random() % 101 / 100.f),
                    CCCallFuncO::create(this, callfuncO_selector(CollectableItems::explosionParticle), collect_sprite),
                    CCRemoveSelf::create(),
                    NULL));
        }
        _item->runAction(CCCallFuncO::create(this, callfuncO_selector(CollectableItems::collected), _item));
    }
    else{
        _item->runAction(CCSequence::create(CCScaleTo::create(0.1f, scale_*1.5f),
                                            CCCallFuncO::create(this, callfuncO_selector(CollectableItems::explosionParticle), _item),
                                            CCDelayTime::create(0.1f),
                                            CCScaleTo::create(0.8f, m_destScale),
                                            CCScaleTo::create(0.1f, m_destScale*1.2f),
                                            CCScaleTo::create(0.1f, m_destScale),
                                            CCRemoveSelf::create(),
                                            NULL));
        
        _item->runAction(CCSequence::create(CCMoveTo::create(0.2f, point1),
                                            CCDelayTime::create(0.1f),
                                            CCJumpTo::create(0.6f, point2, -20, 1),
                                            CCJumpTo::create(0.2f, m_destPosition, 20, 1),
                                            CCCallFuncO::create(this, callfuncO_selector(CollectableItems::collected), _item),
                                            CCCallFuncO::create(this, callfuncO_selector(CollectableItems::explosionParticle), _item),
                                            NULL));
    }




    m_running = true;

    m_items.push_back(_item);

    return true;

}

void CollectableItems::collected(CCObject* object_) {

    int i = 0;
    for (vector<CollectableItem*>::iterator it=m_items.begin(); it!=m_items.end(); ++it) {
        if (*it == object_) {
            m_items.erase(m_items.begin()+i);
            break;
        }
        i++;
    }

    m_running = false;
    if (onCollected) {
        onCollected(object_, m_sender);
    }

}

void CollectableItems::explosionParticle(cocos2d::CCNode *onNode) {

    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < 360; i+=60) {
            CCSprite *star = CCSprite::create("common/vfx/star.png");
            star->setPosition(onNode->getPosition());
            star->setScale(sizeMultiplier*0.2f);
            this->addChild(star, onNode->getZOrder() - 1);

            int startAngle = (k == 0 ? 0 : -30);
            int angle = startAngle + i;
            float fangle = angle*M_PI/180;

            float maxDistance = MAX(onNode->getContentSize().width*onNode->getScale()*1.f, onNode->getContentSize().height*onNode->getScale()*1.f);
            float distance = maxDistance*(70 + arc4random()%31)/100.f;

            CCPoint startPoint = onNode->getPosition();

            CCPoint endPoint = onNode->getPosition() + ccp(distance*cosf(fangle), distance*sinf(fangle));

            star->setPosition(startPoint);

            star->runAction(CCSequence::create(CCEaseOut::create(CCMoveTo::create(0.4f, endPoint), 3.0f),
                                               NULL));
            
            star->runAction(CCSequence::create(CCDelayTime::create(0.1f),
                                               CCEaseOut::create(CCScaleTo::create(0.3f, 0), 3.f),
                                               CCRemoveSelf::create(),
                                               NULL));
        }
    }
}

bool CollectableItems::isRunning() {
    
    return m_running;
    
}

void CollectableItems::setSender(void* sender_) {
    
    m_sender = sender_;
    
}