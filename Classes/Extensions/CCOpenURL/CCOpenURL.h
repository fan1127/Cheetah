//
//  CCOpenURL.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 4/12/2557 BE.
//
//

#ifndef __Cheetah__CCOpenURL__
#define __Cheetah__CCOpenURL__

#include <iostream>

class CCOpenURL {
    
    
public:
    static bool canOpenURL(std::string stringURL);
    static bool openURL(std::string stringURL);
    static void openAndroidMarket();
};

#endif /* defined(__Cheetah__CCOpenURL__) */
