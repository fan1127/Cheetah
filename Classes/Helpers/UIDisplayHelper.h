//
//  UIDisplayHelper.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/5/56 BE.
//
//

#ifndef __Cheetah__UIDisplayHelper__
#define __Cheetah__UIDisplayHelper__

#include <stdio.h>
#include <string>

namespace cocos2d { class CCNode; }

class UIDisplayHelper {
    
public:
    
    static void convertToDisplayTime(int secs, char* buffer, size_t size);
    static void convertToDisplayTime2(int secs, char* buffer, size_t size);
    
    static std::string stringToUpperCase(char* buffer);

    static void centerItems(cocos2d::CCNode* parent, cocos2d::CCNode*item0, cocos2d::CCNode*item1, float space, float yRatio);
};

#endif /* defined(__Cheetah__UIDisplayHelper__) */
