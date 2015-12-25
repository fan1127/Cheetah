//
//  PlayerBuildingVO.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#include "PlayerBuildingVO.h"
USING_NS_CC;

#include <spine/Json.h>
#include "../Helpers/StringHelper.h"
using namespace cocos2d::extension;

PlayerBuildingVO::PlayerBuildingVO() {
    
    buid = "";
    puid = "";
    type = "";
    flip = false;
    for (int i=0; i<BUILDING_SPACE_GRID_NUM; i++) {
        map[i] = -1;
    }
    level = 1;
    status = BUILDING_STATUS_PREVIEW;
    timestamp = 0;
    
}

bool PlayerBuildingVO::parse(void *json) {
    
    Json* _root = (Json*)json;
    if (!_root || typeid(*_root) != typeid(Json)) return false;
    
    buid = Json_getString(_root, "buid", NULL);
    //puid = Json_getString(_root, "uuid", NULL);
    puid = "11bca12e-7e7f-418e-9b6a-3dd8bdbe2208";
    type = Json_getString(_root, "type", NULL);
    flip = (bool)Json_getInt(_root, "flip", NULL);
    
    Json* _maps = Json_getItem(_root, "map");
    int _nmaps = Json_getSize(_maps);
    
    int i;
    for (i=0; i<_nmaps; i++) {
        Json* _map = Json_getItemAt(_maps, i);
        int k = (int)_map->valueint;
        map[i] = k;
    }
    while (i < BUILDING_SPACE_GRID_NUM) {
        map[i] = -1;
        i++;
    }
    
    level = Json_getInt(_root, "level", NULL);
    status = Json_getString(_root, "status", NULL);
    if (Json_getItem(_root, "timestamp")) {
        timestamp = (unsigned int)StringHelper::toInt(Json_getString(_root, "timestamp", 0));
    }else {
        timestamp = (unsigned int)StringHelper::toInt(Json_getString(_root, "updated", 0));
    }
    
    return true;
    
}

