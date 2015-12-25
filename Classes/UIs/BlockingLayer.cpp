//
//  BlockingLayer.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 11/28/2557 BE.
//
//

#include "BlockingLayer.h"

extern float sizeMultiplier;

BlockingLayer* BlockingLayer::create() {

    BlockingLayer *obj = new BlockingLayer();
    if (!obj->init()) {
        return NULL;
    }
    obj->autorelease();

    return obj;
}

bool BlockingLayer::init()
{
    if (!CCLayerColor::initWithColor(ccc4(0, 0, 0, 0)))
    {
        return false;
    }

    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);
    setTouchPriority(-1000000);

    return true;
}

void BlockingLayer::show()
{
    CCScene *runningScene = CCDirector::sharedDirector()->getRunningScene();
    runningScene->addChild(this, 100000000);
    this->setOpacity(0);
}

void BlockingLayer::hide()
{
    removeFromParent();
}

bool BlockingLayer::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    CCPoint point = convertTouchToNodeSpace(touch);
    CCRect rect = boundingBox();
    if (rect.containsPoint(point))
    {
        return true;
    }
    return false;
}
