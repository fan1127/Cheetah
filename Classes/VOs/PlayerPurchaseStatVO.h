//
//  PlayerPurchaseStatVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/25/2557 BE.
//
//

#ifndef __Cheetah__PlayerPurchaseStatVO__
#define __Cheetah__PlayerPurchaseStatVO__

#include <stdio.h>
#include <string>
#include <spine/Json.h>

struct PlayerPurchaseStatVO {
    
    bool dataReceived;
    std::string account_created;
    int total_purchase;
    std::string first_purchase_datetime;
    std::string last_purchase_datetime;
    float total_purchase_value;
    std::string highest_package_purchased;
    
    PlayerPurchaseStatVO();
    bool parseJSON(cocos2d::extension::Json* root);
    void print();
    
};

#endif /* defined(__Cheetah__PlayerPurchaseStatVO__) */
