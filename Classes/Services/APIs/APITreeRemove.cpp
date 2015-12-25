//
//  APITreeRemove.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/19/2557 BE.
//
//

#include "APITreeRemove.h"
using namespace std;

#include "../../Entities/PlayerState.h"
#include "../../Entities/Decoration.h"

APITreeRemove *APITreeRemove::create(void *ptr_tree,
                                     bool instant,
                                     void *sender,
                                     void (*callback)(void*,std::string)) {
    
    APITreeRemove *p = new APITreeRemove();
    p->m_ptr_tree = ptr_tree;
    p->m_instant = instant;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APITreeRemove::name() {
    
    return "tree_remove";
    
}

string APITreeRemove::param() {
    
    Decoration *_tree = (Decoration*)m_ptr_tree;
    CCAssert(_tree!=NULL, "Tree must not be NULL!");
    
    _tree->pdVo.status = kDecorationStatusKept;
    
    CREATE_CHAR_BUFFER(_buffer, 16);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    _params += "&duid=" + _tree->pdVo.duid;
    _params += "&posx=";
    snprintf(_buffer, _buffer_size, "%f", _tree->getPositionX());
    _params.append(_buffer);
    _params += "&posy=";
    snprintf(_buffer, _buffer_size, "%f", _tree->getPositionY());
    _params.append(_buffer);
    if (m_instant) {
        _params += "&instant=1";
    }
    
    return _params;
    
}
