//
//  CCUrlSprite.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 1/10/2557 BE.
//
//

#ifndef __Cheetah__CCUrlSprite__
#define __Cheetah__CCUrlSprite__

#include "cocos2d.h"
#include "CCHttpRequest.h"

USING_NS_CC;

struct MemoryStruct {
    char* memory;
    size_t size;
    size_t writepos;
};
//This one is to handle which image is loaded (in my game I launch three or four threads and I need to know which image is loaded)
struct DataStruct {
    const char* URL;
    const char* name;
};

class CCUrlSprite : public cocos2d::CCSprite
{
    
    const char *_name;
public:
    
    static CCUrlSprite *create(char *name, char *pUrl);
    bool init(char *name, char *pUrl);
    virtual void update(float delta);
    void onHttpRequestCompleted(cocos2d::CCObject *pSender, void *data);
};

#endif /* defined(__Cheetah__CCUrlSprite__) */
