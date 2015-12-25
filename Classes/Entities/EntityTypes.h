//
//  EntityTypes.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/19/2557 BE.
//
//

#ifndef __Cheetah__EntityTypes__
#define __Cheetah__EntityTypes__

#include <iostream>

enum EntityType {
    ET_UNKNOWN,
    ET_UNWANTED_TREE,
    ET_DECORATION_REMOVABLE,
    ET_BUILDING_HQ,
    ET_BUILDING_UNCLE_HOUSE,
    ET_BUILDING_TICKETBOOTH,
    ET_BUILDING_COIN_STORAGE,
    ET_BUILDING_HABITAT,
    ET_BUILDING_ANIMAL,
    ET_BUILDING_MARKET,
    ET_BUILDING_INVENTORY,
    ET_BUILDING_ASSOCIATION,
    ET_BUILDING_PRODUCER,
    ET_BUILDING_FOOD_STORAGE,
    ET_BUILDING_TRAINING,
    ET_BUILDING_EXPLORER,
    ET_BUILDING_LIBRARY,
};

EntityType getEntityType(void *entity_);

#endif /* defined(__Cheetah__EntityTypes__) */
