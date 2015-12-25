//
//  CloudBlock.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/5/2557 BE.
//
//

#ifndef __Cheetah__CloudBlock__
#define __Cheetah__CloudBlock__

#include "cocos2d.h"

class CloudBlock : public cocos2d::CCNode {
    
    cocos2d::CCSpriteBatchNode *m_batchNode;
    
public:
  
    virtual bool init();
    CREATE_FUNC(CloudBlock);
    
    void updateCloud(int current_level);
    
};

#endif /* defined(__Cheetah__CloudBlock__) */
