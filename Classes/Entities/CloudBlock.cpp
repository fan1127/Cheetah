//
//  CloudBlock.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/5/2557 BE.
//
//

#include "CloudBlock.h"
USING_NS_CC;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

bool CloudBlock::init() {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_batchNode = CCSpriteBatchNode::create(g_gameContent->correctFileNameWithPath("ui.plist").c_str());
    addChild(m_batchNode);
    
    
    
    return true;
    
}

void CloudBlock::updateCloud(int current_level) {
    /*
    if (m_batchNode) {
        
        m_batchNode->removeAllChildrenWithCleanup(true);
        
        int column = sqrt(N_ZONE);
        for (int j=0; j<column; j++) {
            for (int i=column-1; i>=0; i--) {
                int k = j*column+i;
                if (current_level < g_gameContent->BlockZone[k]) {
                    CCSprite *_cloud = CCSprite::createWithSpriteFrameName("_cloud4x4.png");
                    _cloud->setPosition(ccp(i*g_gameContent->tileSize.width*2.0f + j*g_gameContent->tileSize.width*2.0f,
                                            i*g_gameContent->tileSize.height*2.0f - j*g_gameContent->tileSize.height*2.0f));// + ));
                    m_batchNode->addChild(_cloud);
                }
            }
        }
        
        m_batchNode->setPosition(ccp(-g_gameContent->tileSize.width*18.25f, g_gameContent->tileSize.height));
    
    }
    */
}
