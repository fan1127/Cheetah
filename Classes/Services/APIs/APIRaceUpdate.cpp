//
//  APIRaceUpdate.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/9/2558 BE.
//
//

#include "APIRaceUpdate.h"
using namespace std;

extern string race_id;

APIRaceUpdate *APIRaceUpdate::create(float player_time,
                                     string player_replay,
                                     float opponent_time,
                                     string opponent_replay,
                                     string helper_pet_type,
                                     string helper_pet_replay,
                                     bool timeup,
                                     float ai_difficulty,
                                     int garage_level,
                                     void *sender,
                                     void (*callback)(void *, std::string)) {
    
    APIRaceUpdate *p = new APIRaceUpdate();
    p->m_player_time = player_time;
    p->m_player_replay = player_replay;
    p->m_opponent_time = opponent_time;
    p->m_opponent_replay = opponent_replay;
    p->m_helper_pet_type = helper_pet_type;
    p->m_helper_pet_replay = helper_pet_replay;
    p->m_timeup = timeup;
    p->m_ai_difficulty = ai_difficulty;
    p->m_garage_level = garage_level;
    p->setCallback(sender, callback);
    p->autorelease();
    return p;
    
}

string APIRaceUpdate::param() {
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    string _params = "";
    _params += PlayerState::getInstance()->getUserParam();
    
    snprintf(_buffer, _buffer_size, "&garage_level=%d", m_garage_level);
    _params += _buffer;
    _params += "&race_id=" + race_id;
    snprintf(_buffer, _buffer_size, "&guest_time=%f", m_player_time);
    _params += _buffer;
    snprintf(_buffer, _buffer_size, "&host_time=%f", m_opponent_time);
    _params += _buffer;
    _params += m_timeup?"&timeup=1":"";
    snprintf(_buffer, _buffer_size, "&ai_difficulty=%f", m_ai_difficulty);
    _params += _buffer;
    
    return _params;
    
}

string APIRaceUpdate::param2() {
    
    string _params = "";
    
    _params += "&guest_replay=" + m_player_replay;
    _params += "&host_replay=" + m_opponent_replay;
    if (m_helper_pet_type != "" && m_helper_pet_replay != "") {
        _params += "&helperpet_type=" + m_helper_pet_type;
        _params += "&helperpet_replay=" + m_helper_pet_replay;
    }
    
    return _params;
    
}