;//
//  GDItemVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/20/2556 BE.
//
//

#ifndef __Cheetah__GDItemVO__
#define __Cheetah__GDItemVO__

#include <iostream>

#define ITEM_COIN   "coin"

struct GDItemVO {
    
    std::string type;
    int rarity_point;
    int gem_value;
    int coins_value; //price when player is selling
    int buy_value; //price when player is buying
    std::string src;
    std::string title;
    std::string title_plural;
    std::string description;
    std::string description2;
    
    GDItemVO();
    
};

#endif /* defined(__Cheetah__GDItemVO__) */
