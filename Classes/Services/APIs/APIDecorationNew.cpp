//
//  APIDecorationNew.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 12/4/2557 BE.
//
//

#include "APIDecorationNew.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APIDecorationNew *APIDecorationNew::create(void *ptr_decoration,
                                           bool instant,
                                           void *sender,
                                           void (*callback)(void*,std::string)) {
    
    APIDecorationNew *p = new APIDecorationNew();
    p->m_ptr_decoration = ptr_decoration;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIDecorationNew::name() {
    
    return "decoration_new";
    
}

string APIDecorationNew::param() {
    
    Decoration *_decoration = (Decoration*)m_ptr_decoration;
    CCAssert(_decoration!=NULL, "Decoration must not be NULL!");
    
    CREATE_CHAR_BUFFER(_buffer, 16);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&type=" + _decoration->pdVo.type;
    snprintf(_buffer, _buffer_size, "&flip=%d", (int)_decoration->pdVo.flip);
    _params.append(_buffer);
    
    CREATE_CHAR_BUFFER(map_, (BUILDING_SPACE_GRID_NUM*5)+2);
    snprintf(map_,map__size, "[");
    bool added = false;
    for (int i=0; i<BUILDING_SPACE_GRID_NUM; i++) {
        if (_decoration->pdVo.map[i] >= 0) {
            snprintf(map_,map__size, "%s%d,", map_, _decoration->pdVo.map[i]);
            added = true;
        }
    }
    string map__;
    map__.clear();
    map__.append(map_);
    if (added && map__.size() > 0) map__.resize(map__.size()-1);
    map__.append("]");
    _params.append("&map=");
    _params.append(map__);
    
    if (m_instant) {
        _params += "&instant=1";
    }
    
    return _params;
    
}
