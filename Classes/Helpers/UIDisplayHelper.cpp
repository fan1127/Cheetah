//
//  UIDisplayHelper.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/5/56 BE.
//
//

#include "UIDisplayHelper.h"
#include "cocos2d.h"
#include "../Settings.h"
#include <algorithm>

void UIDisplayHelper::convertToDisplayTime(int secs, char* buffer, size_t size) {

    if (secs > 0) {
        
        if (secs >= 60*60*24) {
            snprintf(buffer, size, "%dd", int(secs/(60*60*24)));
        }else if (secs >= (60*60)) {
            snprintf(buffer, size, "%dh", int(secs/(60*60)));
        }else if (secs >= 60) {
            snprintf(buffer, size, "%dm", int(secs/60));
        }else {
            snprintf(buffer, size, "%ds", secs);
        }
        
    }else {
        snprintf(buffer, size, "");
    }
    
}

void UIDisplayHelper::convertToDisplayTime2(int secs, char* buffer, size_t size) {

    if (secs >= 0) {
        
        if (secs >= 60*60*24) {
            int _day = int(secs/(60*60*24));
            int _hrs = int((secs%(60*60*24))/(60*60));
            if (_day > 0 && _hrs > 0) {
                snprintf(buffer, size, "%dd %dh", _day, _hrs);
            }else {
                snprintf(buffer, size, "%dd", _day);
            }
        }else if (secs >= (60*60)) {
            int _hrs = int(secs/(60*60));
            int _min = int(secs%(60*60)/60);
            if (_hrs > 0 && _min > 0) {
                snprintf(buffer, size, "%dh %dm", _hrs, _min);
            }else {
                snprintf(buffer, size, "%dh", _hrs);
            }
        }else if (secs >= 60) {
            int _min = int(secs/60);
            int _sec = int(secs%60);
            if (_min > 0 && _sec > 0) {
                snprintf(buffer, size, "%dm %ds", _min, _sec);
            }else {
                snprintf(buffer, size, "%dm", _min);
            }
        }else {
            snprintf(buffer, size, "%ds", secs);
        }
        
    }else {
        snprintf(buffer, size, "");
    }
    
}

std::string UIDisplayHelper::stringToUpperCase(char* string_)
{
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "%s", string_);
    std::string _str = _buffer;
    std::transform(_str.begin(), _str.end(), _str.begin(), ::toupper);

    return _str;
}


void UIDisplayHelper::centerItems(cocos2d::CCNode* parent, cocos2d::CCNode* item0, cocos2d::CCNode* item1, float space, float yRatio)
{
    extern float sizeMultiplier;

    float width = item0->getContentSize().width + item1->getContentSize().width + space*sizeMultiplier;
    float offset = width*0.5f;

    item0->setAnchorPoint(ccp(0.0f, 0.5f));
    item1->setAnchorPoint(ccp(0.0f, 0.5f));

    if (item0->getContentSize().width > width*0.5f)
    {
        offset = width*0.5f;
    }
    else
    {
        offset = width*0.4f;
    }

    item0->setPosition(ccp(parent->getContentSize().width*0.5f - offset,
            parent->getContentSize().height*yRatio));

    item1->setPosition(ccp(item0->getPositionX() + item0->getContentSize().width + space * sizeMultiplier, item0->getPositionY()));
}
