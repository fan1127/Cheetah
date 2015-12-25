//
//  CCLoadSpriteURL.h
//  Cheetah
//
//  Created by Teerapat on 16/5/2557 BE.
//
//

#ifndef __Cheetah__CCLoadSpriteURL__
#define __Cheetah__CCLoadSpriteURL__

#include "cocos2d.h"

#include "cocos-ext.h"

class CCLoadSpriteURL : public cocos2d::CCSprite {
    
    void onImageDownLoaded(cocos2d::extension::CCHttpClient* pSender, cocos2d::extension::CCHttpResponse* pResponse);
    void downLoadImage(char *name, char *pUrl);
    
public:
    
    cocos2d::extension::CCHttpRequest* request;
    
    static CCLoadSpriteURL *create(char *defaultSprite, cocos2d::CCSize defaultSize, char *name, char *pUrl);
    bool init(char *defaultSprite, cocos2d::CCSize defaultSize, char *name, char *pUrl);
    
};

#endif /* defined(__Cheetah__CCLoadURL__) */
