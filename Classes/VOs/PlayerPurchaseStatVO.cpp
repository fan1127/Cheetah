//
//  PlayerPurchaseStatVO.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/25/2557 BE.
//
//

#include "PlayerPurchaseStatVO.h"
using namespace cocos2d::extension;

PlayerPurchaseStatVO::PlayerPurchaseStatVO()  {
    
    dataReceived = false;
    account_created = "";
    total_purchase = 0;
    first_purchase_datetime = "";
    last_purchase_datetime = "";
    total_purchase_value = 0.0f;
    highest_package_purchased = "";
    
}

bool PlayerPurchaseStatVO::parseJSON(Json *root) {
    
    if (!root) return false;
    
    Json* _purchase_stat_json = Json_getItem(root, "purchase_stat");
    if (!_purchase_stat_json) return false;
    
    dataReceived = true;
    account_created = Json_getString(_purchase_stat_json, "account_created", account_created.c_str());
    total_purchase = Json_getInt(_purchase_stat_json, "total_purchase", total_purchase);
    first_purchase_datetime = Json_getString(_purchase_stat_json, "first_purchase_datetime", first_purchase_datetime.c_str());
    last_purchase_datetime = Json_getString(_purchase_stat_json, "last_purchase_datetime", last_purchase_datetime.c_str());
    total_purchase_value = Json_getFloat(_purchase_stat_json, "total_purchase_value", total_purchase_value);
    highest_package_purchased = Json_getString(_purchase_stat_json, "highest_package_purchased", highest_package_purchased.c_str());
    
    return true;
    
}

void PlayerPurchaseStatVO::print() {
    
    printf("################### Purchase Stat ###################\n");
    printf("# account_created: %s\n", account_created.c_str());
    printf("# total_purchase: %d\n", total_purchase);
    printf("# first_purchase_datetime: %s\n", first_purchase_datetime.c_str());
    printf("# last_purchase_datetime: %s\n", last_purchase_datetime.c_str());
    printf("# total_purchase_value: %.2f\n", total_purchase_value);
    printf("# highest_package_purchased: %s\n", highest_package_purchased.c_str());
    printf("#####################################################\n");
    
}
