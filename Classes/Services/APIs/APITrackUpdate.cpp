//
//  APITrackUpdate.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/8/2558 BE.
//
//

#include "APITrackUpdate.h"
using namespace std;

#include "../../Entities/PlayerState.h"

APITrackUpdate *APITrackUpdate::create(void *sender,
                                       void (*callback)(void *, std::string)) {
    
    APITrackUpdate *p = new APITrackUpdate();
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APITrackUpdate::name() {
    
    return "track_update";
    
}

string APITrackUpdate::param() {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    string _params = "";
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    _params += _playerState->getUserParam();

    snprintf(_buffer, _buffer_size, "&start=%d&direction=%d&status=%d",
            _playerState->player.racing.start,
            (int)_playerState->player.racing.direction,
            (int)_playerState->player.racing.status);
    _params += _buffer;
    
    if (_playerState->player.racing.host != "") {
        if (Pet* _host = _playerState->getPet(_playerState->player.racing.host)) {
            int _horse_power = _host->ppVo.top_speed + _host->ppVo.corner_speed + _host->ppVo.jumping;
            snprintf(_buffer, _buffer_size, "&horse_power=%d", _horse_power);
            _params += _buffer;
        }
    }

    return _params;
    
}

string APITrackUpdate::param2() {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    string _params;
    _params += "&data=" + _playerState->getTrackData();
    _params += "&host=" + _playerState->player.racing.host;
    
    string _building = _playerState->getChangedBuildingsJSON();
    if (_building != "")
        _params += "&building=" + _building;
    
    string _decoration = _playerState->getChangedDecorationsJSON();
    if (_decoration != "")
        _params += "&decoration=" + _decoration;
    
    return  _params;
    
}