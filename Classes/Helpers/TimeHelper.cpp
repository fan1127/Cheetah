//
//  TimeHelper.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 7/9/2557 BE.
//
//

#include "TimeHelper.h"
using namespace std;

unsigned long int TimeHelper::getCurrentLocalTime() {
    
    long int t = static_cast<long int>(time(NULL));
    printf("%ld",t);
    
    return t;
    
}