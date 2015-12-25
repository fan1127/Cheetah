//
//  CommonStruct.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/24/2556 BE.
//
//

#ifndef __Cheetah__CommonStruct__
#define __Cheetah__CommonStruct__

#include <iostream>

struct FloatKeyValue {
    
    std::string key;
    float value;
    
    FloatKeyValue();
    
};

struct IntKeyValue {
    
    std::string key;
    int value;
    
    IntKeyValue();
    IntKeyValue(std::string key_, int value_);
    
};

#endif /* defined(__Cheetah__CommonStruct__) */
