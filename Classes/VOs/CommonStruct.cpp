//
//  CommonStruct.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/24/2556 BE.
//
//

#include "CommonStruct.h"

FloatKeyValue::FloatKeyValue() {
    
    key                             = "";
    value                           = 0.0f;
    
}

IntKeyValue::IntKeyValue() {
    
    key                             = "";
    value                           = 0;
    
}

IntKeyValue::IntKeyValue(std::string key_, int value_) {
    
    key                             = key_;
    value                           = value_;
    
}